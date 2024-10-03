#include <iostream>
#include <thread>
#include <chrono>
#include <cmath>
using namespace std;

//HW1

// 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25
// 0  0  1  0  1  0  1  1  1   0   1   0   1   1   1   0                               1
// i=97
//O(log log n)

/*

void eratosthenes(bool primes[], uint64_t n) {
    uint64_t count = 0;
    for (uint64_t i = 2; i <= n; i++) {
        primes[i] = true;
    }

    for (uint64_t i = 2; i <= n; i++) {
        if (primes[i]) {
            count++;
            for (uint64_t j = i * i; j <= n; j += 2*i) {
                primes[j] = false;
            }
        }
    }
    
}

*/

void eratosthenes(bool* primes, uint64_t n) {
    for (uint64_t i = 2; i <= n; i++) { // set all values to true at frist
        primes[i] = true;
    }

    // Set non-primes to false
    for (uint64_t i = 2; i * i <= n; i++) { //iteration that you are on 
        if (primes[i]) { // if (true)
            for (uint64_t j = i * i; j <= n; j += i) { // all the mutiplies of i is then set to false 
                primes[j] = false;
            }
        }
    }
}

void countPrimeMutipleThreads(uint64_t a, uint64_t b, bool* primes, uint64_t* pcount) {
     uint64_t count = 0;
    for (uint64_t i = a; i <= b; i++) {
        if (primes[i]) {
            count++;
        }
    }
    *pcount = count;
}

uint64_t oneThreads(bool* primes, uint64_t n ) {
    uint64_t count1 = 0;
    thread t1(countPrimeMutipleThreads, 2, n, primes, &count1); // counts the amount of primes 1 - 50
    t1.join(); // joins


    return count1;

}

uint64_t testwoThreads(bool* primes, uint64_t n)  {
    uint64_t count1 = 0, count2 = 0;
    thread t1(countPrimeMutipleThreads, 2, n/2, primes, &count1); // counts the amount of primes 1 - 50
    thread t2(countPrimeMutipleThreads, n/2 + 1, n, primes, &count2); // count the amount from function 50 - 100
    t1.join(); // joins
    t2.join(); //joins

    return count1 + count2;

}

uint64_t testFourThreads(bool* primes, uint64_t n)  {
    uint64_t count1 = 0, count2 = 0, count3 = 0, count4 = 0;
    thread t1(countPrimeMutipleThreads, 2, n/4, primes, &count1); // counts the amount of primes 1 - 50
    thread t2(countPrimeMutipleThreads, n/4 + 1, n/2, primes, &count2);
    thread t3(countPrimeMutipleThreads, n/2 + 1, n*3/4, primes, &count3); // counts the amount of primes 1 - 50
    thread t4(countPrimeMutipleThreads, n*3/4 + 1, n, primes, &count4); // count the amount from function 50 - 100
    t1.join(); // joins
    t2.join(); //joins
    t3.join(); // joins
    t4.join(); //joins

    return count1 + count2 + count3 + count4;

}


int main(int argc, char* argcv[]) {
    uint64_t n = atol(argcv[1]); // reads in the user input (converting str to long int)
    bool* primes = new bool[n+1]; // dynamically allocating memory for array
    eratosthenes(primes,n); // checking to see the amount of primes using the eratosthenes function

    // for (int i = 2; i <= n; i++) {
    //     cout << i << ": " << (primes[i] ? "prime" : "not prime") << '\n';
    // } // checking to see if eratosthenes function is working properly

    auto t0 = std::chrono::high_resolution_clock::now();
    uint64_t count = oneThreads(primes, n); 
    auto t1 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
    cout << duration <<": This is the speed of one thread\n";
    cout << "count for One Thread: " << count << '\n';
    

    t0 = std::chrono::high_resolution_clock::now();
    count = testwoThreads(primes, n); 
    t1 = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
    cout << duration << ": This is the speed of four threads\n";
    cout << "count for Two Threads: " << count << '\n';

    
    t0 = std::chrono::high_resolution_clock::now();
    count = testFourThreads(primes, n); 
    t1 = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
    cout << duration << ": This is the speed of four threads\n";
    cout << "count for four Threads: " << count << '\n';

    delete[] primes;
       
}