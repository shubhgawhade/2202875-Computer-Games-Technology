using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Serialization;
using Random = UnityEngine.Random;

public class HandleDataServer : MonoBehaviour
{
    private AsynchronousSocketListener server;
    
    NetworkTimer networkTimer;
    
    public const int MaxPlayers = 4;
    
    public List<int> _playerIDs = new List<int>();
    // private int[] playerIDs = { 1, 2, 3, 4 };

    public GameState gameState = new GameState();

    public Queue<InputData> inputQueue = new Queue<InputData>();
    
    public bool acceptNewConnections = true;

    [Serializable]
    public struct StatePayload
    {
        public int tick;
        public Vector3 position;
    };
    
    public StatePayload[][] stateBuffer;

    public int[] lastProcessedTick;
    
    private void Awake()
    {
        server = AsynchronousSocketListener.asynchronousSocketListener;
        networkTimer = new NetworkTimer(30);
    }

    // Start is called before the first frame update
    void Start()
    {
        for (int i = 0; i < MaxPlayers; i++)
        {
            _playerIDs.Add(i);
        }
    }

    private InputData[] inputData;
    private async void FixedUpdate()
    {
        while (networkTimer.ShouldTick()) 
        {
            // print(networkTimer.CurrentTick + $" - {DateTime.UtcNow.Second}:{DateTime.UtcNow.Millisecond}");
            
            //Handle server tick
            // If received input in Queue, go to last performed step ad re-simulate to the tick for new input
            // Store the input and position states in buffers, same as tick number and process from that point

            while (inputQueue.Count > 0)
            {
                if (inputData == null)
                {
                    inputData = new InputData[ExternServer.ConnectedPlayers.Count];
                }
                
                InputData peekInputQueue = inputQueue.Peek();
                inputData[peekInputQueue.playerID] = inputQueue.Dequeue();
                
                // foreach (GameObject o in ObjectsInScene)
                for (int i = 0; i < ExternServer.ConnectedPlayers.Count; i++)
                {
                    if(inputData[i] == null) continue;
                    
                    if (ObjectsInScene[i].GetComponent<OnlinePlayerController>().id == inputData[i].playerID)
                    {
                        // print(o.name);
                        if (inputData[i].tick < lastProcessedTick[inputData[i].playerID])
                        {
                            while ((stateBuffer[i][lastProcessedTick[inputData[i].playerID]].position - ObjectsInScene[i].transform.position)
                                   .magnitude > 0.1f)
                            {
                                // o.transform.position = Vector3.Lerp(o.transform.position,
                                //     stateBuffer[lastProcessedTick[inputData.playerID]].position, Time.fixedDeltaTime);

                                ObjectsInScene[i].transform.position = stateBuffer[i][lastProcessedTick[inputData[i].playerID]].position;
                            }
                            // o.GetComponent<Rigidbody>().velocity = stateBuffer[lastProcessedTick].velocity;
                        }
                    }
                }

                // foreach (GameObject o in ObjectsInScene)
                // {
                //     // print(o.name);
                //     if (o.GetComponent<OnlinePlayerController>().id == inputData.playerID)
                //     {
                //         // Add to Input Queue with tick number
                //
                //         // print(inputData.playerID);
                //         float move = inputData.right;
                //         while (inputData.tick < networkTimer.CurrentTick)
                //         {
                //             Physics.simulationMode = SimulationMode.Script;
                //             o.GetComponent<OnlinePlayerController>().Move(move, networkTimer.MinTimeBetweenTicks);
                //             Physics.Simulate(Time.fixedDeltaTime);
                //             Physics.simulationMode = SimulationMode.FixedUpdate;
                //
                //             print(inputData.tick);
                //             stateBuffer[inputData.tick] = new StatePayload
                //             {
                //                 tick = inputData.tick,
                //                 position = o.transform.position,
                //                 // rb = o.GetComponent<Rigidbody>()
                //                 velocity = o.GetComponent<Rigidbody>().velocity
                //             };
                //             lastProcessedTick = inputData.tick;
                //             inputData.tick++;
                //         }
                //     }
                // }
            }

            if (inputData == null) return;
            
            // foreach (GameObject o in ObjectsInScene)
            for (int i = 0; i < ExternServer.ConnectedPlayers.Count; i++)
            {
                if(inputData[i] == null) continue;
                
                // print(o.name);
                if (ObjectsInScene[i].GetComponent<OnlinePlayerController>().id == inputData[i].playerID)
                {
                    if (networkTimer.CurrentTick == 0)
                    {
                        inputData[i].tick = 0;
                    }
                    
                    // Add to Input Queue with tick number
                        
                    // print(inputData.playerID);
                    while (inputData[i].tick <= networkTimer.CurrentTick)
                    {
                    
                        // print($"CT {inputData[i].tick} : NT {networkTimer.CurrentTick}");
                        stateBuffer[i][inputData[i].tick] = new StatePayload
                        {
                            tick = inputData[i].tick,
                            position = ObjectsInScene[i].transform.position
                        };
                        lastProcessedTick[inputData[i].playerID] = inputData[i].tick;
                        
                        Vector3 moveDir = new Vector3(inputData[i].right, 0, inputData[i].forward);
                        ObjectsInScene[i].GetComponent<OnlinePlayerController>().Move(moveDir, networkTimer.MinTimeBetweenTicks);
                        
                        // SEND SERVER SIMULATION EVERY 10 ticks = 0.33s, 15 ticks = 0.495s
                        if (networkTimer.CurrentTick % 10 == 0)
                        {
                            Player tempPlayer = new Player();
                            
                            foreach (Player player in server.playersConnected)
                            {
                                if (player.PlayerID == inputData[i].playerID)
                                {
                                    tempPlayer.dataUpdateType = DataUpdateType.Transform;
                                    TransformData td = (TransformData) tempPlayer.returnDataStruct; 
                                    
                                    td = new TransformData
                                    {
                                        tick = networkTimer.CurrentTick,
                                        playerID = inputData[i].playerID,
                                        pos = new Pos
                                        {
                                            _posX = ObjectsInScene[i].transform.position.x,
                                            _posY = ObjectsInScene[i].transform.position.y,
                                            _posZ = ObjectsInScene[i].transform.position.z
                                        }
                                    };
                                    
                                    tempPlayer.dataToSend = tempPlayer.ObjectToByteArray(td);
                                    await SendData.Send(tempPlayer, tempPlayer.dataToSend, SendData.SendType.ReplyAll);
                                }
                            }
                        }
                        
                        inputData[i].tick++;
                    }
                    
                    // while (inputData.tick < networkTimer.CurrentTick)
                    // {
                    // Physics.simulationMode = SimulationMode.Script;
                    // Physics.Simulate(MathF.Abs(networkTimer.CurrentTick - inputData.tick) * networkTimer.MinTimeBetweenTicks);
                    // o.GetComponent<OnlinePlayerController>().Move(inputData.right, networkTimer.MinTimeBetweenTicks);
                    // // inputData.tick++;
                    // print(inputData.tick);
                    // Physics.simulationMode = SimulationMode.FixedUpdate;
                    // stateBuffer[networkTimer.CurrentTick] = new StatePayload
                    // {
                    //     tick = inputData.tick,
                    //     position = o.transform.position,
                    //     // rb = o.GetComponent<Rigidbody>()
                    //     velocity = o.GetComponent<Rigidbody>().velocity
                    // };
                    // lastProcessedTick = networkTimer.CurrentTick;
                    // }
                        
                    // Get the ticks between inputs, simulate and send position data back
                        
                    // Physics.Simulate(Time.fixedDeltaTime * 5);
                    // print(o.transform.position.x);
                }
            }
        }
    }

