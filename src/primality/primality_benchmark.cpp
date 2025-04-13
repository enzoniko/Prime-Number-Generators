#include "../../include/primality/primality_tester.h"
#include "../../include/prng/xoshiro.h"
#include "../../include/utils/timing.h"
#include "../../include/utils/mpz_utils.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <fstream>
#include <chrono>
#include <map>

/**
 * @brief Benchmark primality testing algorithms
 */
class PrimalityBenchmark {
private:
    // Bit sizes to benchmark
    const std::vector<int> bit_sizes = {40, 56, 80, 128, 168, 224, 256, 512, 1024, 2048, 4096};
    
    // Output file for CSV results
    const std::string find_prime_file = "results/find_prime_benchmark.csv";
    const std::string test_prime_file = "results/test_prime_benchmark.csv";
    
    // Global GMP random state
    gmp_randstate_t gmp_randstate;
    
    // Map to store found primes for testing
    std::map<int, mpz_t> found_primes;
    
    /**
     * @brief Find a prime number of the specified bit size
     * 
     * @param tester The PrimalityTester to use
     * @param type The type of primality test to use
     * @param bits The bit size of the prime to find
     * @param result Output parameter for the prime
     * @return double Time taken to find the prime in milliseconds
     */
    double find_prime_timed(PrimalityTester& tester, PrimalityTester::TestType type, int bits, mpz_t result) {
        auto start = std::chrono::high_resolution_clock::now();
        bool found = tester.find_prime(result, bits, type);
        auto end = std::chrono::high_resolution_clock::now();
        
        if (!found) {
            std::cerr << "Warning: Could not find a prime of " << bits << " bits" << std::endl;
            return -1.0;
        }
        
        std::chrono::duration<double, std::milli> duration = end - start;
        return duration.count();
    }
    
    /**
     * @brief Test a number for primality and time the operation
     * 
     * @param tester The PrimalityTester to use
     * @param type The type of primality test to use
     * @param n The number to test
     * @return double Time taken to test the primality in milliseconds
     */
    double test_primality_timed(PrimalityTester& tester, PrimalityTester::TestType type, const mpz_t n) {
        const int num_runs = 10;  // Number of runs to average over
        
        auto test_func = [&]() {
            tester.is_prime(n, type);
        };
        
        return TimingUtils::measure_average_time_ms(test_func, num_runs);
    }
    
public:
    /**
     * @brief Constructor
     * 
     * Initializes the GMP random state
     */
    PrimalityBenchmark() {
        unsigned long seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        gmp_randinit_mt(gmp_randstate);
        gmp_randseed_ui(gmp_randstate, seed);
        
        // Initialize the map of found primes
        for (int bits : bit_sizes) {
            mpz_init(found_primes[bits]);
        }
    }
    
    /**
     * @brief Destructor
     * 
     * Cleans up the GMP random state and found primes
     */
    ~PrimalityBenchmark() {
        gmp_randclear(gmp_randstate);
        
        // Clear the map of found primes
        for (auto& pair : found_primes) {
            mpz_clear(pair.second);
        }
    }
    
