using System;
using System.Net.Sockets;
using System.Threading.Tasks;
using UnityEngine;

public class SendData : MonoBehaviour
{
    public enum SendType
    {
        ReplyOne,
        ReplyAllButSender,
        ReplyAll
    }

    public static async Task Send(Player state, byte[] dataRecd, SendType sendType, Socket socket = null)
    {
        byte[] sizeOfMsg = System.Text.Encoding.ASCII.GetBytes(dataRecd.Length.ToString() + (int)state.dataUpdateType);

        
        switch (sendType)
        {
            case SendType.ReplyOne:
                
                await InternalSend(state, sizeOfMsg, dataRecd);
                
                break;
            
            case SendType.ReplyAllButSender:
                
                foreach (Player player in ExternServer.ConnectedPlayers)
                {
                    if (player.playerName != state.playerName)
                    {
                        await InternalSend(player, sizeOfMsg, dataRecd);
                    }
                }
                
                break;
            
            case SendType.ReplyAll:
                    
                foreach (Player player in ExternServer.ConnectedPlayers)
                {
                    await InternalSend(player, sizeOfMsg, dataRecd);
                }
                    
                break;
        }
    }

    private static async Task InternalSend(Player player, byte[] sizeOfMsg, byte[] dataRecd)
    {
        try 
        {
            await player.workSocket.SendAsync(sizeOfMsg, 0);
            await player.workSocket.SendAsync(dataRecd, 0);
        }
        catch (Exception e)
        {
            Debug.Log(e);
            throw;
        }

        player.dataToSend = new byte[4];
        Debug.Log($"Sent {player.playerName} {System.Text.Encoding.ASCII.GetString(sizeOfMsg)} bytes");
    }
    
    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        // if (a != null)
        // {
        //     print(a);
        // }
    }
}
