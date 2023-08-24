//
// Created by 夏乐扬 on 2023/7/17.
//
#include <iostream>
#include <vector>
#include <cmath>
#include <numeric>
#include <vector>
#include "fileinout.h"
#include <queue>



// 计算Wasserstein距离
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

//double KLDivergence(const std::vector<double> & distribution1, const std::vector<double> & distribution2) {
//    int n = distribution1.size();
//    double distance = 0.0;
//
//    for (int i = 0; i < n; i++) {
//        if (distribution1[i] != 0.0 && distribution2[i] != 0.0)
//        distance += distribution1[i]* log(distribution1[i] / distribution2[i]);
//    }
//
//    return distance;
//}
//
//
//double JSDivergence(const std::vector<double> & distribution1, const std::vector<double> & distribution2) {
//    std::vector<double> distribution3(distribution1.size(), 0);
//    for (int i = 0; i < distribution1.size(); ++i) {
//        distribution3[i] = (distribution1[i] + distribution2[i]) / 2;
//    }
//    return 0.5 * KLDivergence(distribution1, distribution3) + 0.5 * KLDivergence(distribution2, distribution3);
//}


 //确定下一个批次的度
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


        if (distance1 < minDistance) {
            minDistance = distance1;
            nextDegree = i + 1;
        }
    }
    std::cout << "WassersteinDistance= " << minDistance << std::endl;
    return nextDegree;
}



int main() {
    std::string filename1 = "/Users/leyang/Desktop/bats_code_utilities-master/julia/bettersimDegreeK640M32m8.txt";
//    std::string filename1 = "../wasserstein1simdegree.txt";
    std::vector<double> targetDistribution = readDataFromFile(filename1); // 目标度分布
    //std::vector<double> targetDistribution = {0, 0.2, 0, 0.5, 0.05, 0.15};
    int k = targetDistribution.size();
    //std::string filename2 = "../wasserstein1simdegree.txt";

    std::ofstream outfile("../TheNextDegree.txt");
    std::vector<double> degrees(k, 0); // 开始时的度分布

    // 确定下一个批次的度
  for (int i = 0; i < 1000; i++) {
        int nextDegree = getNextBatchDegree(degrees, targetDistribution);
        std::cout << std::endl;
        std::cout << "下一个批次的度为：" << nextDegree << std::endl;
        // 将 nextDegree 写入文件
        outfile << nextDegree << std::endl;
        degrees[nextDegree - 1] += 1;
  }

  //统计各个度选取次数
  std::cout << "统计各个度选取次数: " << std::endl;
  for (int i = 0; i < k; i++) {
      std::cout << "度为" << i + 1 << "的批次数目为： " << degrees[i] << std::endl;
  }

  //计算度分布
  std::cout << "算法产生的度分布：" << std::endl;
  int sum = std::accumulate(degrees.begin(), degrees.end(), 0.0);
  for (int i = 0; i < k; i++) {
      std::cout << degrees[i] / sum << std::endl;
  }

    return 0;
}
