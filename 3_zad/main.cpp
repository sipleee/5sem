#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include "matrix_io.h"
#include "qr_algorithm.h"
#include "utils.h"

int main(int argc, char **argv)
{
    if (argc != 5 && argc != 6) {
        fprintf(stderr, "Usage: %s n m eps k [filename]\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    int m = atoi(argv[2]);
    double eps = atof(argv[3]);
    int k = atoi(argv[4]);

    if (n<=0 || m<=0 || eps<=0.0 || k<0 || k>4) {
        fprintf(stderr, "Bad args\n");
        return 1;
    }
    if (k==0 && argc!=6) {
        fprintf(stderr, "filename required when k=0\n");
        return 1;
    }

    double *A  = (double*)malloc((size_t)n*n*sizeof(double));
    double *A0 = (double*)malloc((size_t)n*n*sizeof(double));
    double *x  = (double*)malloc((size_t)n*sizeof(double));
    double *work = (double*)malloc((size_t)2*n*sizeof(double));

    if (!A || !A0 || !x || !work) {
        fprintf(stderr, "No memory\n");
        if(A) free(A); if(A0) free(A0); if(x) free(x); if(work) free(work);
        return 2;
    }

    if (k==0) {
        int rc = read_matrix(argv[5], n, A);
        if (rc != 0) {
            fprintf(stderr, "Cannot read file / bad format\n");
            free(A); free(A0); free(x); free(work);
            return 3;
        }
    } else {
        init_matrix(k, n, A);
    }

    memcpy(A0, A, (size_t)n*n*sizeof(double));

    printf("A:\n");
    print_matrix_mxm(n, A, m);

    clock_t t0 = clock();
    solve(n, A, x, eps, work);
    clock_t t1 = clock();

    printf("eigenvalues (increased precision):\n");
    print_eigs_prec(n, x, m);

    double r1, r2;
    residuals_invariants(n, A0, x, &r1, &r2);
    printf("residual1 = %.15e\n", r1);
    printf("residual2 = %.15e\n", r2);

    double sec = (double)(t1 - t0) / (double)CLOCKS_PER_SEC;
    printf("time = %.2f\n", sec);

    free(A); free(A0); free(x); free(work);
    return 0;
}
