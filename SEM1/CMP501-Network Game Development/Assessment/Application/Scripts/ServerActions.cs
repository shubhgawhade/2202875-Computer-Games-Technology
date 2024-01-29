using System;
using UnityEngine;

public class ServerActions : MonoBehaviour
{
    private OnlinePlayerController opc;
    
    private void Awake()
    {
        opc = GetComponent<OnlinePlayerController>();
    }

    private void Start()
    {
        
    }

    private void OnTriggerEnter(Collider other)
    {
        if (other.CompareTag("Coin"))
        {
            other.gameObject.SetActive(false);
            print($"{opc.id} COLLECTED A COIN");

            foreach (Player player in ExternServer.ConnectedPlayers)
            {
                if (player.PlayerID == opc.id)
                {
                    player.score += 1;
                    
                    player.dataUpdateType = DataUpdateType.PointsData;
                    PointsData pointsData = (PointsData) player.returnDataStruct;
                    pointsData.playerID = player.PlayerID;
                    pointsData.score = player.score;

                    foreach (Coin coin in CoinSpawner.CoinsList)
                    {
                        if (other.gameObject == coin.coinObject)
                        {
                            pointsData.coinId = coin.id;
                        }
                    }

                    player.dataToSend = player.ObjectToByteArray(pointsData);
                    SendData.Send(player, player.dataToSend, SendData.SendType.ReplyAll);
                }
            }
        }
    }
}