    // Update is called once per frame
    void Update()
    {
        networkTimer.Update(Time.deltaTime);
        // networkTimer.ShouldTick();
        // print(networkTimer.CurrentTick + $" - {DateTime.UtcNow.Second}:{DateTime.UtcNow.Millisecond}");
        if (server.receivedDataToHandle.Count > 1)
        {
            print("SERVER Q COUNT : " + server.receivedDataToHandle.Count);
        }

        if (server.playersConnected.Count == 0)
        {
            gameState.ChangeState(GameState.gameStateEnum.Lobby);

            foreach (GameObject o in ObjectsInScene)
            {
                Destroy(o);
            }
        }
        
        while (server.receivedDataToHandle.Count > 0)
        {
            DataToHandle data = server.receivedDataToHandle.Dequeue();

            switch (data.dataUpdateType)
            {
                case DataUpdateType.Joining:
                    
                    JoiningData joiningData = (JoiningData) data.deserializedData;
                    if (data.player.playerName == null)
                    {
                        data.player.playerName = joiningData.playerName;
                        data.player.dataUpdateType = DataUpdateType.JoiningDataReply;
                        
                        JoinLeaveData joinLeaveData = (JoinLeaveData) data.player.returnDataStruct;

                        if (_playerIDs.Count == 0 || !acceptNewConnections)
                        {
                            Debug.LogWarning($"{data.player.playerName} tried to join when SERVER HAS MAX PLAYERS");
                            // MAX PLAYERS : SEND DISCONNECTION

                            server.QuitClient(data.player.workSocket, data.player, -1);
                    
                            break;
                        }
                
                        // print(_playerIDs.Count);
                        if (_playerIDs.Count > 0)
                        {
                            data.player.PlayerID = _playerIDs[0];
                            Debug.Log($"{data.player.playerName} CONNECTED!");
                    
                            _playerIDs.Remove(_playerIDs[0]);
                        }
                        else
                        {
                    
                        }

                        // Server Prediction Variables
                        lastProcessedTick = new int[ExternServer.ConnectedPlayers.Count];
                        stateBuffer = new StatePayload[ExternServer.ConnectedPlayers.Count][];
                        for (int i = 0; i < stateBuffer.Length; i++)
                        {
                            stateBuffer[i] = new StatePayload[networkTimer.MaxTick];
                        }
                        
                        joinLeaveData.playersConnected = new string[ExternServer.ConnectedPlayers.Count];
                        joinLeaveData.playerIDs = new int[ExternServer.ConnectedPlayers.Count];
                        joinLeaveData.ready = new bool[ExternServer.ConnectedPlayers.Count];
                        for (int i = 0; i < ExternServer.ConnectedPlayers.Count; i++)
                        {
                            joinLeaveData.playersConnected[i] = ExternServer.ConnectedPlayers[i].playerName;
                            joinLeaveData.playerIDs[i] = ExternServer.ConnectedPlayers[i].PlayerID;
                            joinLeaveData.ready[i] = ExternServer.ConnectedPlayers[i].ready;
                        }
                        // joiningDataReply.playersConnected = "AA";
                        
                        joinLeaveData.tick = networkTimer.CurrentTick;
                        joinLeaveData.subTick = networkTimer.timer;
                        
                        // print(networkTimer.CurrentTick);
                        
                        data.player.dataToSend = data.player.ObjectToByteArray(joinLeaveData);
                        Debug.Log($"{data.player.dataToSend.Length} {data.player.dataUpdateType}");
                        SendData.Send(data.player, data.player.dataToSend, SendData.SendType.ReplyAll);
                    }
            
                    break;
                
                case DataUpdateType.Ready:

                    ReadyStatus readyStatus = (ReadyStatus) data.deserializedData;
                    // readyStatus.playerID = 1;
                    readyStatus.playerID = data.player.PlayerID;
                    data.player.ready = readyStatus.ready;

                    data.player.dataToSend = data.player.ObjectToByteArray(readyStatus);
                    SendData.Send(data.player, data.player.dataToSend, SendData.SendType.ReplyAll);

                    // UNCOMMENT FOR MINIMUM 2 PLAYERS
                    // if (ExternServer.ConnectedPlayers.Count > 1)
                    {
                        bool allReady = false;
                        foreach (Player player in server.playersConnected)
                        {
                            if (!player.ready)
                            {
                                allReady = false;
                                break;
                            }

                            allReady = true;
                        }
                        
                        if (allReady)
                        {
                            acceptNewConnections = false;

                            print("Start Game");

                            StartCoroutine(Wait());
                            

                            gameState.ChangeState(GameState.gameStateEnum.Game);
                            
                            foreach (Player player in server.playersConnected)
                            {
                                // RESET READY TO USE WHEN ALL PLAYERS LOAD GAME SCENE TO START TIMER
                                player.ready = false;
                            }
                        }
                    }
                
                    break;
                
                case DataUpdateType.StartGame:
                    
                    StartGameData startGameData = (StartGameData) data.deserializedData;
                    data.player.ready = true;

                    int lastJoinTick;
                    bool hasLoadedGame = false;
                    foreach (Player player in server.playersConnected)
                    {
                        if (!player.ready)
                        {
                            hasLoadedGame = false;
                            break;
                        }

                        hasLoadedGame = true;
                    }

                    if (hasLoadedGame)
                    {
                        lastJoinTick = startGameData.tickAtSceneLoad;

                        startGameData.gameLengthInSeconds = 300;
                        // Send a future tick greater than twice the difference of lastJoinTick and current tick
                        startGameData.tickAtSceneLoad =
                            (networkTimer.CurrentTick + (networkTimer.CurrentTick - lastJoinTick)) %
                            networkTimer.MaxTick;
                        
                        print(networkTimer.CurrentTick - lastJoinTick);
                        
                        print($"START TIMER TICK {startGameData.tickAtSceneLoad}");
                        data.player.dataToSend = data.player.ObjectToByteArray(startGameData);
                        SendData.Send(data.player, data.player.dataToSend, SendData.SendType.ReplyAll);
                    }
                    
                    break;
                
                case DataUpdateType.Input:

                    InputData inputDataRecd = (InputData) data.deserializedData;

                    // print(inputData.right);

                    inputQueue.Enqueue(inputDataRecd);
                    // foreach (GameObject o in ObjectsInScene)
                    // {
                    //     print(o.name);
                    //     if (o.GetComponent<OnlinePlayerController>().id == inputData.playerID)
                    //     {
                    //         // Add to Input Queue with tick number
                    //         
                    //         print(inputData.playerID);
                    //         o.GetComponent<OnlinePlayerController>().Move(inputData.right, networkTimer.MinTimeBetweenTicks);
                    //         // Physics.simulationMode = SimulationMode.Script;
                    //         
                    //         // Get the ticks between inputs, simulate and send position data back
                    //         
                    //         // Physics.Simulate(Time.fixedDeltaTime * 5);
                    //         // print(o.transform.position.x);
                    //     }
                    // }
                    
                    break;
                
                case DataUpdateType.Transform:

                    TransformData transformData = (TransformData) data.deserializedData;
                    string content = $"{transformData.pos._posX}, {transformData.pos._posY}, {transformData.pos._posZ}";
                    Debug.Log($"{data.player.playerName} : {content}");
                
                    transformData.playerID = data.player.PlayerID;
                    data.player.dataToSend = data.player.ObjectToByteArray(transformData);
                    SendData.Send(data.player, data.player.dataToSend, SendData.SendType.ReplyAllButSender);
                    
                    break;
                
                case DataUpdateType.JoiningDataReply:

                    JoinLeaveData joinLeaveDataa = (JoinLeaveData) data.deserializedData;
                    if (joinLeaveDataa.errorCode != -1)
                    {
                        _playerIDs.Insert(0, data.player.PlayerID);
                    }
                
                    break;
            }
        }
    }

