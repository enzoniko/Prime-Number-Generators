#ifndef MILLER_RABIN_H
#define MILLER_RABIN_H

#include <gmp.h>
#include <stdexcept>

/**
 * @brief Miller-Rabin primality test implementation
 * 
 * The Miller-Rabin primality test is a probabilistic primality test: 
 * - If it returns false, the number is definitely composite
 * - If it returns true, the number is probably prime, with the probability
 *   of a false positive decreasing as the number of rounds increases
 * 
 * References:
 * - Cormen, T. H., et al. (2009). Introduction to Algorithms.
 * - Wikipedia: Miller–Rabin primality test
 */
namespace MillerRabin {
    /**
     * @brief Perform the Miller-Rabin primality test
     * 
     * @param n Number to test for primality
     * @param k Number of rounds/iterations (higher = more accurate)
     * @param gmp_randstate GMP random state to use
     * @return bool True if the number is probably prime, false if composite
     */
    bool test(const mpz_t n, int k, gmp_randstate_t gmp_randstate) {
        // Handle base cases
        if (mpz_cmp_ui(n, 2) < 0) return false;  // n < 2
        if (mpz_cmp_ui(n, 2) == 0) return true;  // n = 2
        if (mpz_cmp_ui(n, 3) == 0) return true;  // n = 3
        if (mpz_even_p(n)) return false;         // n is even and > 2
        
        // Initialize temporary variables
        mpz_t n_minus_1, d, a, x, two;
        mpz_init(n_minus_1);
        mpz_init(d);
        mpz_init(a);
        mpz_init(x);
        mpz_init_set_ui(two, 2);
        
        // Calculate n_minus_1 = n - 1
        mpz_sub_ui(n_minus_1, n, 1);
        
        // Find d and s such that n - 1 = 2^s * d, where d is odd
        mpz_set(d, n_minus_1);
        unsigned long s = 0;
        
        // While d is even (lowest bit is 0)
        while (mpz_even_p(d)) {
            mpz_fdiv_q_2exp(d, d, 1); // d = d / 2
            s++;
        }
        // Now d is odd, s is the exponent of 2
        
        // Perform k rounds of testing
        for (int i = 0; i < k; ++i) {
            // Choose random witness 'a' in range [2, n-2]
            // Need n-3 for upper bound of mpz_urandomm
            mpz_t n_minus_3;
            mpz_init(n_minus_3);

            if (mpz_cmp_ui(n, 4) <= 0) {
                // Handle small n cases
                mpz_set_ui(n_minus_3, 1);
            } else {
                mpz_sub_ui(n_minus_3, n, 3);  // n_minus_3 = n - 3
            }
            
            mpz_urandomm(a, gmp_randstate, n_minus_3);  // a = random in [0, n-4]
            mpz_add_ui(a, a, 2);                      // a = random in [2, n-2]
            mpz_clear(n_minus_3);
            
            // Calculate x = a^d mod n
            mpz_powm(x, a, d, n);
            
            // If x == 1 or x == n-1, this iteration passes
            if (mpz_cmp_ui(x, 1) == 0 || mpz_cmp(x, n_minus_1) == 0) {
                continue;
            }
            
            // Check remaining iterations of squaring
            bool maybe_prime = false;
            for (unsigned long r = 1; r < s; ++r) {
                mpz_powm(x, x, two, n);  // x = x^2 mod n
                
                // If x == 1, we found a non-trivial sqrt of 1 => composite
                if (mpz_cmp_ui(x, 1) == 0) {
                    mpz_clear(n_minus_1);
                    mpz_clear(d);
                    mpz_clear(a);
                    mpz_clear(x);
                    mpz_clear(two);
                    return false;  // Composite
                }
                
                // If x == n-1, this iteration passes
                if (mpz_cmp(x, n_minus_1) == 0) {
                    maybe_prime = true;
                    break;
                }
            }
            
            // If we didn't find x == n-1 in the loop, then n is composite
            if (!maybe_prime) {
                mpz_clear(n_minus_1);
                mpz_clear(d);
                mpz_clear(a);
                mpz_clear(x);
                mpz_clear(two);
                return false;  // Composite
            }
        }
        
        // Clean up resources
        mpz_clear(n_minus_1);
        mpz_clear(d);
        mpz_clear(a);
        mpz_clear(x);
        mpz_clear(two);
        
        // If all k rounds passed, n is probably prime
        return true;
    }
};

#endif // MILLER_RABIN_H 