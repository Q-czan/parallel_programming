#include <iostream>
#include <omp.h>
#include <cstdlib>

// ------------------------------------------------------------
// Метод 1: барьер в цикле по turn
// ------------------------------------------------------------
void method1(int num_threads) {
    omp_set_num_threads(num_threads);

    #pragma omp parallel
    {
        int id = omp_get_thread_num();
        int n  = omp_get_num_threads();

        for (int turn = n - 1; turn >= 0; --turn) {
            if (id == turn) {
                #pragma omp critical
                std::cout << "Метод 1: Поток " << id << std::endl;
            }
            #pragma omp barrier
        }
    }
}

// ------------------------------------------------------------
// Метод 2: критическая секция + busy-wait на переменной turn
// ------------------------------------------------------------
void method2(int num_threads) {
    omp_set_num_threads(num_threads);
    int turn;

    #pragma omp parallel shared(turn)
    {
        int id = omp_get_thread_num();
        int n  = omp_get_num_threads();

        #pragma omp single
        turn = n - 1;

        #pragma omp barrier

        while (true) {
            #pragma omp flush(turn)
            if (turn == id) {
                #pragma omp critical
                {
                    std::cout << "Метод 2: Поток " << id << std::endl;
                    turn--;
                }
                break;
            }
        }
    }
}

// ------------------------------------------------------------
// Метод 3: блокировки OpenMP (omp_lock_t)
// ------------------------------------------------------------
void method3(int num_threads) {
    omp_set_num_threads(num_threads);
    omp_lock_t lock;
    omp_init_lock(&lock);
    int turn;

    #pragma omp parallel shared(turn, lock)
    {
        int id = omp_get_thread_num();
        int n  = omp_get_num_threads();

        #pragma omp single
        turn = n - 1;

        #pragma omp barrier

        while (true) {
            omp_set_lock(&lock);
            if (turn == id) {
                std::cout << "Метод 3: Поток " << id << std::endl;
                turn--;
                omp_unset_lock(&lock);
                break;
            }
            omp_unset_lock(&lock);
        }
    }

    omp_destroy_lock(&lock);
}

// ------------------------------------------------------------
// Метод 4: флаги и ожидание соседнего потока
// ------------------------------------------------------------
void method4(int num_threads) {
    omp_set_num_threads(num_threads);
    int n = num_threads;

    bool* printed = new bool[n];
    for (int i = 0; i < n; ++i) printed[i] = false;

    #pragma omp parallel shared(printed)
    {
        int id = omp_get_thread_num();

        if (id == n - 1) {
            #pragma omp critical
            std::cout << "Метод 4: Поток " << id << std::endl;
            printed[id] = true;
            #pragma omp flush(printed)
        } else {
            while (!printed[id + 1]) {
                #pragma omp flush(printed)
            }
            #pragma omp critical
            std::cout << "Метод 4: Поток " << id << std::endl;
            printed[id] = true;
            #pragma omp flush(printed)
        }
    }

    delete[] printed;
}

// ------------------------------------------------------------
// Метод 5: барьер перед проверкой в цикле по i
// ------------------------------------------------------------
void method5(int num_threads) {
    omp_set_num_threads(num_threads);

    #pragma omp parallel
    {
        int id = omp_get_thread_num();
        int n  = omp_get_num_threads();

        for (int i = n - 1; i >= 0; --i) {
            #pragma omp barrier
            if (id == i) {
                #pragma omp critical
                std::cout << "Метод 5: Поток " << id << std::endl;
            }
        }
    }
}

// ------------------------------------------------------------
// main
// ------------------------------------------------------------
int main(int argc, char* argv[]) {
    int num_threads = 8;
    int method = 1;

    if (argc > 1) num_threads = std::atoi(argv[1]);
    if (argc > 2) method = std::atoi(argv[2]);

    switch (method) {
        case 1: method1(num_threads); break;
        case 2: method2(num_threads); break;
        case 3: method3(num_threads); break;
        case 4: method4(num_threads); break;
        case 5: method5(num_threads); break;
        default:
            std::cerr << "Неизвестный метод: " << method
                      << ". Доступны методы 1..5." << std::endl;
            return 1;
    }

    return 0;
}
