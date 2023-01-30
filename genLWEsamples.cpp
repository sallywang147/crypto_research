#include "regevEncryption.h"

PVWsk PVWGenerateSecretKeyBinary(const PVWParam& param, int hamming_weight){
    int n = param.n;
    int q = param.q;

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
    datafile.open ("../LWEdata/datafile.txt", fstream::app);

    for(size_t i = 0; i < ct.a.GetLength(); i++){
        datafile << ct.a[i].ConvertToInt() << ", ";
    }
    for(size_t i = 0; i < ct.b.GetLength(); i++){
        datafile << ct.b[i].ConvertToInt() << "\n";
    }

    datafile.close();
}


void preparingDatabase(int numOfDataPieces){

    srand (time(NULL));
    auto params = PVWParam(512, 1073741824, 3.2, 16000, 1); // The last "1" means it is an LWE sample. "PVW" is an LWE-based encryption system, when ell = 1, it is exactly what we have talked about. 
    auto sk = PVWGenerateSecretKeyBinary(params, 32);
    for(int i = 0; i < params.ell; i++){
        cout << sk[i] << endl;
    }

    vector<int> zeros(params.ell, 0);
    vector<int> ones(params.ell, 1);

#pragma omp parallel for
    for(int i = 0; i < numOfDataPieces; i++){
        if(i < numOfDataPieces/2){
            PVWCiphertext tempclue;
            PVWEncSK(tempclue, zeros, sk, params);
            saveData(tempclue, i);
            // vector<int> tmp;
            // PVWDec(tmp, tempclue, sk, params);
            // for(int i = 0; i < params.ell; i++){
            //     cout << tmp[i] << endl;
            // }
        } else {
            PVWCiphertext tempclue;
            PVWEncSK(tempclue, ones, sk, params);
            saveData(tempclue, i);
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
    preparingDatabase(10000000);
}