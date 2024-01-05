using System;
using System.Collections.Generic;
using System.Threading;
using UnityEngine;

public class ExternServer : MonoBehaviour
{
    private Thread thread;

    public static List<Player> ConnectedPlayers = new();
    public List<Player> CONNECTIONS = new();
    // public static HandleData HandleData;

    private void Awake()
    {
        DontDestroyOnLoad(gameObject);
        // HandleData = GetComponent<HandleData>();
    }

    // Start is called before the first frame update
    void Start()
    {
        thread = new Thread(AsynchronousSocketListener.Main);
        thread.Start();
    }

    private void Update()
    {
        CONNECTIONS = ConnectedPlayers;

        // Enable Start option
        if (CONNECTIONS.Count == 2)
        {
            // print("START GAME");
        }
        
        // print(CONNECTIONS[0].data.pos._posX);
    }

    private void OnDestroy()
    {
        AsynchronousSocketListener.SD();
    }

    private void OnApplicationQuit()
    {
        AsynchronousSocketListener.SD();
        // thread.Abort();
    }
}