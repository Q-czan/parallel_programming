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

// Чтение точек из файла
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

// ---- Версия 1: распараллеливание цикла ----
void compute_center_loop(const std::vector<Point>& points,
                         double& cx, double& cy, double& cz) {
    double sum_x = 0.0, sum_y = 0.0, sum_z = 0.0;
    int n = static_cast<int>(points.size());

    #pragma omp parallel for reduction(+:sum_x, sum_y, sum_z)
    for (int i = 0; i < n; ++i) {
        sum_x += points[i].x;
        sum_y += points[i].y;
        sum_z += points[i].z;
    }

    cx = sum_x / n;
    cy = sum_y / n;
    cz = sum_z / n;
}

// ---- Версия 2: функциональная декомпозиция (sections) ----
void compute_center_sections(const std::vector<Point>& points,
                             double& cx, double& cy, double& cz) {
    double sum_x = 0.0, sum_y = 0.0, sum_z = 0.0;
    int n = static_cast<int>(points.size());

    #pragma omp parallel sections
    {
        #pragma omp section
        {
            double local = 0.0;
            for (int i = 0; i < n; ++i) local += points[i].x;
            sum_x = local;
        }
        #pragma omp section
        {
            double local = 0.0;
            for (int i = 0; i < n; ++i) local += points[i].y;
            sum_y = local;
        }
        #pragma omp section
        {
            double local = 0.0;
            for (int i = 0; i < n; ++i) local += points[i].z;
            sum_z = local;
        }
    }

    cx = sum_x / n;
    cy = sum_y / n;
    cz = sum_z / n;
}

int main(int argc, char* argv[]) {
    SetConsoleOutputCP(65001);

    if (argc < 2) {
        std::cerr << "Использование: task5.exe <файл_с_точками> [число_потоков]" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    int num_threads = 4;
    if (argc > 2) num_threads = std::atoi(argv[2]);

    omp_set_num_threads(num_threads);

    // Чтение данных
    auto t0 = std::chrono::high_resolution_clock::now();
    std::vector<Point> points = read_points(filename);
    auto t1 = std::chrono::high_resolution_clock::now();

    if (points.empty()) {
        std::cerr << "Файл пуст или не найден." << std::endl;
        return 1;
    }

    std::cout << "Прочитано точек: " << points.size() << std::endl;
    std::cout << "Потоков: " << num_threads << std::endl;

    double cx, cy, cz;

    // ---- Версия 1: параллельный цикл ----
    auto s1 = std::chrono::high_resolution_clock::now();
    compute_center_loop(points, cx, cy, cz);
    auto e1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> d1 = e1 - s1;

    std::cout << "\nВерсия 1 (параллельный цикл):" << std::endl;
    std::cout << "  Центр: (" << cx << ", " << cy << ", " << cz << ")" << std::endl;
    std::cout << "  Время: " << d1.count() << " с" << std::endl;

    // ---- Версия 2: функциональная декомпозиция ----
    auto s2 = std::chrono::high_resolution_clock::now();
    compute_center_sections(points, cx, cy, cz);
    auto e2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> d2 = e2 - s2;

    std::cout << "\nВерсия 2 (функциональная декомпозиция):" << std::endl;
    std::cout << "  Центр: (" << cx << ", " << cy << ", " << cz << ")" << std::endl;
    std::cout << "  Время: " << d2.count() << " с" << std::endl;

    return 0;
}