    [SerializeField] private GameObject[] placedCoins;
    IEnumerator Wait()
    {
        yield return new WaitForSeconds(2);
        SpawnPlayers();
        
        // SPAWN COINS
        // SpawnCoins();
    }
    
    public List<GameObject> ObjectsInScene = new List<GameObject>();
    [SerializeField] private GameObject playerPrefab;
    [SerializeField] private GameObject coinPrefab;
    [SerializeField] private GameObject[] coinsArray;
    
    async void SpawnPlayers()
    {
        // playerObjects = new GameObject[ExternServer.ConnectedPlayers.Count];
        for (int i = 0; i < ExternServer.ConnectedPlayers.Count; i++)
        {
            Vector3 tempPos = new Vector3(i * 2, 0, 0);
            GameObject temp = Instantiate(playerPrefab, tempPos, Quaternion.identity);
            ObjectsInScene.Add(temp); 
            
            OwnedObject ownedObject =  RegisterObject(ExternServer.ConnectedPlayers[i], ObjectsInScene[i]);
            ownedObject.startPos = new Pos
            {
                _posX = tempPos.x,
                _posY = tempPos.y,
                _posZ = tempPos.z
            };
            ownedObject.playerID = ExternServer.ConnectedPlayers[i].PlayerID;
            temp.GetComponent<OnlinePlayerController>().id = ownedObject.playerID;
            temp.AddComponent<ServerActions>();
            
            // Send connected players,their objects to spawn
            ExternServer.ConnectedPlayers[i].dataUpdateType = DataUpdateType.OwnedObject;
            ExternServer.ConnectedPlayers[i].dataToSend = ExternServer.ConnectedPlayers[i].ObjectToByteArray(ownedObject);
            await SendData.Send(ExternServer.ConnectedPlayers[i], ExternServer.ConnectedPlayers[i].dataToSend,
                SendData.SendType.ReplyAll);
        }
    }

