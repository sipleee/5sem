#include "utils.h"
#include <cmath>

void givens(double a, double b, double *c, double *s) {
    double r = hypot(a,b);
    if (r == 0.0) { *c = 1.0; *s = 0.0; return; }
    *c = a / r;
    *s = -b / r;
}

void residuals_invariants(int n, const double *A0, const double *eig, double *r1, double *r2) {
    double trA = 0.0, sumEig = 0.0;
    double normA2 = 0.0, sumEig2 = 0.0;
    
    for (int i=0; i<n; i++){
        trA += A0[(size_t)i*n + i];
        sumEig += eig[i];
        sumEig2 += eig[i]*eig[i];
    }
    for (int i=0; i<n; i++){
        for (int j=0; j<n; j++){
            double v = A0[(size_t)i*n + j];
            normA2 += v*v;
        }
    }
    *r1 = fabs(trA - sumEig);
    *r2 = fabs(normA2 - sumEig2);
}
