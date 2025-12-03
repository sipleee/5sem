#include <iostream>
#include <cmath>
#include <cstring>
#include <pthread.h>
#include "solver.h"

using namespace std;


void normalize_row(double** aug, int k, int n) {
    double pivot = aug[k][k];
    for (int j = k; j < 2 * n; j++) {
        aug[k][j] /= pivot;
    }
}


void* process_rows(void* arg) {
    ThreadData* data = (ThreadData*)arg;

    for (int i = data->start_row; i < data->end_row; i++) {
        if (i != data->k) {
            double factor = data->aug[i][data->k];
            for (int j = data->k; j < 2 * data->n; j++) {
                data->aug[i][j] -= factor * data->aug[data->k][j];
            }
        }
    }

    return nullptr;
}

bool jordan_inverse(double** A, double** A_inv, double** aug, int n, int num_threads) {
    if (num_threads <= 0) num_threads = 1;
    if (num_threads > n) num_threads = n;

    
    for (int i = 0; i < n; i++) {
        memcpy(aug[i], A[i], n * sizeof(double));
        for (int j = n; j < 2 * n; j++) {
            aug[i][j] = (j - n == i) ? 1.0 : 0.0;
        }
    }

    
    pthread_t* threads = new pthread_t[num_threads];
    ThreadData* thread_data = new ThreadData[num_threads];

    for (int k = 0; k < n; k++) {
        if (fabs(aug[k][k]) < 1e-15) {
            cerr << "Error: Matrix is singular at step " << k + 1
                << " (a_{" << k + 1 << k + 1 << "} = " << aug[k][k] << ")" << endl;

            
            delete[] threads;
            delete[] thread_data;
            return false;
        }

        
        normalize_row(aug, k, n);

        
        int chunk_size = n / num_threads;

        for (int t = 0; t < num_threads; t++) {
            thread_data[t].aug = aug;
            thread_data[t].n = n;
            thread_data[t].k = k;
            thread_data[t].start_row = t * chunk_size;
            thread_data[t].end_row = (t == num_threads - 1) ? n : (t + 1) * chunk_size;

            // Пропускаем текущую строку k
            if (thread_data[t].start_row <= k && k < thread_data[t].end_row) {
                if (thread_data[t].start_row == k) {
                    thread_data[t].start_row = k + 1;
                }
                else if (thread_data[t].end_row > k) {
                    // Разделяем диапазон на две части
                    int temp_end = thread_data[t].end_row;
                    thread_data[t].end_row = k;

                    // Создаем дополнительный поток для оставшейся части
                    if (t < num_threads - 1) {
                        thread_data[t + 1].start_row = k + 1;
                    }
                    else {
                        // Если это последний поток, обрабатываем оставшуюся часть в текущем
                        thread_data[t].end_row = k;
                        // Обработка оставшейся части после join
                        for (int i = k + 1; i < temp_end; i++) {
                            if (i != k) {
                                double factor = aug[i][k];
                                for (int j = k; j < 2 * n; j++) {
                                    aug[i][j] -= factor * aug[k][j];
                                }
                            }
                        }
                    }
                }
            }

            
            if (thread_data[t].start_row < thread_data[t].end_row) {
                pthread_create(&threads[t], nullptr, process_rows, &thread_data[t]);
            }
            else {
                threads[t] = pthread_t(); 
            }
        }

        
        for (int t = 0; t < num_threads; t++) {
            if (thread_data[t].start_row < thread_data[t].end_row) {
                pthread_join(threads[t], nullptr);
            }
        }

        
        if (n <= 4) {
            cout << "\nStep " << k + 1 << " (after processing column " << k + 1 << "):" << endl;
            cout << "Extended matrix [A|E]:" << endl;
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    printf("%8.4f ", aug[i][j]);
                }
                cout << "| ";
                for (int j = n; j < 2 * n; j++) {
                    printf("%8.4f ", aug[i][j]);
                }
                cout << endl;
            }
        }
    }

   
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            A_inv[i][j] = aug[i][j + n];
        }
    }

    
    delete[] threads;
    delete[] thread_data;

    return true;

}
