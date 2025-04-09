#include <iostream>
#include <set>
#include <random>
#include <assert.h>

#include <chrono>

#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <cstdio>

#include "zipf.h"
#include "latest-generator.h"
#include "bplustree.h"

void Zipfian(const int write, const int read, Bplustree<Key>& bpt) {
    // Zipfian distribution generator
    init_zipf_generator(0, write);

    // Insert keys following Zipfian distribution
    auto w_start = Clock::now();
    for (int i = 1; i <= write; ++i) {
        Key key = nextValue() % write+1;        
        bpt.Insert(key);
    }
    auto w_end = Clock::now();
    std::cout << "After Insert\n";

    // Calculate insertion time
    float w_time = std::chrono::duration_cast<std::chrono::nanoseconds>(w_end - w_start).count() * 0.001;

    // Search for keys following Zipfian distribution
    auto r_start = Clock::now();
    for (int i = 1; i <= read; ++i) {
        Key key = nextValue() % read+1;
        bpt.Contains(key);
    }
    auto r_end = Clock::now();

    // Calculate search time
    float r_time = std::chrono::duration_cast<std::chrono::nanoseconds>(r_end - r_start).count() * 0.001;

    // Display results
    printf("\n[Zipfian] Insertion = %.2lf µs, Lookup = %.2lf µs\n", w_time, r_time);
}

void Uniform(const int write, const int read, Bplustree<Key>& bpt) {
    // Uniformly distributed random generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distr(1, write);

    // Insert random keys
    auto w_start = Clock::now();
    for (int i = 1; i <= write; ++i) {
        bpt.Insert(distr(gen)+1);
    }
    auto w_end = Clock::now();
    std::cout << "After Insert\n";

    // Calculate insertion time
    float w_time = std::chrono::duration_cast<std::chrono::nanoseconds>(w_end - w_start).count() * 0.001;

    // Search for random keys
    auto r_start = Clock::now();
    for (int i = 1; i <= read; ++i) {
        bpt.Contains(distr(gen)+1);
    }
    auto r_end = Clock::now();

    // Calculate search time
    float r_time = std::chrono::duration_cast<std::chrono::nanoseconds>(r_end - r_start).count() * 0.001;

    // Display results
    printf("\n[Uniform] Insertion = %.2lf µs, Lookup = %.2lf µs\n", w_time, r_time);
}

void RevSequential(const int write, const int read, Bplustree<Key>& bpt) {
    // Insert keys reverse sequentially
    auto w_start = Clock::now();
    for (int i = write; i > 0; i--) {
        bpt.Insert(i);
    }
    auto w_end = Clock::now();
    std::cout << "After Insert\n";

    // Calculate insertion time
    float w_time = std::chrono::duration_cast<std::chrono::nanoseconds>(w_end - w_start).count() * 0.001;

    // Search for keys reverse sequentially
    auto r_start = Clock::now();
    for (int i = read; i > 0; i--) {
        bpt.Contains(i);
    }
    auto r_end = Clock::now();

    // Calculate search time
    float r_time = std::chrono::duration_cast<std::chrono::nanoseconds>(r_end - r_start).count() * 0.001;

    // Display results
    printf("\n[Rev-Sequential] Insertion = %.2lf µs, Lookup = %.2lf µs\n", w_time, r_time);
}

void Sequential(const int write, const int read, Bplustree<Key>& bpt) {
    // Insert keys sequentially
    auto w_start = Clock::now();
    for (int i = 1; i <= write; ++i) {
        bpt.Insert(i);
    }
    auto w_end = Clock::now();
    std::cout << "After Insert\n";

    // Calculate insertion time
    float w_time = std::chrono::duration_cast<std::chrono::nanoseconds>(w_end - w_start).count() * 0.001;

    // Search for keys sequentially
    auto r_start = Clock::now();
    for (int i = 1; i <= read; ++i) {
        bpt.Contains(i);
    }
    auto r_end = Clock::now();

    // Calculate search time
    float r_time = std::chrono::duration_cast<std::chrono::nanoseconds>(r_end - r_start).count() * 0.001;

    // Display results
    printf("\n[Sequential] Insertion = %.2lf µs, Lookup = %.2lf µs\n", w_time, r_time);
}

