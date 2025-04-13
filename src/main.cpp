#include "../include/prng/lcg.h"
#include "../include/prng/xoshiro.h"
#include "../include/primality/primality_tester.h"
#include "../include/utils/mpz_utils.h"
#include <iostream>
#include <chrono>
#include <string>
#include <memory>
#include <stdexcept>
#include <cstdlib>

/**
 * @brief Entry point for the prime number generation and testing application
 * 
 * This program demonstrates the use of pseudo-random number generators and
 * primality testing algorithms implemented in this project.
 */

void print_help() {
    std::cout << "Prime Number Generation and Testing\n";
    std::cout << "Usage:\n";
    std::cout << "  main [command] [options]\n\n";
    std::cout << "Commands:\n";
    std::cout << "  help                  Display this help message\n";
    std::cout << "  generate <bits>       Generate a random prime number of the specified size\n";
    std::cout << "  test <number>         Test if a number is prime\n";
    std::cout << "  benchmark             Run all benchmarks\n";
    std::cout << "  benchmark-prng        Run only PRNG benchmarks\n";
    std::cout << "  benchmark-primality   Run only primality testing benchmarks\n\n";
    std::cout << "Options:\n";
    std::cout << "  --iterations=<n>      Number of iterations for Miller-Rabin test (default: 40)\n";
    std::cout << "  --algorithm=<alg>     Primality test algorithm: mr (Miller-Rabin) or bpsw (Baillie-PSW) (default: mr)\n";
}

/**
 * @brief Generate a random prime number
 * 
 * @param bits Number of bits
 * @param iterations Number of iterations for Miller-Rabin
 */
void generate_prime(unsigned int bits, unsigned int iterations) {
    PrimalityTester tester;
    mpz_t prime;
    mpz_init(prime);
    
    auto start = std::chrono::high_resolution_clock::now();
    tester.generate_prime(prime, bits);
    auto end = std::chrono::high_resolution_clock::now();
    
    std::chrono::duration<double, std::milli> duration = end - start;
    
    std::cout << "Found a " << bits << "-bit prime in " << duration.count() << " ms:\n";
    gmp_printf("%Zd\n", prime);
    
    mpz_clear(prime);
}

/**
 * @brief Test if a number is prime
 * 
 * @param number_str Number to test as a string
 * @param algo_type Type of primality test algorithm to use
 * @param iterations Number of iterations for Miller-Rabin
 */
void test_prime(const std::string& number_str, PrimalityTester::TestType algo_type, unsigned int iterations) {
    PrimalityTester tester;
    mpz_t number;
    mpz_init(number);
    
    if (mpz_set_str(number, number_str.c_str(), 0) != 0) {
        std::cerr << "Error: Invalid number format.\n";
        mpz_clear(number);
        return;
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    bool is_prime = tester.is_prime(number, algo_type, iterations);
    auto end = std::chrono::high_resolution_clock::now();
    
    std::chrono::duration<double, std::milli> duration = end - start;
    
    std::cout << "Number: " << number_str << std::endl;
    std::cout << "Algorithm: " << (algo_type == PrimalityTester::MILLER_RABIN ? "Miller-Rabin" : "Baillie-PSW") << std::endl;
    std::cout << "Iterations: " << iterations << std::endl;
    std::cout << "Result: " << (is_prime ? "Probably Prime" : "Composite") << std::endl;
    std::cout << "Time: " << duration.count() << " ms" << std::endl;
    
    mpz_clear(number);
}

/**
 * @brief Run the specified benchmarks
 * 
 * @param benchmark_type Type of benchmark to run
 */
void run_benchmark(const std::string& benchmark_type) {
    if (benchmark_type == "prng" || benchmark_type == "all") {
        std::cout << "Running PRNG benchmarks...\n";
        int ret = std::system("./prng_benchmark");
        if (ret != 0) {
            std::cerr << "Warning: PRNG benchmark returned non-zero exit code: " << ret << std::endl;
        }
    }
    
    if (benchmark_type == "primality" || benchmark_type == "all") {
        std::cout << "Running primality testing benchmarks...\n";
        int ret = std::system("./primality_benchmark");
        if (ret != 0) {
            std::cerr << "Warning: Primality benchmark returned non-zero exit code: " << ret << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_help();
        return 0;
    }
    
    std::string command = argv[1];
    
    if (command == "help") {
        print_help();
        return 0;
    }
    
    // Default parameters
    unsigned int iterations = 40;
    PrimalityTester::TestType algo_type = PrimalityTester::MILLER_RABIN;
    
    // Parse additional options
    for (int i = 2; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg.substr(0, 13) == "--iterations=") {
            iterations = std::stoi(arg.substr(13));
        } else if (arg.substr(0, 12) == "--algorithm=") {
            std::string algo = arg.substr(12);
            if (algo == "mr") {
                algo_type = PrimalityTester::MILLER_RABIN;
            } else if (algo == "bpsw") {
                algo_type = PrimalityTester::BAILLIE_PSW;
            } else {
                std::cerr << "Error: Invalid algorithm. Use mr (Miller-Rabin) or bpsw (Baillie-PSW).\n";
                return 1;
            }
        }
    }
    
    try {
        if (command == "generate" && argc >= 3) {
            unsigned int bits = std::stoi(argv[2]);
            generate_prime(bits, iterations);
        } else if (command == "test" && argc >= 3) {
            test_prime(argv[2], algo_type, iterations);
        } else if (command == "benchmark") {
            run_benchmark("all");
        } else if (command == "benchmark-prng") {
            run_benchmark("prng");
        } else if (command == "benchmark-primality") {
            run_benchmark("primality");
        } else {
            std::cerr << "Error: Invalid command or missing parameters.\n";
            print_help();
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
} 