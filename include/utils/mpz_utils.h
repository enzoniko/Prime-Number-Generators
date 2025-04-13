#ifndef MPZ_UTILS_H
#define MPZ_UTILS_H

#include <gmp.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <chrono>

/**
 * @brief Utilities for working with GMP mpz_t integers
 */
namespace MPZUtils {
    /**
     * @brief Initialize a GMP random state with a seed based on current time
     * 
     * @param state The GMP random state to initialize
     */
    void init_gmp_random(gmp_randstate_t state) {
        unsigned long seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        gmp_randinit_mt(state);
        gmp_randseed_ui(state, seed);
    }
    
    /**
     * @brief Clear a GMP random state
     * 
     * @param state The GMP random state to clear
     */
    void clear_gmp_random(gmp_randstate_t state) {
        gmp_randclear(state);
    }
    
    /**
     * @brief Convert a GMP integer to a string
     * 
     * @param n The GMP integer
     * @return std::string The string representation
     */
    std::string to_string(const mpz_t n) {
        char* str = mpz_get_str(NULL, 10, n);
        std::string result(str);
        free(str);
        return result;
    }
    
    /**
     * @brief Format a GMP integer with specified notation and precision
     * 
     * @param n The GMP integer
     * @param base The base to use (2-62)
     * @param prefix Whether to include the base prefix (0x, 0b, etc.)
     * @return std::string The formatted string
     */
    std::string format(const mpz_t n, int base = 10, bool prefix = false) {
        if (base < 2 || base > 62) {
            throw std::invalid_argument("Base must be between 2 and 62");
        }
        
        char* str = mpz_get_str(NULL, base, n);
        std::string result(str);
        free(str);
        
        if (prefix) {
            if (base == 2) return "0b" + result;
            if (base == 8) return "0" + result;
            if (base == 16) return "0x" + result;
        }
        
        return result;
    }
    
    /**
     * @brief Generate a random odd number with specified number of bits
     * 
     * @param result Output parameter for the generated number
     * @param bits Number of bits
     * @param state GMP random state to use
     */
    void random_odd(mpz_t result, unsigned int bits, gmp_randstate_t state) {
        mpz_urandomb(result, state, bits);
        
        // Ensure the number has exactly 'bits' bits by setting the highest bit
        if (bits > 0) {
            mpz_setbit(result, bits - 1);
        }
        
        // Make the number odd by setting the lowest bit
        mpz_setbit(result, 0);
    }
    
    /**
     * @brief Get the bit length of a GMP integer
     * 
     * @param n The GMP integer
     * @return int Number of bits
     */
    int bit_length(const mpz_t n) {
        return mpz_sizeinbase(n, 2);
    }
};

#endif // MPZ_UTILS_H 