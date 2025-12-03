#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <pthread.h>
#include "matrix_io.h"

using namespace std;

struct MatrixThreadData {
    double** matrix;
    int k;
    int n;
    int start_row;
    int end_row;
};

struct NormThreadData {
    double** A;
    int n;
    int start_row;
    int end_row;
    double partial_norm;
};

struct ResidualThreadData {
    double** A;
    double** A_inv;
    double** temp;
    int n;
    int start_row;
    int end_row;
};

double** allocate_matrix(int n, int m) {
    double** matrix = new double*[n];
    for (int i = 0; i < n; i++) {
        matrix[i] = new double[m]();
    }
    return matrix;
}

void free_matrix(double** matrix, int n) {
    for (int i = 0; i < n; i++) {
        delete[] matrix[i];
    }
    delete[] matrix;
}

double** read_matrix_from_file(const char* filename, int n) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Cannot open file " << filename << endl;
        return nullptr;
    }

    double** matrix = allocate_matrix(n, n);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (!(file >> matrix[i][j])) {
                cerr << "Error: Invalid data format" << endl;
                free_matrix(matrix, n);
                return nullptr;
            }
        }
    }

    file.close();
    return matrix;
}

int my_max(int a, int b) {
    return (a > b) ? a : b;
}

double f(int k, int n, int i, int j) {
    switch (k) {
        case 1: return n - my_max(i, j) + 1;
        case 2: return my_max(i, j);
        case 3: return abs(i - j);
        case 4: return 1.0 / (i + j - 1);
        default: return 0;
    }
}

void* init_matrix_part(void* arg) {
    MatrixThreadData* data = (MatrixThreadData*)arg;

    for (int i = data->start_row; i < data->end_row; i++) {
        for (int j = 0; j < data->n; j++) {
            data->matrix[i][j] = f(data->k, data->n, i + 1, j + 1);
        }
    }

    return nullptr;
}

double** init_matrix_by_formula(int k, int n, int num_threads) {
    if (num_threads <= 0) num_threads = 1;
    if (num_threads > n) num_threads = n;

    double** matrix = allocate_matrix(n, n);

    pthread_t* threads = new pthread_t[num_threads];
    MatrixThreadData* thread_data = new MatrixThreadData[num_threads];

    int chunk_size = n / num_threads;

    for (int t = 0; t < num_threads; t++) {
        thread_data[t].matrix = matrix;
        thread_data[t].k = k;
        thread_data[t].n = n;
        thread_data[t].start_row = t * chunk_size;
        thread_data[t].end_row = (t == num_threads - 1) ? n : (t + 1) * chunk_size;

        pthread_create(&threads[t], nullptr, init_matrix_part, &thread_data[t]);
    }

    for (int t = 0; t < num_threads; t++) {
        pthread_join(threads[t], nullptr);
    }

    delete[] threads;
    delete[] thread_data;

    return matrix;
}

void print_matrix(double** matrix, int n, int m) {
    int print_size = (n < m) ? n : m;

    for (int i = 0; i < print_size; i++) {
        for (int j = 0; j < print_size; j++) {
            printf(" %10.3e", matrix[i][j]);
        }
        cout << endl;
    }
}

void* compute_partial_norm(void* arg) {
    NormThreadData* data = (NormThreadData*)arg;
    data->partial_norm = 0;

    for (int i = data->start_row; i < data->end_row; i++) {
        double row_sum = 0;
        for (int j = 0; j < data->n; j++) {
            row_sum += abs(data->A[i][j]);
        }
        if (row_sum > data->partial_norm) {
            data->partial_norm = row_sum;
        }
    }

    return nullptr;
}

double matrix_norm(double** A, int n, int num_threads) {
    if (num_threads <= 0) num_threads = 1;
    if (num_threads > n) num_threads = n;

    pthread_t* threads = new pthread_t[num_threads];
    NormThreadData* thread_data = new NormThreadData[num_threads];

    int chunk_size = n / num_threads;

    for (int t = 0; t < num_threads; t++) {
        thread_data[t].A = A;
        thread_data[t].n = n;
        thread_data[t].start_row = t * chunk_size;
        thread_data[t].end_row = (t == num_threads - 1) ? n : (t + 1) * chunk_size;

        pthread_create(&threads[t], nullptr, compute_partial_norm, &thread_data[t]);
    }

    double norm = 0;
    for (int t = 0; t < num_threads; t++) {
        pthread_join(threads[t], nullptr);
        if (thread_data[t].partial_norm > norm) {
            norm = thread_data[t].partial_norm;
        }
    }

    delete[] threads;
    delete[] thread_data;

    return norm;
}

void* compute_partial_product(void* arg) {
    ResidualThreadData* data = (ResidualThreadData*)arg;

    for (int i = data->start_row; i < data->end_row; i++) {
        for (int j = 0; j < data->n; j++) {
            data->temp[i][j] = 0;
            for (int k = 0; k < data->n; k++) {
                data->temp[i][j] += data->A[i][k] * data->A_inv[k][j];
            }
        }
    }

    return nullptr;
}

double residual_norm_inverse(double** A, double** A_inv, int n, int num_threads) {
    if (num_threads <= 0) num_threads = 1;
    if (num_threads > n) num_threads = n;

    double** temp = allocate_matrix(n, n);

    pthread_t* threads = new pthread_t[num_threads];
    ResidualThreadData* thread_data = new ResidualThreadData[num_threads];

    int chunk_size = n / num_threads;

    for (int t = 0; t < num_threads; t++) {
        thread_data[t].A = A;
        thread_data[t].A_inv = A_inv;
        thread_data[t].temp = temp;
        thread_data[t].n = n;
        thread_data[t].start_row = t * chunk_size;
        thread_data[t].end_row = (t == num_threads - 1) ? n : (t + 1) * chunk_size;

        pthread_create(&threads[t], nullptr, compute_partial_product, &thread_data[t]);
    }

    for (int t = 0; t < num_threads; t++) {
        pthread_join(threads[t], nullptr);
    }

    for (int i = 0; i < n; i++) {
        temp[i][i] -= 1.0;
    }

    double norm = matrix_norm(temp, n, num_threads);

    delete[] threads;
    delete[] thread_data;
    free_matrix(temp, n);

    return norm;
}
