#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include "matrix_io.h"

using namespace std;

double** allocate_matrix(int n, int m) {
    double** matrix = new double*[n];
    for (int i = 0; i < n; i++) {
        matrix[i] = new double[m];
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

double** init_matrix_by_formula(int k, int n) {
    double** matrix = allocate_matrix(n, n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matrix[i][j] = f(k, n, i + 1, j + 1);
        }
    }
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

double matrix_norm(double** A, int n) {
    double norm = 0;
    for (int i= 0; i < n;i++) {
        double row_sum = 0;
        for (int j= 0; j< n; j++){
            row_sum += abs(A[i][j]);
        }
        if (row_sum > norm) norm = row_sum;
    }
    return norm;
}

void matrix_norm2(double** A, double* a, int n) {
    for (int i = 0; i < n;i++) {
        for (int j = 0; j < n; j++) {
            a[i] += abs(A[i][j]);
        }
    }
}

double matrix_norm3(double* a, int n) {
    double row_sum = a[0];
    for (int i = 0; i < n; i++) {
        if (a[i] > row_sum) row_sum = a[i];
    }
    return row_sum;
}


double residual_norm_inverse(double** A, double** A_inv, int n) {
    double** temp = allocate_matrix(n, n);
    double* a = new double[n];
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            temp[i][j] = 0;
            for (int k = 0; k < n; k++) {
                temp[i][j] += A[i][k] * A_inv[k][j];
            }
        }
    }
    
    for (int i = 0; i < n; i++) {
        temp[i][i] -= 1.0;
    }
    
    matrix_norm2(temp, a, n);
    double norm = matrix_norm3(a, n);
    free_matrix(temp, n);
    return norm;
}
