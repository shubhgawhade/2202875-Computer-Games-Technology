using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using UnityEngine;

public class OnlinePlayerController : MonoBehaviour
{
    public int id;
    public bool isOwner;
    
    // private float moveForce = 30;
    private float moveSpeed = 4;
    private Vector3 lastMoveSent;

    private InputData inputData;

    private Queue<InputData> sendInputQueue = new Queue<InputData>();
    
    public InputData[] inputBuffer = new InputData[2048];
    public Vector3[] positionBuffer = new Vector3[2048];

    public bool shouldReconcile;

    [SerializeField] private GameObject canvas;

    private void Awake()
    {
        DontDestroyOnLoad(gameObject);
    }

    // Start is called before the first frame update
    void Start()
    {
        if (ClientGameManager.client != null && ClientGameManager.client.localPlayer.PlayerID == id)
        {
            isOwner = true;
            
            ClientGameManager.client.localPlayer.dataUpdateType = DataUpdateType.Input;
            inputData = (InputData)ClientGameManager.client.localPlayer.returnDataStruct;
            inputData.playerID = id;
        }
        
        if (isOwner)
        {
            transform.GetChild(0).gameObject.SetActive(true);
        
            ClientGameManager.client.localPlayer.dataUpdateType = DataUpdateType.StartGame;
            StartGameData startGameData = (StartGameData)ClientGameManager.client.localPlayer.returnDataStruct;
            startGameData.playerID = id;
            startGameData.tickAtSceneLoad = ClientGameManager.client.networkTimer.CurrentTick;
        
            ClientGameManager.client.localPlayer.dataToSend =
                ClientGameManager.client.localPlayer.ObjectToByteArray(startGameData);
            SendData.Send(ClientGameManager.client.localPlayer, ClientGameManager.client.localPlayer.dataToSend,
                SendData.SendType.ReplyOne);
        }
    }

    private void FixedUpdate()
    {
        if(!isOwner) return;
        
        if (ClientGameManager.client.networkTimer != null)
        {
            // networkTimer.Update(Time.deltaTime);
            
            while (ClientGameManager.client.networkTimer.ShouldTick() && ClientGameManager.client.localPlayer.canControl) 
            {
                // print(ClientGameManager.client.networkTimer.CurrentTick + $" - {DateTime.UtcNow.Second}:{DateTime.UtcNow.Millisecond}");
                
                Inputs();

                if (shouldReconcile)
                {
                    Reconcile();
                }
            }
        }
    }

    public void Inputs()
    {
        float horizontalInput = Input.GetAxis("Horizontal");
        float verticalInput = Input.GetAxis("Vertical");
        
        if (horizontalInput != 0 || verticalInput != 0)
        {
            if (horizontalInput > 0)
            {
                if(inputData.right != 1)
                {
                    inputData.right = 1;

                    // sendInputQueue.Enqueue(inputData);
                }
            }
            else if (horizontalInput < 0)
            {
                if(inputData.right != -1)
                {    
                    inputData.right = -1;
                    
                    // sendInputQueue.Enqueue(inputData);
                }
            }
            
            
            if (verticalInput > 0)
            {
                if(inputData.forward != 1)
                {
                    inputData.forward = 1;

                    // sendInputQueue.Enqueue(inputData);
                }
            }
            else if (verticalInput < 0)
            {
                if(inputData.forward != -1)
                {    
                    inputData.forward = -1;
                    
                    // sendInputQueue.Enqueue(inputData);
                }
            }
        }
        
        if (horizontalInput == 0 || verticalInput == 0)
        {
            if (horizontalInput == 0)
            {
                if(inputData.right != 0)
                {
                    inputData.right = 0;
                    
                    // sendInputQueue.Enqueue(inputData);
                }
            }

            if (verticalInput == 0)
            {
                if (inputData.forward != 0)
                {
                    inputData.forward = 0;
                    
                    // sendInputQueue.Enqueue(inputData);
                }
            }
        }
        
        positionBuffer[ClientGameManager.client.networkTimer.CurrentTick] = transform.position;
        
        Vector3 moveDir = new Vector3(inputData.right, 0, inputData.forward);

        if (moveDir != lastMoveSent)
        {
            inputData.tick = ClientGameManager.client.networkTimer.CurrentTick;
            sendInputQueue.Enqueue(inputData);
            lastMoveSent = moveDir;
        }
        
        Move(moveDir, ClientGameManager.client.networkTimer.MinTimeBetweenTicks);
    }
    
