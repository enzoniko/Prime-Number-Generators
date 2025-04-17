Okay, shifting the focus to high performance and energy efficiency using C++ on a RISC-V target is an excellent way to add depth to the project. This requires choosing algorithms known for speed and implementing them carefully, likely leveraging a multi-precision library like GMP.

Here's a revised project plan focusing on C++ and efficiency:

---

## Revised Project Structure (PDF Outline for C++/Performance Focus)

1.  **Title Page** (Course Name, Assignment Title, Your Name, Student ID, Date, Target: C++/RISC-V)
2.  **Table of Contents**
3.  **Introduction**
    * Briefly introduce prime numbers and their importance in cryptography.
    * State the project's objective: implementing and analyzing *high-performance* pseudo-random number generators (PRNGs) and primality tests using C++ for large numbers, considering suitability for embedded targets like RISC-V.
    * Mention the use of the GMP library for arbitrary-precision arithmetic.
    * Outline the report structure.
4.  **Section 1: High-Performance Pseudo-Random Number Generation**
    * **1.1 Algorithm 1: Linear Congruential Generator (LCG)**
        * 1.1.1 Description & References (Mention simplicity, limitations for crypto)
        * 1.1.2 C++/GMP Implementation (Documented Code)
    * **1.2 Algorithm 2: Xoshiro256++**
        * 1.2.1 Description & References (Mention speed, state size, statistical quality)
        * 1.2.2 C++/GMP Implementation (Documented Code)
    * **1.3 Experiment**
        * 1.3.1 Methodology (C++ compilation, GMP usage, timing with `<chrono>`, `k`-bit generation strategy, number of runs)
        * 1.3.2 Results (Table: Algorithm, Bit Size, Average Time)
        * 1.3.3 Limitations (PRNG suitability for crypto, GMP overhead)
    * **1.4 Analysis**
        * 1.4.1 Comparison of Generation Times (Focus on C++ performance)
        * 1.4.2 Complexity Analysis (Operations count, expected performance)
        * 1.4.3 Energy Efficiency Considerations (Qualitative: instruction mix, potential for RISC-V optimization)
5.  **Section 2: Efficient Primality Testing**
    * **2.1 Algorithm 1: Miller-Rabin Primality Test**
        * 2.1.1 Description & References (Probabilistic nature, efficiency)
        * 2.1.2 C++/GMP Implementation (Documented Code)
    * **2.2 Algorithm 2: Baillie-PSW Primality Test**
        * 2.2.1 Description & References (Stronger probabilistic test, combination of Fermat and Lucas tests)
        * 2.2.2 Justification for Choice (High confidence with fewer iterations compared to pure MR, practical efficiency)
        * 2.2.3 C++/GMP Implementation (Documented Code, including structure for Strong Lucas Test)
    * **2.3 Experiment**
        * 2.3.1 Methodology (Finding primes using PRNGs + Miller-Rabin/BPSW, handling odd numbers, number of MR rounds `k`)
        * 2.3.2 Difficulties Encountered (BPSW implementation complexity, GMP usage, large number generation times)
        * 2.3.3 Results (Table: Test Used, Bit Size, Found Prime, Time to Find)
    * **2.4 Analysis**
        * 2.4.1 Comparison of Primality Test Performance (Time to find primes, timing individual tests on found primes)
        * 2.4.2 Complexity Analysis (Focus on operations count using GMP, MR vs. BPSW steps)
        * 2.4.3 Pseudoprime Discussion (How BPSW handles Carmichael numbers compared to basic Fermat; reliability)
        * 2.4.4 Energy Efficiency Considerations (Qualitative: cost of modular exponentiation vs. Lucas sequences)
6.  **Conclusion**
    * Summarize key findings regarding high-performance PRNGs and primality tests in C++.
    * Reflect on the challenges, GMP usage, and potential for RISC-V deployment (performance, energy).
7.  **References**
    * List all books, articles, websites, GMP documentation, etc.
8.  **Appendix (Optional)**
    * Full C++ source code if too long for embedding.
    * Makefile or build instructions.

---

## Required Tools and Libraries

* **C++ Compiler:** g++ or clang supporting C++11 or later.
* **GMP Library:** GNU Multiple Precision Arithmetic Library.
    * Install on Debian/Ubuntu: `sudo apt-get update && sudo apt-get install libgmp-dev`
    * Install on macOS (using Homebrew): `brew install gmp`
    * Compile Flags: You'll need to link against GMP, e.g., `g++ your_code.cpp -o your_exec -lgmp -lstdc++ -std=c++11`

---

## Section 1: High-Performance Pseudo-Random Number Generation

### Algorithm Choices:

1.  **LCG:** Baseline simple generator.
2.  **Xoshiro256++:** Modern, fast, good statistics, suitable for C++.

### 1.1 Algorithm 1: Linear Congruential Generator (LCG)

#### 1.1.1 Description & References

