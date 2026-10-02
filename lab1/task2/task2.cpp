#include <iostream>
#include <vector>
#include <omp.h>
#include <cstdlib>
#include <chrono>

int main(int argc, char* argv[]) {
    int num_threads = 8;
    int n = 16000;

    if (argc > 1) num_threads = std::atoi(argv[1]);
    if (argc > 2) n = std::atoi(argv[2]);

    if (n < 3) {
        std::cerr << "Размер массива должен быть >= 3" << std::endl;
        return 1;
    }

    omp_set_num_threads(num_threads);

    std::vector<double> a(n), b(n);

    // Инициализация: a[i] = i
    #pragma omp parallel for
    for (int i = 0; i < n; ++i) {
        a[i] = static_cast<double>(i);
    }

    // Обработка крайних элементов
    b[0]     = a[0];
    b[n - 1] = a[n - 1];

    // Параллельное вычисление средних значений
    auto start = std::chrono::high_resolution_clock::now();

    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i < n - 1; ++i) {
        b[i] = (a[i - 1] + a[i] + a[i + 1]) / 3.0;
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "Потоков: " << num_threads
              << ", размер массива: " << n
              << ", время выполнения: " << elapsed.count() << " с" << std::endl;
    std::cout << "b[1]     = " << b[1]     << std::endl;
    std::cout << "b[n/2]   = " << b[n / 2] << std::endl;
    std::cout << "b[n-2]   = " << b[n - 2] << std::endl;

    return 0;
}
