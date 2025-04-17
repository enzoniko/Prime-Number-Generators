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
#include <algorithm>
#include <cmath>
#include <tuple>

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
    const int num_runs = 30;
    
    // Output file for CSV results
    const std::string output_file = "results/prng_benchmark.csv";
    
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
            // Vector to store all time measurements
            std::vector<double> time_measurements;
            
            // Run the benchmark multiple times for statistical significance
            for (int run = 0; run < num_runs; run++) {
                // Measure time to generate a random number of the specified bit size
                double run_time = TimingUtils::measure_time_ms([&]() {
                    prng.randbits(num, bits);
                });
                
                time_measurements.push_back(run_time);
            }
            
            // Calculate statistics
            double mean_time = 0.0;
            for (double time : time_measurements) {
                mean_time += time;
            }
            mean_time /= time_measurements.size();
            
            double median_time = calculate_median(time_measurements);
            double stddev_time = calculate_stddev(time_measurements, mean_time);
            
            // Format the result string
            std::ostringstream result;
            result << name << "," << bits << "," 
                   << std::fixed << std::setprecision(6) << mean_time << ","
                   << std::fixed << std::setprecision(6) << median_time << ","
                   << std::fixed << std::setprecision(6) << stddev_time;
            results.push_back(result.str());
            
            std::cout << "  " << bits << " bits: Mean=" << mean_time 
                      << " ms, Median=" << median_time 
                      << " ms, StdDev=" << stddev_time << " ms" << std::endl;
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
        results.push_back("Algorithm,BitSize,MeanTimeMs,MedianTimeMs,StdDevTimeMs");
        
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