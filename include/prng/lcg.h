#ifndef LCG_H
#define LCG_H

#include "prng_interface.h"
#include <cmath>
#include <chrono>
#include <stdexcept>

/**
 * @brief Linear Congruential Generator implementation
 * 
 * LCG generates random numbers using the recurrence relation:
 * X_{n+1} = (a * X_n + c) mod m
 * where X is the sequence of pseudo-random values, and
 * - a is the multiplier
 * - c is the increment
 * - m is the modulus (2^64 in this implementation)
 * 
 * Reference: Knuth, D. E. (1997). The Art of Computer Programming, Vol 2.
 */
class LCG : public PRNGInterface {
private:
    uint64_t state;  // Current state of the generator
    
    // LCG parameters - using values from the POSIX standard
    static const uint64_t LCG_A = 6364136223846793005ULL;
    static const uint64_t LCG_C = 1ULL;
    // Modulus M is implicitly 2^64 due to uint64_t overflow

public:
    /**
     * @brief Construct a new LCG with a seed
     * 
     * @param seed Initial seed value, or 0 for automatic seeding
     */
    LCG(uint64_t seed = 0) : state(seed) {
        if (seed == 0) {
            // Use system clock as seed if none provided
            state = std::chrono::high_resolution_clock::now().time_since_epoch().count();
            if (state == 0) state = 1; // Avoid state being 0
        }
    }
    
    /**
     * @brief Generate the next 64-bit pseudo-random value
     * 
     * @return uint64_t The next random value
     */
    uint64_t next_u64() override {
        state = LCG_A * state + LCG_C; // Modulo 2^64 is implicit due to overflow
        return state;
    }
    
    /**
     * @brief Generate a k-bit random number
     * 
     * @param result GMP integer to store the result
     * @param k Number of bits
     */
    void randbits(mpz_t result, int k) override {
        if (k <= 0) {
            mpz_set_ui(result, 0);
            return;
        }

        int bits_per_output = 64;
        int num_outputs = std::ceil(static_cast<double>(k) / bits_per_output);

        mpz_set_ui(result, 0); // Initialize GMP integer to 0

        for (int i = 0; i < num_outputs; ++i) {
            uint64_t rand_val = next_u64();
            mpz_mul_2exp(result, result, bits_per_output); // Left shift by 64 bits
            mpz_add_ui(result, result, rand_val); // Add the new 64 bits
        }

        // Trim excess bits if necessary
        int bits_generated = num_outputs * bits_per_output;
        if (bits_generated > k) {
            mpz_fdiv_q_2exp(result, result, bits_generated - k); // Right shift
        }

        // Ensure the number has exactly k bits (set MSB to 1)
        if (k > 0) {
            mpz_setbit(result, k - 1);
        }
    }
};

#endif // LCG_H 