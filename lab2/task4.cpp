#include <iostream>
#include <vector>
#include <omp.h>
#include <cstdlib>
#include <chrono>
#include <windows.h>

// Последовательное умножение матриц C = A * B
void multiply_sequential(const std::vector<std::vector<double>>& A,
                         const std::vector<std::vector<double>>& B,
                         std::vector<std::vector<double>>& C,
                         int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            double sum = 0.0;
            for (int k = 0; k < n; ++k) {
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }
}

// Параллельное умножение матриц (распараллелен внешний цикл по i)
void multiply_parallel(const std::vector<std::vector<double>>& A,
                       const std::vector<std::vector<double>>& B,
                       std::vector<std::vector<double>>& C,
                       int n) {
    #pragma omp parallel for
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            double sum = 0.0;
            for (int k = 0; k < n; ++k) {
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }
}

int main(int argc, char* argv[]) {
    SetConsoleOutputCP(65001);

    int n = 500;            // размер матрицы по умолчанию
    int num_threads = 4;    // число потоков по умолчанию

    if (argc > 1) n = std::atoi(argv[1]);
    if (argc > 2) num_threads = std::atoi(argv[2]);

    if (n <= 0) {
        std::cerr << "Размер матрицы должен быть > 0" << std::endl;
        return 1;
    }

    std::cout << "Размер матрицы: " << n << " x " << n << std::endl;
    std::cout << "Число потоков: " << num_threads << std::endl;

    // Создаём матрицы
    std::vector<std::vector<double>> A(n, std::vector<double>(n));
    std::vector<std::vector<double>> B(n, std::vector<double>(n));
    std::vector<std::vector<double>> C(n, std::vector<double>(n, 0.0));

    // Заполняем A и B простыми значениями (без random — чтобы результаты повторялись)
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            A[i][j] = (i + j) % 10;
            B[i][j] = (i - j + 10) % 10;
        }
    }

    // ---- Последовательная версия ----
    auto start_seq = std::chrono::high_resolution_clock::now();
    multiply_sequential(A, B, C, n);
    auto end_seq = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_seq = end_seq - start_seq;

    std::cout << "\nПоследовательная версия:" << std::endl;
    std::cout << "  Время: " << elapsed_seq.count() << " с" << std::endl;

    // ---- Параллельная версия ----
    omp_set_num_threads(num_threads);

    // Обнуляем C перед повторным использованием
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            C[i][j] = 0.0;
        }
    }

    auto start_par = std::chrono::high_resolution_clock::now();
    multiply_parallel(A, B, C, n);
    auto end_par = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_par = end_par - start_par;

    std::cout << "\nПараллельная версия:" << std::endl;
    std::cout << "  Потоков: " << num_threads << std::endl;
    std::cout << "  Время: " << elapsed_par.count() << " с" << std::endl;

    // ---- Ускорение ----
    double speedup = elapsed_seq.count() / elapsed_par.count();
    std::cout << "\nУскорение (speedup): " << speedup << "x" << std::endl;

    // Контрольная сумма для проверки корректности
    double checksum = 0.0;
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            checksum += C[i][j];
    std::cout << "Контрольная сумма: " << checksum << std::endl;

    return 0;
}