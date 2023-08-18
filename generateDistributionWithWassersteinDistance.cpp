//
// Created by 夏乐扬 on 2023/7/17.
//

#include <iostream>
#include <vector>
#include <random>
#include "fileinout.h"


// 生成服从均匀分布的随机数
double generateUniformRandom() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(0.0, 1.0);
    return dis(gen);
}

// 计算两个概率分布之间的Wasserstein距离
double calculateWassersteinDistance(const std::vector<double>& distribution1, const std::vector<double>& distribution2) {
    int n = distribution1.size();
    double distance = 0.0;

    for (int i = 0; i < n; i++) {
        distance += std::abs(distribution1[i] - distribution2[i]);
    }

    return distance;
}

// 生成具有目标Wasserstein距离的分布
std::vector<double> generateDistributionWithWassersteinDistance(const std::vector<double>& targetDistribution, double targetDistance) {
    int numPoints = targetDistribution.size();
    std::vector<double> distribution(numPoints, 0.0);

    // 随机初始化分布
    for (double& value : distribution) {
        value = generateUniformRandom();
    }

    // 归一化分布
    double sum = std::accumulate(distribution.begin(), distribution.end(), 0.0);
    for (double& value : distribution) {
        value /= sum;
    }

    // 迭代调整分布以逼近目标Wasserstein距离
    int maxIterations = 1000;
    int iterations = 0;
    double currentDistance = calculateWassersteinDistance(distribution, targetDistribution);

    while (currentDistance > targetDistance && iterations < maxIterations) {
        // 调整分布
        for (int i = 0; i < numPoints; i++) {
            distribution[i] += (targetDistribution[i] - distribution[i]) * 0.1;
        }

        // 归一化分布
        sum = std::accumulate(distribution.begin(), distribution.end(), 0.0);
        for (double& value : distribution) {
            value /= sum;
        }

        // 更新当前距离
        currentDistance = calculateWassersteinDistance(distribution, targetDistribution);
        iterations++;
    }

    return distribution;
}



int main() {
    // 假设我们有一个概率分布
    std::string filename1 = "/Users/leyang/Desktop/bats_code_utilities-master/julia/bettersimDegreeK640M32m8.txt";
    std::vector<double> targetDistribution = readDataFromFile(filename1);


    // 生成与给定分布的Wasserstein距离为targetDistance的分布
    double targetDistance = 0.7;

    std::vector<double> distribution = generateDistributionWithWassersteinDistance(targetDistribution, targetDistance);
    std::string filename2 = "../wasserstein1simdegree.txt";

    writeDataToFile(distribution, filename2);

    return 0;
}

