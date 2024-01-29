using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using UnityEngine;
using UnityEngine.Serialization;

public class ClientNew : MonoBehaviour
{
    public bool tickSynced;
    public float pingTimer;
    public NetworkTimer networkTimer;
    
    private Socket _clientSocket;
    static readonly Int32 Port = 7777;
    private static readonly IPAddress ServerAddress = IPAddress.Parse("192.168.0.118");
    // private static readonly IPAddress ServerAddress = IPAddress.Parse("127.0.0.1");

    // public static Action<int> ClientStatusAction;
    public GameState gameState = new GameState();

    // is NULL till the client joins the server(client -> player)
    public Player localPlayer;

    public ClientStatus clientStatus = ClientStatus.Offline;
    
    public List<Player> playersConnected = new List<Player>();
    
    public List<GameObject> objectsInScene = new List<GameObject>();
    
    public Queue<DataToHandle> receivedDataToHandle = new Queue<DataToHandle>();
    
    public enum ClientStatus
    {
        Offline,
        Connecting,
        Connected,
        Disconnected
    }
    
    public Player OnlinePlayer;


    private void Awake()
    {
        DontDestroyOnLoad(this);

        ClientGameManager.client = this;
    }

    // Start is called before the first frame update
    void Start()
    {
        // Call from a connect button
        // ConnectToServer();
    }
    
    // private Byte[] bytes = new Byte[4];

    // private void FixedUpdate()
    // {
    //     // if (!tickSynced)
    //     // {
    //     //     pingTimer += Time.deltaTime;
    //     // }
    //     
    //     if (networkTimer != null)
    //     {
    //         // networkTimer.Update(Time.deltaTime);
    //         
    //         while (networkTimer.ShouldTick()) 
    //         {
    //             print(networkTimer.CurrentTick + $" - {DateTime.UtcNow.Second}:{DateTime.UtcNow.Millisecond}");
    //             
    //             
    //         }
    //     }
    // }

    // Update is called once per frame
    void Update()
    {
        if (networkTimer != null)
        {
            networkTimer.Update(Time.deltaTime);
            // networkTimer.ShouldTick();
            // print(networkTimer.CurrentTick + $" - {DateTime.UtcNow.Second}:{DateTime.UtcNow.Millisecond}");
        }
        
        if (!tickSynced)
        {
            pingTimer += Time.deltaTime;
        }
        
        // if (Input.GetKeyDown(KeyCode.R))
        // {
        //     localPlayer.dataUpdateType = DataUpdateType.Ready;
        //     ReadyStatus readyStatus = (ReadyStatus)localPlayer.returnDataStruct;
        //     readyStatus.ready = true;
        //     localPlayer.dataToSend = localPlayer.ObjectToByteArray(readyStatus);
        //     SendData.Send(localPlayer, localPlayer.dataToSend, SendData.SendType.ReplyOne);
        // }
        
        // float horizontalAxis = Input.GetAxis("Horizontal");
        // if (horizontalAxis != 0)
        // {
        //     transform.position += new Vector3(horizontalAxis, 0, 0);
        //
        //     if (timeSinceLastSend > 0.5f)
        //     {
        //         localPlayer.dataUpdateType = DataUpdateType.Transform;
        //         
        //         TransformData transformData = (TransformData)localPlayer.returnDataStruct;
        //         transformData.pos = new Pos
        //         {
        //             _posX = transform.position.x,
        //             _posY = transform.position.y,
        //             _posZ = transform.position.z
        //         };
        //
        //         
        //         localPlayer.dataToSend = localPlayer.ObjectToByteArray(transformData); 
        //         print($"{localPlayer.dataToSend.Length} {localPlayer.dataUpdateType}");
        //         // print(bytes.Length);
        //
        //         // Send(sizeOfMsg, bytes);
        //         SendData.Send(localPlayer, localPlayer.dataToSend, SendData.SendType.ReplyOne);
        //         // _clientSocket.BeginSend()
        //         timeSinceLastSend = 0;
        //     }
        // }
        //
        // timeSinceLastSend += Time.deltaTime;
    }
    
    public async void ConnectToServer(string serverAddress, string playerName)
    {
        // Create TCP Socket
        _clientSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp );
        
        // IPHostEntry ipHostInfo = Dns.Resolve(Dns.GetHostName());
        // IPAddress ipAddress = ipHostInfo.AddressList[1];
        // Debug.Log(IPAddress.Parse(ipAddress.ToString()));
        
