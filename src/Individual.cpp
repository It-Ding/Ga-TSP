#include "Individual.h"

Individual::Individual(map<pair<int, int>, double> *city) {
    //生成初代
    for (int i = 0; i < CITY_NUM; ++i)
        gene.push_back(i);
    random_device rd;
    mt19937 rnd(rd());
    shuffle(gene.begin(), gene.end(), rnd);
    this->cityDisMat = city;
    this->fitness = evaluateFitness();
}

Individual::Individual(vector<int> gene, map<pair<int, int>, double> *city) {
    //用于生成子代
    this->gene = gene;
    this->cityDisMat = city;
    this->fitness = evaluateFitness();
}

double Individual::evaluateFitness() {
    //用于计算种群适应度
    double fit = 0.0;
    for (int i = 0; i < gene.size() - 1; ++i) {
        int fromIndex = gene[i];
        int toIndex = gene[i + 1];
        fit += (*cityDisMat)[{fromIndex, toIndex}];
    }
    fit += (*cityDisMat)[{gene.front(), gene.back()}];
    return fit;
}

vector<int> Individual::getGene() {
    return this->gene;
}

void Individual::setGene(vector<int> gene) {
    this->gene = gene;
}

double Individual::getFitness() {
    return fitness;
}

bool Individual::operator<(const Individual another) const {
    //排序规则根据适应度从小到大
    return this->fitness < another.fitness;
}
