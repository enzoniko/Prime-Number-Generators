#ifndef BAILLIE_PSW_H
#define BAILLIE_PSW_H

#include <gmp.h>
#include <stdexcept>
#include <cmath>

/**
 * @brief Baillie-PSW primality test implementation
 * 
 * The Baillie-PSW test is a combination of:
 * 1. Trial division by small primes
 * 2. A base-2 Miller-Rabin test
 * 3. A strong Lucas probable prime test
 * 
 * No composite number is known to pass the Baillie-PSW test, making it
 * very reliable for practical purposes.
 * 
 * References:
 * - Baillie, R., & Wagstaff Jr, S. S. (1980). Lucas pseudoprimes. Mathematics of Computation, 35(152), 1391-1417.
 * - Pomerance, C., Selfridge, J. L., & Wagstaff Jr, S. S. (1980). The pseudoprimes to 25·10⁹.
 * - Crandall, R., & Pomerance, C. (2005). Prime Numbers: A Computational Perspective. Springer.
 */
namespace BailliePSW {
    // Small primes list for trial division
    const int small_primes[] = {
        2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 
        73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 
        157, 163, 167, 173, 179, 181, 191, 193, 197, 199, 211, 223, 227, 229, 233, 
        239, 241, 251, 257, 263, 269, 271, 277, 281, 283, 293, 307, 311, 313, 317, 
        331, 337, 347, 349, 353, 359, 367, 373, 379, 383, 389, 397, 401, 409, 419, 
        421, 431, 433, 439, 443, 449, 457, 461, 463, 467, 479, 487, 491, 499, 503, 509
    };
    constexpr int num_small_primes = sizeof(small_primes) / sizeof(small_primes[0]);
    
    /**
     * @brief Calculate the Jacobi symbol (a/n)
     * 
     * A more efficient implementation would use GMP's mpz_jacobi function.
     * This is included here for educational purposes.
     * 
     * @param a Upper parameter
     * @param n Lower parameter (odd positive integer)
     * @return int Jacobi symbol value (-1, 0, or 1)
     */
    int jacobi_symbol(const mpz_t a, const mpz_t n) {
        return mpz_jacobi(a, n);
    }
    
    /**
     * @brief Calculate Lucas V sequence values
     * 
     * Computes the Lucas V sequence value V_k(P,Q) mod n for given parameters.
     * 
     * @param v_k Output parameter for V_k
     * @param P Lucas sequence parameter P
     * @param Q Lucas sequence parameter Q
     * @param k Index to compute
     * @param n Modulus
     */
    void lucas_v_mod(mpz_t v_k, const mpz_t P, const mpz_t Q, const mpz_t k, const mpz_t n) {
        mpz_t v0, v1, k_copy, q_power;
        mpz_init_set_ui(v0, 2);           // V_0 = 2
        mpz_init(v1);                     // V_1 = P
        mpz_set(v1, P);                   
        mpz_init(k_copy);
        mpz_init(q_power);
        mpz_set(k_copy, k);
        
        // If k = 0, return V_0 = 2
        if (mpz_sgn(k_copy) == 0) {
            mpz_set(v_k, v0);
            mpz_clear(v0);
            mpz_clear(v1);
            mpz_clear(k_copy);
            mpz_clear(q_power);
            return;
        }
        
        // If k = 1, return V_1 = P
        if (mpz_cmp_ui(k_copy, 1) == 0) {
            mpz_set(v_k, v1);
            mpz_clear(v0);
            mpz_clear(v1);
            mpz_clear(k_copy);
            mpz_clear(q_power);
            return;
        }
        
        // Double-and-add algorithm for computing V_k efficiently
        mpz_t d, t1, t2, t3;
        mpz_init(d);
        mpz_init(t1);
        mpz_init(t2);
        mpz_init(t3);
        
        // Start from k-1 (index 0 in binary), doubling each time
        mpz_sub_ui(k_copy, k_copy, 1);
        
        mpz_set(d, v1);  // d = V_1 initially
        
        // Process each bit of k_copy from second-lowest to highest (already handled bit 0)
        while (mpz_cmp_ui(k_copy, 1) > 0) {
            // t1 = d^2
            mpz_mul(t1, d, d);
            mpz_mod(t1, t1, n);
            
            // t2 = P*d
            mpz_mul(t2, P, d);
            mpz_mod(t2, t2, n);
            
            // t3 = Q^(j-1) * V_2
            mpz_mul(t3, Q, v0);
            mpz_mod(t3, t3, n);
            
            // v0 = d^2 - 2Q^j-1
            mpz_sub(v0, t1, t3);
            mpz_mod(v0, v0, n);
            
            // v1 = P*d - Q^j-1 * V_1
            mpz_mul(t3, Q, v1);
            mpz_mod(t3, t3, n);
            mpz_sub(v1, t2, t3);
            mpz_mod(v1, v1, n);
            
            // Halve k_copy
            mpz_tdiv_q_2exp(k_copy, k_copy, 1);
        }
        
        mpz_set(v_k, d);
        
        mpz_clear(v0);
        mpz_clear(v1);
        mpz_clear(k_copy);
        mpz_clear(q_power);
        mpz_clear(d);
        mpz_clear(t1);
        mpz_clear(t2);
        mpz_clear(t3);
    }
    