        // Create the connection endpoint and connect
        try
        {
            IPEndPoint localEndPoint = new IPEndPoint(IPAddress.Parse(serverAddress), Port);
            await _clientSocket.ConnectAsync(localEndPoint);
            
            // Set the client status to be connected
            clientStatus = ClientStatus.Connecting;
            // ClientStatusAction((int)clientStatus);
            
            gameState.ChangeState(GameState.gameStateEnum.Lobby);

            
            Debug.Log("NEW PLAYER CREATED");
            localPlayer = new Player
            {
                dataUpdateType = DataUpdateType.Joining,
                // data = new Data
                // {
                //     playerName = Random.Range(0, 100).ToString()
                // }
            };
                
            // player.playerName = player.data.playerName;
            JoiningData joiningData = (JoiningData)localPlayer.returnDataStruct;
            // joiningData.playerName = Random.Range(0, 100).ToString();
            joiningData.playerName = playerName;
            // int randomID = Random.Range(1000, 9999);
            // joiningData.playerID = randomID;
            // gameObject.name = joiningData.playerName;
            localPlayer.playerName = joiningData.playerName;
            // ServerPlayer.playerName = joiningData.playerName;
            // playersConnected.Add(localPlayer);
            localPlayer.workSocket = _clientSocket;
            // print(joiningData.playerName);

            OnlinePlayer.dataToSend = localPlayer.ObjectToByteArray(joiningData);
            // bytes = ObjectToByteArray(player.data);
            byte[] sizeOfMsg = new byte[sizeof(int)];
            sizeOfMsg = System.Text.Encoding.ASCII.GetBytes(OnlinePlayer.dataToSend.Length.ToString() + (int)localPlayer.dataUpdateType);
            print($"{OnlinePlayer.dataToSend.Length} {localPlayer.dataUpdateType}");

            // Send(sizeOfMsg, bytes);
            await SendData.Send(localPlayer, OnlinePlayer.dataToSend, SendData.SendType.ReplyOne);
            
            // networkTimer = new NetworkTimer(30);
            
            OnlinePlayer.dataToSend = new byte[sizeof(int)];

            // try
            // {
            //     await _clientSocket.SendAsync(sizeOfMsg, 0);
            //     await _clientSocket.SendAsync(bytes, 0);
            // }
            // catch (Exception e)
            // {
            //     print(e);
            //     throw;
            // }

            // ServerPlayer = new Player();
            _clientSocket.BeginReceive( OnlinePlayer.dataRecd, 0, 4, 0, 
                new AsyncCallback(CheckForDataLength), OnlinePlayer);
            pingTimer = 0;
        }
        catch (Exception e)
        {
            // ClientStatusAction((int)clientStatus);
            
            gameState.ChangeState(GameState.gameStateEnum.JoinScreen);
            
            print(e);
            throw;
        }
    }

    private void CheckForDataLength(IAsyncResult ar)
    {
        Player serverPlayer = (Player) ar.AsyncState;
        
        int bytesRead = _clientSocket.EndReceive(ar);
        
        if (!tickSynced)
        {
            tickSynced = true;
        }
        
        if (bytesRead == 0)
        {
            Disconnect(2);
            
            // print("DISCONNECTED");
            // _clientSocket.Close();
            // QuitClient(handler, state);
            return;
        }

        Debug.Log($"{bytesRead} Bytes Received");
        byte[] sizeVal = new byte[bytesRead - 1];
        byte[] updateType = new byte[1];
        updateType[0] = serverPlayer.dataRecd[bytesRead - 1];
        // state.updateVal = ;
        serverPlayer.dataUpdateType = (DataUpdateType)int.Parse(System.Text.Encoding.ASCII.GetString(updateType));
        
        for (int i = 0; i < bytesRead - 1; i++)
        {
            sizeVal[i] = serverPlayer.dataRecd[i];
        }
        
        // if (bytesRead > 0)
        {
            int size = int.Parse(System.Text.Encoding.ASCII.GetString(sizeVal));
            serverPlayer.dataRecd = new byte[size];
            // print(size);
            
            try
            {
                _clientSocket.BeginReceive( serverPlayer.dataRecd, 0, size, 0, 
                    new AsyncCallback(ReceiveData), serverPlayer);
            }
            catch (Exception e)
            {
                print(e);
                throw;
            }
        }
        
    }
    
    private void ReceiveData(IAsyncResult ar)
    {
        Player serverPlayer = (Player) ar.AsyncState;
        print(serverPlayer.dataRecd.Length);
        
        int bytesRead = _clientSocket.EndReceive(ar);
        
        // print($"{bytesRead} converted");
        // print(player.data.playerName);

        // Read data from the client socket. 

        if (bytesRead == 0)
        {
            Disconnect(2);
            
            // print("DISCONNECTED");
            // _clientSocket.Close();
            // QuitClient(handler, state);
            return;
        }

        if (bytesRead > 0)
        { 
            // GetComponent<HandleData>().Handle(serverPlayer, serverPlayer.dataRecd);
            // HandleDataPlayer(serverPlayer, serverPlayer.dataRecd);
            // HandleData(serverPlayer, serverPlayer.dataRecd);

            object deserialized = (object) serverPlayer.ByteArrayToObject(serverPlayer.dataRecd);

            DataToHandle dataToHandle = new DataToHandle
            {
                dataUpdateType = serverPlayer.dataUpdateType,
                deserializedData = deserialized
            };
            
            receivedDataToHandle.Enqueue(dataToHandle);

            
            // AddToTaskList();
            
            // serverPlayer = new Player();
            // if (!receivedData)
            {
                serverPlayer.dataRecd = new byte[sizeof(int)];
            }
            
            _clientSocket.BeginReceive( serverPlayer.dataRecd, 0, 4, 0, 
                new AsyncCallback(CheckForDataLength), serverPlayer);
        }

        // player.dataRecd = new byte[sizeof(int)];
    }
    
    public void Disconnect(int errorCode)
    {
        // _clientSocket.Shutdown(SocketShutdown.Both);
        _clientSocket.Close();
        
        Debug.LogWarning($"ERROR CODE : {errorCode}");
        print("SERVER CLOSED CONNECTION");
        
        clientStatus = ClientStatus.Disconnected;
        playersConnected = new List<Player>();
        // ClientStatusAction((int) clientStatus);
        
        gameState.ChangeState(GameState.gameStateEnum.JoinScreen);
        
        DestroyImmediate(gameObject);
    }
    
    private void OnApplicationQuit()
    {
        if (_clientSocket is { Connected: true })
        {
            try
            {
                _clientSocket.Shutdown(SocketShutdown.Both);
            }
            finally
            {
                _clientSocket.Close();
            }
        }
    }
}