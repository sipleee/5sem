#ifndef MATRIX_IO_H
#define MATRIX_IO_H

void init_matrix(int k, int n, double *A);

int read_matrix(const char *filename, int n, double *A);

void print_matrix_mxm(int n, const double *A, int m);

void print_eigs_prec(int n, const double *x, int m);

#endif
