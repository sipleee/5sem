#include <iostream>
#include <ctime>
#include <cstdlib>
#include "matrix_io.h"
#include "solver.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 5) {
        cout << "Usage: " << argv[0] << " n m k threads [filename]" << endl;
        cout << "  n - matrix size" << endl;
        cout << "  m - print size" << endl;
        cout << "  k - matrix type (1-4) or 0 for file" << endl;
        cout << "  threads - number of threads (1, 2, 3, ...)" << endl;
        cout << "  filename - file with matrix (only if k=0)" << endl;
        return 1;
    }

    int n = atoi(argv[1]);
    int m = atoi(argv[2]);
    int k = atoi(argv[3]);
    int num_threads = atoi(argv[4]);

    if (n <= 0 || m <= 0 || num_threads <= 0) {
        cerr << "Error: Invalid parameters" << endl;
        return 1;
    }

    double** A = nullptr;

    if (k == 0) {
        if (argc < 6) {
            cerr << "Error: Need filename when k=0" << endl;
            return 1;
        }
        A = read_matrix_from_file(argv[5], n);
    }
    else {
        A = init_matrix_by_formula(k, n, num_threads);
    }

    if (A == nullptr) return 1;

    cout << "Matrix A (" << n << "x" << n << "):" << endl;
    print_matrix(A, n, m);
    cout << endl;

    double** A_inv = allocate_matrix(n, n);
    double** aug = allocate_matrix(n, 2 * n);

    cout << "Using " << num_threads << " thread(s)" << endl;
    clock_t start = clock();
    bool success = jordan_inverse(A, A_inv, aug, n, num_threads);
    clock_t end = clock();

    if (!success) {
        cerr << "Calculation failed!" << endl;
        free_matrix(A, n);
        free_matrix(A_inv, n);
        free_matrix(aug, n);
        return 1;
    }

    cout << "Inverse matrix:" << endl;
    print_matrix(A_inv, n, m);
    
    double residual = residual_norm_inverse(A, A_inv, n, num_threads);
    cout << "\nResidual norm ||A*A_inv - E||: " << scientific << residual << endl;
    cout << "Time: " << fixed << ((double)(end - start)) / CLOCKS_PER_SEC << " seconds" << endl;

    free_matrix(A, n);
    free_matrix(A_inv, n);
    free_matrix(aug, n);

    return 0;
}