    /**
     * @brief Calculate Lucas U and V sequence values
     * 
     * Computes both the Lucas U and V sequence values U_k(P,Q) and V_k(P,Q) mod n
     * 
     * @param u_k Output parameter for U_k
     * @param v_k Output parameter for V_k
     * @param P Lucas sequence parameter P
     * @param Q Lucas sequence parameter Q
     * @param k Index to compute
     * @param n Modulus
     */
    void lucas_sequence_mod(mpz_t u_k, mpz_t v_k, const mpz_t P, const mpz_t Q, const mpz_t k, const mpz_t n) {
        mpz_t u0, u1, v0, v1, k_copy, q_k;
        mpz_init_set_ui(u0, 0);    // U_0 = 0
        mpz_init_set_ui(u1, 1);    // U_1 = 1
        mpz_init_set_ui(v0, 2);    // V_0 = 2
        mpz_init(v1);              // V_1 = P
        mpz_set(v1, P);
        mpz_init(k_copy);
        mpz_init(q_k);
        mpz_set(k_copy, k);
        
        // If k = 0, return U_0 = 0, V_0 = 2
        if (mpz_sgn(k_copy) == 0) {
            mpz_set(u_k, u0);
            mpz_set(v_k, v0);
            mpz_clear(u0); mpz_clear(u1); mpz_clear(v0); mpz_clear(v1);
            mpz_clear(k_copy); mpz_clear(q_k);
            return;
        }
        
        // Binary method for computing Lucas sequences
        // Reference: Algorithm 3.6.7 in Cohen's "A Course in Computational Algebraic Number Theory"
        mpz_t t1, t2, t3, t4;
        mpz_init(t1); mpz_init(t2); mpz_init(t3); mpz_init(t4);
        
        // Get binary representation of k
        size_t bit_length = mpz_sizeinbase(k_copy, 2);
        
        for (size_t i = bit_length - 1; i > 0; --i) {
            // t1 = U_m * U_m
            mpz_mul(t1, u0, u0);
            mpz_mod(t1, t1, n);
            
            // t2 = V_m * V_m
            mpz_mul(t2, v0, v0);
            mpz_mod(t2, t2, n);
            
            // t3 = U_m * V_m
            mpz_mul(t3, u0, v0);
            mpz_mod(t3, t3, n);
            
            // U_{2m} = U_m * V_m
            mpz_set(u0, t3);
            
            // V_{2m} = V_m^2 - 2Q^m
            mpz_set_ui(t4, 2);
            mpz_neg(t4, t4); // t4 = -2
            mpz_submul(t2, t4, Q); // t2 = V_m^2 - 2Q^m
            mpz_mod(t2, t2, n);
            mpz_set(v0, t2);
            
            // If the i-th bit of k is 1, update for U_{2m+1} and V_{2m+1}
            if (mpz_tstbit(k_copy, i - 1)) {
                // t1 = U_{2m} * P
                mpz_mul(t1, u0, P);
                mpz_mod(t1, t1, n);
                
                // t2 = V_{2m}
                mpz_set(t2, v0);
                
                // U_{2m+1} = (P*U_{2m} + V_{2m}) / 2
                mpz_add(u0, t1, t2);
                if (mpz_odd_p(u0)) {
                    mpz_add(u0, u0, n);
                }
                mpz_tdiv_q_2exp(u0, u0, 1);
                mpz_mod(u0, u0, n);
                
                // V_{2m+1} = (P*V_{2m} + U_{2m}*D) / 2
                mpz_mul(t1, t2, P);
                mpz_mod(t1, t1, n);
                
                mpz_mul(t2, u0, Q);
                mpz_mod(t2, t2, n);
                
                mpz_sub(v0, t1, t2);
                mpz_mod(v0, v0, n);
            }
        }
        
        mpz_set(u_k, u0);
        mpz_set(v_k, v0);
        
        mpz_clear(u0); mpz_clear(u1); mpz_clear(v0); mpz_clear(v1);
        mpz_clear(k_copy); mpz_clear(q_k); mpz_clear(t1); mpz_clear(t2);
        mpz_clear(t3); mpz_clear(t4);
    }
    
