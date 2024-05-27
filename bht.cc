#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <iostream>
#include <cmath>
#include "sim_bp.h"

using namespace std;

BranchHistoryTable::BranchHistoryTable(string type, int n, int k, int m1, int m2){
    
    BimodalSize = 0;
    GshareSize = 0;
    ChooserSize = 0;
    if(type == "bimodal"){
        BHTtype = 'b';
        M2 = m2;
        N = 0;
        K = 0;
        M1 = 0;
        BimodalSize = pow(2,M2);
        GshareSize = 0;

        

    }else if (type == "gshare"){
        BHTtype = 'g';
        M1 = m1;
        N = n;
        M2 = 0;
        K = 0;
        GshareSize = pow(2,M1);
        BimodalSize = 0;
        
        
    }else if (type == "hybrid"){
        BHTtype = 'h';
        N = n;
        K = k;
        M1 = m1;
        M2 = m2;
        BimodalSize = pow(2,M2);
        GshareSize = pow(2,M1);
        ChooserSize = pow(2,K);
    }

    numPredictions = 0;
    numMispredictions = 0;
    rateMispredictions = 0.0;

    
    BimodalBHT = new int[BimodalSize];
    GshareBHT = new int[GshareSize];
    chooserTable =  new int[ChooserSize];
    BranchHistoryReg = 0;

    for(int i = 0; i < BimodalSize; i++){
        BimodalBHT[i] = 2;
    }
    for(int i = 0; i < GshareSize; i++){
        GshareBHT[i] = 2;
        
    }
    for(int i = 0; i < ChooserSize; i++){
        chooserTable[i] = 1;
    }

    
        
}

void BranchHistoryTable::printVar(){
    cout << "====TEST PRINT====" << endl;
    cout << "K: " << K << endl;
    cout << "N: " << N << endl;
    cout << "M1: " << M1 << endl;
    cout << "M2: " << M2 << endl;
    cout << "Type: " << BHTtype << endl;
    cout << "Array Size: " << BimodalSize << endl;
    cout << "==================" << endl;
    
}

void BranchHistoryTable::printStats(){
    cout << "OUTPUT" << endl;
    cout << "number of predictions:     " << numPredictions << endl;
    cout << "number of mispredictions:  " << numMispredictions << endl;

    rateMispredictions = 100* (float)numMispredictions/(float)numPredictions;

    cout << "misprediction rate:        ";
    printf("%.2f",rateMispredictions);
    cout << "%" << endl;

    
    
    
    if(BHTtype == 'b'){
        cout << "FINAL BIMODAL CONTENTS" << endl;
        for(int i = 0; i < BimodalSize; i++){
        cout << i << "\t" << BimodalBHT[i] <<endl;
    }
    }
    else if(BHTtype == 'g'){
        cout << "FINAL GSHARE CONTENTS" << endl;
        for(int i = 0; i < GshareSize; i++){
        cout << i << "\t" << GshareBHT[i] <<endl;
    }
    }
    else if(BHTtype == 'h'){
        cout << "FINAL CHOOSER CONTENTS" << endl;
        for(int i = 0; i < ChooserSize; i++){
        cout << i << "\t" << chooserTable[i] <<endl;
    }
        cout << "FINAL GSHARE CONTENTS" << endl;
        for(int i = 0; i < GshareSize; i++){
        cout << i << "\t" << GshareBHT[i] <<endl;
    }
    cout << "FINAL BIMODAL CONTENTS" << endl;
        for(int i = 0; i < BimodalSize; i++){
        cout << i << "\t" << BimodalBHT[i] <<endl;
    }
    }
    
}

