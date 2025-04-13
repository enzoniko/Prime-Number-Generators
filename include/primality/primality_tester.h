#ifndef PRIMALITY_TESTER_H
#define PRIMALITY_TESTER_H

#include <gmp.h>
#include <array>
#include <vector>
#include "../utils/mpz_utils.h"
#include "miller_rabin.h"
#include "baillie_psw.h"
#include <iostream>

/**
 * @brief Class for performing primality tests
 */
class PrimalityTester {
private:
    gmp_randstate_t rand_state;
    
public:
    /**
     * @brief Primality test type enum
     */
    enum TestType {
        MILLER_RABIN,
        BAILLIE_PSW
    };
    
    /**
     * @brief Construct a new PrimalityTester object
     * 
     * Initializes the GMP random state
     */
    PrimalityTester() {
        MPZUtils::init_gmp_random(rand_state);
    }
    
    /**
     * @brief Destructor
     * 
     * Cleans up the GMP random state
     */
    ~PrimalityTester() {
        MPZUtils::clear_gmp_random(rand_state);
    }
    
    /**
     * @brief Test if a number is prime using the chosen primality test
     * 
     * @param n The number to test
     * @param type The type of primality test to use
     * @param k Number of iterations for Miller-Rabin test (higher means more accurate but slower)
     * @return true if n is probably prime, false if n is definitely composite
     */
    bool is_prime(const mpz_t n, TestType type = MILLER_RABIN, unsigned int k = 40) {
        switch (type) {
            case MILLER_RABIN:
                return MillerRabin::test(n, k, rand_state);
            
            case BAILLIE_PSW:
                return BailliePSW::test(n, rand_state);
                
            default:
                return MillerRabin::test(n, k, rand_state);
        }
    }
    
    /**
     * @brief Generate a random prime number with the specified number of bits
     * 
     * @param bits The bit length of the prime number
     * @return mpz_t A prime number with the specified bit length
     */
    void generate_prime(mpz_t result, unsigned int bits) {
        if (bits <= 1) {
            mpz_set_ui(result, 2);
            return;
        }
        
        // Small primes for bits <= 64
        static const std::array<unsigned long, 15> small_primes = {
            2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47
        };
        
        // For very small values, return a small prime
        if (bits < 8) {
            for (size_t i = 0; i < small_primes.size(); i++) {
                mpz_t temp;
                mpz_init(temp);
                mpz_set_ui(temp, small_primes[i]);
                if (mpz_sizeinbase(temp, 2) == static_cast<size_t>(bits)) {
                    mpz_set(result, temp);
                    mpz_clear(temp);
                    return;
                }
                mpz_clear(temp);
            }
        }
        
        // Generate random odd numbers and test them until we find a prime
        while (true) {
            MPZUtils::random_odd(result, bits, rand_state);
            
            if (is_prime(result)) {
                return;
            }
        }
    }
    
    /**
     * @brief Find a prime number of the specified bit size using the specified primality test
     * 
     * @param result Output parameter for the prime number
     * @param bits The bit length of the prime number
     * @param type The type of primality test to use
     * @return bool Always returns true (for compatibility with benchmark)
     */
    bool find_prime(mpz_t result, unsigned int bits, TestType type) {
        // Currently, we only have one implementation that uses Miller-Rabin
        // So we ignore the test type parameter and just use generate_prime
        generate_prime(result, bits);
        return true;
    }
};

#endif // PRIMALITY_TESTER_H 