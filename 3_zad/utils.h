#ifndef UTILS_H
#define UTILS_H

void givens(double a, double b, double *c, double *s);

void residuals_invariants(int n, const double *A0, const double *eig, double *r1, double *r2);

#endif
