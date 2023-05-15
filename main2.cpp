#include <iostream>
#include <fstream>  
#include <string>
#include <sstream>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <chrono>
#include <vector>
#include <queue>
#include <random>

using namespace std;

const int M = 100000;
const int N = 10000000;
int BUFFER_SIZE;
int P;
int C;
int howFilledIsTheBuffer[M];
queue<int> buffer;
mutex mtx;
condition_variable posicoes_vazias;
condition_variable posicoes_cheias;
int outp
// Número de números consumidos
atomic<int> num_consumed;
atomic_flag numConsumedLock = ATOMIC_FLAG_INIT;
auto tempo0 = chrono::high_resolution_clock::now();

void acquire(){
    while (numConsumedLock.test_and_set()) {}
}
void release(){
    numConsumedLock.clear();
}
bool isPrime(int num){
    acquire();
    num_consumed++;
    howFilledIsTheBuffer[num_consumed] = buffer.size();
    // cout << num_consumed << endl;
    if(num_consumed > M){
        auto tempo1 = chrono::high_resolution_clock::now();
        auto tempo_total = chrono::duration_cast<chrono::microseconds>(tempo1 - tempo0).count();
        cout << tempo_total/1000000.0 << endl;
        ostringstream oss;
        oss << "outputBuffer_" << BUFFER_SIZE << "_" << P << "_" << C << ".txt";
        ofstream fout(oss.str());
        if (fout.is_open())
        {
            for(int i = 0; i < M; i++){
                fout << howFilledIsTheBuffer[i] << ",";
            }
            fout.close();
        }
        terminate();
    }
    release();
    for (int i = 2; i <= num / 2; ++i) {
        if (num % i == 0) {
            return false;
        }
    }
    return true;
}    

void produtor() {
    int num;
    while (true) {
        num = rand() % N + 1;
        {
            unique_lock<mutex> lock(mtx);
            // Aguarda até ter posições vazias
            posicoes_vazias.wait(lock, []{return buffer.size() < BUFFER_SIZE;});
            buffer.push(num);
            // cout << "Produziu " << num << " (posições ocupadas:" << buffer.size() << ")" << std::endl;
            posicoes_cheias.notify_one();
        }
        // this_thread::sleep_for(chrono::milliseconds(30));
    }
}

void consumidor() {
    int num;
    while (true) {
        {
            unique_lock<mutex> lock(mtx);
            // Aguarda até ter posições preenchidas
            posicoes_cheias.wait(lock, []{return !buffer.empty();});
            num = buffer.front();
            buffer.pop();
            // cout << "Consumiu  " << num << " (posições ocupadas:" << buffer.size() << ")" << endl;
            posicoes_vazias.notify_one();
        }
        bool is_prime = isPrime(num);
        // cout << "É Primo? " << is_prime << endl;
        // this_thread::sleep_for(chrono::milliseconds(30));
    }
}

int main() {
    cout << "Tamanho do Buffer? " << endl;
    cin >> BUFFER_SIZE;
    cout << "Quantos Produtores? " << endl;
    cin >> P;
    cout << "Quantos Consumidores? " << endl;
    cin >> C;
    thread produtores[P];
    thread consumidores[C];
    if(P > C){}
    for(int i = 0; i < P; i++){
        produtores[i] = thread(produtor);
    }
    for(int j = 0; j < C; j++){
        consumidores[j] = thread(consumidor);
    }
    tempo0 = chrono::high_resolution_clock::now();
    for(int i = 0; i < P; i++){
        produtores[i].join();
    }
    for(int j = 0; j < C; j++){
        consumidores[j].join();
    }
    return 0;
}