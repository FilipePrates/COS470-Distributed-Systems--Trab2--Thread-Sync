#include <iostream>
#include <thread>
#include <atomic>

using namespace std;

const int N = 10000000;
int vetorASerSomado[N];
int K;

// SomaTotal compartilhada
atomic<int> somaTotal;
// variável pro spinlock busy-wait
atomic_flag lock = ATOMIC_FLAG_INIT;

void soma(int start, int end) {
    int somaLocal = 0;
    for (int i = start; i < end; i++) somaLocal += vetorASerSomado[i];
    while (lock.test_and_set()) {}
    somaTotal += somaLocal;
    cout << somaLocal << endl;
    lock.clear();
}

int main() {
    cout << "Quantas Threads?"; 
    cin >> K;
    for (int i = 0; i < N; i++) {
        vetorASerSomado[i] = rand() % 201 - 100;
    }
    int somaSingleThread = 0;
    for (int i = 0; i < N; i++) {
        somaSingleThread += vetorASerSomado[i];
    }
    cout << "Soma correta: " << somaSingleThread << endl;
    double total_time = 0.0;
    thread threads[K];
    for (int i = 0; i < K; i++) {
        int s = i * (N / K);
        int e = (i == K-1) ? N : (i+1) * (N / K);
        threads[i] = thread(soma, s, e);
    }
    auto tempo0 = chrono::high_resolution_clock::now();
    for (int i = 0; i < K; i++) {
        threads[i].join();
    }
    auto tempo1 = chrono::high_resolution_clock::now();
    auto tempo_total = chrono::duration_cast<chrono::microseconds>(tempo1 - tempo0).count();
    cout << tempo_total;
    if (somaTotal != somaSingleThread) {
        cout << "Soma incorreta" << endl;
        return -1;
    }
    somaTotal = 0;
    return 0;
}