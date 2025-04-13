#include "../../include/prng/lcg.h"
#include "../../include/prng/xoshiro.h"
#include <iostream>
#include <string>
#include <chrono>
#include <stdexcept>
#include <cstring>
#include <cstdint>
#include <memory>
#include <x86intrin.h> // For _rdtsc()

/**
 * @brief Program to measure PRNG algorithm execution time with high precision
 * 
 * This program measures the execution time of generating random numbers
 * with various PRNG algorithms at different bit sizes.
 * 
 * Usage: measure_prng_time <algorithm> <bits>
 *   algorithm: lcg or xoshiro
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
        std::cerr << "  algorithm: lcg or xoshiro" << std::endl;
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
    
    // Fixed seed for repeatability
    uint64_t seed = 12345678901234ULL;
    
    // Create the PRNG
    std::unique_ptr<PRNGInterface> prng;
    
    if (algorithm == "lcg") {
        prng = std::make_unique<LCG>(seed);
    } else if (algorithm == "xoshiro") {
        prng = std::make_unique<Xoshiro256pp>(seed);
    } else {
        std::cerr << "Error: Unknown algorithm: " << algorithm << std::endl;
        std::cerr << "Supported algorithms: lcg, xoshiro" << std::endl;
        return 1;
    }
    
    // Initialize GMP integer
    mpz_t result;
    mpz_init(result);
    
    // Warm-up runs
    for (int i = 0; i < 3; i++) {
        prng->randbits(result, bits);
    }
    
    // Measure time
    uint64_t start_time = rdtsc();
    
    // Call the algorithm
    prng->randbits(result, bits);
    
    uint64_t end_time = rdtsc();
    uint64_t elapsed_cycles = end_time - start_time;
    
    // Output just the raw measurement (to be processed by the calling script)
    std::cout << elapsed_cycles << std::endl;
    
    // Clean up
    mpz_clear(result);
    
    return 0;
} 