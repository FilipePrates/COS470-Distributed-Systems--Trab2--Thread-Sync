#include <iostream>
#include <thread>
#include <atomic>

using namespace std;

const int N = 1000000000;
int vetorASerSomado[N];
int K;

// SomaTotal compartilhada
atomic<int> somaTotal;
// variável pro spinlock busy-wait
atomic_flag lock = ATOMIC_FLAG_INIT;

void acquire(){
    while (lock.test_and_set()) {}
}
void release(){
    lock.clear();
}

void soma(int start, int end) {
    int somaLocal = 0;
    for (int i = start; i < end; i++) somaLocal += vetorASerSomado[i];
    acquire();
    somaTotal += somaLocal;
    release();
}

int main() {
    cout << "Quantas Threads?"; 
    cin >> K;
    somaTotal = 0;
    for (int i = 0; i < N; i++) {
        vetorASerSomado[i] = rand() % 201 - 100;
    }
    int somaSingleThread = 0;
    for (int i = 0; i < N; i++) {
        somaSingleThread += vetorASerSomado[i];
    }
    cout << "Soma correta: " << somaSingleThread << endl;
    for (int batch = 0; batch < 1; batch ++){
        thread threads[K];
        for (int i = 0; i < K; i++) {
            threads[i] = thread(soma, i * (N / K), (i == K-1) ? N : (i+1) * (N / K));
        }
        auto tempo0 = chrono::high_resolution_clock::now();
        for (int i = 0; i < K; i++) {
            threads[i].join();
        }
        auto tempo1 = chrono::high_resolution_clock::now();
        auto tempo_total = chrono::duration_cast<chrono::microseconds>(tempo1 - tempo0).count();
        // cout << "Tempo Batch: " << batch << " " << tempo_total / 1000000.0 << "s" << endl;
        cout << tempo_total/1000000.0 << ",";
        if (somaTotal != somaSingleThread) {
            cout << "Erro" << endl;
            return -1;
        }
        somaTotal = 0;
    }
    cout << endl;
    return 0;
}