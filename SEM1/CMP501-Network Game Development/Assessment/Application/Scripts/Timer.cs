using UnityEngine;

public class Timer : MonoBehaviour
{
    public float gameDurationInSeconds = 60;
    private NetworkTimer gameTime;

    public float timeLeftInSeconds;

    public bool isRunning;
    
    void Awake()
    {
        // gameTime = new NetworkTimer(30,0,0, (int)(gameDurationInSeconds/(1.0f/30)));
        // timeLeftInSeconds = gameDurationInSeconds;
    }

    public void SetTimer(int gameDurationInSeconds)
    {
        this.gameDurationInSeconds = gameDurationInSeconds;
        gameTime = new NetworkTimer(30,0,0, (int)(gameDurationInSeconds/(1.0f/30)));
        timeLeftInSeconds = gameDurationInSeconds;
    }

    public void StartTimer()
    {
        isRunning = true;
    }
    
    private void Update()
    {
        if (Input.GetKeyDown(KeyCode.T)) isRunning = true;

        if (isRunning)
        {
            gameTime.Update(Time.deltaTime);
        }
    }
    
    void FixedUpdate()
    {
        while (gameTime.ShouldTick())
        {
            timeLeftInSeconds = (gameTime.MaxTick - gameTime.CurrentTick) * (1.0f / 30);
            // print((gameTime.MaxTick - gameTime.CurrentTick) * (1.0f/30));

            if (gameTime.MaxTick == gameTime.CurrentTick + 1)
            {
                print("GAME OVER");
                timeLeftInSeconds = 0;
                isRunning = false;
            }
        }
    }
}
