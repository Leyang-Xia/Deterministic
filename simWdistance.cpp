//
// Created by 夏乐扬 on 2023/7/20.
//
#include <iostream>
#include <numeric>
#include "fileinout.h"

double calculateWassersteinDistance(const std::vector<double>& distribution1, const std::vector<double>& distribution2) {
    int n = distribution1.size();
    double distance = 0.0;

    for (int i = 0; i < n; i++) {
        if (distribution1[i] != 0.0 || distribution2[i] != 0.0) {
            distance += std::abs(distribution1[i] - distribution2[i]);
        }
    }
    return distance;
}

int main() {
    std::string filename1 = "../data.txt";
    std::vector<double> realDegree = readDataFromFile(filename1); //真实度值
    std::string filename2 = "/Users/leyang/Desktop/bats_code_utilities-master/julia/bettersimDegreeK640M32m8.txt";
    std::vector<double> targetDegreeDistribution = readDataFromFile(filename2);

    std::vector<double> realDegreeDistribution(targetDegreeDistribution.size(), 0);
    //计算度分布
    for (int i = 0; i < realDegree.size(); i++) {
        realDegreeDistribution[realDegree[i] - 1]++;
    }

    double sum = std::accumulate(realDegreeDistribution.begin(), realDegreeDistribution.end(), 0.0);

    for (double & value: realDegreeDistribution) {
        value /= sum;
    }

    std::cout << "n = " << realDegree.size() << std::endl;
    std::cout << calculateWassersteinDistance(realDegreeDistribution, targetDegreeDistribution);
}