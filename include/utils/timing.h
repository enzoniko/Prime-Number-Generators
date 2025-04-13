#ifndef TIMING_H
#define TIMING_H

#include <chrono>
#include <functional>
#include <vector>
#include <numeric>
#include <algorithm>

/**
 * @brief Utilities for measuring execution time
 */
namespace TimingUtils {
    /**
     * @brief Measure execution time of a function (single run)
     * 
     * @param func Function to measure
     * @return double Time in milliseconds
     */
    double measure_time_ms(std::function<void()> func) {
        auto start = std::chrono::high_resolution_clock::now();
        func();
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration_ms = end - start;
        return duration_ms.count();
    }
    
    /**
     * @brief Measure average execution time over multiple runs
     * 
     * @param func Function to measure
     * @param num_runs Number of runs
     * @return double Average time in milliseconds
     */
    double measure_average_time_ms(std::function<void()> func, int num_runs) {
        std::vector<double> times;
        times.reserve(num_runs);
        
        for (int i = 0; i < num_runs; ++i) {
            times.push_back(measure_time_ms(func));
        }
        
        double sum = std::accumulate(times.begin(), times.end(), 0.0);
        return sum / times.size();
    }
    
    /**
     * @brief Measure execution statistics over multiple runs
     * 
     * @param func Function to measure
     * @param num_runs Number of runs
     * @return std::tuple<double, double, double, double> Min, max, average, and median time in milliseconds
     */
    std::tuple<double, double, double, double> measure_time_stats(std::function<void()> func, int num_runs) {
        std::vector<double> times;
        times.reserve(num_runs);
        
        for (int i = 0; i < num_runs; ++i) {
            times.push_back(measure_time_ms(func));
        }
        
        double min = *std::min_element(times.begin(), times.end());
        double max = *std::max_element(times.begin(), times.end());
        double sum = std::accumulate(times.begin(), times.end(), 0.0);
        double avg = sum / times.size();
        
        // Calculate median
        std::sort(times.begin(), times.end());
        double median;
        if (times.size() % 2 == 0) {
            median = (times[times.size() / 2 - 1] + times[times.size() / 2]) / 2.0;
        } else {
            median = times[times.size() / 2];
        }
        
        return std::make_tuple(min, max, avg, median);
    }
};

#endif // TIMING_H 