#include "../../include/prng/lcg.h"
#include "../../include/prng/xoshiro.h"
#include "../../include/primality/primality_tester.h"
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <atomic>
#include <iomanip>
#include <stdexcept>
#include <cstring>
#include <cstdint>
#include <csignal>
#include <memory>

/**
 * @brief Program to continuously run algorithms for energy consumption measurement
 * 
 * This program continuously runs the specified algorithm for a given duration,
 * periodically reporting statistics to allow energy consumption measurement.
 * 
 * Usage: continuous_operation <algorithm> <bits> <duration_seconds>
 *   algorithm: lcg, xoshiro, miller_rabin, or baillie_psw
 *   bits: number of bits (e.g., 40, 56, 80, ..., 4096)
 *   duration_seconds: how long to run in seconds
 */

// Global flag for graceful termination
std::atomic<bool> g_running(true);

// Signal handler for Ctrl+C
void signal_handler(int signal) {
    if (signal == SIGINT) {
        g_running = false;
    }
}

// Function to run a PRNG continuously
void run_prng(PRNGInterface& prng, int bits, int duration_seconds) {
    mpz_t number;
    mpz_init(number);
    
    uint64_t iterations = 0;
    auto start_time = std::chrono::steady_clock::now();
    auto next_report = start_time + std::chrono::seconds(1);
    auto end_time = start_time + std::chrono::seconds(duration_seconds);
    
    std::cout << "Running PRNG for " << duration_seconds << " seconds with " << bits << " bits" << std::endl;
    std::cout << "Progress: " << std::setw(6) << "0%" << " | Rate: " << std::setw(12) << "0" << " ops/sec\r" << std::flush;
    
    // Set up for statistics
    const int stats_interval = 10; // seconds
    auto next_stat = start_time + std::chrono::seconds(stats_interval);
    uint64_t interval_iterations = 0;
    
    while (g_running && std::chrono::steady_clock::now() < end_time) {
        // Generate a random number
        prng.randbits(number, bits);
        iterations++;
        interval_iterations++;
        
        // Report progress every second
        auto now = std::chrono::steady_clock::now();
        if (now >= next_report) {
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start_time).count();
            double progress = static_cast<double>(elapsed) / duration_seconds * 100.0;
            double rate = static_cast<double>(iterations) / elapsed;
            
            std::cout << "Progress: " << std::fixed << std::setprecision(2)
                      << std::setw(6) << progress << "% | Rate: " 
                      << std::setw(12) << static_cast<uint64_t>(rate) << " ops/sec\r" << std::flush;
            
            next_report = now + std::chrono::seconds(1);
        }
        
        // Output detailed statistics every stats_interval seconds
        if (now >= next_stat) {
            auto interval = std::chrono::duration_cast<std::chrono::seconds>(now - (next_stat - std::chrono::seconds(stats_interval))).count();
            double interval_rate = static_cast<double>(interval_iterations) / interval;
            
            std::cout << std::endl << "STAT: " 
                      << std::chrono::duration_cast<std::chrono::seconds>(now - start_time).count() 
                      << "s - Rate: " << std::setw(12) << static_cast<uint64_t>(interval_rate) 
                      << " ops/sec" << std::endl;
            
            next_stat = now + std::chrono::seconds(stats_interval);
            interval_iterations = 0;
        }
    }
    
    auto final_time = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(final_time - start_time).count();
    double rate = static_cast<double>(iterations) / elapsed;
    
    std::cout << std::endl << "Completed " << iterations << " iterations in "
              << elapsed << " seconds (" << static_cast<uint64_t>(rate) << " ops/sec)" << std::endl;
    
    mpz_clear(number);
}

