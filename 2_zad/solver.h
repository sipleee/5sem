#ifndef SOLVER_H
#define SOLVER_H
 
#include <pthread.h>
 
// Структура для передачи данных в потоки
struct ThreadData {
    double** aug;
    int n;
    int k;
    int start_row;
    int end_row;
    double factor;
    pthread_mutex_t* mutex;
};
 
bool jordan_inverse(double** A, double** A_inv, double** aug, int n, int num_threads = 1);
 
#endif
