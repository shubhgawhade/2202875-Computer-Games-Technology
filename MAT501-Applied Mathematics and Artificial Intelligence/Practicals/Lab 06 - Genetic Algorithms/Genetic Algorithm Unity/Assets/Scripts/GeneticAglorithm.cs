using System;
using System.Collections.Generic;

public class GeneticAglorithm<T> {

	public List<DNA<T>> Population { get; private set; }
	public int Generation { get; private set; }
	public float BestFitness { get; private set; }
	public T[] BestGenes { get; private set; }


	public float MutationRate;

	private Random random;
	private float fitnessSum;

	public GeneticAglorithm(int populationSize, int dnaSize, Random random, Func<T> getRandomGene, Func<int, float> fitnessFunction, float mutationRate = 0.01f)
	{
		Generation = 1;
		MutationRate = mutationRate;
		Population = new List<DNA<T>>();
		this.random = random;

		BestGenes = new T[dnaSize];

		for(int i = 0; i < populationSize; i++)
		{
			Population.Add(new DNA<T>(dnaSize, random, getRandomGene, fitnessFunction, shouldInitGenes: true));
		}
	}

	public void NewGeneration()
	{
		if(Population.Count <= 0) {
			return;
		}

		CalculateFitness();

		List<DNA<T>> newPopulation = new List<DNA<T>>();

		for(int i = 0; i < Population.Count; i++)
		{
			DNA<T> parent1 = ChooseParent();
			DNA<T> parent2 = ChooseParent();

			DNA<T> child = parent1.Crossover(parent2);

			child.Mutate(MutationRate);

			newPopulation.Add(child);
		}

		Population = newPopulation;

		Generation++;
	}

	public void CalculateFitness()
	{
		fitnessSum = 0;

		DNA<T> best = Population[0];

		for(int i = 0; i < Population.Count; i++)
		{
			fitnessSum += Population[i].CalculateFitness(i);

			if(Population[i].Fitness > best.Fitness)
			{
				best = Population[i];
			}
		}

		BestFitness = best.Fitness;
		best.Genes.CopyTo(BestGenes, 0);
	}

	private DNA<T> ChooseParent()
	{
		double randomNumber = random.NextDouble() * fitnessSum;

		for(int i = 0; i < Population.Count; i++)
		{
			if( randomNumber < Population[i].Fitness)
			{
				return Population[i];
			}

			randomNumber -= Population[i].Fitness;
		}

		return Population[random.Next(0, Population.Count)];
	}
}
