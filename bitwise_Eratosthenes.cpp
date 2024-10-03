// Group Members:
// Christopher Caponegro, Nahum Arnet
#include <iostream>
#include <thread>
#include <vector>
#include <cmath>
#include <mutex>

class prime_bits {
private:
    uint64_t n;  // the number of bits
    uint64_t num_words; // the number of 64-bit words needed to store
    uint64_t* p; // point to the dynamic memory for the bits

public:
    prime_bits(uint64_t n) : n(n+1), num_words(n/128+1), p(new uint64_t[n/128+1]()) {}

    ~prime_bits() {
        delete[] p;
    }

    prime_bits(const prime_bits& orig) = delete;
    prime_bits& operator=(const prime_bits& orig) = delete;

    void clear_prime(uint64_t i) {
        p[i / 128] |= (1LL << ((i%128) >> 1));
    }

    bool is_prime(uint64_t i) const {
        return (p[i / 128] & (1LL << ((i%128) >> 1))) == 0;
    }

    uint64_t eratosthenes(uint64_t size) {
        uint64_t count = 1; // 2 is a special case
        uint64_t lim = sqrt(size);
        for (uint64_t i = 3; i <= lim; i += 2) {
            if (is_prime(i)) {
                count++;
                for (uint64_t j = i * i; j <= size; j += 2 * i) {
                    clear_prime(j);
                }
            }
        }
        for (uint64_t i = (lim + 2) | 1; i <= size; i += 2) {
            if (is_prime(i)) {
                count++;
            }
        }
        return count;
    }

    uint64_t count_primes(uint64_t size) const {
        uint64_t count = 1; // counting 2
        for (uint64_t i = 3; i <= size; i += 2) {
            if (is_prime(i)) {
                count++;
            }
        }
        return count;
    }
};

// Parallel Sieve Worker Function
void sieve_worker(prime_bits& primes, uint64_t start, uint64_t end, const std::vector<uint64_t>& small_primes) {
    for (uint64_t prime : small_primes) {
        uint64_t first_multiple = (start / prime) * prime;
        if (first_multiple < prime * prime) {
            first_multiple = prime * prime;
        }
        if (first_multiple % 2 == 0) {
            first_multiple += prime;
        }
        for (uint64_t j = first_multiple; j <= end; j += 2 * prime) {
            primes.clear_prime(j);
        }
    }
}

// Parallelized Eratosthenes Function
void parallel_eratosthenes(prime_bits& primes, uint64_t n) {
    uint64_t lim = sqrt(n);
    std::vector<uint64_t> small_primes;

    // calculate primes up to sqrt(n)
    primes.eratosthenes(lim);
    for (uint64_t i = 3; i <= lim; i += 2) {
        if (primes.is_prime(i)) {
            small_primes.push_back(i);
        }
    }

    // Now split the work beyond sqrt(n) into multiple threads.
    uint64_t num_threads = 4;
    uint64_t chunk_size = (n - lim) / num_threads;
    std::vector<std::thread> threads;

    for (uint64_t t = 0; t < num_threads; ++t) {
        uint64_t start = lim + t * chunk_size + 1;
        if (start % 2 == 0) {
            start++;
        }
        uint64_t end = (t == num_threads - 1) ? n : start + chunk_size - 1;

        threads.emplace_back(sieve_worker, std::ref(primes), start, end, std::cref(small_primes));
    }

    // Join the threads.
    for (auto& thread : threads) {
        thread.join();
    }
}


// Usage:
// ./executebale_name (n)
// One arguement (n)
int main(int argc, char* argv[]) {
    if (argc < 2) {
		exit(-1);
	}
	uint64_t n = atol(argv[1]);
    //uint64_t n = 1e7;  // Limit for prime calculation
    //uint64_t n = 10000000;
    prime_bits primes(n);

    auto start_time = std::chrono::high_resolution_clock::now();
    parallel_eratosthenes(primes, n);
    auto end_time = std::chrono::high_resolution_clock::now();

    uint64_t count = primes.count_primes(n);
    std::chrono::duration<double> elapsed = end_time - start_time;

    std::cout << "multithreaded: " << count << " primes, elapsed: " << elapsed.count() << " usec\n";
    return 0;
}
