#include "Ga.h"

Ga::Ga(CityInformation ctifm) {
    this->cityDisMat = (ctifm.cityDisMat);
    this->bestFitness = -1;
}

vector<Individual> Ga::crossover() {
    random_device rd;
    mt19937 rnd(rd());
    vector<Individual> newGeneration;
    shuffle(individualList.begin(), individualList.end(), rnd);
    for (int i = 0; i < INDI_NUM - 1; i += 2) {
        //父代基因
        vector<int> genes1 = individualList[i].getGene();
        vector<int> genes2 = individualList[i + 1].getGene();
        int index1 = rnd() % (CITY_NUM - 1);
        int index2 = index1 + rnd() % (CITY_NUM - 1 - index1) + 1;
        unordered_map<int, int> hash1;
        //首先进行片段交换
        for (int j = index1; j <= index2; ++j) {
            int a = genes1[j], b = genes2[j];
            if (hash1.find(a) != hash1.end()) {
                a = hash1[a];
            }
            if (hash1.find(b) != hash1.end()) {
                b = hash1[b];
            }
            hash1[a] = b;
            hash1[b] = a;
            swap(genes1[j], genes2[j]);
        }
        //处理左侧基因冲突
        for (int j = 0; j < index1; j++) {
            if (hash1.find(genes1[j]) != hash1.end()) {
                genes1[j] = hash1[genes1[j]];
            }
            if (hash1.find(genes2[j]) != hash1.end()) {
                genes2[j] = hash1[genes2[j]];
            }
        }
        //处理右侧基因冲突
        for (int j = index2 + 1; j < CITY_NUM; j++) {
            if (hash1.find(genes1[j]) != hash1.end()) {
                genes1[j] = hash1[genes1[j]];
            }
            if (hash1.find(genes2[j]) != hash1.end()) {
                genes2[j] = hash1[genes2[j]];
            }
        }
        newGeneration.push_back(Individual(genes1, &cityDisMat));
        newGeneration.push_back(Individual(genes2, &cityDisMat));
    }
    return newGeneration;
}

void Ga::mutation(vector<Individual> newGeneration) {
    random_device rnd;
    // mt19937 rnd(rd());
    for (Individual &individual : newGeneration) {
        if (MUTATE_PROB * RAND_MAX > rnd() % RAND_MAX) //一定概率变异
        {
            if (0.9 * RAND_MAX > rnd() % RAND_MAX) {
                vector<int> genes = individual.getGene();
                int times = rnd() % 6; //随机变异0~5次
                while (times--) {
                    int index1 = rnd() % (CITY_NUM - 1);
                    int index2 = rnd() % (CITY_NUM - 1);
                    swap(genes[index1], genes[index2]);
                }
            }
            if (0.9 * RAND_MAX > rnd() % RAND_MAX) {
                vector<int> genes = individual.getGene();
                individual.setGene(genes);
                // std::vector<int> genes = individual.getGene();
                int index1 = rnd() % (CITY_NUM - 1);
                int index2 = index1 + rnd() % (CITY_NUM - 1 - index1) + 1;
                std::reverse(genes.begin() + index1, genes.begin() + index2);
                individual.setGene(genes);
            }
        }
        this->individualList.push_back(individual);
    }
}

void Ga::select() {
    //锦标赛算法
    //以下两个个数据是根据默认数据200设置的，种群数量变化时需适当修改
    random_device rd;
    mt19937 rnd(rd());
    int groupNum = 10;
    int groupSize = 10;
    int groupWinnerSize = INDI_NUM / groupNum;
    vector<Individual> winners;
    for (int i = 0; i < groupNum; ++i) {
        vector<Individual> group;
        for (int j = 0; j < groupSize; ++j) {
            int index = rnd() % individualList.size();
            group.push_back(individualList[index]);
        }
        sort(group.begin(), group.end());
        for (int j = 0; j < groupWinnerSize; ++j)
            winners.push_back(group[j]);
    }
    individualList.swap(winners);
}

void Ga::nextGeneration() {
    //交叉
    vector<Individual> newGeneration = this->crossover();
    //变异
    this->mutation(newGeneration);
    //选择
    this->select();
    //获得这一带的结果
    for (Individual now : individualList) {
        if (bestFitness == -1 || bestFitness > now.getFitness()) {
            bestFitness = now.getFitness();
            bestResult = now.getGene();
        }
    }
}

vector<vector<int>> Ga::getResultList() {
    return this->resultList;
}

vector<double> Ga::getFitnessList() {
    return this->fitnessList;
}

double Ga::getBestFitness() {
    return this->bestFitness;
}

vector<int> Ga::getBestResult() {
    return this->bestResult;
}

void Ga::train() {
    for (int i = 0; i < INDI_NUM; ++i) {
        individualList.push_back(Individual(&cityDisMat));
    }
    //迭代
    for (int i = 0; i < GENE_NUM; ++i) {
        //产生子代
        this->nextGeneration();
        //连接首尾
        bestResult.push_back(bestResult.front());
        //将结果存入列表中
        resultList.push_back(bestResult);
        fitnessList.push_back(bestFitness);
    }
}