void Zipfian_Delete(const int write, const int read, Bplustree<Key>& bpt) {
    // Zipfian distribution generator
    init_zipf_generator(0, write);

    // Insert keys following Zipfian distribution
    auto w_start = Clock::now();
    for (int i = 1; i <= write; ++i) {
        Key key = nextValue() % write+1;        
        bpt.Insert(key);
    }
    auto w_end = Clock::now();
    std::cout << "After Insert\n";

    // Calculate insertion time
    float w_time = std::chrono::duration_cast<std::chrono::nanoseconds>(w_end - w_start).count() * 0.001;

    // Delete for keys following Zipfian distribution
    auto r_start = Clock::now();
    for (int i = 1; i <= read; ++i) {
        Key key = nextValue() % read+1;
        bpt.Delete(key);
    }
    auto r_end = Clock::now();

    // Calculate search time
    float r_time = std::chrono::duration_cast<std::chrono::nanoseconds>(r_end - r_start).count() * 0.001;

    // Display results
    printf("\n[Zipfian Delete] Insertion = %.2lf µs, Deletion = %.2lf µs\n", w_time, r_time);
}

void Uniform_Delete(const int write, const int read, Bplustree<Key>& bpt) {
    // Uniformly distributed random generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distr(1, write);

    // Insert random keys
    auto w_start = Clock::now();
    for (int i = 1; i <= write; ++i) {
        bpt.Insert(distr(gen)+1);
    }
    auto w_end = Clock::now();
    std::cout << "After Insert\n";

    // Calculate insertion time
    float w_time = std::chrono::duration_cast<std::chrono::nanoseconds>(w_end - w_start).count() * 0.001;

    // Delete for random keys
    auto r_start = Clock::now();
    for (int i = 1; i <= read; ++i) {
        bpt.Delete(distr(gen)+1);
    }
    auto r_end = Clock::now();

    // Calculate search time
    float r_time = std::chrono::duration_cast<std::chrono::nanoseconds>(r_end - r_start).count() * 0.001;

    // Display results
    printf("\n[Uniform Delete] Insertion = %.2lf µs, Deletion = %.2lf µs\n", w_time, r_time);
}

void Uniform_Scan(const int write, const int read, Bplustree<Key> &bpt) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distr(0, write);

    auto w_start = Clock::now();
    for(int i = 1; i <= write; i++) {
        //Key key = distr(gen)+1;
        Key key = i;
        bpt.Insert(key);
    }
    auto w_end = Clock::now();
    printf("After Insert\n");
    auto r_start = Clock::now();
    for(int i = 1; i <= read; i++) {
        Key key = distr(gen)+1;
        bpt.Scan(key, 1000);
    }
    auto r_end = Clock::now();

    float r_time, w_time;
    r_time = std::chrono::duration_cast<std::chrono::nanoseconds>(r_end - r_start).count() * 0.001;
    w_time = std::chrono::duration_cast<std::chrono::nanoseconds>(w_end - w_start).count() * 0.001;
    printf("\n[Uniform-Scan] Insertion = %.2lf µs, Lookup = %.2lf µs\n", w_time, r_time);
}

void printUsage(const char* programName) {
    std::cerr << "\nUsage: " << programName << " [Write Count] [Read Count] [Benchmark #]\n\n"
              << "Benchmark can be selected by number or name.\n\n"
              << "Synthetic Benchmarks:\n"
              << " 0 - Sequential\n"
              << " 1 - Rev-Sequential\n"
              << " 2 - Uniform\n"
              << " 3 - Zipfian\n"
              << " 4 - Uniform Delete\n"
              << " 5 - Zipfian Delete\n"
              << " 6 - Scan\n";
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printUsage(argv[0]);
        return 1;
    }

    const int W = std::atoi(argv[1]);  // Insertion count
    const int R = std::atoi(argv[2]);  // Lookup count
    const int B = std::atoi(argv[3]);  // Benchmark type

    Bplustree<Key> bpt;

    auto runBenchmarkType1 = [&](const std::string& name, void (*benchmarkFunc)(int, int, Bplustree<Key>&)) {
        std::cout << "\n[" << name << " Benchmark in progress...]\n\n";
        benchmarkFunc(W, R, bpt);
    };

    switch (B) {
        // Type 1:
        case 0: runBenchmarkType1("Sequential", Sequential); break;
        case 1: runBenchmarkType1("Rev-Sequential", RevSequential); break;
        case 2: runBenchmarkType1("Uniform", Uniform); break;
        case 3: runBenchmarkType1("Zipfian", Zipfian); break;
        case 4: runBenchmarkType1("Uniform Delete", Uniform_Delete); break;
        case 5: runBenchmarkType1("Zipfian Delete", Zipfian_Delete); break;
        case 6: runBenchmarkType1("Scan", Uniform_Scan); break;

        default:
            std::cerr << "Invalid benchmark option provided.\n";
            printUsage(argv[0]);
            return 1;
    }

    return 0;
}