#ifndef MATRIX_IO_H
#define MATRIX_IO_H

double** allocate_matrix(int n);
void free_matrix(double** matrix, int n);
double** read_matrix_from_file(const char* filename, int n);
double** init_matrix_by_formula(int k, int n);
void print_matrix(double** matrix, int n, int m);
double matrix_norm(double** A, int n);
double residual_norm_inverse(double** A, double** A_inv, int n);

#endif
