using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class Jumping : MonoBehaviour {

	[Header("Genetic Algorithm")]
	[SerializeField] int gridWidth = 7;
	[SerializeField] int gridHeight = 7;
	[SerializeField] float mutationRate = 0.01f;
	[SerializeField] int innerCount = 40;
	[SerializeField] float innerScale = 400.0f;
	[SerializeField] float timeScale = 1;

	[Header("Dead Zone")]
	[SerializeField] GameObject deadZone;

	[Header("Text Labels")]
	[SerializeField] Text jumpStatusText;
	[SerializeField] Text bestFitnessText;
	[SerializeField] Text bestJumpStrength;
	[SerializeField] Text numGenText;

	[Header("Button Text")]
	[SerializeField] Text buttonText;

	[Header("Agent Prefab")]
	[SerializeField] GameObject agentPrefab;

	private GeneticAglorithm<float> ga;		// Genetic Aglorithm with each Gene being a float
	private AgentManager agentManager;		// Manager for handling a jumping agents we need to make
	private System.Random random;			// Random for the RNG
	private bool running = false;			// Flag for if the GA is to run
	private float bestJump = 0;				// Store for the highest jump so far - Used for Mutation

	// Use this for initialization
	void Start () {
		// Create the Random class
		random = new System.Random();

		// Create our Agent Manager and give them the height and width of grid along with agent prefab
		agentManager = new AgentManager(gridHeight, gridWidth, agentPrefab);

		// Create genetic algorithm class
		ga = new GeneticAglorithm<float>(agentManager.agents.Count, innerCount, random, GetRandomGene, FitnessFunction, mutationRate:mutationRate);

		// Calculate DeadZone position and size
		deadZone.transform.position = new Vector3(gridWidth, deadZone.transform.position.y, gridHeight);
		deadZone.transform.localScale = new Vector3(gridWidth*2.5f, 1, gridHeight*2.5f);
	}
	
	// Update is called once per frame
	void Update () {

		// Update time scale based on Editor value - do this every frame so we capture changes instantly
		Time.timeScale = timeScale;
		UpdateText();

		if(running)
		{
			// If the agents are currently jumping, stop the script if the ALL enter the DeadZone
			if(agentManager.AreAgentsJumping())
			{
				if(agentManager.AllAgentsTouchedDeadZone())
				{
					this.enabled = false;
				}
			}
			// Make the agents jump if they are not jumping
			else
			{
				agentManager.UpdateAgentJumpingStrength(ga.Population);
				ga.NewGeneration();
				bestJump = ga.BestFitness;
				agentManager.MakeAgentsJump();
			}
		}
	}

	private float GetRandomGene()
	{
		// Generate a new jump value based on the current best jump (the higher the best jump, the greater the next random gene)
		float next = (float)random.NextDouble();
		float value = (innerScale + bestJump)/(float)innerCount;
		return (next*value);
	}

	private float FitnessFunction(int index)
	{
		// Go through each gene in a member of the population and make their fitness equal to their jump strength minus the game ticks
		// they spent in the DeadZone
		float score = 0;
		DNA<float> dna = ga.Population[index];
		for(int i = 0; i < dna.Genes.Length; i++)
		{
			score += dna.Genes[i] - agentManager.agents[index].GetComponent<AgentJumper>().ticksInDeadZone;
		}

		return score;
	}

	public void onButtonClick()
	{
		// Pause and Start the algorithm
		if(running)
		{
			running = false;
			if(buttonText)
			{
				buttonText.text = "Start";
			}
		}
		else
		{
			running = true;
			if(buttonText)
			{
				buttonText.text = "Pause";
			}
		}
	}

	private void UpdateText()
	{
		// If the script has been passed a valid Text object then update that text
		if(jumpStatusText)
		{
			jumpStatusText.text = agentManager.AreAgentsJumping().ToString();
		}

		if(bestFitnessText)
		{
			bestFitnessText.text = ga.BestFitness.ToString();
		}

		if(numGenText)
		{
			numGenText.text = ga.Generation.ToString();
		}

		if(bestJumpStrength)
		{
			// Combine all the jump strengths
			float totalStrength = 0;

			for(int i = 0; i < ga.BestGenes.Length; i++)
			{
				totalStrength *= ga.BestGenes[i];
			}

			bestJumpStrength.text = totalStrength.ToString();
		}
	}
}
