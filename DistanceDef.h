//
// Created by 夏乐扬 on 2023/8/4.
//

#ifndef SIMBATS_MASTER_DISTANCEDEF_H
#define SIMBATS_MASTER_DISTANCEDEF_H
#include <iostream>
#include <numeric>
// 计算Wasserstein距离
double calculateWassersteinDistance(const std::vector<double>& distribution1, std::vector<int> distribution2) {

    int n = distribution1.size();
    double distance = 0.0;

    for (int i = 0; i < n; i++) {
        if (distribution1[i] != 0.0 || distribution2[i] != 0.0) {
            distance += std::abs(distribution1[i] - distribution2[i]);
        }
    }
    return distance;
}

double KLDivergence(const std::vector<double> & distribution1, std::vector<int> distribution2) {
    int n = distribution1.size();
    double distance = 0.0;

    for (int i = 0; i < n; i++) {
        if (distribution1[i] != 0.0 && distribution2[i] != 0.0)
            distance += distribution1[i]* log(distribution1[i] / distribution2[i]);
    }

    return distance;
}


double JSDivergence(const std::vector<double> & distribution1, std::vector<int> distribution2) {
    std::vector<double> distribution3(distribution1.size(), 0);
    for (int i = 0; i < distribution1.size(); ++i) {
        distribution3[i] = (distribution1[i] + distribution2[i]) / 2;
    }
    return 0.5 * KLDivergence(distribution1, distribution3) + 0.5 * KLDivergence(distribution2, distribution3);
}

int getNextBatchDegree(std::vector<double> degree, const std::vector<double>& targetDistribution) {
    int k = degree.size();
    int nextDegree = 1;


    double minDistance = std::numeric_limits<double>::max();

    for (int i = 0; i < k; i++) {
        if (targetDistribution[i] == 0.0) {
            continue; // Skip the degree if the target probability is zero
        }

        std::vector<double> newDegrees(degree.begin(), degree.end());

        newDegrees[i] += 1;

        int sum2 = std::accumulate(newDegrees.begin(), newDegrees.end(), 0.0);

        for (double& value : newDegrees) {
            value /= sum2;
        }

        double distance1 = calculateWassersteinDistance(newDegrees, targetDistribution);
        double distance2 = KLDivergence(newDegrees, targetDistribution);
        double distance3 = JSDivergence(newDegrees, targetDistribution);

        if (distance3 < minDistance) {
            minDistance = distance3;
            nextDegree = i + 1;
        }
    }
    std::cout << "WassersteinDistance= " << minDistance << std::endl;
    return nextDegree;
}
#endif //SIMBATS_MASTER_DISTANCEDEF_H
