using System;
using System.Collections.Generic;
using UnityEngine;

public class ProcessDataClient : MonoBehaviour
{
    private ClientNew client;
    
    [SerializeField] private GameObject playerPrefab;
    [SerializeField] private GameObject coinPrefab;

    // [SerializeField] private UIManager uiManager;

    private int tickToStartGame = -1;
    private int gameLength;
    public static Action<int> StartInGameTimer;

    private void Awake()
    {
        client = GetComponent<ClientNew>();
    }

    // Start is called before the first frame update
    void Start()
    {
        
    }

    private bool updateOtherPos;
    private TransformData[] otherPayersPos;
    
    // [Serializable]
    // public class PositionInfo
    // {
    //     public Queue<TransformData> transformData = new Queue<TransformData>();
    // }
    //
    // public PositionInfo[] positionInfo;
    // private TransformData[] prevTransforms = new TransformData[2];
    // private Vector3[] prevlocations = new Vector3[2];
    
    private void FixedUpdate()
    {
        if (updateOtherPos && otherPayersPos != null)
        {
            // foreach (GameObject o in client.objectsInScene)
            for (int i = 0; i < client.objectsInScene.Count; i++)
            {
                if(otherPayersPos[i] == null) continue;
                
                if (client.objectsInScene[i].GetComponent<OnlinePlayerController>().id == otherPayersPos[i].playerID)
                {
                    Vector3 tempPos = new Vector3(otherPayersPos[i].pos._posX, otherPayersPos[i].pos._posY,
                        otherPayersPos[i].pos._posZ);
                    // while ((tempPos - o.transform.position).magnitude > 0.3f)
                    {
                        client.objectsInScene[i].transform.position = Vector3.LerpUnclamped(client.objectsInScene[i].transform.position,
                            tempPos, client.networkTimer.MinTimeBetweenTicks);
                    }
                }
            }
        }
        
        // if (updateOtherPos)
        // {
        //     for (int i = 0; i < client.objectsInScene.Count; i++)
        //     {
        //         if(positionInfo[i] == null) continue;
        //
        //         while (positionInfo[i].transformData.Count > 2)
        //         {
        //             for (int j = 0; j < 2; j++)
        //             {
        //                 prevTransforms[j] = positionInfo[i].transformData.Dequeue();
        //                 prevlocations[j] = new Vector3(prevTransforms[j].pos._posX, prevTransforms[j].pos._posY,
        //                     prevTransforms[j].pos._posZ);
        //             }
        //
        //             
        //             
        //             // if (transformData != null)
        //             // {
        //             //     if (client.objectsInScene[i].GetComponent<OnlinePlayerController>().id == transformData.playerID)
        //             //     {
        //             //         Vector3 tempPos = new Vector3(transformData.pos._posX, transformData.pos._posY,
        //             //             transformData.pos._posZ);
        //             //         // while ((tempPos - o.transform.position).magnitude > 0.3f)
        //             //         {
        //             //             client.objectsInScene[i].transform.position = Vector3.LerpUnclamped(client.objectsInScene[i].transform.position,
        //             //                 tempPos, client.networkTimer.MinTimeBetweenTicks);
        //             //         }
        //             //     }
        //             // }
        //         }
        //         
        //         foreach (TransformData prevTransform in prevTransforms)
        //         {
        //             if (prevTransform == null)
        //             {
        //                 return;
        //             }
        //         }
        //             
        //         Vector3 speed = (prevlocations[1] - prevlocations[0]) / (prevTransforms[1].tick - prevTransforms[0].tick);
        //         speed.Normalize();
        //                  
        //         while (prevTransforms[1].tick < client.networkTimer.CurrentTick)
        //         {
        //             client.objectsInScene[i].transform.position = Vector3.LerpUnclamped(client.objectsInScene[i].transform.position,
        //                 prevlocations[1] + speed, client.networkTimer.MinTimeBetweenTicks);
        //             // client.objectsInScene[i].transform.position += speed * (3 * client.networkTimer.MinTimeBetweenTicks);
        //
        //             prevTransforms[1].tick++;
        //         }
        //
        //     }
        // }
    }

