using UnityEngine;
using System.Collections;

public class MiniGameLaunch : MonoBehaviour 
{
	public OTSprite gameLaunchSprite;
	public string gameToLaunch = "Pong";
	
	// Use this for initialization
	void Start () 
    {
		gameLaunchSprite.onInput = OnGameLaunch;
	}
	
	private void OnGameLaunch(OTObject view)
	{
        bool inputFlag = Constants.GetInputDown();
        if (inputFlag)
		{
			LaunchGame();
		}
	}
	
	private void LaunchGame()
	{
        OT.Print("MiniGameLaunch Button Clicked");

		if (gameToLaunch == "Pong")
		{
			// To be triggered to become dynamic for next step
			PongLogic logic = GameObject.Find (gameToLaunch).GetComponent<PongLogic>();

            logic.gameEndCallback = onGameEnd;
    		logic.StartGame();

            gameLaunchSprite.visible = false;
	    	Constants.stageLocked = true;
		}
	}
	
	// Update is called once per frame
    void onGameEnd ()
    {
        gameLaunchSprite.visible = true;
        Constants.stageLocked = false;
	}
}
