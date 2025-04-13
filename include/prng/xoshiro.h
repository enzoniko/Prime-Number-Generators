#ifndef XOSHIRO_H
#define XOSHIRO_H

#include "prng_interface.h"
#include <cmath>
#include <chrono>
#include <random>
#include <stdexcept>

/**
 * @brief Helper function for 64-bit left rotation
 * 
 * @param x Value to rotate
 * @param k Number of bits to rotate by
 * @return uint64_t Rotated value
 */
static inline uint64_t rotl(const uint64_t x, int k) {
    return (x << k) | (x >> (64 - k));
}

/**
 * @brief Xoshiro256++ pseudo-random number generator
 * 
 * A fast, high-quality PRNG with a state size of 256 bits.
 * 
 * Reference: Blackman, D., & Vigna, S. (2019). Scrambled Linear Pseudorandom 
 * Number Generators. arXiv preprint arXiv:1805.01407v5.
 * 
 * Official implementation: https://prng.di.unimi.it/
 */
class Xoshiro256pp : public PRNGInterface {
private:
    uint64_t s[4]; // 256 bits of state
    
    /**
     * @brief Seed the state array
     * 
     * @param seed_val Initial seed value
     */
    void seed_state(uint64_t seed_val) {
         std::mt19937_64 seeder(seed_val);
         // Fill the state array with values from std::mt19937_64
         for (int i = 0; i < 4; ++i) {
             s[i] = seeder();
         }
         
         // Ensure the initial state is not all zeros
         bool all_zeros = true;
         for (int i = 0; i < 4; ++i) {
             if (s[i] != 0) {
                 all_zeros = false;
                 break;
             }
         }
         if (all_zeros) {
             s[0] = 1; // Set a non-zero state
         }
    }

public:
    /**
     * @brief Construct a new Xoshiro256++ generator
     * 
     * @param seed Initial seed value, or 0 for automatic seeding
     */
    Xoshiro256pp(uint64_t seed = 0) {
        if (seed == 0) {
            // Use system clock as seed if none provided
            seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        }
        seed_state(seed);
    }
    
    /**
     * @brief Generate the next 64-bit pseudo-random value
     * 
     * @return uint64_t The next random value
     */
    uint64_t next_u64() override {
        // The core Xoshiro256++ algorithm:
        const uint64_t result = rotl(s[0] + s[3], 23) + s[0]; // ++ scrambler part

        const uint64_t t = s[1] << 17;

        s[2] ^= s[0];
        s[3] ^= s[1];
        s[1] ^= s[2];
        s[0] ^= s[3];

        s[2] ^= t;

        s[3] = rotl(s[3], 45);

        return result;
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

#endif // XOSHIRO_H 