    // Update is called once per frame
    void Update()
    {
        while (client.receivedDataToHandle.Count > 0)
        {
            DataToHandle data = client.receivedDataToHandle.Dequeue();
            
            switch (data.dataUpdateType)
            {
                case DataUpdateType.Ready:

                    ReadyStatus readyStatus = (ReadyStatus) data.deserializedData;
                    foreach (Player player in client.playersConnected)
                    {
                        if (player.PlayerID == readyStatus.playerID)
                        {
                            player.ready = readyStatus.ready;
                        }
                        print($"{player.PlayerID} {player.ready}");
                    }
                
                    break;
                
                case DataUpdateType.JoiningDataReply:
    
                    JoinLeaveData joinLeaveData = (JoinLeaveData) data.deserializedData;

                    if (joinLeaveData.errorCode == -1)
                    {
                        print("SERVER HAS MAX PLAYERS");
                        break;
                    }

                    if (client.tickSynced)
                    {
                        // print("SETTING");
                        // float a = joinLeaveData.tick + client.pingTimer*15;

                        // if (client.pingTimer > 1.0f/30) client.pingTimer /= 2;

                        if (client.pingTimer >= 1.0f/30)
                        {
                            // float ping = client.pingTimer / 2;
                            // float setTimer = (ping + joinLeaveData.subTick) % 1.0f/30;
                            int a = (int)Mathf.Floor((client.pingTimer/4 + joinLeaveData.subTick) / (1.0f/30));
                            client.networkTimer = new NetworkTimer(30, joinLeaveData.tick + a +1, 
                                client.pingTimer / 2 + joinLeaveData.subTick);
                        }
                        else
                        {
                            int a = (int)Mathf.Floor((client.pingTimer + joinLeaveData.subTick) / (1.0f/30));
                            client.networkTimer = new NetworkTimer(30, joinLeaveData.tick + a, 
                                joinLeaveData.subTick + client.pingTimer/2);
                        }
                    }
                    
                    // New player joined Server
                    if (client.playersConnected.Count <= joinLeaveData.playersConnected.Length)
                    {
                        for (int i = 0; i < joinLeaveData.playersConnected.Length; i++)
                        {
                            if (client.playersConnected.Count == 0)
                            {
                                print(joinLeaveData.playerIDs[i]);
                                client.localPlayer.PlayerID = joinLeaveData.playerIDs[joinLeaveData.playersConnected.Length - 1];
                            }
                            
                            if (i < client.playersConnected.Count)
                            {
                                client.playersConnected[i].playerName = joinLeaveData.playersConnected[i];
                                client.playersConnected[i].PlayerID = joinLeaveData.playerIDs[i];
                                client.playersConnected[i].ready = joinLeaveData.ready[i];
                            }
                            else
                            {
                                Player player = new Player
                                {
                                    playerName = joinLeaveData.playersConnected[i],
                                    PlayerID = joinLeaveData.playerIDs[i],
                                    ready = joinLeaveData.ready[i]
                                };

                                // print(player.PlayerID);
                                client.playersConnected.Add(player);
                                Debug.Log($"{player.playerName} CONNECTED!");
                                client.clientStatus = ClientNew.ClientStatus.Connected;
                            }
                        }
                    }
                    // Player left Server
                    else if (client.playersConnected.Count > joinLeaveData.playersConnected.Length)
                    {
                        foreach (Player player in client.playersConnected)
                        {
                            bool stillConnected = false;
                            foreach (int playerID in joinLeaveData.playerIDs)
                            {
                                if (player.PlayerID == playerID)
                                {
                                    stillConnected = true;
                                    break;
                                }
                            }

                            if (!stillConnected)
                            {
                                foreach (GameObject obj in client.objectsInScene)
                                {
                                    if (obj.GetComponent<OnlinePlayerController>().id == player.PlayerID)
                                    {
                                        Destroy(obj);
                                        client.objectsInScene.Remove(obj);
                                        break;
                                    }
                                }
                                client.playersConnected.Remove(player);
                                break;
                            }
                        }
                    }

                    break;
                
                case DataUpdateType.OwnedObject:
                
                    client.gameState.ChangeState(GameState.gameStateEnum.Game);
                    
                    OwnedObject ownedObject = (OwnedObject) data.deserializedData;
                    print(ownedObject.objectType);
                    
                    switch ((ObjectType)ownedObject.objectType)
                    {
                        case ObjectType.Player:

                            bool exists = false;
                            foreach (OwnedObject localPlayerPlayerOwnedObject in client.localPlayer.playerOwnedObjects)
                            {
                                if (localPlayerPlayerOwnedObject.playerID == ownedObject.playerID)
                                {
                                    exists = true;
                                }
                            }

                            if (!exists)
                            {
                                Vector3 tempPos = new Vector3(ownedObject.startPos._posX, ownedObject.startPos._posY,
                                    ownedObject.startPos._posZ);
                                
                                GameObject temp = Instantiate(playerPrefab, tempPos, Quaternion.identity);
                                client.objectsInScene.Add(temp);

                                        
                                temp.GetComponent<OnlinePlayerController>().id = ownedObject.playerID;
                                
                                if (client.localPlayer.PlayerID == ownedObject.playerID)
                                {
                                    client.localPlayer.playerOwnedObjects.Add(ownedObject);
                                }
                            }
                        
                            break;
                        
                        // case ObjectType.Coin:
                        //
                        //     foreach (OwnedObject coin in coinList)
                        //     {
                        //         Vector3 coinPos = new Vector3(coin.startPos._posX, coin.startPos._posY,
                        //             coin.startPos._posZ);
                        //             
                        //         Instantiate(coinPrefab, coinPos, Quaternion.identity); 
                        //     }
                        //     
                        //     break;
                    
                        // case ObjectType.Bullet
                    }
                
                    break;
                
                case DataUpdateType.StartGame:
                    
                    StartGameData startGameData = (StartGameData) data.deserializedData;
                    tickToStartGame = startGameData.tickAtSceneLoad;
                    gameLength = startGameData.gameLengthInSeconds;
                    
                    break;
                
                case DataUpdateType.Transform:

                    TransformData transformData = (TransformData) data.deserializedData;
                    string content = $"{transformData.pos._posX}, {transformData.pos._posY}, {transformData.pos._posZ}";

                    // foreach (GameObject o in client.objectsInScene)
                    for (int i = 0; i < client.objectsInScene.Count; i++)
                    {
                        if (client.objectsInScene[i].GetComponent<OnlinePlayerController>().id == transformData.playerID)
                        {
                            if (!client.objectsInScene[i].GetComponent<OnlinePlayerController>().ShouldReconcile(transformData))
                            {
                                updateOtherPos = true;

                                if (otherPayersPos == null)
                                {
                                    otherPayersPos = new TransformData[client.objectsInScene.Count];
                                }
                                
                                otherPayersPos[i] = transformData;
                                
                                
                                // if (positionInfo.Length == 0)
                                // {
                                //     positionInfo = new PositionInfo[client.objectsInScene.Count];
                                //     positionInfo[i] = new PositionInfo();
                                // }
                                //
                                // positionInfo[i].transformData.Enqueue(transformData);
                                
                                // Vector3 tempPos = new Vector3(transformData.pos._posX, transformData.pos._posY,
                                //     transformData.pos._posZ);
                                //
                                // while ((tempPos - o.transform.position).magnitude > 0.3f)
                                // {
                                //     o.transform.position = Vector3.Lerp(o.transform.position,
                                //         tempPos, client.networkTimer.MinTimeBetweenTicks);
                                // }
                            }
                            else
                            {
                                // updateOtherPos = true;
                            }
                            // o.transform.position = tempPos;
                        }
                    }
                    
                    foreach (Player player in client.playersConnected)
                    {
                        if (player.PlayerID == transformData.playerID)
                        {
                            Debug.Log($"{player.playerName} : {content}");
                        }
                    }
                
                    break;
                
                case DataUpdateType.PointsData:
                    
                    PointsData pointsData = (PointsData) data.deserializedData;
                    foreach (Player player in client.playersConnected)
                    {
                        if (player.PlayerID == pointsData.playerID)
                        {
                            player.score = pointsData.score;
                        }
                    }

                    foreach (Coin coin in CoinSpawner.CoinsList)
                    {
                        if (coin.id == pointsData.coinId)
                        {
                            coin.coinObject.SetActive(false);
                        }
                    }
                    
                    break;
            }
        }

        if (client.networkTimer != null && client.networkTimer.CurrentTick == tickToStartGame)
        {
            StartInGameTimer(gameLength);
            client.localPlayer.canControl = true;
            tickToStartGame = -1;
        }
        
        // if (player.receivedData)
        {
            // switch (client.ServerPlayer.dataUpdateType)
            // {
            //     case DataUpdateType.OwnedObject:
            //         
            //         OwnedObject ownedObject = (OwnedObject)client.ServerPlayer.ByteArrayToObject(client.localPlayer.dataRecd);
            //
            //         switch ((ObjectType)ownedObject.objectType)
            //         {
            //             case ObjectType.Player:
            //
            //                 Vector3 tempPos = new Vector3(ownedObject.startPos._posX, ownedObject.startPos._posY,
            //                     ownedObject.startPos._posZ);
            //                 
            //                 GameObject temp = Instantiate(playerPrefab, tempPos, Quaternion.identity);
            //                 client.objectsInScene.Add(temp);
            //
            //                         
            //                 temp.GetComponent<OnlinePlayerController>().id = ownedObject.playerID;
            //             
            //                 break;
            //         
            //             // case ObjectType.Bullet
            //         }
            //         
            //         break;
            //     
            //     case DataUpdateType.Transform:
            //         
            //         TransformData transformData = (TransformData)client.ServerPlayer.ByteArrayToObject(client.localPlayer.dataRecd);
            //
            //         foreach (GameObject o in client.objectsInScene)
            //         {
            //             if (o.GetComponent<OnlinePlayerController>().id == transformData.playerID)
            //             {
            //                 Vector3 tempPos = new Vector3(transformData.pos._posX, transformData.pos._posY,
            //                     transformData.pos._posZ);
            //                 o.transform.position = tempPos;
            //             }
            //         }
            //         
            //         break;
            // }
            //
            // client.localPlayer.dataRecd = new byte[4];
            // player.receivedData = false;
        }
    }
}
