#ifndef SIMRECODER_H
#define SIMRECODER_H

#include "Utilities.h"
#include "FiniteField.h"

#include <string.h>

const int shuffle_cnt = 10;

class SimRecoder {
public:
	SimRecoder(int M, int L, int Q, double* rank, MTRand* psrand) : M(M), L(L), Q(Q), rankSampler(rank, M+1), psrand(psrand) {
		H = mallocMat<SymbolType>(M, M);
        tmp = mallocMat<SymbolType>(M, M);
        psrand->seed();
	}
	~SimRecoder() {
		freeMat<SymbolType>(H, M);
        freeMat<SymbolType>(tmp, M);
	}
	int genBatch(SymbolType** dstBatchWithoutID, SymbolType** srcBatchWithoutID) {
		int curRank;

		curRank = rankSampler.sample(psrand->rand());
        
        for (int i = 0; i < curRank; i++) {
            memcpy(dstBatchWithoutID[i], srcBatchWithoutID[i], L*sizeof(SymbolType));
        }
        for (int i = curRank; i < M; i++) {
            memset(dstBatchWithoutID[i], 0, L*sizeof(SymbolType));
        }

        return curRank;
        
    }
private:
	//Parameters
	int M; //BatchSize
	int L; //Total packet length in bytes
	int Q; //No. of element in finite field minus one. For use in PRNG
	//Rank distribution
	DistSampler rankSampler;

	//Internal buffer for transfer matrix
	SymbolType** H;
    SymbolType** tmp;
    
	//Access to PRNG
	MTRand* psrand;
};


#endif
/* SIMRECODER_H */