    /**
     * @brief Perform the strong Lucas primality test
     * 
     * @param n Number to test for primality
     * @return bool True if the number passes the test, false otherwise
     */
    bool strong_lucas_test(const mpz_t n) {
        // Find the first D in the sequence 5, -7, 9, -11, ... such that Jacobi(D/n) = -1
        mpz_t D, abs_D, P, Q, gcd, U, V, n_plus_1, d, V_term, two;
        mpz_init(D);
        mpz_init(abs_D);
        mpz_init_set_ui(P, 1);       // We'll use P = 1 for simplicity
        mpz_init(Q);
        mpz_init(gcd);
        mpz_init(U);
        mpz_init(V);
        mpz_init(n_plus_1);
        mpz_init(d);
        mpz_init(V_term);
        mpz_init_set_ui(two, 2);
        
        int D_val = 5;
        int sign = 1;
        bool found_D = false;
        
        // Find D such that Jacobi(D/n) = -1
        while (!found_D) {
            mpz_set_si(D, D_val * sign);
            
            // Compute Jacobi(D/n)
            int jacobi = mpz_jacobi(D, n);
            
            if (jacobi == 0) {
                // If Jacobi symbol is 0, gcd(|D|, n) > 1
                mpz_abs(abs_D, D);
                mpz_gcd(gcd, abs_D, n);
                
                // If gcd(|D|, n) = n, then n = |D| (unlikely)
                if (mpz_cmp(gcd, n) == 0) {
                    mpz_clear(D); mpz_clear(abs_D); mpz_clear(P); mpz_clear(Q);
                    mpz_clear(gcd); mpz_clear(U); mpz_clear(V); mpz_clear(n_plus_1);
                    mpz_clear(d); mpz_clear(V_term); mpz_clear(two);
                    return mpz_probab_prime_p(n, 5) > 0;  // Extra check
                }
                
                // Otherwise, n has a proper factor
                mpz_clear(D); mpz_clear(abs_D); mpz_clear(P); mpz_clear(Q);
                mpz_clear(gcd); mpz_clear(U); mpz_clear(V); mpz_clear(n_plus_1);
                mpz_clear(d); mpz_clear(V_term); mpz_clear(two);
                return false;
            }
            
            if (jacobi == -1) {
                found_D = true;
                break;
            }
            
            // Try next D in sequence
            sign *= -1;
            if (sign == 1) {
                D_val += 2;
            }
            
            // Safety limit (should never be reached in practice)
            if (D_val > 1000) {
                mpz_clear(D); mpz_clear(abs_D); mpz_clear(P); mpz_clear(Q);
                mpz_clear(gcd); mpz_clear(U); mpz_clear(V); mpz_clear(n_plus_1);
                mpz_clear(d); mpz_clear(V_term); mpz_clear(two);
                return false;
            }
        }
        
        // Calculate Q = (1 - D) / 4
        mpz_sub_ui(Q, P, 1);  // Q = P - 1 = 0
        mpz_sub(Q, Q, D);     // Q = (P - 1) - D = -D
        mpz_tdiv_q_ui(Q, Q, 4); // Q = -D/4
        
        // Calculate n + 1 = d * 2^s where d is odd
        mpz_add_ui(n_plus_1, n, 1);
        mpz_set(d, n_plus_1);
        unsigned long s = 0;
        
        while (mpz_even_p(d)) {
            mpz_tdiv_q_2exp(d, d, 1);  // d = d / 2
            s++;
        }
        
        // Compute U_d and V_d
        lucas_sequence_mod(U, V, P, Q, d, n);
        
        // Check if U_d = 0 (mod n)
        if (mpz_sgn(U) == 0) {
            mpz_clear(D); mpz_clear(abs_D); mpz_clear(P); mpz_clear(Q);
            mpz_clear(gcd); mpz_clear(U); mpz_clear(V); mpz_clear(n_plus_1);
            mpz_clear(d); mpz_clear(V_term); mpz_clear(two);
            return true;
        }
        
        // Check if V_d = 0 (mod n) or V_{d*2^r} = 0 (mod n) for some 0 ≤ r < s
        mpz_set(V_term, V);
        
        for (unsigned long r = 0; r < s; ++r) {
            if (mpz_sgn(V_term) == 0) {
                mpz_clear(D); mpz_clear(abs_D); mpz_clear(P); mpz_clear(Q);
                mpz_clear(gcd); mpz_clear(U); mpz_clear(V); mpz_clear(n_plus_1);
                mpz_clear(d); mpz_clear(V_term); mpz_clear(two);
                return true;
            }
            
            if (r < s - 1) {
                // Calculate V_{2k} from V_k: V_{2k} = V_k^2 - 2*Q^k
                mpz_mul(V_term, V_term, V_term);  // V_term = V_term^2
                mpz_mod(V_term, V_term, n);
                
                mpz_set_ui(Q, 2);
                mpz_neg(Q, Q);  // Q = -2
                mpz_add(V_term, V_term, Q);  // V_term = V_term^2 - 2
                mpz_mod(V_term, V_term, n);
            }
        }
        
        // If no condition is met, the test fails
        mpz_clear(D); mpz_clear(abs_D); mpz_clear(P); mpz_clear(Q);
        mpz_clear(gcd); mpz_clear(U); mpz_clear(V); mpz_clear(n_plus_1);
        mpz_clear(d); mpz_clear(V_term); mpz_clear(two);
        return false;
    }
    
