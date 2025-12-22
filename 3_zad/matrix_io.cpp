#include "matrix_io.h"
#include <cstdio>
#include <cstdlib>
#include <cmath>

static double f_ij(int k, int n, int i1, int j1) {
    if (k == 1) {
        int mx = (i1 > j1 ? i1 : j1);
        return (double)(n - mx + 1);
    }
    if (k == 2) {
        if (i1 == j1) return 2.0;
        if (std::abs(i1 - j1) == 1) return -1.0;
        return 0.0;
    }
    if (k == 3) {
        if (i1 == j1 && i1 < n) return 1.0;
        if (j1 == n) return (double)i1;
        if (i1 == n) return (double)j1;
        return 0.0;
    }

    return 1.0 / (double)(i1 + j1 - 1);
}

void init_matrix(int k, int n, double *A) {
    for (int i=0; i<n; i++)
        for (int j=0; j<n; j++)
            A[(size_t)i*n + j] = f_ij(k, n, i+1, j+1);
}

int read_matrix(const char *filename, int n, double *A) {
    FILE *f = fopen(filename, "r");
    if (!f) return -1;
    for (int i=0; i<n; i++){
        for (int j=0; j<n; j++){
            if (fscanf(f, "%lf", &A[(size_t)i*n + j]) != 1) {
                fclose(f);
                return -2;
            }
        }
    }
    fclose(f);
    return 0;
}

void print_matrix_mxm(int n, const double *A, int m) {
    int lim = (n < m ? n : m);
    for (int i=0; i<lim; i++){
        for (int j=0; j<lim; j++){
            printf(" %10.3e", A[(size_t)i*n + j]);
        }
        printf("\n");
    }
}

void print_eigs_prec(int n, const double *x, int m) {
    int lim = (n < m ? n : m);
    for (int i=0; i<lim; i++){
        printf(" %.15e\n", x[i]);
    }
}
