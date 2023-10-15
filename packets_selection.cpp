//
// Created by 夏乐扬 on 2023/10/10.
//
#include <iostream>
#include <random>
#include "fileinout.h"
#include <ostream>
void packet_selection (int degree, int* idx, int K, int* numberCount) {
    int* minElement = std::min_element(numberCount, numberCount + K);
    int min = *minElement;
    int lowerCount = 0;
    for (int i = 0; i < K; i++){
        if (*(numberCount + i) == min)
            lowerCount++;
    }

    if (degree == K){
        for(int i = 0; i < degree; i++) {
            numberCount[i] += 1;
            idx[i] = i;
        }
        return;
    }else if (degree < K && degree >= lowerCount) {
        int availableIndices[K - lowerCount];
        for (int i = 0, j = 0; i < K; i++) {
            if (numberCount[i] == min + 1) {
                availableIndices[j] = i;
                j++;
            }
        }


        for (int i = 0, j = 0; i < K; ++i) {
            if (numberCount[i] == min) {
                idx[j] = i;
                numberCount[i] += 1;
                j++;
            }
        }
        std::shuffle(availableIndices, availableIndices + K - lowerCount, std::mt19937(std::random_device()()));
        for (int i = lowerCount - 1; i < degree; i++) {
            int index = availableIndices[i];
            idx[i] = index;
            numberCount[index] += 1;
        }
        std::sort(idx, idx + degree);
        return;
    }else {
        int availableIndices[lowerCount];
        for (int i = 0, j = 0; i < K; i++) {
            if (numberCount[i] == *minElement) {
                availableIndices[j] = i;
                j++;
            }
        }
        std::shuffle(availableIndices, availableIndices + lowerCount, std::mt19937(std::random_device()()));
        for (int i = 0; i < degree; i++) {
            int index = availableIndices[i];
            idx[i] = index;
            numberCount[index] += 1;
        }
        std::sort(idx, idx + degree);
        return;
    }
}

int main() {
    std::vector<int> degree;
    int packetsNum = 160;
    int numCount[packetsNum];

    std::ifstream filename("../TheNextDegree.txt");
    int number;
    while( ! filename.eof()) {
        filename >> number;
        degree.push_back(number);
    }

    for (int i = 0; i < packetsNum; i++) {
        numCount[i] = 0;
    }

    for (int i = 0; i < 10; i++){
        int idx[degree[i]];
        packet_selection(degree[i], idx, packetsNum, numCount);

    }


//    int idx[degree[3]];
//    packet_selection(degree[3], idx, packetsNum, numCount);

    for (int i = 0; i < packetsNum; i++) {
        std::cout << numCount[i] << std::endl;
    }

    return 0;
}