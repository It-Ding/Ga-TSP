#pragma once
#include "Individual.h"
#include <random>

class CityInformation {
public:
    vector<pair<double, double>> cityList;
    map<pair<int, int>, double> cityDisMat; //城市之间的距离矩阵

    CityInformation();

    CityInformation(ifstream &city);

    void outPutCityList();
};
