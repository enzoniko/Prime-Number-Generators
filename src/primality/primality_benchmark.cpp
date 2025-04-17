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
#include <algorithm>
#include <cmath>
#include <tuple>

/**
 * @brief Benchmark primality testing algorithms
 */
class PrimalityBenchmark {
private:
    // Bit sizes to benchmark
    const std::vector<int> bit_sizes = {40, 56, 80, 128, 168, 224, 256, 512, 1024, 2048, 4096};
    
    // Number of runs for statistical significance
    const int num_runs = 30;
    
    // Output file for CSV results
    const std::string find_prime_file = "results/find_prime_benchmark.csv";
    const std::string test_prime_file = "results/test_prime_benchmark.csv";
    
    // Global GMP random state
    gmp_randstate_t gmp_randstate;
    
    // Map to store found primes for testing
    std::map<int, mpz_t> found_primes;
    
    /**
     * @brief Calculate standard deviation
     * 
     * @param values Vector of values
     * @param mean Mean value
     * @return double Standard deviation
     */
    double calculate_stddev(const std::vector<double>& values, double mean) {
        double variance = 0.0;
        for (double value : values) {
            variance += (value - mean) * (value - mean);
        }
        return std::sqrt(variance / values.size());
    }
    
    /**
     * @brief Calculate median
     * 
     * @param values Vector of values
     * @return double Median value
     */
    double calculate_median(std::vector<double> values) {
        std::sort(values.begin(), values.end());
        if (values.size() % 2 == 0) {
            return (values[values.size() / 2 - 1] + values[values.size() / 2]) / 2.0;
        } else {
            return values[values.size() / 2];
        }
    }
    
    /**
     * @brief Find a prime number of the specified bit size
     * 
     * @param tester The PrimalityTester to use
     * @param type The type of primality test to use
     * @param bits The bit size of the prime to find
     * @param result Output parameter for the prime
     * @return Vector of timing data in milliseconds
     */
    std::vector<double> find_prime_timed(PrimalityTester& tester, PrimalityTester::TestType type, int bits, mpz_t result) {
        // Find the prime and time it
        auto start = std::chrono::high_resolution_clock::now();
        bool found = tester.find_prime(result, bits, type);
        auto end = std::chrono::high_resolution_clock::now();
        
        if (!found) {
            std::cerr << "Warning: Could not find a prime of " << bits << " bits" << std::endl;
            return {};
        }
        
        std::chrono::duration<double, std::milli> duration = end - start;
        
        // Store the first timing result
        std::vector<double> timings;
        timings.push_back(duration.count());
        
        // Run multiple times for statistical significance for all bit sizes
        // Always perform full prime finding operations, even for large bit sizes
        if (bits >= 2048) {
            std::cout << "  Running 29 more iterations for " << bits 
                      << "-bit prime (performing complete prime finding for each)..." << std::endl;
        }
        
        // For all bit sizes, find 29 more primes
        for (int i = 1; i < num_runs; i++) {
            mpz_t temp_result;
            mpz_init(temp_result);
            
            start = std::chrono::high_resolution_clock::now();
            tester.find_prime(temp_result, bits, type);
            end = std::chrono::high_resolution_clock::now();
            
            duration = end - start;
            timings.push_back(duration.count());
            
            mpz_clear(temp_result);
            
            // Print progress indicator every 5 iterations for larger bit sizes
            if (bits >= 2048 && i % 5 == 0) {
                std::cout << "    Completed " << i << " of " << (num_runs-1) 
                        << " additional iterations" << std::endl;
            }
        }
        
        return timings;
    }
    
    /**
     * @brief Test a number for primality and time the operation
     * 
     * @param tester The PrimalityTester to use
     * @param type The type of primality test to use
     * @param n The number to test
     * @return Vector of timing data in milliseconds
     */
    std::vector<double> test_primality_timed(PrimalityTester& tester, PrimalityTester::TestType type, const mpz_t n) {
        std::vector<double> timings;
        
        // Multiple runs for statistical significance
        for (int i = 0; i < num_runs; i++) {
            auto start = std::chrono::high_resolution_clock::now();
            tester.is_prime(n, type);
            auto end = std::chrono::high_resolution_clock::now();
            
            std::chrono::duration<double, std::milli> duration = end - start;
            timings.push_back(duration.count());
        }
        
        return timings;
    }
    
