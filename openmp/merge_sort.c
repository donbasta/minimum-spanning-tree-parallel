#include <stdio.h>
#include <stdlib.h>
#ifdef _OPENMP
    #include <omp.h>
#endif

void merge_sort(int* arr, int l, int r) {
    #ifdef _OPENMP
        int my_rank = omp_get_thread_num();
        int thread_count = omp_get_num_threads();
    #else 
        int my_rank = 0;
        int thread_count = 1;
    #endif
}