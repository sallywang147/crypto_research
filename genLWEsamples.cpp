#include "regevEncryption.h"
#include<cmath>
#include <random>
#include <iostream>
#include <ctime>

//https://sagecell.sagemath.org/
//https://bitbucket.org/malb/lwe-estimator/raw/HEAD/estimator.py
//higher hamming weights and n increases security bits: hw and n are the only two param
//we need to tweak
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

    ofstream datafile;
    datafile.open ("../LWEdata/tenary.txt", fstream::app|fstream::out);
    for(size_t i = 0; i < ct.a.GetLength(); i++){
        datafile << ct.a[i].ConvertToInt() << ",";
    }
    for(size_t i = 0; i < ct.b.GetLength(); i++){
        datafile << ct.b[i].ConvertToInt() << ",";
    }
    datafile << "\n";
    datafile.close();
}


void saveRandomizedData(const PVWCiphertext& ct, const PVWParam& param, int transaction_num){
    
    ofstream datafile;
    long n = param.n;
    long q = param.q;
    datafile.open("../LWEdata/randtenary.txt", fstream::app|fstream::out);

    //https://stackoverflow.com/questions/19665818/generate-random-numbers-using-c11-random-library
    //MSR talk on why we should use the following: https://www.youtube.com/watch?v=LDPMpc-ENqY
    lbcrypto::DiscreteUniformGeneratorImpl<NativeVector> dug;
    dug.SetModulus(q);


    for(size_t i = 0; i < ct.a.GetLength(); i++){
        datafile << dug.GenerateInteger() << ",";
    }
    for(size_t i = 0; i < ct.b.GetLength(); i++){
        datafile << dug.GenerateInteger(); << ",";
    }
    datafile << "\n";
    datafile.close();
}

void preparingDatabase(int numOfDataPieces){

    srand (time(NULL));
    //this is (n = 512, q = 2^50, sigma = 3,2)
    auto params = PVWParam(512, pow(2.0, 50), 3.2, 16000, 1); // The last "1" means it is an LWE sample. "PVW" is an LWE-based encryption system, when ell = 1, it is exactly what we have talked about. 
   //this function hsould be updated 
   //use this func: regevGenerateSecretKey from h file
    auto sk = PVWGenerateTenaryKey(params, 32);

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
            saveRandomizedData(tempclue, params , i);
            // vector<int> tmp;
            // PVWDec(tmp, tempclue, sk, params);
            // for(int i = 0; i < params.ell; i++){
            //     cout << tmp[i] << endl;
            // }
        } else {
            PVWCiphertext tempclue;
            PVWEncSK(tempclue, ones, sk, params);
            saveData(tempclue, i);
            saveRandomizedData(tempclue, params, i);
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
    //let's get 51200 data points first to avoid unnecessarily large files 
    preparingDatabase(51200);
}