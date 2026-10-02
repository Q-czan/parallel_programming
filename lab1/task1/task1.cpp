#include <iostream>
#include <omp.h>
#include <cstdlib>

int main(int argc, char* argv[]) {
    int num_threads = 8;
    if (argc > 1) {
        num_threads = std::atoi(argv[1]);
    }

    omp_set_num_threads(num_threads);

    #pragma omp parallel
    {
        int id = omp_get_thread_num();
        int n  = omp_get_num_threads();

        #pragma omp critical
        {
            std::cout << "Поток " << id
                      << " из " << n
                      << ": Hello World" << std::endl;
        }
    }

    return 0;
}
