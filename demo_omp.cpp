#include <omp.h>
#include <iostream>
#include <vector>
#include <chrono>

using namespace std;
using namespace std::chrono;

/*
Christopher Caponegro and Nahum jay
Sources Codium, Stack Overflow


1. Results (laptop has 8 cores)
Number of cores: 8
Benchmark with identity matrices:
Threads: 1 Time: 3832 ms
Threads: 2 Time: 1856 ms
Threads: 4 Time: 972 ms
Threads: 8 Time: 664 ms
Threads: 16 Time: 666 ms

2.
Benchmark with identity matrices:
Threads: 1 Time: 3832 ms
Threads: 2 Time: 1856 ms
Threads: 4 Time: 972 ms
Threads: 8 Time: 664 ms
Threads: 16 Time: 666 ms

Benchmark with diagonal matrices (3s and 2s):
Threads: 1 Time: 3435 ms
Threads: 2 Time: 2129 ms
Threads: 4 Time: 933 ms
Threads: 8 Time: 664 ms
Threads: 16 Time: 673 ms

3. As you can see sequentially reading and writing is quicker here: compared to previous results:
Benchmark with transposed B:
Threads: 1 Time: 1427 ms
Threads: 2 Time: 712 ms
Threads: 4 Time: 369 ms
Threads: 8 Time: 211 ms
Threads: 16 Time: 227 ms

4. Add vectorization. You can put all functions in a single file.
Benchmark with vectorized transposed B:
Threads: 1 Time: 1142 ms
Threads: 2 Time: 570 ms
Threads: 4 Time: 303 ms
Threads: 8 Time: 161 ms
Threads: 16 Time: 190 ms

5. My computer had 8 threads. When increasing to to 16 threads we noticed that it the performance decreased very slightly
because the amount of threads exceed the amount cores. Based on the results hyperthreading was not effective.

*/

// Multithreaded matrix multiplication using OpenMP
void multiplymatrix_omp(float a[], float b[], float c[], int n, int num_threads) {
    omp_set_num_threads(num_threads);
    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        //#pragma omp parallel for
        for (int j = 0; j < n; j++) {
            float dot = 0.0;
            //#pragma omp parallel for
            for (int k = 0; k < n; k++) {
                dot += a[i * n + k] * b[k * n + j];
            }
            c[i * n + j] = dot;
        }
    }
}

// Matrix transpose function
void transposematrix(float b[], float bt[], int n) {
    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            bt[j * n + i] = b[i * n + j];
        }
    }
}

// Multiply matrix A with transposed B
void multiplymatrix_transposed(float a[], float bt[], float c[], int n, int num_threads) {
    omp_set_num_threads(num_threads);
    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            float dot = 0.0;
            for (int k = 0; k < n; k++) {
                dot += a[i * n + k] * bt[j * n + k];
            }
            c[i * n + j] = dot;
        }
    }
}

// Vectorized transpose function using OpenMP
void transposematrix_vectorized(float b[], float bt[], int n) {
    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        #pragma omp simd
        for (int j = 0; j < n; j++) {
            bt[j * n + i] = b[i * n + j];
        }
    }
}



// Benchmarking function
void benchmark(void (*multiply_fn)(float[], float[], float[], int, int), float a[], float b[], float c[], int n, int num_threads) {
    auto start = high_resolution_clock::now();
    multiply_fn(a, b, c, n, num_threads);
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start).count();
    cout << "Threads: " << num_threads << " Time: " << duration << " ms" << endl;
}

int main() {
    int n = 1024;
    int num_cores = omp_get_num_procs();
    cout << "Number of cores: " << num_cores << endl;

    // Create matrices
    vector<float> a(n * n, 0), b(n * n, 0), c(n * n, 0), bt(n * n, 0);

    // Identity matrix
    for (int i = 0; i < n; i++) {
        a[i * n + i] = 1.0;
        b[i * n + i] = 1.0;
    }

    // Test with identity matrices 
    cout << "Benchmark with identity matrices:" << endl;
    benchmark(multiplymatrix_omp, a.data(), b.data(), c.data(), n, 1);
    benchmark(multiplymatrix_omp, a.data(), b.data(), c.data(), n, 2);
    benchmark(multiplymatrix_omp, a.data(), b.data(), c.data(), n, 4);
    benchmark(multiplymatrix_omp, a.data(), b.data(), c.data(), n, 8);
    benchmark(multiplymatrix_omp, a.data(), b.data(), c.data(), n, 16);

    // Set diagonals to 3 and other elements to 2
    fill(a.begin(), a.end(), 2.0);
    fill(b.begin(), b.end(), 2.0);
    for (int i = 0; i < n; i++) {
        a[i * n + i] = 3.0;
        b[i * n + i] = 3.0;
    }

    cout << "Benchmark with diagonal matrices (3s and 2s):" << endl;
    benchmark(multiplymatrix_omp, a.data(), b.data(), c.data(), n, 1);
    benchmark(multiplymatrix_omp, a.data(), b.data(), c.data(), n, 2);
    benchmark(multiplymatrix_omp, a.data(), b.data(), c.data(), n, 4);
    benchmark(multiplymatrix_omp, a.data(), b.data(), c.data(), n, 8);
    benchmark(multiplymatrix_omp, a.data(), b.data(), c.data(), n, 16);

    // Test with transposed B
    transposematrix(b.data(), bt.data(), n);

    cout << "Benchmark with transposed B:" << endl;
    benchmark(multiplymatrix_transposed, a.data(), bt.data(), c.data(), n, 1);
    benchmark(multiplymatrix_transposed, a.data(), bt.data(), c.data(), n, 2);
    benchmark(multiplymatrix_transposed, a.data(), bt.data(), c.data(), n, 4);
    benchmark(multiplymatrix_transposed, a.data(), bt.data(), c.data(), n, 8);
    benchmark(multiplymatrix_transposed, a.data(), bt.data(), c.data(), n, 16);


    // Test with vectorized transpose
    transposematrix_vectorized(b.data(), bt.data(), n);
    cout << "Benchmark with vectorized transposed B:" << endl;
    benchmark(multiplymatrix_transposed, a.data(), bt.data(), c.data(), n, 1);
    benchmark(multiplymatrix_transposed, a.data(), bt.data(), c.data(), n, 2);
    benchmark(multiplymatrix_transposed, a.data(), bt.data(), c.data(), n, 4);
    benchmark(multiplymatrix_transposed, a.data(), bt.data(), c.data(), n, 8);
    benchmark(multiplymatrix_transposed, a.data(), bt.data(), c.data(), n, 16);


    return 0;
}
