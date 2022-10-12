#include "aco.h"


int main(int argc, char* argv[]){
    int run = atoi(argv[1]);
    int iter = atoi(argv[2]);
    double alpha = atof(argv[3]);
    double beta = atof(argv[4]);
    double Qv = atof(argv[5]);
    double decrease = atof(argv[6]);
    int ant = atoi(argv[7]);
    string file = argv[8];

    ACO alg(alpha, beta, Qv, decrease);
    alg.ALG_run(run, iter, file, ant);
}