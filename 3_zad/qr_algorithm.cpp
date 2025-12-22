#include "qr_algorithm.h"
#include "utils.h"        
#include <cmath>
#include <algorithm>

static inline double Aget(const double *A, int n, int i, int j) { return A[(size_t)i*n + j]; }
static inline void   Aset(double *A, int n, int i, int j, double v) { A[(size_t)i*n + j] = v; }

static void tridiagonalize_givens(double *A, int n, double *d, double *e)
{
    for (int j=0; j<=n-3; ++j) {
        for (int i=n-1; i>=j+2; --i) {
            double a = Aget(A,n,i-1,j);
            double b = Aget(A,n,i,  j);
            double c,s;
            givens(a,b,&c,&s);

            for (int k=j; k<n; ++k) {
                double x = Aget(A,n,i-1,k);
                double y = Aget(A,n,i,  k);
                Aset(A,n,i-1,k, c*x - s*y);
                Aset(A,n,i,  k, s*x + c*y);
            }
            for (int k=0; k<n; ++k) {
                double x = Aget(A,n,k,i-1);
                double y = Aget(A,n,k,i);
                Aset(A,n,k,i-1, c*x - s*y);
                Aset(A,n,k,i,   s*x + c*y);
            }

            Aset(A,n,i,j,0.0);
            Aset(A,n,j,i,0.0);
        }
    }

    for (int i=0;i<n;i++) d[i] = Aget(A,n,i,i);
    e[0] = 0.0;
    for (int i=1;i<n;i++) e[i] = Aget(A,n,i,i-1);
}

static int tqli(double *d, double *e, int n, double eps)
{
    for (int i=1;i<n;i++) e[i-1] = e[i];
    e[n-1] = 0.0;

    for (int l=0;l<n;l++) {
        int iter = 0;
        while (1) {
            int m = l;
            for (; m < n-1; ++m) {
                double dd = fabs(d[m]) + fabs(d[m+1]) + 1.0;
                if (fabs(e[m]) <= eps * dd) break;
            }
            if (m == l) break;

            if (++iter > 2000) return -1;

            double g = (d[l+1] - d[l]) / (2.0 * e[l]);
            double r = hypot(g, 1.0);
            g = d[m] - d[l] + e[l] / (g + (g >= 0.0 ? fabs(r) : -fabs(r)));

            double s = 1.0, c = 1.0, p = 0.0;
            for (int i=m-1; i>=l; --i) {
                double f = s * e[i];
                double b = c * e[i];

                r = hypot(f, g);
                if (r == 0.0) {
                    d[i+1] -= p;
                    e[m] = 0.0;
                    break;
                }
                e[i+1] = r;
                s = f / r;
                c = g / r;

                double di1 = d[i+1];
                double di  = d[i];

                g = di1 - p;
                double t = (di - g) * s + 2.0 * c * b;
                p = s * t;

                d[i+1] = g + p;
                g = c * t - b;
            }
            d[l] -= p;
            e[l] = g;
            e[m] = 0.0;
        }
    }
    return 0;
}

void solve(int n, double *A, double *x, double eps, double *work)
{
    double *d = work;
    double *e = work + n;

    tridiagonalize_givens(A, n, d, e);
    (void)tqli(d, e, n, eps);

    for (int i=0;i<n;i++) x[i] = d[i];
}
