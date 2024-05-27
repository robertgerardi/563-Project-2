#ifndef SIM_BP_H
#define SIM_BP_H
#include <string>
using namespace std;

typedef struct bp_params{
    unsigned long int K;
    unsigned long int M1;
    unsigned long int M2;
    unsigned long int N;
    char*             bp_name;
}bp_params;

class BranchHistoryTable {

public:

    int numPredictions;
    int numMispredictions;
    float rateMispredictions;

    BranchHistoryTable(string type, int n, int k, int m1, int m2);
    void printVar();
    void checkTables(unsigned long int addr, char outcome);
    void printStats();

private:

    int N; //Global branch history register bits for Gshare table
    int K; //Index bits for chooser table (hybrid)
    int M1; //PC bits for Gshare table
    int M2; //Index bits for bimodal table
    

    char BHTtype; //b = bimodal, g = gshare, h = hybrid

    int *BimodalBHT;
    int *GshareBHT;
    int BranchHistoryReg;
    int BimodalSize;
    int GshareSize;
    int ChooserSize;
    int *chooserTable;


};

#endif