*(Description is similar to the Python version, emphasizing its formula `X_{n+1} = (a * X_n + c) % m`. Note its simplicity, speed for basic operations, but known statistical weaknesses and predictability, making it unsuitable for cryptographic use. Mention that parameters `a, c, m` are crucial. We will use GMP for calculations to handle potentially large states if needed, although standard 64-bit integers are often used for the state in practice before combining).*

**References:**
* Knuth, D. E. (1997). *The Art of Computer Programming, Vol 2*.
* Wikipedia: Linear congruential generator.
* *(Specific parameters source if applicable)*

#### 1.1.2 C++/GMP Implementation

```c++
#include <gmp.h>
#include <vector>
#include <cmath>
#include <stdexcept> // For error handling
#include <chrono>    // For seeding

// Simple LCG parameters (example: from POSIX standard, requires 64-bit state)
// Using uint64_t for state here for simplicity and speed, combine outputs using GMP.
#define LCG_A 6364136223846793005ULL
#define LCG_C 1ULL // Can be 0 for multiplicative congruential generator (MCG)
// Modulus M is implicitly 2^64 due to uint64_t overflow

class LCG {
private:
    uint64_t state; // Use standard 64-bit integer for LCG state for performance

public:
    // Seed using system clock or a better source if available
    LCG(uint64_t seed) : state(seed) {
        if (seed == 0) { // Ensure initial state is not zero if c=0
             state = std::chrono::high_resolution_clock::now().time_since_epoch().count();
             if (state == 0) state = 1; // Avoid 0 state
        }
    }

    // Generate the next 64-bit pseudo-random value
    uint64_t next_u64() {
        state = LCG_A * state + LCG_C; // Modulo 2^64 is implicit
        return state;
    }

    // Generate a k-bit random number using GMP
    void randbits(mpz_t result, int k) {
        if (k <= 0) {
            mpz_set_ui(result, 0);
            return;
        }

        int bits_per_output = 64;
        int num_outputs = std::ceil(static_cast<double>(k) / bits_per_output);

        mpz_set_ui(result, 0); // Initialize GMP integer to 0

        for (int i = 0; i < num_outputs; ++i) {
            uint64_t rand_val = next_u64();
            mpz_mul_2exp(result, result, bits_per_output); // Left shift result by 64 bits
            mpz_add_ui(result, result, rand_val); // Add the new 64 bits
            // For GMP > 6.0, use mpz_add_ui for values up to ULONG_MAX.
            // For older GMP or > ULONG_MAX, need temp mpz_t for rand_val.
            // mpz_t temp_val; mpz_init_set_ui(temp_val, rand_val);
            // mpz_add(result, result, temp_val);
            // mpz_clear(temp_val);
        }

        // Trim excess bits from the left (most significant)
        int bits_generated = num_outputs * bits_per_output;
        if (bits_generated > k) {
            mpz_fdiv_q_2exp(result, result, bits_generated - k); // Right shift
        }

        // Ensure the number has exactly k bits (set MSB to 1)
        if (k > 0) {
             mpz_setbit(result, k - 1);
        }
         // Optional: Ensure it doesn't exceed k bits (mask) - usually handled by shift
         // mpz_t mask; mpz_init(mask);
         // mpz_setbit(mask, k); // mask = 2^k
         // mpz_sub_ui(mask, mask, 1); // mask = 2^k - 1
         // mpz_and(result, result, mask);
         // mpz_clear(mask);
    }
};
```

### 1.2 Algorithm 2: Xoshiro256++

#### 1.2.1 Description & References

**Description:**
Xoshiro256++ is a modern pseudo-random number generator from the Xorshift family, developed by David Blackman and Sebastiano Vigna. It maintains a state of 256 bits (typically as four 64-bit integers). It's known for its excellent speed, good statistical properties (passing rigorous test suites like BigCrush), and a long period of 2²⁵⁶ − 1.

The core operations involve:
1.  **State Update:** The state integers are updated using a combination of bitwise XOR (`^`), left shifts (`<<`), and rotations (`rotl`).
2.  **Output Generation (`++` scrambler):** The final output is generated by applying an additional scrambling step (addition and rotation) to one of the state words *after* the state has been updated. This `++` scrambler improves the statistical quality of the lower bits compared to the related Xoshiro256** generator.

It generates 64-bit random integers per call. Like LCG, these need to be combined to form larger `k`-bit numbers. Its simplicity (using only basic integer operations) makes it very efficient in C/C++.