void BranchHistoryTable::checkTables(unsigned long int addr, char outcome){

    int GshareTableIndex = 0;
    int BimodalTableIndex = 0;
    int ChooseIndex = 0;

    int regXOR = 0;
    regXOR = (BranchHistoryReg << (M1 - N));

    if(BHTtype == 'b'){
        BimodalTableIndex = (addr >> 2) & (BimodalSize-1);
    }

    if(BHTtype == 'g'){
        if(N == 0){
            GshareTableIndex =  ((addr >> 2) & (GshareSize-1));
        }else {
            GshareTableIndex =  ((addr >> 2) & (GshareSize-1)) ^ regXOR;
        }
       
    }
    
    if(BHTtype == 'h'){
        BimodalTableIndex = (addr >> 2) & (BimodalSize-1);
        GshareTableIndex =  ((addr >> 2) & (GshareSize-1)) ^ regXOR;
        ChooseIndex = (addr >> 2) & (ChooserSize-1);
    }
    

    
   // cout << "test" << endl;

   
    /*cout << "addr: " << hex << addr << endl;
    cout << "index: " << hex <<  BimodalTableIndex<< endl; 
    cout << BimodalSize << endl;
    */
    numPredictions++;

    char Bimodalprediction;
    char Gshareprediction;

    if(BimodalBHT[BimodalTableIndex] >= 2){
        Bimodalprediction = 't';
    }
    else{
        Bimodalprediction = 'n';
    }
    if(GshareBHT[GshareTableIndex] >= 2){
        Gshareprediction = 't';
    }
    else{
        Gshareprediction = 'n';
    }

    
    if(BHTtype == 'h'){
        char chooseType;
        if(chooserTable[ChooseIndex] >= 2){
            chooseType = 'g';
        }
        else{
            chooseType = 'b';
        }

        if(chooseType == 'b'){
            if(outcome != Bimodalprediction){
             numMispredictions++; }
        }
        if(chooseType == 'g'){
        if(outcome != Gshareprediction){
             numMispredictions++;
            }
        }
        switch(outcome){
        
        case('t'):
            if(chooseType == 'b'){
            if(BimodalBHT[BimodalTableIndex] != 3){
                BimodalBHT[BimodalTableIndex]++;
            }
            BranchHistoryReg = (BranchHistoryReg >> 1) ^ (1 << (N-1));
            }
            if(chooseType == 'g'){
                if(GshareBHT[GshareTableIndex] != 3){
                GshareBHT[GshareTableIndex]++;
            }
                BranchHistoryReg = (BranchHistoryReg >> 1) ^ (1 << (N-1));
               
            }
            break;

        case('n'):
            if(chooseType == 'b'){
            if(BimodalBHT[BimodalTableIndex] != 0){
                BimodalBHT[BimodalTableIndex]--;
            }
            BranchHistoryReg = (BranchHistoryReg >> 1);
            }
            if(chooseType == 'g'){
                if(GshareBHT[GshareTableIndex] != 0){
                GshareBHT[GshareTableIndex]--;
            }
                BranchHistoryReg = (BranchHistoryReg >> 1);
               
            }
            break;
     }

     if((outcome == Bimodalprediction) && (outcome != Gshareprediction)){
        if(chooserTable[ChooseIndex] != 0){
                chooserTable[ChooseIndex]--;
            }
     }
     if((outcome != Bimodalprediction) && (outcome == Gshareprediction)){
            if(chooserTable[ChooseIndex] != 3){
                chooserTable[ChooseIndex]++;
            }
     }
        
        return;
     }

    /*
    cout << "TEST PREDICTION: " << prediction << endl;
    cout << "TEST OUTCOME: "<< outcome << endl;
    */
    if(BHTtype == 'b'){
        if(outcome != Bimodalprediction){
             numMispredictions++; }
    }
    
        
    if(BHTtype == 'g'){
        if(outcome != Gshareprediction){
             numMispredictions++;
            }
    }
    

    switch(outcome){
        
        case('t'):
            if(BHTtype == 'b'){
            if(BimodalBHT[BimodalTableIndex] != 3){
                BimodalBHT[BimodalTableIndex]++;
            }
            }
            if(BHTtype == 'g'){
                if(GshareBHT[GshareTableIndex] != 3){
                GshareBHT[GshareTableIndex]++;
            }
                BranchHistoryReg = (BranchHistoryReg >> 1) ^ (1 << (N-1));
               
            }
            break;

        case('n'):
            if(BHTtype == 'b'){
            if(BimodalBHT[BimodalTableIndex] != 0){
                BimodalBHT[BimodalTableIndex]--;
            }
            }
            if(BHTtype == 'g'){
                if(GshareBHT[GshareTableIndex] != 0){
                GshareBHT[GshareTableIndex]--;
            }
                BranchHistoryReg = (BranchHistoryReg >> 1);
               
            }
            break;
    }
}


