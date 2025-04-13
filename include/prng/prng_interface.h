#ifndef PRNG_INTERFACE_H
#define PRNG_INTERFACE_H

#include <gmp.h>
#include <stdint.h>

/**
 * @brief Interface for Pseudo-Random Number Generators
 * 
 * This class defines the common interface that all PRNGs should implement.
 */
class PRNGInterface {
public:
    /**
     * @brief Virtual destructor
     */
    virtual ~PRNGInterface() {}
    
    /**
     * @brief Generate the next 64-bit unsigned integer in the sequence
     * 
     * @return uint64_t The next random value
     */
    virtual uint64_t next_u64() = 0;
    
    /**
     * @brief Generate a random number with exactly k bits
     * 
     * @param result GMP integer to store the result
     * @param k Number of bits
     */
    virtual void randbits(mpz_t result, int k) = 0;
};

#endif // PRNG_INTERFACE_H 