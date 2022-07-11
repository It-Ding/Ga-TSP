#include "CityInformation.h"

CityInformation::CityInformation()
{
	random_device rd;
	mt19937 rnd(rd());
	uniform_real_distribution<> dis(0, 1);
	for (int i = 0; i < CITY_NUM; ++i)
	{
		double x = dis(rnd);
		double y = dis(rnd);
		cityList.push_back( { x, y });
	}
	for (int i = 0; i < CITY_NUM; ++i)
	{
		for (int j = 0; j < i; ++j)
		{
			double x1 = cityList[i].first, y1 = cityList[i].second;
			double x2 = cityList[j].first, y2 = cityList[j].second;
			double derx = x1 - x2, dery = y1 - y2;
			double distance = sqrt(derx * derx + dery * dery);
			cityDisMat[{i, j}] = distance;
			cityDisMat[{j, i}] = distance;
		}
	}
}

CityInformation::CityInformation(ifstream& in)
{
	int n;
	in >> n;
	for (int i = 0; i < n; ++i)
	{
		double x, y;
		in >> x >> y;
		cityList.push_back({ x,y });
	}
	for (int i = 0; i < cityList.size(); ++i)
	{
		for (int j = 0; j < i; ++j)
		{
			double x1 = cityList[i].first, y1 = cityList[i].second;
			double x2 = cityList[j].first, y2 = cityList[j].second;
			double derx = x1 - x2, dery = y1 - y2;
			double distance = sqrt(derx * derx + dery * dery);
			cityDisMat[{i, j}] = distance;
			cityDisMat[{j, i}] = distance;
		}
	}
}

void CityInformation::outPutCityList()
{
	ofstream out("cityList.in");
	out << cityList.size() << endl;
	for (auto& city : cityList)
		out << city.first << " " << city.second << endl;
}
