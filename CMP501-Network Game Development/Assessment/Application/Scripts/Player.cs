using System;
using System.Collections.Generic;
using System.IO;
using System.Net.Sockets;
using System.Runtime.Serialization.Formatters.Binary;
using UnityEngine;
using UnityEngine.Serialization;

/// <summary>
/// C1: Send Connection Request with data
/// S: Read player socket and add to connected clients list
///     store socket in player object
///     Check for size of data
///     Read the data with size
///     Update player Object with Name
///     Player	                Server
//      Request Join    ->  Creates Player
// 	                    <- Players Connected to Lobby
//      Last Player Ready -> Start Game
//      Creates local player <- Player Creation info
/// </summary>

[Serializable]
public enum DataUpdateType
{
    Joining,
    JoiningDataReply,
    Ready,
    StartGame,
    OwnedObject,
    Input,
    Transform,
    PointsData,
    Heath
}

[Serializable]
public class Player
{
    // Client  socket.
    public Socket workSocket = null;

    public string playerName;
    public int PlayerID;

    public bool ready;
    public bool canControl;

    public int score;

    public List<OwnedObject> playerOwnedObjects = new List<OwnedObject>();
    
    // Receive and Send buffers
    public byte[] dataRecd = new byte[sizeof(int)];
    public byte[] dataToSend = new byte[sizeof(int)];
    
    public DataUpdateType dataUpdateType;
    
    // DataType must be created before sending
    public object returnDataStruct => ReturnDataStruct();
    
    private object ReturnDataStruct()
    {
        switch (dataUpdateType)
        {
            case DataUpdateType.Joining:

                JoiningData jd = new JoiningData();
                return jd;
                
                break;
            
            case DataUpdateType.JoiningDataReply:

                JoinLeaveData jdr = new JoinLeaveData();
                return jdr;
                
                break;
            
            case DataUpdateType.Ready:

                ReadyStatus rs = new ReadyStatus();
                return rs;
                
                break;
            
            case DataUpdateType.OwnedObject:

                OwnedObject oo = new OwnedObject();
                return oo;
                
                break;
            
            case DataUpdateType.StartGame:

                StartGameData sgd = new StartGameData();
                return sgd;
                    
                break;
            
            case DataUpdateType.Input:

                InputData id = new InputData();
                return id;
                
                break;
            
            case DataUpdateType.Transform:

                TransformData td = new TransformData();
                return td;
                
                break;
            
            case DataUpdateType.PointsData:

                PointsData pd = new PointsData();
                return pd;
                
                break;
        }

        return null;
    }

    // public byte[] DataToSend => ObjectToByteArray(returnDataStruct);
    public byte[] ObjectToByteArray(object data)
    {
        try
        {
            // Create new BinaryFormatter
            BinaryFormatter binaryFormatter = new BinaryFormatter(); 
            
            // Create target memory stream
            using MemoryStream memoryStream = new MemoryStream();             
            
            // Serialize object to stream
            binaryFormatter.Serialize(memoryStream, data);       
            
            // Return stream as byte array
            return memoryStream.ToArray();                              
        }
        catch (Exception e)
        {
            Debug.Log(e);
            throw;
        }
    }

    // public object DataReceived => ByteArrayToObject(DataToSend);
    public object ByteArrayToObject(byte[] data)
    {
        try
        {
            // Create new BinaryFormatter
            BinaryFormatter binaryFormatter = new BinaryFormatter();
            
            // Convert buffer to memorystream
            using MemoryStream memoryStream = new MemoryStream(data);
            memoryStream.Seek(0, SeekOrigin.Begin);
            
            // Deserialize stream to an object
            switch (dataUpdateType)
            {
                case DataUpdateType.Joining:
                    
                    JoiningData joiningData = (JoiningData) binaryFormatter.Deserialize(memoryStream);
                    return joiningData;
                    
                    break;
                
                case DataUpdateType.JoiningDataReply:

                    JoinLeaveData joinLeaveData = (JoinLeaveData) binaryFormatter.Deserialize(memoryStream);
                    return joinLeaveData;
                    
                    break;
                
                case DataUpdateType.Ready:

                    ReadyStatus readyStatus = (ReadyStatus) binaryFormatter.Deserialize(memoryStream);
                    return readyStatus;
                    
                    break;
                
                case DataUpdateType.OwnedObject:

                    OwnedObject ownedObject = (OwnedObject) binaryFormatter.Deserialize(memoryStream);
                    return ownedObject;
                    
                    break;
                
                case DataUpdateType.StartGame:
                    
                    StartGameData startGameData = (StartGameData) binaryFormatter.Deserialize(memoryStream);
                    return startGameData;
                    
                    break;
                
                case DataUpdateType.Input:
                    
                    InputData inputData = (InputData) binaryFormatter.Deserialize(memoryStream);
                    return inputData;
                    
                    break;
                
                case DataUpdateType.Transform:

                    TransformData transformData = (TransformData) binaryFormatter.Deserialize(memoryStream);
                    return transformData;
                    
                    break;
                
                case DataUpdateType.PointsData:
                    
                    PointsData pointsData = (PointsData) binaryFormatter.Deserialize(memoryStream);
                    return pointsData;
                    
                    break;
            }
        }
        catch (Exception e)
        {
            Debug.LogWarning(e + $"{dataUpdateType}");
            // throw;
        }

        return null;
    }
}

[Serializable]
public class Tick
{
    public int tick;
    public float subTick;
}

// [Serializable]
public class DataToHandle
{
    public Player player = null;
    public DataUpdateType dataUpdateType;
    public object deserializedData;

}

[Serializable]
public class InputData : PlayerID
{
    public int tick;
    public int forward;
    public int right;
    public bool jump;
}

[Serializable]
public class PlayerID
{
    public int playerID = -1;
}

[Serializable]
public class JoiningData : PlayerID
{
    public string playerName;
}

[Serializable]
public class JoinLeaveData : Tick
{
    public int errorCode;
    // CODES
    // -1 : MAX PLAYERS
    // 0 : DISCONNECTED NORMALLY
    // 1 : SERVER - PLAYER CLOSED CONNECTION
    // 2 : PLAYER - SERVER CLOSED CONNECTION
    
    
    public string[] playersConnected;
    public int[] playerIDs;
    public bool[] ready;
}

[Serializable]
public class ReadyStatus : PlayerID
{
    public bool ready;
}

[Serializable]
public enum ObjectType
{
    Player,
    Coin,
    Bullet
}

[Serializable]
public class OwnedObject : PlayerID
{
    public int objectType;
    public Pos startPos;
}

[Serializable]
public class StartGameData : PlayerID
{
    public int tickAtSceneLoad;
    public int gameLengthInSeconds;
}

[Serializable]
public class Pos
{
    public float _posX;
    public float _posY;
    public float _posZ;
}

[Serializable]
public class TransformData : PlayerID
{
    public int tick;
    public Pos pos;
    
    // [Serializable]
    // public class Rot
    // {
    //     public float _rotX;
    //     public float _rotY;
    //     public float _rotZ;
    // }
    //
    // public Rot rot;
}

[Serializable]
public class PointsData : PlayerID
{
    public int coinId;
    public int score;
}