    // Update is called once per frame
    async void Update()
    {
        if(!isOwner) return;

        if (ClientGameManager.client.gameState.gameState != GameState.gameStateEnum.Game)
        {
            foreach (GameObject o in ClientGameManager.client.objectsInScene)
            {
                Destroy(o);
                Destroy(ClientGameManager.client.gameObject);
            }
        }

        await HandeSendInputQueue();
        
        // RAW POSITION UPDATE (OLD TEST)
        // float horizontalAxis = Input.GetAxis("Horizontal") * Time.deltaTime;
        // if (horizontalAxis != 0)
        // {
        //     transform.position += new Vector3(horizontalAxis, 0, 0);
        //
        //     if (timeSinceLastSend > 0.5f)
        //     {
        //         ClientGameManager.client.localPlayer.dataUpdateType = DataUpdateType.Transform;
        //         
        //         TransformData transformData = (TransformData)ClientGameManager.client.localPlayer.returnDataStruct;
        //         transformData.pos = new Pos
        //         {
        //             _posX = transform.position.x,
        //             _posY = transform.position.y,
        //             _posZ = transform.position.z
        //         };
        //
        //         
        //         ClientGameManager.client.localPlayer.dataToSend = ClientGameManager.client.localPlayer.ObjectToByteArray(transformData); 
        //         print($"{ClientGameManager.client.localPlayer.dataToSend.Length} {ClientGameManager.client.localPlayer.dataUpdateType}");
        //         // print(bytes.Length);
        //
        //         // Send(sizeOfMsg, bytes);
        //         SendData.Send(ClientGameManager.client.localPlayer, ClientGameManager.client.localPlayer.dataToSend, SendData.SendType.ReplyOne);
        //         // _clientSocket.BeginSend()
        //         timeSinceLastSend = 0;
        //     }
        // }
        //
        // timeSinceLastSend += Time.deltaTime;
    }

    private async Task HandeSendInputQueue()
    {
        while (sendInputQueue.Count > 0)
        {
            InputData inputs = sendInputQueue.Dequeue();
            inputBuffer[ClientGameManager.client.networkTimer.CurrentTick] = inputs;

            ClientGameManager.client.localPlayer.dataUpdateType = DataUpdateType.Input;
            // inputData.tick = ClientGameManager.client.networkTimer.CurrentTick;
            ClientGameManager.client.localPlayer.dataToSend =
                ClientGameManager.client.localPlayer.ObjectToByteArray(inputs);
            await SendData.Send(ClientGameManager.client.localPlayer, ClientGameManager.client.localPlayer.dataToSend,
                SendData.SendType.ReplyOne);
        }
    }

    public void Move(Vector3 moveDir, float timeBetweenTicks)
    {
        // print(horizontalInput);
        
        moveDir.Normalize();
        transform.position += moveDir * (moveSpeed * timeBetweenTicks);
        // rb.AddForce(moveDir * (moveForce * timeBetweenTIcks), ForceMode.VelocityChange);
    }

    private Vector3 tempPos;
    private Vector3 predictedPlayerPos;
    public bool ShouldReconcile(TransformData transformData)
    {
        if (!isOwner)
        {
            return false;
        }

        int dataTick = transformData.tick;
        tempPos = new Vector3(transformData.pos._posX, transformData.pos._posY,
            transformData.pos._posZ);
                
        // Simulate from the new position to current tick and then check if reconciliation needed
        while (dataTick <= ClientGameManager.client.networkTimer.CurrentTick)
        {
            Vector3 moveDir = new Vector3(inputBuffer[dataTick].right, 0, inputBuffer[dataTick].forward);
            moveDir.Normalize();
            
            if (tempPos != Vector3.zero)
            {
                predictedPlayerPos = tempPos +
                                     moveDir * (moveSpeed * ClientGameManager.client.networkTimer.MinTimeBetweenTicks);
            }
            else
            {
                predictedPlayerPos += moveDir * (moveSpeed * ClientGameManager.client.networkTimer.MinTimeBetweenTicks);
            }
            
            // predictedPlayerPos = tempPos +
            //                moveDir * (moveSpeed * ClientGameManager.client.networkTimer.MinTimeBetweenTicks);

            if (dataTick == ClientGameManager.client.networkTimer.CurrentTick && 
                (positionBuffer[ClientGameManager.client.networkTimer.CurrentTick] - predictedPlayerPos).magnitude > 0.5f 
                )//&& ClientGameManager.client.networkTimer.CurrentTick%2==0)
            {
                shouldReconcile = true;
            }
            else
            {
                shouldReconcile = false;
            }

            tempPos = Vector3.zero;
            // tempPos = predictedPlayerPos;
            dataTick++;
        }

        return true;
    }
    
    private void Reconcile()
    {
        transform.position = Vector3.LerpUnclamped(transform.position,
                predictedPlayerPos, ClientGameManager.client.networkTimer.MinTimeBetweenTicks);
    }
}