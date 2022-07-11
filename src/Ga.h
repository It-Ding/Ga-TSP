#pragma once
#include "Individual.h"
#include "CityInformation.h"
#include <unordered_map>

class Ga
{
private:
	vector<Individual> individualList;//个体列表
	vector<int> bestResult;
	vector<vector<int>> resultList;//每一代解的列表
	double bestFitness;//最优适应度
	vector<double> fitnessList;//适应度列表
	map<pair<int, int>, double> cityDisMat;//城市之间的距离矩阵

public:
	Ga(CityInformation);

	void train();

	vector<Individual> crossover();

	void mutation(vector<Individual>);

	void select();

	void nextGeneration();

	vector<vector<int>> getResultList();

	vector<double> getFitnessList();

	double getBestFitness();

	vector<int> getBestResult();
};

