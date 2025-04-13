#include "../../include/primality/primality_tester.h"
#include <iostream>
#include <string>
#include <chrono>
#include <stdexcept>
#include <cstring>
#include <cstdint>
#include <memory>
#include <x86intrin.h> // For _rdtsc()

/**
 * @brief Program to measure primality testing algorithm execution time with high precision
 * 
 * This program measures the execution time of testing the primality of numbers
 * using various algorithms at different bit sizes.
 * 
 * Usage: measure_primality_time <algorithm> <bits>
 *   algorithm: miller_rabin or baillie_psw
 *   bits: number of bits (e.g., 40, 56, 80, ..., 4096)
 */

// Function to get high-precision time using rdtsc
inline uint64_t rdtsc() {
    return __rdtsc();
}

// Use std::chrono for systems where rdtsc is not available
uint64_t high_resolution_time() {
    return std::chrono::high_resolution_clock::now().time_since_epoch().count();
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <algorithm> <bits>" << std::endl;
        std::cerr << "  algorithm: miller_rabin or baillie_psw" << std::endl;
        std::cerr << "  bits: number of bits (e.g., 40, 56, 80, ..., 4096)" << std::endl;
        return 1;
    }
    
    std::string algorithm = argv[1];
    int bits;
    
    try {
        bits = std::stoi(argv[2]);
    } catch (const std::exception& e) {
        std::cerr << "Error: Invalid bit size: " << argv[2] << std::endl;
        return 1;
    }
    
    if (bits <= 0) {
        std::cerr << "Error: Bit size must be positive" << std::endl;
        return 1;
    }
    
    // Create the primality tester
    PrimalityTester tester;
    PrimalityTester::TestType test_type;
    
    if (algorithm == "miller_rabin") {
        test_type = PrimalityTester::MILLER_RABIN;
    } else if (algorithm == "baillie_psw") {
        test_type = PrimalityTester::BAILLIE_PSW;
    } else {
        std::cerr << "Error: Unknown algorithm: " << algorithm << std::endl;
        std::cerr << "Supported algorithms: miller_rabin, baillie_psw" << std::endl;
        return 1;
    }
    
    // Generate a prime number of the specified bit size
    mpz_t prime;
    mpz_init(prime);
    
    // Find a prime of the specified bit size
    tester.generate_prime(prime, bits);
    
    // Warm-up runs
    for (int i = 0; i < 3; i++) {
        tester.is_prime(prime, test_type);
    }
    
    // Measure time
    uint64_t start_time = rdtsc();
    
    // Call the algorithm
    tester.is_prime(prime, test_type);
    
    uint64_t end_time = rdtsc();
    uint64_t elapsed_cycles = end_time - start_time;
    
    // Output just the raw measurement (to be processed by the calling script)
    std::cout << elapsed_cycles << std::endl;
    
    // Clean up
    mpz_clear(prime);
    
    return 0;
} 