    // async void SpawnCoins()
    // {
    //     int numberOfCoins = 15;
    //     coinsArray = new GameObject[15];
    //     
    //     for (int i = 0; i < 15; i++)
    //     {
    //         Vector3 tempCoinPos = Vector3.zero;
    //         
    //         int collisions;
    //         do
    //         {
    //             tempCoinPos = new Vector3(Random.Range(-10, 10), 0, Random.Range(-10, 10));
    //             Collider[] col = Physics.OverlapBox(tempCoinPos, coinPrefab.transform.localScale / 2, Quaternion.identity);
    //             collisions = col.Length;
    //
    //         } while (collisions > 1);
    //         
    //         GameObject tempCoin = Instantiate(coinPrefab, tempCoinPos, Quaternion.identity);
    //         OwnedObject ownedObject =
    //             RegisterObject(tempCoin, ObjectType.Coin);
    //         ownedObject.startPos = new Pos
    //         {
    //             _posX = tempCoinPos.x,
    //             _posY = tempCoinPos.y,
    //             _posZ = tempCoinPos.z
    //         };
    //         
    //         coinsArray[i] = tempCoin;
    //         
    //         ExternServer.ConnectedPlayers[0].dataUpdateType = DataUpdateType.OwnedObject;
    //         ExternServer.ConnectedPlayers[0].dataToSend = ExternServer.ConnectedPlayers[0].ObjectToByteArray(ownedObject);
    //         await SendData.Send(ExternServer.ConnectedPlayers[0], ExternServer.ConnectedPlayers[0].dataToSend,
    //             SendData.SendType.ReplyAll);
    //     }
    //     
    //     // foreach (GameObject placedCoin in placedCoins)
    //     // {
    //     //     OwnedObject ownedObject =
    //     //         RegisterObject(placedCoin, ObjectType.Coin);
    //     //     ownedObject.startPos = new Pos
    //     //     {
    //     //         _posX = placedCoin.transform.position.x,
    //     //         _posY = placedCoin.transform.position.y,
    //     //         _posZ = placedCoin.transform.position.z
    //     //     };
    //     //     
    //     //     ExternServer.ConnectedPlayers[0].dataUpdateType = DataUpdateType.OwnedObject;
    //     //     ExternServer.ConnectedPlayers[0].dataToSend = ExternServer.ConnectedPlayers[0].ObjectToByteArray(ownedObject);
    //     //     await SendData.Send(ExternServer.ConnectedPlayers[0], ExternServer.ConnectedPlayers[0].dataToSend,
    //     //         SendData.SendType.ReplyAll);
    //     // }
    // }

    private OwnedObject RegisterObject(Player player, GameObject objToOwn)
    {
        OwnedObject ownedObject = new OwnedObject
        {
            objectType = (int)ObjectType.Player
        };

        player.playerOwnedObjects.Add(ownedObject);

        return ownedObject;
    }
    
    private void OnApplicationQuit()
    {
        
    }
}