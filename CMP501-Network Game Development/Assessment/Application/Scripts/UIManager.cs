using System;
using TMPro;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.Serialization;
using UnityEngine.UI;
using Random = UnityEngine.Random;

public class UIManager : MonoBehaviour
{
    [SerializeField] private GameObject joinLobbyCanvas;
    [SerializeField] private GameObject lobbyCanvas;
    [SerializeField] private GameObject inGameUICanvas;
    
    [SerializeField] private ClientNew clientConnectionObject;
    [SerializeField] private Button connectButton;
    public bool clickedConnect;
    public float connectButtonTimeout;

    [SerializeField] private TMP_InputField nameInput;
    [SerializeField] private TMP_InputField ipAddressInput;

    [SerializeField] private TMP_Dropdown gameDurationDropdown;
    [SerializeField] private Button readyButton;
    
    [Header("Lobby Player Variables")]
    [SerializeField] 
    
    private string _playerName;
    private string _ipAddress;

    public bool readyStatus;

    // private bool clickedReady;
    // private float readyButtonTimeout;
    
    
    [Header("In Game UI")]
    
    [SerializeField] private TextMeshProUGUI timerText;
    [SerializeField] private GameObject scoreBoardPanel;
    [SerializeField] private TextMeshProUGUI[] playerNames;
    [SerializeField] private TextMeshProUGUI[] playerScores;
    

    private Timer timer;

    private void Awake()
    {
        DontDestroyOnLoad(gameObject);
        
        // nameInput.text = Random.Range(1000, 9999).ToString();
        _playerName = nameInput.text;
        _ipAddress = ipAddressInput.text;

        ProcessDataClient.StartInGameTimer += StartInGameTimer;
        // joinLobbyCanvas = transform.GetChild(0).gameObject;
    }

    private void Start()
    {
        // ClientNew.ClientStatusAction += UIStatus;
    }
    
    public void OnConnectButtonPressed()
    {
        if (_playerName == "")
        {
            _playerName = Random.Range(1000, 9999).ToString();
        }
        
        clickedConnect = true;
        connectButton.interactable = false;
        clientConnectionObject.ConnectToServer(_ipAddress, _playerName);
    }

    public void OnPlayerNameEditEnd()
    {
        _playerName = nameInput.text;
    }

    public void OnIpAddressEditEnd()
    {
        _ipAddress = ipAddressInput.text;
    }

    public void OnGameDurationSelected()
    {
        // SEND GAME TIME TO SERVER AND CHANGE FOR ALL PLAYERS
        // gameDurationDropdown.value
    }

    public void OnReadyButtonPressed()
    {
        // clickedReady = true;
        readyButton.interactable = false;
        
        clientConnectionObject.localPlayer.dataUpdateType = DataUpdateType.Ready;
        ReadyStatus readyStatus = (ReadyStatus) clientConnectionObject.localPlayer.returnDataStruct;
        readyStatus.ready = true;
        clientConnectionObject.localPlayer.dataToSend = clientConnectionObject.localPlayer.ObjectToByteArray(readyStatus);
        SendData.Send(clientConnectionObject.localPlayer, clientConnectionObject.localPlayer.dataToSend, SendData.SendType.ReplyOne);
    }
    
    private void Update()
    {
        ConnectButtonTimeout();
        
        // ReadyButtonTimeout();
        
        UpdateLobbyInfo();
        
        GameStateChanged();

        if (clientConnectionObject.gameState.gameState == GameState.gameStateEnum.Game)
        {
            UpdateIngameUI();
        }

        if (timer != null)
        {
            TimerDisplay();
        }
    }

    // private void ReadyButtonTimeout()
    // {
    //     if (clickedReady)
    //     {
    //         readyButtonTimeout += Time.deltaTime;
    //
    //         if (readyButtonTimeout > 5 && !clientConnectionObject.localPlayer.ready)
    //         {
    //             readyButton.interactable = true;
    //             readyButtonTimeout = 0;
    //             clickedReady = false;
    //         }
    //     }
    // }

    private void ConnectButtonTimeout()
    {
        if (clickedConnect)
        {
            connectButtonTimeout += Time.deltaTime;
            
            if (connectButtonTimeout > 3)
            {
                connectButton.interactable = true;
                connectButtonTimeout = 0;
                clickedConnect = false;
            }
        }
    }

    public void UpdateLobbyInfo()
    {
        
    }

    public void UpdateIngameUI()
    {
        if (playerNames.Length == 0 || playerScores.Length == 0)
        {
            playerNames = new TextMeshProUGUI[clientConnectionObject.playersConnected.Count];
            playerScores = new TextMeshProUGUI[clientConnectionObject.playersConnected.Count];
        }
        
        for (int i = 0; i < clientConnectionObject.playersConnected.Count; i++)
        {
            if (!scoreBoardPanel.transform.GetChild(i).gameObject.activeSelf)
            {
                scoreBoardPanel.transform.GetChild(i).gameObject.SetActive(true);
                playerNames[i] = scoreBoardPanel.transform.GetChild(i).GetChild(0).GetComponent<TextMeshProUGUI>();
                playerScores[i] = scoreBoardPanel.transform.GetChild(i).GetChild(1).GetComponent<TextMeshProUGUI>();

                playerNames[i].text = clientConnectionObject.playersConnected[i].playerName;
                playerScores[i].text = clientConnectionObject.playersConnected[i].score.ToString();
            }
            else
            {
                // SET SCORES
                if (playerScores[i].text != clientConnectionObject.playersConnected[i].score.ToString())
                {
                    playerScores[i].text = clientConnectionObject.playersConnected[i].score.ToString();
                }   
            }
        }
    }

    private void GameStateChanged()
    {
        if (clientConnectionObject.gameState.stateChanged)
        {
            clientConnectionObject.gameState.stateChanged = false;

            switch (clientConnectionObject.gameState.gameState)
            {
                case GameState.gameStateEnum.JoinScreen:

                    if (SceneManager.GetActiveScene().buildIndex != 0)
                    {
                        Destroy(gameObject);
                        SceneManager.LoadScene(0);
                    }
                    
                    joinLobbyCanvas.SetActive(true);
                    lobbyCanvas.SetActive(false);

                    break;

                case GameState.gameStateEnum.Lobby:

                    lobbyCanvas.SetActive(true);
                    joinLobbyCanvas.SetActive(false);

                    break;

                case GameState.gameStateEnum.Game:

                    if (SceneManager.GetActiveScene().buildIndex != 1)
                    {
                        SceneManager.LoadScene(1);
                    }
                    
                    lobbyCanvas.SetActive(false);
                    inGameUICanvas.SetActive(true);

                    break;
            }
        }
    }

    private void StartInGameTimer(int gameLength)
    {
        timer = gameObject.AddComponent<Timer>();
        timer.SetTimer(gameLength);
        timer.StartTimer();
    }
    
    private void TimerDisplay()
    {
        // minutes.ToString("#00") + ":" + seconds.ToString("#00.000s")
        int minutesLeft = (int)(timer.timeLeftInSeconds / 60);
        timerText.text = minutesLeft.ToString("#00") + ":" + (timer.timeLeftInSeconds % 60).ToString("#00.000s");
    }

    private void OnDestroy()
    {
        ProcessDataClient.StartInGameTimer -= StartInGameTimer;
    }

    private void OnApplicationQuit()
    {
        ProcessDataClient.StartInGameTimer -= StartInGameTimer;
        // ClientNew.ClientStatusAction -= UIStatus;
    }
}
