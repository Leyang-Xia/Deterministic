#ifndef BATSBASIC_H
#define	BATSBASIC_H

#include "Utilities.h"
#include "FiniteField.h" //provide finite field computation
#include <fstream>
#include <cmath>
#define BATSDECODER_MAXBATCH 65535




class BatsBasic {
protected:
    double precodeRate;
    // Basic encoding parameters
    int batchSize; // batch size
    int packetNum; // packet number, not including check packets//
    int packetSize; // packet length in Bytes
    int checkNum; // number of check packets
    int totalNum; // packetNum + checkNum
    vector<int> numberCount;
    // input packets
    SymbolType **packets;

    // precode parameters
    int ldpcNum; // number of LDPC packets
    int hdpcNum; // number of HDPC packets
    
    int packetAndLDNum; // packetNum + ldpcNum
    
    int ldpcVarDegree; // variable degree of LDPC
    // check packets generated by precode
    SymbolType **checkPackets;


    // Parameters for sparse-matrix encoding
    int piNum; // number of packets for Pre-defined inactivation
    int smNum; // number of packets for sparse-matrix encoding

    int smMinLd; // = smNum - ldpcNum
    int piMinHd; // = piNum - hdpcNum

//    int maxInact; // the maximum allowed number of inactivation 
//    int maxRedun; // the maximum redundancy used to decode inactive packets
    int piDegree; // PI degree in a batch
    // degree distribution
    //int D;
    DistSampler *dist;

    // psudo random generator
    MTRand *psrand;
    //        MTRand *genRand;

    // finite field and parameters
    int fieldSizeMinOne;
    int fieldOrder;
    
    // packet coding vector
    int nSymInHead;
    int nFFInSym;
    SymbolType maskDec; //typedef unsigned char SymbolType
    SymbolType maskEnc;
    vector<int> targetDegree;
public:
    BatsBasic(int M, int K, int T) : batchSize(M), packetNum(K), packetSize(T) {
        //初始化包计数向量
        for (int i = 0; i < K; i++) {
            numberCount.push_back(0);
        }

        ifstream filename("../TheNextDegree.txt");
        int number;
        while( ! filename.eof()) {
            filename >> number;
            targetDegree.push_back(number);
        }

        precodeRate = 0.0101;

        if (packetNum < 20000) {
            // this value is good for 1600, 16000
            ldpcNum = (int)(precodeRate * packetNum + sqrt(1.5 * packetNum));
        } else {
            ldpcNum = (int) (precodeRate * packetNum + sqrt(4*packetNum));
        }

        ldpcNum = 1;


        if (ldpcNum > 0) {
            hdpcNum = (int)log(packetNum);
            hdpcNum = (hdpcNum > 5)? hdpcNum : 5;
            //hdpcNum = 0; // for testing only

        } else { // BATS_PRECODE_RATE == 0 for no precoding
            hdpcNum = 0;
        }

        hdpcNum = 2;



        ldpcVarDegree = 3;

        checkNum = ldpcNum+hdpcNum;
        
        totalNum = packetNum + checkNum;
        
        packetAndLDNum = packetNum + ldpcNum;

        piNum =  hdpcNum + sqrt(packetNum);
        piDegree = sqrt(batchSize)+2;
        piDegree = (piDegree < piNum)? piDegree : piNum;

        smNum = totalNum - piNum;

        smMinLd = smNum - ldpcNum;
        piMinHd = piNum - hdpcNum;

        
        // packets will be set separatedly
        packets = NULL;
        checkPackets = NULL;

        // degree distribution will be set separatedly
        dist = NULL;

        // init random generator
        psrand = new MTRand();

        // set finite field paramters
        fieldSizeMinOne = FF.size - 1;
        fieldOrder = FF.order;
        
        // 
        
        nSymInHead = batchSize * fieldOrder / SymbolSize;
        nFFInSym = SymbolSize / fieldOrder;
        maskDec = 0xFF;          
        maskDec <<= (8-fieldOrder);
        maskEnc = 0x80;
        maskEnc >>= (fieldOrder - 1);
    }

    ~BatsBasic(){
        delete psrand;

        if (dist != NULL)
            delete dist;
    }

    void setDegreeDist(double* degreeDist, int maxDeg) {
        if (dist != NULL)
            delete dist;

        dist = new DistSampler(degreeDist, maxDeg);
    }
    
    inline SymbolType* getPkgHead(int pid){
        return (pid < packetNum) ? packets[pid] : checkPackets[pid - packetNum]; //返回的是一个包
    }

