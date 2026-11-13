#include <iostream>
#include <fstream>
#include <cstdlib>

int main() {
    std::ofstream f("points.txt");
    if (!f) { std::cerr << "Cannot open points.txt\n"; return 1; }
    const int N = 1000000;  // 1 миллион точек
    for (int i = 0; i < N; ++i) {
        f << (i % 100) << " " << ((i * 7) % 100) << " " << ((i * 13) % 100) << "\n";
    }
    std::cout << "points.txt created with " << N << " points\n";
    return 0;
}