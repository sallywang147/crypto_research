#include "regevEncryption.h"
#include<cmath>
#include <random>
#include <iostream>
#include <ctime>


PVWsk PVWGenerateSecretKeyBinary(const PVWParam& param, int hamming_weight){
    long n = param.n;
    long q = param.q;

    std::vector<int> theones;
    for (int i = 0; i < n; ++i)
        theones.push_back(i);
    std::random_shuffle(theones.begin(), theones.end());

    lbcrypto::DiscreteUniformGeneratorImpl<regevSK> dug;
    dug.SetModulus(q);
    PVWsk ret(param.ell);
    for(int i = 0; i < param.ell; i++){
        ret[i] = dug.GenerateVector(n);
    }
    for(size_t i = 0; i < ret.size(); i++){
        for(int j = 0; j < param.n; j++){
            ret[i][j] = 0;
        }
        for(int j = 0; j < hamming_weight; j++){
            ret[i][theones[j]] = 1;
        }
    }
    return ret;
}

void saveData(const PVWCiphertext& ct, int transaction_num){
    ofstream datafile_a;
    ofstream datafile_b;
    datafile.open ("../LWEdata/datafile_a.txt", fstream::app|fstream::out);
    datafile.open ("../LWEdata/datafile_b.txt", fstream::app|fstream::out);

    for(size_t i = 0; i < ct.a.GetLength(); i++){
        datafile_a << ct.a[i].ConvertToInt() << "\n";
    }
    for(size_t i = 0; i < ct.b.GetLength(); i++){
        datafile_b << ct.b[i].ConvertToInt() << "\n";
    }

    datafile.close();
}


void saveRandomizedData(const PVWCiphertext& ct, int transaction_num){
    ofstream datafile_a;
    ofstream datafile_b;
    datafile.open("../LWEdata/randata_a.txt", fstream::app|fstream::out);
    datafile.open("../LWEdata/randata_b.txt", fstream::app|fstream::out);

    //https://stackoverflow.com/questions/19665818/generate-random-numbers-using-c11-random-library
    //MSR talk on why we should use the following: https://www.youtube.com/watch?v=LDPMpc-ENqY
    std::random_device rd; //this is seed
    std::mt19937 generator(rd());

    for(size_t i = 0; i < ct.a.GetLength(); i++){
        datafile_a << ct.a[i].ConvertToInt() << "\n ";
    }
    for(size_t i = 0; i < ct.b.GetLength(); i++){
        datafile_b << generator() << "\n";
    }
    datafile.close();
}

void preparingDatabase(int numOfDataPieces){

    srand (time(NULL));
    //this is (n = 512, q = 2^50, sigma = 3,2)
    auto params = PVWParam(512, pow(2.0, 50), 3.2, 16000, 1); // The last "1" means it is an LWE sample. "PVW" is an LWE-based encryption system, when ell = 1, it is exactly what we have talked about. 
    auto sk = PVWGenerateSecretKeyBinary(params, 32);
    for(int i = 0; i < params.ell; i++){
        cout << sk[i] << endl;
    }

    vector<int> zeros(params.ell, 0);
    vector<int> ones(params.ell, 1);

#pragma omp parallel for
//this is for non-randomized (a, b) pairs
    for(int i = 0; i < numOfDataPieces; i++){
        if(i < numOfDataPieces/2){
            PVWCiphertext tempclue;
            PVWEncSK(tempclue, zeros, sk, params);
            saveData(tempclue, i);
            saveRandomizedData(tempclue, i);
            // vector<int> tmp;
            // PVWDec(tmp, tempclue, sk, params);
            // for(int i = 0; i < params.ell; i++){
            //     cout << tmp[i] << endl;
            // }
        } else {
            PVWCiphertext tempclue;
            PVWEncSK(tempclue, ones, sk, params);
            saveData(tempclue, i);
            saveRandomizedData(tempclue, i);
            // vector<int> tmp;
            // PVWDec(tmp, tempclue, sk, params);
            // for(int i = 0; i < params.ell; i++){
            //     cout << tmp[i] << endl;
            // }
        }
    }

    return;
}

int main(){
    //let's get 100 data points first to avoid unnecessarily large files 
    preparingDatabase(100);
}