    /**
     * @brief Benchmark finding prime numbers
     */
    void benchmark_find_prime() {
        std::cout << "Benchmarking prime number generation..." << std::endl;
        
        PrimalityTester tester;
        std::vector<std::string> results;
        
        // Add CSV header
        results.push_back("Algorithm,BitSize,TimeMs,Prime");
        
        // Test both algorithms for finding primes
        for (int bits : bit_sizes) {
            // Benchmark Miller-Rabin
            std::cout << "Finding " << bits << "-bit prime using Miller-Rabin..." << std::endl;
            mpz_t mr_prime;
            mpz_init(mr_prime);
            double mr_time = find_prime_timed(tester, PrimalityTester::MILLER_RABIN, bits, mr_prime);
            
            if (mr_time >= 0) {
                // Store the found prime for later testing
                mpz_set(found_primes[bits], mr_prime);
                
                // Format result
                std::string prime_str = MPZUtils::to_string(mr_prime);
                // Truncate very large primes for CSV
                if (prime_str.length() > 50) {
                    prime_str = prime_str.substr(0, 24) + "..." + prime_str.substr(prime_str.length() - 24);
                }
                results.push_back("Miller-Rabin," + std::to_string(bits) + "," + 
                                 std::to_string(mr_time) + "," + prime_str);
            } else {
                results.push_back("Miller-Rabin," + std::to_string(bits) + ",failed,failed");
            }
            
            mpz_clear(mr_prime);
            
            // Benchmark Baillie-PSW
            std::cout << "Finding " << bits << "-bit prime using Baillie-PSW..." << std::endl;
            mpz_t bpsw_prime;
            mpz_init(bpsw_prime);
            double bpsw_time = find_prime_timed(tester, PrimalityTester::BAILLIE_PSW, bits, bpsw_prime);
            
            if (bpsw_time >= 0) {
                // If we don't already have a prime for this bit size, store this one
                if (mpz_sgn(found_primes[bits]) == 0) {
                    mpz_set(found_primes[bits], bpsw_prime);
                }
                
                // Format result
                std::string prime_str = MPZUtils::to_string(bpsw_prime);
                // Truncate very large primes for CSV
                if (prime_str.length() > 50) {
                    prime_str = prime_str.substr(0, 24) + "..." + prime_str.substr(prime_str.length() - 24);
                }
                results.push_back("Baillie-PSW," + std::to_string(bits) + "," + 
                                 std::to_string(bpsw_time) + "," + prime_str);
            } else {
                results.push_back("Baillie-PSW," + std::to_string(bits) + ",failed,failed");
            }
            
            mpz_clear(bpsw_prime);
        }
        
        // Write results to file
        std::ofstream out(find_prime_file);
        if (!out) {
            std::cerr << "Error: Could not open output file " << find_prime_file << std::endl;
            return;
        }
        
        for (const auto& line : results) {
            out << line << std::endl;
        }
        
        out.close();
        
        std::cout << "Prime finding benchmark results written to " << find_prime_file << std::endl;
    }
    
    /**
     * @brief Benchmark primality testing time on found primes
     */
    void benchmark_test_prime() {
        std::cout << "Benchmarking primality testing on found primes..." << std::endl;
        
        PrimalityTester tester;
        std::vector<std::string> results;
        
        // Add CSV header
        results.push_back("Algorithm,BitSize,TimeMs");
        
        // Test both algorithms on the found primes
        for (int bits : bit_sizes) {
            // Skip if we don't have a prime for this bit size
            if (mpz_sgn(found_primes[bits]) == 0) {
                std::cout << "Skipping " << bits << " bits (no prime found)" << std::endl;
                continue;
            }
            
            // Benchmark Miller-Rabin
            std::cout << "Testing " << bits << "-bit prime using Miller-Rabin..." << std::endl;
            double mr_time = test_primality_timed(tester, PrimalityTester::MILLER_RABIN, found_primes[bits]);
            results.push_back("Miller-Rabin," + std::to_string(bits) + "," + std::to_string(mr_time));
            
            // Benchmark Baillie-PSW
            std::cout << "Testing " << bits << "-bit prime using Baillie-PSW..." << std::endl;
            double bpsw_time = test_primality_timed(tester, PrimalityTester::BAILLIE_PSW, found_primes[bits]);
            results.push_back("Baillie-PSW," + std::to_string(bits) + "," + std::to_string(bpsw_time));
        }
        
        // Write results to file
        std::ofstream out(test_prime_file);
        if (!out) {
            std::cerr << "Error: Could not open output file " << test_prime_file << std::endl;
            return;
        }
        
        for (const auto& line : results) {
            out << line << std::endl;
        }
        
        out.close();
        
        std::cout << "Primality testing benchmark results written to " << test_prime_file << std::endl;
    }
    
    /**
     * @brief Run all benchmarks
     */
    void run() {
        benchmark_find_prime();
        benchmark_test_prime();
        
        std::cout << "All benchmarks completed." << std::endl;
    }
};

int main(int argc, char* argv[]) {
    PrimalityBenchmark benchmark;
    benchmark.run();
    return 0;
} 