using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class Shakespeare : MonoBehaviour {

	[Header("Genetic Algorithm")]
	[SerializeField] string targetString = "To be, or not to be, that is the question.";
	[SerializeField] string validCharacters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ,.|!#$£%^&*()=+?@ 1234567890";
	[SerializeField] int populationSize = 200;
	[SerializeField] float mutationRate = 0.01f;


	[Header("Text Components")]
	[SerializeField] Text targetText;
	[SerializeField] Text bestText;
	[SerializeField] Text bestFitnessText;
	[SerializeField] Text generationText;
	[SerializeField] Text scrollDNA;

	[Header("Button Text")]
	[SerializeField] Text buttonText;

	private GeneticAglorithm<char> ga;
	private System.Random random;
	private bool running = false;

	// Use this for initialization
	void Start () {
		random = new System.Random();
		ga = new GeneticAglorithm<char>(populationSize, targetString.Length, random, GetRandomGene, FitnessFunction, mutationRate:mutationRate);
	}
	
	// Update is called once per frame
	void Update () {

		if(running)
		{
			ga.NewGeneration();

			updateText();

			if(ga.BestFitness == 1)
			{
				// Remove button through nasty evilness here
				GameObject button = GameObject.Find("StartButton");
				if(button)
				{
					button.SetActive(false);
				}

				this.enabled = false;
			}
		}
	}

	private char GetRandomGene()
	{
		int i = random.Next(validCharacters.Length);
		return validCharacters[i];
	}

	private float FitnessFunction(int index)
	{
		float score = 0;
		DNA<char> dna = ga.Population[index];

		for(int i = 0; i < dna.Genes.Length; i++)
		{
			if(dna.Genes[i] == targetString[i])
			{
				score += 1;
			}
		}

		// Normalise the score so it's between 0 and 1 (0 being 0% correct and 1 being 100% correct)
		score /= targetString.Length;

		return score;
	}

	private void updateText()
	{
		if(targetText)
		{
			targetText.text = targetString;
		}

		if(bestFitnessText)
		{
			bestFitnessText.text = ga.BestFitness.ToString();
		}

		if(bestText)
		{
			string input = "";
			for(int i = 0; i < ga.BestGenes.Length; i++)
			{
				input += ga.BestGenes[i];
			}
			bestText.text = input;

		}

		if(generationText)
		{
			generationText.text = ga.Generation.ToString();
		}

		if(scrollDNA)
		{
			string input = "";
			int columns = 0;
			
			foreach(DNA<char> dna in ga.Population)
			{
				for(int i = 0; i < dna.Genes.Length; i++)
				{
					input += dna.Genes[i];
				}

				if(columns < 5)
				{
					input += "\t";
					columns++;
				}
				else
				{
					input += "\n";
					columns = 0;
				}
			}

			scrollDNA.text = input;
		}
	}

	public void onButtonClick()
	{
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
}
