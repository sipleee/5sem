#include <iostream>
#include <cmath>
#include "solver.h"

using namespace std;

bool jordan_inverse(double** A, double** A_inv, int n) {
    double** aug = new double* [n];
    for (int i = 0; i < n; i++) {
        aug[i] = new double[2 * n];

        for (int j = 0; j < n; j++) {
            aug[i][j] = A[i][j];
        }

        for (int j = n; j < 2 * n; j++) {
            aug[i][j] = (j - n == i) ? 1.0 : 0.0;
        }
    }

    for (int k = 0; k < n; k++) {
        if (fabs(aug[k][k]) < 1e-15) {
            cerr << "Error: Matrix is singular at step " << k + 1
                << " (a_{" << k + 1 << k + 1 << "} = " << aug[k][k] << ")" << endl;

            for (int i = 0; i < n; i++) delete[] aug[i];
            delete[] aug;
            return false;
        }

        double pivot = aug[k][k];
        for (int j = k; j < 2 * n; j++) {
            aug[k][j] /= pivot;
        }

        for (int i = 0; i < n; i++) {
            if (i != k) {
                double factor = aug[i][k];
                for (int j = k; j < 2 * n; j++) {
                    aug[i][j] -= factor * aug[k][j];
                }
            }
        }

        // Отладочный вывод для маленьких матриц
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

    for (int i = 0; i < n; i++) {
        delete[] aug[i];
    }
    delete[] aug;

    return true;
}
