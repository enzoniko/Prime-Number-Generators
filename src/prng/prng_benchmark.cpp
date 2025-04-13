#include "../../include/prng/lcg.h"
#include "../../include/prng/xoshiro.h"
#include "../../include/utils/timing.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <memory>
#include <fstream>
#include <sstream>

/**
 * @brief Benchmark PRNGs for random number generation
 */
class PRNGBenchmark {
private:
    // Common seed for fair comparison
    const uint64_t seed = 12345678901234ULL;
    
    // Bit sizes to benchmark
    const std::vector<int> bit_sizes = {40, 56, 80, 128, 168, 224, 256, 512, 1024, 2048, 4096};
    
    // Number of runs to average over
    const int num_runs = 100;
    
    // Output file for CSV results
    const std::string output_file = "results/prng_benchmark.csv";
    
    /**
     * @brief Benchmark a single PRNG
     * 
     * @param prng The PRNG to benchmark
     * @param name The name of the PRNG for output
     * @param results Vector to store result strings
     */
    void benchmark_prng(PRNGInterface& prng, const std::string& name, std::vector<std::string>& results) {
        std::cout << "Benchmarking " << name << "..." << std::endl;
        
        mpz_t num;
        mpz_init(num);
        
        for (int bits : bit_sizes) {
            // Measure average time to generate a random number of the specified bit size
            double avg_time = TimingUtils::measure_average_time_ms([&]() {
                prng.randbits(num, bits);
            }, num_runs);
            
            // Format the result string
            std::ostringstream result;
            result << name << "," << bits << "," << std::fixed << std::setprecision(6) << avg_time;
            results.push_back(result.str());
            
            std::cout << "  " << bits << " bits: " << avg_time << " ms" << std::endl;
        }
        
        mpz_clear(num);
    }
    
public:
    /**
     * @brief Run the PRNG benchmarks
     */
    void run() {
        std::vector<std::string> results;
        
        // Add CSV header
        results.push_back("Algorithm,BitSize,TimeMs");
        
        // Create and benchmark each PRNG
        {
            LCG lcg(seed);
            benchmark_prng(lcg, "LCG", results);
        }
        
        {
            Xoshiro256pp xoshiro(seed);
            benchmark_prng(xoshiro, "Xoshiro256++", results);
        }
        
        // Write results to file
        std::ofstream out(output_file);
        if (!out) {
            std::cerr << "Error: Could not open output file " << output_file << std::endl;
            return;
        }
        
        for (const auto& line : results) {
            out << line << std::endl;
        }
        
        out.close();
        
        std::cout << "Benchmark results written to " << output_file << std::endl;
    }
};

int main() {
    PRNGBenchmark benchmark;
    benchmark.run();
    return 0;
} 