// Function to run a primality test continuously
void run_primality(PrimalityTester& tester, PrimalityTester::TestType type, int bits, int duration_seconds) {
    // First, generate a prime number of the specified bit size
    mpz_t prime;
    mpz_init(prime);
    
    std::cout << "Generating a " << bits << "-bit prime for testing..." << std::endl;
    tester.generate_prime(prime, bits);
    
    uint64_t iterations = 0;
    auto start_time = std::chrono::steady_clock::now();
    auto next_report = start_time + std::chrono::seconds(1);
    auto end_time = start_time + std::chrono::seconds(duration_seconds);
    
    std::cout << "Running primality test for " << duration_seconds << " seconds with " << bits << " bits" << std::endl;
    std::cout << "Progress: " << std::setw(6) << "0%" << " | Rate: " << std::setw(12) << "0" << " ops/sec\r" << std::flush;
    
    // Set up for statistics
    const int stats_interval = 10; // seconds
    auto next_stat = start_time + std::chrono::seconds(stats_interval);
    uint64_t interval_iterations = 0;
    
    while (g_running && std::chrono::steady_clock::now() < end_time) {
        // Test the primality of the number
        tester.is_prime(prime, type);
        iterations++;
        interval_iterations++;
        
        // Report progress every second
        auto now = std::chrono::steady_clock::now();
        if (now >= next_report) {
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start_time).count();
            double progress = static_cast<double>(elapsed) / duration_seconds * 100.0;
            double rate = static_cast<double>(iterations) / elapsed;
            
            std::cout << "Progress: " << std::fixed << std::setprecision(2)
                      << std::setw(6) << progress << "% | Rate: " 
                      << std::setw(12) << static_cast<uint64_t>(rate) << " ops/sec\r" << std::flush;
            
            next_report = now + std::chrono::seconds(1);
        }
        
        // Output detailed statistics every stats_interval seconds
        if (now >= next_stat) {
            auto interval = std::chrono::duration_cast<std::chrono::seconds>(now - (next_stat - std::chrono::seconds(stats_interval))).count();
            double interval_rate = static_cast<double>(interval_iterations) / interval;
            
            std::cout << std::endl << "STAT: " 
                      << std::chrono::duration_cast<std::chrono::seconds>(now - start_time).count() 
                      << "s - Rate: " << std::setw(12) << static_cast<uint64_t>(interval_rate) 
                      << " ops/sec" << std::endl;
            
            next_stat = now + std::chrono::seconds(stats_interval);
            interval_iterations = 0;
        }
    }
    
    auto final_time = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(final_time - start_time).count();
    double rate = static_cast<double>(iterations) / elapsed;
    
    std::cout << std::endl << "Completed " << iterations << " iterations in "
              << elapsed << " seconds (" << static_cast<uint64_t>(rate) << " ops/sec)" << std::endl;
    
    mpz_clear(prime);
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <algorithm> <bits> <duration_seconds>" << std::endl;
        std::cerr << "  algorithm: lcg, xoshiro, miller_rabin, or baillie_psw" << std::endl;
        std::cerr << "  bits: number of bits (e.g., 40, 56, 80, ..., 4096)" << std::endl;
        std::cerr << "  duration_seconds: how long to run in seconds" << std::endl;
        return 1;
    }
    
    // Register signal handler
    std::signal(SIGINT, signal_handler);
    
    std::string algorithm = argv[1];
    int bits;
    int duration_seconds;
    
    try {
        bits = std::stoi(argv[2]);
        duration_seconds = std::stoi(argv[3]);
    } catch (const std::exception& e) {
        std::cerr << "Error: Invalid numeric argument" << std::endl;
        return 1;
    }
    
    if (bits <= 0) {
        std::cerr << "Error: Bit size must be positive" << std::endl;
        return 1;
    }
    
    if (duration_seconds <= 0) {
        std::cerr << "Error: Duration must be positive" << std::endl;
        return 1;
    }
    
    // Fixed seed for repeatability
    uint64_t seed = 12345678901234ULL;
    
    if (algorithm == "lcg") {
        LCG lcg(seed);
        run_prng(lcg, bits, duration_seconds);
    } else if (algorithm == "xoshiro") {
        Xoshiro256pp xoshiro(seed);
        run_prng(xoshiro, bits, duration_seconds);
    } else if (algorithm == "miller_rabin" || algorithm == "baillie_psw") {
        PrimalityTester tester;
        PrimalityTester::TestType test_type = 
            (algorithm == "miller_rabin") ? PrimalityTester::MILLER_RABIN : PrimalityTester::BAILLIE_PSW;
        run_primality(tester, test_type, bits, duration_seconds);
    } else {
        std::cerr << "Error: Unknown algorithm: " << algorithm << std::endl;
        std::cerr << "Supported algorithms: lcg, xoshiro, miller_rabin, baillie_psw" << std::endl;
        return 1;
    }
    
    return 0;
} 