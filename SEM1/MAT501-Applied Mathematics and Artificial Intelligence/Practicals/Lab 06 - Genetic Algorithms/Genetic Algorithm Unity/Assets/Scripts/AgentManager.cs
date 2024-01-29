using UnityEngine;
using System.Collections.Generic;

public class AgentManager {

	public int gridHeight { get; private set; }
	public int gridWidth { get; private set; }

	public List<GameObject> agents;

	public AgentManager(int gridHeight, int gridWidth, GameObject agentPrefab)
	{
		this.gridWidth = gridWidth;
		this.gridHeight = gridHeight;

		agents = new List<GameObject>();

		float startX = 0;
		float startZ = 0;

		for(int i = 0; i < gridHeight; i++)
		{
			for(int j = 0; j < gridWidth; j++)
			{
				agents.Add(GameObject.Instantiate(agentPrefab, new Vector3(startX + j*2, 0, startZ + i*2), new Quaternion()));
			}
		}
	}

	public bool AreAgentsJumping()
	{
		foreach(GameObject agent in agents)
		{
			AgentJumper jumperScript = agent.GetComponent<AgentJumper>();
			if(jumperScript && jumperScript.isJumping)
			{
				return true;
			}
		}

		return false;
	}

	public void MakeAgentsJump()
	{
		foreach(GameObject agent in agents)
		{
			AgentJumper jumperScript = agent.GetComponent<AgentJumper>();
			if(jumperScript)
			{
				jumperScript.performJump();
			}
		}
	}

	public void UpdateAgentJumpingStrength(List<DNA<float>> genes)
	{
		for(int i = 0; i < genes.Count; i++)
		{
			float combinedJumpStrength = 0;
			DNA<float> dna = genes[i];
			GameObject agent = agents[i];

			for(int j = 0; j < dna.Genes.Length; j++)
			{
				combinedJumpStrength += dna.Genes[j];
			}

			AgentJumper script = agent.GetComponent<AgentJumper>();
			if(script)
			{
				script.jumpStrength = combinedJumpStrength;
			}
		}
	}

	public bool AllAgentsTouchedDeadZone()
	{
		foreach(GameObject agent in agents)
		{
			AgentJumper jumper = agent.GetComponent<AgentJumper>();
			if(jumper && (jumper.ticksInDeadZone == 0))
			{
				return false;
			}
		}
		return true;
	}
}