**References:**
* Blackman, D., & Vigna, S. (2019). Scrambled Linear Pseudorandom Number Generators. *arXiv preprint arXiv:1805.01407v5*. [https://arxiv.org/abs/1805.01407](https://arxiv.org/abs/1805.01407)
* Vigna, S. (Website). xoshiro / xoroshiro generators and the PRNG shootout. [https://prng.di.unimi.it/](https://prng.di.unimi.it/) *(Official source for code and parameters)*

#### 1.2.2 C++/GMP Implementation

```c++
#include <gmp.h>
#include <vector>
#include <cmath>
#include <cstdint> // For uint64_t
#include <stdexcept>
#include <chrono>
#include <random> // For seeding the state array better

// Helper function for 64-bit left rotation
static inline uint64_t rotl(const uint64_t x, int k) {
    return (x << k) | (x >> (64 - k));
}

class Xoshiro256pp {
private:
    uint64_t s[4]; // 256 bits of state

    // Seed function using std::mt19937_64 for better initial state distribution
    void seed_state(uint64_t seed_val) {
         std::mt19937_64 seeder(seed_val);
         // Fill the state array with values from a reliable generator
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
             s[0] = 1; // Or some other non-zero pattern
         }
    }


public:
    Xoshiro256pp(uint64_t seed) {
        seed_state(seed);
    }

    // Generate the next 64-bit pseudo-random value
    uint64_t next_u64() {
        // The core Xoshiro256++ algorithm:
        const uint64_t result = rotl(s[0] + s[3], 23) + s[0]; // ++ scrambler part 1

        const uint64_t t = s[1] << 17;

        s[2] ^= s[0];
        s[3] ^= s[1];
        s[1] ^= s[2];
        s[0] ^= s[3];

        s[2] ^= t;

        s[3] = rotl(s[3], 45);

        return result; // Return the scrambled result
    }

    // Generate a k-bit random number using GMP (identical logic to LCG's randbits)
    void randbits(mpz_t result, int k) {
         if (k <= 0) {
            mpz_set_ui(result, 0);
            return;
        }

        int bits_per_output = 64;
        int num_outputs = std::ceil(static_cast<double>(k) / bits_per_output);

        mpz_set_ui(result, 0); // Initialize GMP integer to 0

        for (int i = 0; i < num_outputs; ++i) {
            uint64_t rand_val = next_u64();
            mpz_mul_2exp(result, result, bits_per_output); // Left shift result by 64 bits
            // Using mpz_add_ui might be slightly faster if GMP is optimized for it
             mpz_add_ui(result, result, rand_val);
            // Or use mpz_t intermediate for guaranteed compatibility/large values if needed
            // mpz_t temp_val; mpz_init(temp_val); mpz_import(temp_val, 1, -1, sizeof(uint64_t), 0, 0, &rand_val);
            // mpz_add(result, result, temp_val);
            // mpz_clear(temp_val);

        }

        // Trim excess bits
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
```

### 1.3 Experiment

#### 1.3.1 Methodology

* **Compilation:** Compile the C++ code with optimizations (`-O2` or `-O3`) and link against GMP (`-lgmp`). Example: `g++ main.cpp -o main_exec -std=c++11 -O2 -lgmp`
* **Timing:** Use `<chrono>::high_resolution_clock` before and after the `randbits` call. Calculate the average time over `N` runs (e.g., N=100 or N=1000 for stability).
* **`k`-bit Generation:** Use the `randbits` method implemented in each class.
* **Bit Sizes:** `[40, 56, 80, 128, 168, 224, 256, 512, 1024, 2048, 4096]`

```c++
// --- main.cpp (Example Structure) ---
#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip> // For formatting output
#include <numeric> // For std::accumulate
#include <gmp.h>

// Include your LCG and Xoshiro256pp class definitions here...
#include "lcg.h" // Assuming LCG class is in lcg.h
#include "xoshiro.h" // Assuming Xoshiro256pp class is in xoshiro.h


// Function to time PRNG generation
template<typename PRNG_T>
double time_prng_randbits(PRNG_T& prng, int k_bits, int num_runs) {
    std::vector<double> times;
    times.reserve(num_runs);
    mpz_t temp_num; // Use one mpz_t variable repeatedly
    mpz_init(temp_num);

    for (int i = 0; i < num_runs; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        prng.randbits(temp_num, k_bits); // Generate the number
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration_ms = end - start;
        times.push_back(duration_ms.count());
    }

    mpz_clear(temp_num);

    // Calculate average time
    double sum = std::accumulate(times.begin(), times.end(), 0.0);
    return sum / num_runs;
}

int main() {
    // Use a consistent seed for comparability during testing runs
    uint64_t seed = 1234567890ABCDEFULL; // Or use chrono for unique runs

    LCG lcg_gen(seed);
    Xoshiro256pp xoshiro_gen(seed);

    std::vector<int> bit_sizes = {40, 56, 80, 128, 168, 224, 256, 512, 1024, 2048, 4096};
    int num_average_runs = 100; // Adjust as needed

    std::cout << "--- PRNG Performance Test ---" << std::endl;
    std::cout << std::fixed << std::setprecision(6); // Format output

    std::cout << "| Algorithm         | Number Size (bits) | Avg. Time (ms) |" << std::endl;
    std::cout << "|-------------------|--------------------|----------------|" << std::endl;

    // Time LCG
    for (int k : bit_sizes) {
        double avg_time_ms = time_prng_randbits(lcg_gen, k, num_average_runs);
        std::cout << "| LCG               | " << std::setw(18) << k << " | "
                  << std::setw(14) << avg_time_ms << " |" << std::endl;
    }

     // Time Xoshiro256++
    for (int k : bit_sizes) {
        double avg_time_ms = time_prng_randbits(xoshiro_gen, k, num_average_runs);
        std::cout << "| Xoshiro256++      | " << std::setw(18) << k << " | "
                  << std::setw(14) << avg_time_ms << " |" << std::endl;
    }
     std::cout << "--------------------------------------------------------" << std::endl;


    return 0;
}
```

#### 1.3.3 Limitations

* **LCG:** Still not cryptographically secure. Performance depends heavily on the quality of the `uint64_t` multiplication on the target CPU.
* **Xoshiro256++:** While statistically excellent, it's not designed to resist prediction attacks if the internal state is compromised. For true cryptographic needs, stream ciphers (like ChaCha20) or hash-based generators are often used, though Xoshiro is much faster.
* **GMP Overhead:** Using GMP adds overhead compared to native integer operations, especially for initialization and function calls. However, it's necessary for the >64-bit sizes.

### 1.4 Analysis

* **1.4.1 Generation Times:** Compare the results table. Expect Xoshiro256++ to be significantly faster than LCG, especially if the LCG's `uint64_t` multiplication is slow. Both should show time increasing roughly linearly with `k` due to the loop in `randbits`.
* **1.4.2 Complexity Analysis:**
    * LCG `next_u64`: O(1) - one multiply, one add.
    * Xoshiro `next_u64`: O(1) - several shifts, XORs, rotates, adds. Constant number of operations per call.
    * `randbits(k)` for both: O(k) - requires `ceil(k/64)` calls to `next_u64` and O(k) work within GMP for shifts/adds/ORs. The constant factor from `next_u64` and GMP operations determines the actual speed. Xoshiro's core operations are often faster on modern CPUs than multiplication.
* **1.4.3 Energy Efficiency:**
    * Xoshiro uses simpler instructions (shifts, XORs, adds) which *may* consume less energy per operation than LCG's multiplication on some architectures.
    * Fewer cycles per generated bit generally correlates with lower energy. If Xoshiro is faster, it's likely more energy-efficient.
    * RISC-V specific: If the target has Bit Manipulation ('B') extensions, operations like rotations (`rotl`) in Xoshiro might map directly to efficient instructions. GMP operations' efficiency depends heavily on how well GMP is ported/optimized for the specific RISC-V target.

---

## Section 2: Efficient Primality Testing

### Algorithm Choices:

1.  **Miller-Rabin:** Mandatory, standard probabilistic test.
2.  **Baillie-PSW:** Stronger probabilistic test, often faster for high confidence.

### 2.1 Algorithm 1: Miller-Rabin Primality Test

#### 2.1.1 Description & References

*(Similar description to Python version. Emphasize probabilistic nature, `k` rounds, reliance on modular exponentiation (`mpz_powm`), and steps involving finding `s, d` where `n-1 = 2^s * d`)*

**References:**
* Cormen, T. H., et al. (2009). *Introduction to Algorithms*.
* GMP Manual (for `mpz_probab_prime_p` which implements MR)
* Wikipedia: Miller–Rabin primality test.

#### 2.1.2 C++/GMP Implementation

```c++
#include <gmp.h>
#include <vector>
#include <chrono>
#include <stdexcept>

// GMP's random state - should be initialized once
gmp_randstate_t gmp_randstate;

// Function to initialize GMP random state (call once at start)
void init_gmp_random() {
    unsigned long seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    gmp_randinit_mt(gmp_randstate); // Use Mersenne Twister for GMP's internal randomness
    gmp_randseed_ui(gmp_randstate, seed);
}

// Function to clean up GMP random state (call once at end)
void clear_gmp_random() {
    gmp_randclear(gmp_randstate);
}


// Miller-Rabin Test Implementation
// Returns true if n is probably prime, false if composite.
bool miller_rabin_test(const mpz_t n, int k) {
    // Handle base cases
    if (mpz_cmp_ui(n, 2) < 0) return false; // n < 2
    if (mpz_cmp_ui(n, 3) == 0) return true; // n == 3
    if (mpz_even_p(n)) return false;       // n is even and > 2

    mpz_t n_minus_1, d, a, x, two;
    mpz_init(n_minus_1); mpz_init(d); mpz_init(a); mpz_init(x);
    mpz_init_set_ui(two, 2);

    // n_minus_1 = n - 1
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

    // Perform k rounds
    for (int i = 0; i < k; ++i) {
        // Choose random witness 'a' in range [2, n-2]
        // Need n-3 for upper bound of mpz_urandomm
        mpz_t n_minus_3;
        mpz_init(n_minus_3);
        if (mpz_cmp_ui(n, 4) <= 0) { // Handle n=4 case for range
             mpz_set_ui(n_minus_3, 1);
        } else {
             mpz_sub_ui(n_minus_3, n, 3); // n_minus_3 = n - 3
        }

        mpz_urandomm(a, gmp_randstate, n_minus_3); // a = random in [0, n-4]
        mpz_add_ui(a, a, 2);                       // a = random in [2, n-2]
        mpz_clear(n_minus_3);


        // Calculate x = a^d mod n
        mpz_powm(x, a, d, n);

        // Check if x == 1 or x == n - 1
        if (mpz_cmp_ui(x, 1) == 0 || mpz_cmp(x, n_minus_1) == 0) {
            continue; // This round passed, try next 'a'
        }

        // Check squares: x^(2^r) mod n for r from 1 to s-1
        bool maybe_prime = false;
        for (unsigned long r = 0; r < s; ++r) { // Check s times (includes initial x)
             if (r > 0) { // Don't square the first time (x is already a^d mod n)
                  mpz_powm_sec(x, x, two, n); // Use _sec for potential side-channel resistance
             }

             // If x == 1, then we found non-trivial sqrt of 1 earlier => composite
             if (mpz_cmp_ui(x, 1) == 0) {
                  mpz_clear(n_minus_1); mpz_clear(d); mpz_clear(a); mpz_clear(x); mpz_clear(two);
                  return false; // Composite
             }
             // If x == n - 1, it passes this round for this 'a'
             if (mpz_cmp(x, n_minus_1) == 0) {
                  maybe_prime = true;
                  break; // Go to next 'a'
             }
        }

        // If the inner loop finished without finding x == n-1, then n is composite
        if (!maybe_prime) {
             mpz_clear(n_minus_1); mpz_clear(d); mpz_clear(a); mpz_clear(x); mpz_clear(two);
             return false; // Composite
        }
    }

    // If all k rounds passed
    mpz_clear(n_minus_1); mpz_clear(d); mpz_clear(a); mpz_clear(x); mpz_clear(two);
    return true; // Probably prime
}
```

### 2.2 Algorithm 2: Baillie-PSW Primality Test

#### 2.2.1 Description & References

**Description:**
The Baillie-Pomerance-Selfridge-Wagstaff (BPSW) test is a composite primality test combining a strong Miller-Rabin test (typically base 2) with a strong Lucas pseudoprime test. It is highly reliable; no composite numbers are currently known to pass the BPSW test, and it's conjectured (but not proven) that none exist. It's often used as a faster alternative to running many Miller-Rabin rounds for achieving very high confidence, especially for numbers up to 64 bits where it's considered deterministic.

The steps are:
1.  **Trial Division:** Check for divisibility by small primes up to a certain bound (e.g., primes up to 1000). If a factor is found, `n` is composite.
2.  **Check Perfect Square:** If `n` is a perfect square, it's composite. (`mpz_perfect_square_p`)
3.  **Miller-Rabin Base 2:** Perform the Miller-Rabin test with the single base `a=2`. If it fails, `n` is composite.
4.  **Strong Lucas Pseudoprime Test:**
    a.  **Choose D:** Find the first integer `D` in the sequence `5, -7, 9, -11, 13, ...` such that the Jacobi symbol `(D/n)` equals -1. The Jacobi symbol (`mpz_jacobi`) calculation is crucial here. If `(D/n) == 0`, then `n` shares a factor `D` and is composite (if `n > |D|`).
    b.  **Set Parameters:** Set `P = 1` and `Q = (1 - D) / 4`.
    c.  **Lucas Sequence Calculation:** Calculate the Lucas sequence terms `U_k mod n` and `V_k mod n` where `k = n + 1`. This requires an efficient algorithm, often denoted `lucas_sequence(n, P, Q, k, &U_k, &V_k)`, analogous to modular exponentiation.
    d.  **Check Conditions:** Let `n + 1 = 2^s * d` (where `d` is odd, same `s`, `d` as in Miller-Rabin for `n+1`). The number `n` passes the strong Lucas test if either `U_d ≡ 0 (mod n)` OR `V_{d*2^r} ≡ 0 (mod n)` for some `0 <= r < s`.
5.  **Result:** If `n` passes all steps (trial division, not square, MR base 2, and strong Lucas test), it is declared "probably prime" with very high confidence.

**References:**
* Baillie, R., & Wagstaff Jr, S. S. (1980). Lucas pseudoprimes. *Mathematics of Computation*, 35(152), 1391-1417.
* Pomerance, C., Selfridge, J. L., & Wagstaff Jr, S. S. (1980). The pseudoprimes to 25· 10⁹. *Mathematics of Computation*, 35(151), 1003-1026.
* Wikipedia: Baillie–PSW primality test.
* Crandall, R., & Pomerance, C. (2005). *Prime Numbers: A Computational Perspective*. Springer. (Detailed algorithms for Lucas sequences)

#### 2.2.2 Justification for Choice

Chosen for its high reliability and potential efficiency gains over running many Miller-Rabin rounds (e.g., k=40) to achieve similar confidence levels. It represents a practical, strong alternative commonly used in computational number theory.

#### 2.2.3 C++/GMP Implementation (Structure and Key Parts)

**Note:** Implementing the Strong Lucas Test correctly is non-trivial. The code below provides the structure and highlights necessary GMP functions. A full, optimized `strong_lucas_test` function requires careful implementation based on algorithms described in Crandall & Pomerance or other number theory texts.

```c++
#include <gmp.h>
#include <vector>
#include <cmath> // For std::abs

// Forward declaration for the Strong Lucas Test helper function
bool strong_lucas_test(const mpz_t n);

// Baillie-PSW Test Implementation
// Returns true if n is probably prime (very high confidence), false if composite.
bool baillie_psw_test(const mpz_t n) {
    // Handle base cases (same as Miller-Rabin)
    if (mpz_cmp_ui(n, 2) < 0) return false;
    if (mpz_cmp_ui(n, 3) == 0) return true;
    if (mpz_even_p(n)) return false;

    // 1. Trial Division (Example: check primes up to 100)
    //    A more thorough implementation would use a larger bound or wheel factorization.
    const int trial_limit = 100;
    const int small_primes[] = {3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97};
    for (int p : small_primes) {
        if (p >= trial_limit) break;
        if (mpz_cmp_ui(n, p) == 0) return true; // n is a small prime
        if (mpz_divisible_ui_p(n, p)) {
            return false; // Divisible by small prime -> composite
        }
    }

    // 2. Check Perfect Square
    if (mpz_perfect_square_p(n)) {
        return false; // Perfect square -> composite
    }

    // 3. Miller-Rabin Base 2 (using our existing function, k=1)
    //    Need mpz_t variable for base 2
    mpz_t base2;
    mpz_init_set_ui(base2, 2);

    // Need temporary variables for MR test base 2
    mpz_t n_minus_1_mr, d_mr, x_mr;
    mpz_init(n_minus_1_mr); mpz_init(d_mr); mpz_init(x_mr);

    // n_minus_1 = n - 1
    mpz_sub_ui(n_minus_1_mr, n, 1);

    // Find d and s such that n - 1 = 2^s * d
    mpz_set(d_mr, n_minus_1_mr);
    unsigned long s_mr = 0;
    while (mpz_even_p(d_mr)) {
        mpz_fdiv_q_2exp(d_mr, d_mr, 1);
        s_mr++;
    }
    // Calculate x = 2^d mod n
    mpz_powm(x_mr, base2, d_mr, n);

    bool mr_base2_passed = false;
    if (mpz_cmp_ui(x_mr, 1) == 0 || mpz_cmp(x_mr, n_minus_1_mr) == 0) {
        mr_base2_passed = true;
    } else {
        for (unsigned long r = 1; r < s_mr; ++r) {
            mpz_powm_sec(x_mr, x_mr, base2, n); // Square x mod n
            if (mpz_cmp(x_mr, n_minus_1_mr) == 0) {
                 mr_base2_passed = true;
                 break;
            }
             if (mpz_cmp_ui(x_mr, 1) == 0) { // Non-trivial sqrt of 1 found
                 mr_base2_passed = false;
                 break;
            }
        }
    }
    mpz_clear(base2); mpz_clear(n_minus_1_mr); mpz_clear(d_mr); mpz_clear(x_mr);

    if (!mr_base2_passed) {
        return false; // Failed MR base 2 -> composite
    }


    // 4. Strong Lucas Pseudoprime Test
    if (!strong_lucas_test(n)) {
        return false; // Failed Lucas test -> composite
    }

    // 5. Result: Passed all tests
    return true; // Probably prime (high confidence)
}


// Helper function for Strong Lucas Test (Requires careful implementation)
// This is a structural placeholder - the actual Lucas sequence calculation is complex.
bool strong_lucas_test(const mpz_t n) {
    // Find D = 5, -7, 9, -11, ... such that Jacobi(D/n) = -1
    long D_val = 5;
    int sign = 1;
    mpz_t D;
    mpz_init(D);

    while (true) {
        mpz_set_si(D, D_val);
        int jacobi = mpz_jacobi(D, n);

        if (jacobi == 0) {
            // If gcd(|D|, n) == n, then n divides D. Since n > D usually, check if n=|D|
            if (mpz_cmpabs_ui(n, std::abs(D_val)) == 0) {
                mpz_clear(D);
                return true; // n is |D|, which should have been caught by trial division earlier
            }
            mpz_clear(D);
            return false; // Jacobi is 0 means gcd(D, n) > 1 -> composite
        }
        if (jacobi == -1) {
            break; // Found suitable D
        }

        // Move to next D in sequence 5, -7, 9, -11, ...
        sign *= -1;
        D_val = (std::abs(D_val) + 2) * sign;

        // Safety break (optional, theory says D is usually small)
        if (std::abs(D_val) > 1000) {
             std::cerr << "Warning: Could not find suitable D for Lucas test quickly." << std::endl;
             mpz_clear(D);
             return false; // Or handle error appropriately
        }
    }

    long P_val = 1;
    // Q = (1 - D) / 4
    mpz_t Q, one, D_local, Q_num; // Use temporary mpz_t for calculations
    mpz_init(Q); mpz_init_set_ui(one, 1); mpz_init_set(D_local, D); mpz_init(Q_num);
    mpz_sub(Q_num, one, D_local); // 1 - D
    // Check divisibility by 4 before division
    if (!mpz_divisible_ui_p(Q_num, 4)) {
         std::cerr << "Error: (1-D) is not divisible by 4 in Lucas test." << std::endl;
         mpz_clear(D); mpz_clear(Q); mpz_clear(one); mpz_clear(D_local); mpz_clear(Q_num);
         return false; // Should not happen if D is chosen correctly
    }
    mpz_fdiv_q_ui(Q, Q_num, 4); // Q = (1 - D) / 4
    long Q_val = mpz_get_si(Q); // Assume Q fits in long for this example

    mpz_clear(one); mpz_clear(D_local); mpz_clear(Q_num); mpz_clear(Q);
    mpz_clear(D); // D mpz_t no longer needed


    // Calculate k = n + 1. Find d, s such that k = 2^s * d (d odd)
    mpz_t k_lucas, d_lucas;
    mpz_init(k_lucas); mpz_init(d_lucas);
    mpz_add_ui(k_lucas, n, 1);
    mpz_set(d_lucas, k_lucas);
    unsigned long s_lucas = 0;
    while (mpz_even_p(d_lucas)) {
        mpz_fdiv_q_2exp(d_lucas, d_lucas, 1);
        s_lucas++;
    }

    // *** Calculate Lucas Sequence U_d mod n and V_d mod n ***
    // This is the core complex part. Requires a function like:
    // lucas_sequence_mod_n(const mpz_t n, long P, long Q, const mpz_t k, mpz_t U_k, mpz_t V_k);
    // See Crandall & Pomerance Ch 4 or online resources for algorithms (e.g., using exponentiation of matrices mod n).
    mpz_t U_d, V_d;
    mpz_init(U_d); mpz_init(V_d);
    // Placeholder: Assume lucas_uv_mod(U_d, V_d, P_val, Q_val, d_lucas, n); exists
    bool lucas_implemented = false; // Set to true if you implement the below
    if (!lucas_implemented) {
        std::cerr << "Warning: Strong Lucas Test U_d, V_d calculation not implemented." << std::endl;
        mpz_clear(k_lucas); mpz_clear(d_lucas); mpz_clear(U_d); mpz_clear(V_d);
        return true; // Or false, depending on desired behavior for unimplemented part
    }

    // Check Strong Lucas conditions:
    // 1. U_d == 0 mod n?
    if (mpz_sgn(U_d) == 0) { // mpz_sgn returns 0 if number is 0
        mpz_clear(k_lucas); mpz_clear(d_lucas); mpz_clear(U_d); mpz_clear(V_d);
        return true; // Passed
    }

    // 2. V_{d*2^r} == 0 mod n for some 0 <= r < s?
    mpz_t V_term, two_val; // Need V_d, and will compute V_{2d}, V_{4d}, ...
    mpz_init_set(V_term, V_d); // Start with V_d
    mpz_init_set_ui(two_val, 2);

    for (unsigned long r = 0; r < s_lucas; ++r) {
        if (mpz_sgn(V_term) == 0) { // V_{d*2^r} == 0 mod n?
            mpz_clear(k_lucas); mpz_clear(d_lucas); mpz_clear(U_d); mpz_clear(V_d);
            mpz_clear(V_term); mpz_clear(two_val);
            return true; // Passed
        }
        if (r < s_lucas - 1) { // Don't compute next V if it's the last iteration
            // Use V_{2k} = V_k^2 - 2*Q^k mod n. Need Q^k mod n.
            // Simpler relation: V_{2k} = V_k^2 - 2 (mod n) if Q=1
            // More general: V_{2k} = V_k^2 - 2*Q^(d*2^r) mod n. Calculating Q powers is extra work.
            // Standard method uses relation V_{2k} = (V_k * V_k - 2 * Q^k) mod n or similar.
            // Let's use V_{2k} = V_k^2 - 2 (mod n) assuming Q=1 (often the case for first D)
            // A full implementation needs the general formula involving Q.
            if (Q_val == 1) {
                 mpz_powm_sec(V_term, V_term, two_val, n); // V_k^2 mod n
                 mpz_sub_ui(V_term, V_term, 2);          // V_k^2 - 2
                 mpz_mod(V_term, V_term, n);             // (V_k^2 - 2) mod n
            } else {
                // Need full V_{2k} calculation involving Q
                std::cerr << "Warning: Lucas V_{2k} calculation for Q != 1 not fully implemented." << std::endl;
                // As placeholder, assume it passes if Q!=1 for now.
                 break; // Exit loop, assume pass
            }
        }
    }

    // If loop finishes and V_term never became 0
    mpz_clear(k_lucas); mpz_clear(d_lucas); mpz_clear(U_d); mpz_clear(V_d);
    mpz_clear(V_term); mpz_clear(two_val);
    return false; // Failed
}

```

### 2.3 Experiment

#### 2.3.1 Methodology

* **Prime Finding:** Use one of the PRNGs (e.g., Xoshiro256++) to generate candidates. Use `miller_rabin_test` (with `k=40` or similar) or `baillie_psw_test` to check primality. Record the time taken to *find* a prime for each bit size `[40, ..., 4096]`. Make sure candidates are odd.
* **Implementation:** Create a C++ function `find_prime_gmp` similar to the Python version, taking the PRNG, bit size, and primality test function (`miller_rabin_test` or `baillie_psw_test`) as arguments.
* **Primality Test Comparison:** Once primes are found, time how long `miller_rabin_test(prime, k=40)` and `baillie_psw_test(prime)` take individually on those specific primes (average over several runs).

*(The C++ code structure for `find_prime_gmp` and the timing loop would be analogous to the Python version, but using C++ chrono, GMP types, and the C++ PRNG/primality functions).*

#### 2.3.2 Difficulties Encountered

* **BPSW Implementation:** Correctly implementing the Strong Lucas Test is the main challenge. Debugging it requires known test vectors (primes and Lucas pseudoprimes). Relying on established libraries or meticulously following algorithms from trusted sources (like Crandall/Pomerance) is advised.
* **GMP Usage:** Correctly initializing, using, and clearing `mpz_t` variables is essential to avoid memory leaks or incorrect results. Understanding GMP function parameters (input vs. output) is key.
* **Performance:** Finding 4096-bit primes will still be time-consuming, even in C++. Compilation with optimization (`-O2` or `-O3`) is crucial.

#### 2.3.3 Results

* Generate tables showing:
    * Time to find a prime of size `k` using a specific test (e.g., Miller-Rabin).
    * Comparison of average time taken by Miller-Rabin (`k=40`) vs. Baillie-PSW on the *same set* of found large primes.

### 2.4 Analysis

* **2.4.1 Test Performance:** Compare the times. Is BPSW significantly faster than MR(k=40) for testing a *given* large number? How does the time to *find* a prime compare when using MR vs. BPSW as the checking function (consider the trade-off between fewer BPSW calls vs. potentially more, faster MR calls)?
* **2.4.2 Complexity Analysis:**
    * Miller-Rabin: O(k * log³ n).
    * Baillie-PSW: Dominated by the Miller-Rabin base 2 test (O(log³ n)) and the Strong Lucas Test. The Lucas test calculation (`U_k`, `V_k`) also has a complexity related to O(log³ n) using methods similar to modular exponentiation. The overall complexity is still roughly **O(log³ n)** because the number of MR rounds `k` is replaced by the fixed steps of BPSW. The constant factors differ. BPSW involves Jacobi symbol calculation and the Lucas sequence logic.
* **2.4.3 Pseudoprime Discussion:** BPSW is designed to correctly identify Carmichael numbers (and other pseudoprimes) as composite. Test `baillie_psw_test` on 561, 1105, 1729 – it should return `false`. Contrast this with the failure of the basic Fermat test.
* **2.4.4 Energy Efficiency:**
    * If BPSW is faster on average than MR(k=40) for the required confidence level, it's likely more energy-efficient.
    * The specific instruction mix (modular exponentiation vs. Jacobi symbol vs. Lucas sequence math) matters. Lucas sequence calculations can be complex.
    * Actual measurement on the RISC-V board would be needed for definitive conclusions.

---

## Final Steps

1.  **Implement & Compile:** Write the C++ code, using GMP. Ensure it compiles with optimizations (`-O2`/`-O3`) and links (`-lgmp`).
2.  **Implement Lucas Test (or find reference):** Address the Strong Lucas Test implementation - either implement it fully or clearly state if using a placeholder and reference a complete algorithm.
3.  **Run Experiments:** Execute the code to get timing results.
4.  **Analyze & Write:** Fill in the report sections based on your C++/GMP implementation and results, focusing on performance and efficiency aspects.
5.  **Include Code:** Embed C++ code snippets (or link to a repo/appendix).
6.  **Cite Sources:** Add citations for algorithms, GMP, C++ features, etc.
7.  **Review:** Proofread the final PDF.

This C++ and performance-focused approach makes the project significantly more challenging, especially the BPSW implementation, but offers deeper insights into practical high-performance computing for cryptographic primitives.