    /**
     * @brief Calculate statistics on timing data
     * 
     * @param timings Vector of timing measurements
     * @return std::tuple<double, double, double> Mean, median, and standard deviation
     */
    std::tuple<double, double, double> calculate_statistics(const std::vector<double>& timings) {
        if (timings.empty()) {
            return std::make_tuple(-1.0, -1.0, -1.0);
        }
        
        // Calculate mean
        double mean = 0.0;
        for (double time : timings) {
            mean += time;
        }
        mean /= timings.size();
        
        // Calculate median
        double median = calculate_median(timings);
        
        // Calculate standard deviation
        double stddev = calculate_stddev(timings, mean);
        
        return std::make_tuple(mean, median, stddev);
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
        results.push_back("Algorithm,BitSize,MeanTimeMs,MedianTimeMs,StdDevTimeMs,Prime");
        
        // Test both algorithms for finding primes
        for (int bits : bit_sizes) {
            // Benchmark Miller-Rabin
            std::cout << "Finding " << bits << "-bit prime using Miller-Rabin..." << std::endl;
            mpz_t mr_prime;
            mpz_init(mr_prime);
            std::vector<double> mr_timings = find_prime_timed(tester, PrimalityTester::MILLER_RABIN, bits, mr_prime);
            
            if (!mr_timings.empty()) {
                // Calculate statistics
                double mean_time, median_time, stddev_time;
                std::tie(mean_time, median_time, stddev_time) = calculate_statistics(mr_timings);
                
                // Store the found prime for later testing
                mpz_set(found_primes[bits], mr_prime);
                
                // Format result
                std::string prime_str = MPZUtils::to_string(mr_prime);
                // Truncate very large primes for CSV
                if (prime_str.length() > 50) {
                    prime_str = prime_str.substr(0, 24) + "..." + prime_str.substr(prime_str.length() - 24);
                }
                
                std::ostringstream result;
                result << "Miller-Rabin," << bits << "," 
                       << std::fixed << std::setprecision(6) << mean_time << ","
                       << std::fixed << std::setprecision(6) << median_time << ","
                       << std::fixed << std::setprecision(6) << stddev_time << ","
                       << prime_str;
                results.push_back(result.str());
                
                std::cout << "  Mean: " << mean_time << " ms, Median: " << median_time 
                          << " ms, StdDev: " << stddev_time << " ms" << std::endl;
            } else {
                results.push_back("Miller-Rabin," + std::to_string(bits) + ",failed,failed,failed,failed");
            }
            
            mpz_clear(mr_prime);
            
            // Benchmark Baillie-PSW
            std::cout << "Finding " << bits << "-bit prime using Baillie-PSW..." << std::endl;
            mpz_t bpsw_prime;
            mpz_init(bpsw_prime);
            std::vector<double> bpsw_timings = find_prime_timed(tester, PrimalityTester::BAILLIE_PSW, bits, bpsw_prime);
            
            if (!bpsw_timings.empty()) {
                // Calculate statistics
                double mean_time, median_time, stddev_time;
                std::tie(mean_time, median_time, stddev_time) = calculate_statistics(bpsw_timings);
                
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
                
                std::ostringstream result;
                result << "Baillie-PSW," << bits << "," 
                       << std::fixed << std::setprecision(6) << mean_time << ","
                       << std::fixed << std::setprecision(6) << median_time << ","
                       << std::fixed << std::setprecision(6) << stddev_time << ","
                       << prime_str;
                results.push_back(result.str());
                
                std::cout << "  Mean: " << mean_time << " ms, Median: " << median_time 
                          << " ms, StdDev: " << stddev_time << " ms" << std::endl;
            } else {
                results.push_back("Baillie-PSW," + std::to_string(bits) + ",failed,failed,failed,failed");
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
        results.push_back("Algorithm,BitSize,MeanTimeMs,MedianTimeMs,StdDevTimeMs");
        
        // Test both algorithms on the found primes
        for (int bits : bit_sizes) {
            // Skip if we don't have a prime for this bit size
            if (mpz_sgn(found_primes[bits]) == 0) {
                std::cout << "Skipping " << bits << " bits (no prime found)" << std::endl;
                continue;
            }
            
            // Benchmark Miller-Rabin
            std::cout << "Testing " << bits << "-bit prime using Miller-Rabin..." << std::endl;
            std::vector<double> mr_timings = test_primality_timed(tester, PrimalityTester::MILLER_RABIN, found_primes[bits]);
            
            double mr_mean, mr_median, mr_stddev;
            std::tie(mr_mean, mr_median, mr_stddev) = calculate_statistics(mr_timings);
            
            std::ostringstream mr_result;
            mr_result << "Miller-Rabin," << bits << "," 
                      << std::fixed << std::setprecision(6) << mr_mean << ","
                      << std::fixed << std::setprecision(6) << mr_median << ","
                      << std::fixed << std::setprecision(6) << mr_stddev;
            results.push_back(mr_result.str());
            
            std::cout << "  Mean: " << mr_mean << " ms, Median: " << mr_median 
                      << " ms, StdDev: " << mr_stddev << " ms" << std::endl;
            
            // Benchmark Baillie-PSW
            std::cout << "Testing " << bits << "-bit prime using Baillie-PSW..." << std::endl;
            std::vector<double> bpsw_timings = test_primality_timed(tester, PrimalityTester::BAILLIE_PSW, found_primes[bits]);
            
            double bpsw_mean, bpsw_median, bpsw_stddev;
            std::tie(bpsw_mean, bpsw_median, bpsw_stddev) = calculate_statistics(bpsw_timings);
            
            std::ostringstream bpsw_result;
            bpsw_result << "Baillie-PSW," << bits << "," 
                        << std::fixed << std::setprecision(6) << bpsw_mean << ","
                        << std::fixed << std::setprecision(6) << bpsw_median << ","
                        << std::fixed << std::setprecision(6) << bpsw_stddev;
            results.push_back(bpsw_result.str());
            
            std::cout << "  Mean: " << bpsw_mean << " ms, Median: " << bpsw_median 
                      << " ms, StdDev: " << bpsw_stddev << " ms" << std::endl;
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