    /**
     * @brief Perform the Baillie-PSW primality test
     * 
     * @param n Number to test for primality
     * @return bool True if the number is probably prime, false if definitely composite
     */
    bool test(const mpz_t n, gmp_randstate_t gmp_randstate) {
        // Handle small cases
        if (mpz_cmp_ui(n, 2) < 0) return false;  // n < 2
        if (mpz_cmp_ui(n, 2) == 0) return true;  // n = 2
        if (mpz_even_p(n)) return false;         // n is even and > 2
        
        // 1. Trial division by small primes
        for (int i = 0; i < num_small_primes; ++i) {
            // Skip checking division by 2 (already checked even numbers)
            if (small_primes[i] == 2) continue;
            
            // Check if n is a small prime
            if (mpz_cmp_ui(n, small_primes[i]) == 0) return true;
            
            // Check if n is divisible by a small prime
            if (mpz_divisible_ui_p(n, small_primes[i])) return false;
        }
        
        // 2. Check if n is a perfect square
        if (mpz_perfect_square_p(n)) return false;
        
        // 3. Miller-Rabin base 2
        mpz_t base2;
        mpz_init_set_ui(base2, 2);
        
        // Need temporary variables for MR test base 2
        mpz_t n_minus_1, d_mr, x, two;
        mpz_init(n_minus_1);
        mpz_init(d_mr);
        mpz_init(x);
        mpz_init_set_ui(two, 2);
        
        // n_minus_1 = n - 1
        mpz_sub_ui(n_minus_1, n, 1);
        
        // Find d and s such that n - 1 = 2^s * d
        mpz_set(d_mr, n_minus_1);
        unsigned long s = 0;
        while (mpz_even_p(d_mr)) {
            mpz_fdiv_q_2exp(d_mr, d_mr, 1);
            s++;
        }
        // Calculate x = 2^d mod n
        mpz_powm(x, base2, d_mr, n);
        
        bool mr_base2_passed = false;
        if (mpz_cmp_ui(x, 1) == 0 || mpz_cmp(x, n_minus_1) == 0) {
            mr_base2_passed = true;
        } else {
            for (unsigned long r = 1; r < s; ++r) {
                mpz_powm(x, x, two, n); // Square x mod n
                if (mpz_cmp(x, n_minus_1) == 0) {
                    mr_base2_passed = true;
                    break;
                }
                if (mpz_cmp_ui(x, 1) == 0) { // Non-trivial sqrt of 1
                    mr_base2_passed = false;
                    break;
                }
            }
        }
        
        mpz_clear(base2);
        mpz_clear(n_minus_1);
        mpz_clear(d_mr);
        mpz_clear(x);
        mpz_clear(two);
        
        if (!mr_base2_passed) {
            return false; // Failed MR base 2 => composite
        }
        
        // 4. Strong Lucas Primality Test
        return strong_lucas_test(n);
    }
};

#endif // BAILLIE_PSW_H 