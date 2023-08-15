//
// Created by 夏乐扬 on 2023/8/4.
//

#ifndef SIMBATS_MASTER_BATCHDEGREE_H
#define SIMBATS_MASTER_BATCHDEGREE_H
#include <iostream>
#include "BatsBasic.h"
#include "DistanceDef.h"
#include <numeric>

class BatchDegree {
private:
    enum {MAX = 100};
    std::vector<int> degree(MAX);
    KeyType batchID;
    int k;
public:
    BatchDegree() {
        batchID = 0;
        k = 0;
    }
    BatchDegree(KeyType key, const std::vector<int>& targetDistribution, int packetNum);
    ~BatchDegree();
};

BatchDegree::BatchDegree(KeyType key, const vector<int> &targetDistribution, int packetNum) : batchID(key) {
    for (int i = 0; i < k; i++) {
        degree.push_back(0);
    }


}



#endif //SIMBATS_MASTER_BATCHDEGREE_H
