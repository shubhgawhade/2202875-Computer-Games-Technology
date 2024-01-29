public class GameState
{
    public enum gameStateEnum
    {
        JoinScreen,
        Lobby,
        Game,
    }
    
    public bool stateChanged;
    public gameStateEnum gameState = gameStateEnum.Lobby;

    public void ChangeState(gameStateEnum gameStateEnum)
    {
        stateChanged = true;
        gameState = gameStateEnum;
    }
}