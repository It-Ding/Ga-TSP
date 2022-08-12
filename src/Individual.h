#pragma once
#include <algorithm>
#include <fstream>
#include <map>
#include <random>
#include <vector>

#define GENE_NUM 5000   //迭代轮数
#define CITY_NUM 50     //城市数量
#define INDI_NUM 60     //个体数量
#define MUTATE_PROB 0.3 //变异概率

using namespace std;

class Individual {
private:
    double fitness;   //种群适应度
    vector<int> gene; //基因，即旅行路线，从起点经所有城市再回到起点
    map<pair<int, int>, double> *cityDisMat;

public:
    Individual(map<pair<int, int>, double> *);

    Individual(vector<int>, map<pair<int, int>, double> *);

    double evaluateFitness();

    vector<int> getGene();

    void setGene(vector<int>);

    double getFitness();

    bool operator<(const Individual another) const;
};