    // coordinate changes between [active inactive] to 
    // smToExt changes coordinate from [sm ldpc] to [packets check]
    inline int smToExt(int smId){
        return (smId < smMinLd) ? smId : (smId+piMinHd);
    }
    // piToExt changes coordinate from [pi hdpc] to [packets check]
    inline int piToExt(int piId){
        return (piId < piMinHd) ? (piId+smMinLd) : (piId+smNum);
    }
    inline int extToSM(int exId){
        if (exId < smMinLd)
            return exId;
        else if (exId < packetNum)
            return ldpcNum+exId;
        else 
            return exId-piMinHd;
    }

    inline bool isPreInact(int id) {
        return (id >= smMinLd && id < packetNum) || (id >= (packetNum + ldpcNum));
    }
    
    inline int getSmallestBid(){
        return ldpcNum;
    }
    
    inline unsigned int getHdpcKey(){
        return 54896; // good for field size 2^4 and 2^8 bad for 2^2
        //return 12564; // good for field size 2^2 bad for 2
    }
    
    //
    void matMulQ(SymbolType** output, SymbolType** input, int row, bool CMP){
        int packetAndLDNum = packetNum + ldpcNum;
        int degree2 = 2;
        SymbolType * middle = new SymbolType[row];
        
        psrand->seed(getHdpcKey());

        int idVec[hdpcNum];
        for (int i = 0; i < hdpcNum; i++){
            idVec[i] = i;
        }

        // reset 
        for (int k = 0; k < hdpcNum; k++) {
            memset(output[k], 0, row);
        }
        // k=0  
        if (input[0]!=NULL){
            memcpy(middle, input[0], row);
        } else {
            memset(middle, 0, row);
        }

        SymbolType b;
        
        for (int k = 1; k < packetAndLDNum; k++) {
            for (int i = 0; i < degree2; i++) {
                int j = psrand->randInt(hdpcNum - 1 - i) + i;
                swap(idVec[i], idVec[j]);

                FF.addvv(output[idVec[i]], middle, row);
            }
            if (CMP)
                FF.mulvcCMP(middle, 2, row);
            else
                FF.mulvc(middle, 2, row);
            if (input[k]!=NULL){
                FF.addvv(middle, input[k], row);
            }
        }

        b = 1;

        for (int k = 0; k < hdpcNum; k++) {
            if (CMP)
                FF.addvvcCMP(output[k], middle, b, row);
            else
                FF.addvvc(output[k], middle, b, row);
            b = FF.mul(b, 2);
        }

        delete [] middle;
    }
    // Encoding constants
    unsigned long getBatchDegree(int key, int random){
        psrand->seed(key ^ random);
       unsigned long degree = dist->sample(psrand->rand());
       unsigned long deterministic_degree = targetDegree[key - ldpcNum];
        if (degree > smNum){
            degree = smNum;
        }

        if (deterministic_degree > smNum){
            deterministic_degree = smNum;
        }

        return deterministic_degree;

    }

    void genBatchParam(int degree, int* idx, SymbolType** G, int* idxI, SymbolType** GI){
        
        // encoding: BATS parts
        int i,j, idVec[smNum];
        
        for( i=0;i<smNum;i++)
            idVec[i]=i;
        
        for(i=0;i<degree;i++){
            j = psrand->randInt(smNum-1-i)+i;

            swap(idVec[i],idVec[j]);
            
            idx[i] = smToExt(idVec[i]);

            numberCount[idx[i]]++;
            
            for(j=0;j<batchSize;j++){
                G[i][j] = (SymbolType)(psrand->randInt(fieldSizeMinOne));
            }

        }


        
        // encoding: PI parts
        //degree = piDegree;
        
        if (piNum > 0) {
            
            for (i = 0; i < piNum; i++)
                idVec[i] = i;
            
            for (i = 0; i < piDegree; i++) {
                j = psrand->randInt(piNum - 1 - i) + i;
                swap(idVec[i], idVec[j]);
                idxI[i] = piToExt(idVec[i]);
                
                for (j = 0; j < batchSize; j++) {
                    GI[i][j] = (SymbolType) (psrand->randInt(fieldSizeMinOne - 1) + 1);
                }
            }
        }
    }

    void printCountNumber() {
        for (int number : numberCount) {
            std::cout << number << " ";
        }
        std::cout << std::endl;
    }
private:
                  
};

#endif	/* BATSBASIC_H */

