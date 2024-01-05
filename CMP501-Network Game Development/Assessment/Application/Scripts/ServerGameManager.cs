using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.Serialization;

public class ServerGameManager : MonoBehaviour
{
    [SerializeField] private GameObject playerPrefab;
    
    private HandleDataServer _handleDataServer;

    private GameObject serverCam;
    private Vector3 serverCamPos;
    
    [SerializeField] private GameObject lobbyCanvas;
    [SerializeField] private GameObject inGameUICanvas;
    [SerializeField] private GameObject joinLobbyCanvas;
    
    private void Awake()
    {
        serverCamPos = new Vector3(0, 2.25f, -10);
        _handleDataServer = GetComponent<HandleDataServer>();
    }

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        if (_handleDataServer.gameState.stateChanged)
        {
            _handleDataServer.gameState.stateChanged = false;
            
            switch (_handleDataServer.gameState.gameState)
            {
                case GameState.gameStateEnum.Lobby:
                    
                    if (SceneManager.GetActiveScene().buildIndex != 0)
                    {
                        Destroy(lobbyCanvas.transform.parent.gameObject);
                        Destroy(_handleDataServer.gameObject);
                        
                        SceneManager.LoadScene(0);
                    }
                    
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
                    
                    StartCoroutine(CreateServerCamera());
                    
                    break;
            }
        }
    }

    IEnumerator CreateServerCamera()
    {
        yield return new WaitForSeconds(0.001f);
        serverCam = new GameObject
        {
            name = "ServerCamera",
            transform = { position = serverCamPos}
        };
        serverCam.AddComponent<Camera>();
    }
}