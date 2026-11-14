#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <omp.h>
#include <cstdlib>
#include <chrono>
#include <windows.h>

struct Point {
    double x, y, z;
};

std::vector<Point> read_points(const std::string& filename) {
    std::vector<Point> points;
    std::ifstream in(filename);
    if (!in) {
        std::cerr << "Не удалось открыть файл: " << filename << std::endl;
        return points;
    }
    Point p;
    while (in >> p.x >> p.y >> p.z) {
        points.push_back(p);
    }
    return points;
}

int main(int argc, char* argv[]) {
    SetConsoleOutputCP(65001);

    if (argc < 2) {
        std::cerr << "Использование: task6.exe <файл_с_точками> [число_потоков]" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    int num_threads = 4;
    if (argc > 2) num_threads = std::atoi(argv[2]);

    omp_set_num_threads(num_threads);

    std::vector<Point> points = read_points(filename);
    if (points.empty()) {
        std::cerr << "Файл пуст или не найден." << std::endl;
        return 1;
    }

    int N = static_cast<int>(points.size());
    std::cout << "Прочитано точек: " << N << std::endl;
    std::cout << "Потоков: " << num_threads << std::endl;

    double global_sum = 0.0;

    auto start = std::chrono::high_resolution_clock::now();

    #pragma omp parallel sections
    {
        #pragma omp section
        {
            double local_x = 0.0;
            for (int i = 0; i < N; ++i) local_x += points[i].x;
            
            #pragma omp critical
            {
                global_sum += local_x;
            }
        }
        #pragma omp section
        {
            double local_y = 0.0;
            for (int i = 0; i < N; ++i) local_y += points[i].y;
            
            #pragma omp critical
            {
                global_sum += local_y;
            }
        }
        #pragma omp section
        {
            double local_z = 0.0;
            for (int i = 0; i < N; ++i) local_z += points[i].z;
            
            #pragma omp critical
            {
                global_sum += local_z;
            }
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    double result = global_sum / (3.0 * N);

    std::cout << "\nВерсия с функциональной декомпозицией:" << std::endl;
    std::cout << "  Глобальная сумма: " << global_sum << std::endl;
    std::cout << "  Значение (sum_x + sum_y + sum_z) / 3N: " << result << std::endl;
    std::cout << "  Время: " << elapsed.count() << " с" << std::endl;

    return 0;
}