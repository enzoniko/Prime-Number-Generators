# Prime Number Generation and Testing

This repository contains the implementation of pseudo-random number generators and primality testing algorithms for cryptographic applications. The project is structured to provide efficient, modular, and well-documented implementations of these algorithms.

## Project Structure

```
.
├── src/                # Source files
│   ├── prng/           # Pseudo-Random Number Generator implementations
│   ├── primality/      # Primality testing algorithm implementations
│   ├── utils/          # Utility functions
│   ├── tests/          # Test implementations
│   └── experiments/    # Performance measurement programs
├── include/            # Header files
│   ├── prng/           # PRNG headers
│   ├── primality/      # Primality test headers
│   └── utils/          # Utility headers
├── docs/               # Documentation
│   └── diagrams/       # PlantUML diagrams
├── experiments/        # RISC-V performance and energy measurement tools
│   ├── bin/            # Compiled experiment binaries
│   ├── configs/        # Configuration files for experiments
│   ├── results/        # Experiment results
│   └── scripts/        # Experiment execution scripts
├── Makefile            # Build system
├── README.md           # This file
└── results/            # Performance results and analysis
```

## Algorithms Implemented

### Pseudo-Random Number Generators
- Linear Congruential Generator (LCG)
- Xoshiro256++ Generator

### Primality Tests
- Miller-Rabin Primality Test
- Baillie-PSW Primality Test

## Requirements

This project requires:
- C++11 or later
- GNU Multiple Precision Arithmetic Library (GMP)
- PlantUML (for viewing diagrams)
- Python 3.6+ with matplotlib and numpy (for result analysis)

### Installation on Ubuntu/Debian

```bash
sudo apt-get update
sudo apt-get install g++ make libgmp-dev plantuml python3 python3-matplotlib python3-numpy
```

### Installation on macOS (using Homebrew)

```bash
brew install gmp plantuml python
pip3 install matplotlib numpy
```

## Building the Project

```bash
make        # Build all binaries
make test   # Run tests
make bench  # Run benchmarks
```

## RISC-V Performance and Energy Experiments

The repository includes specialized tools for measuring algorithm performance and energy consumption on RISC-V platforms:

```bash
# Set up the experiment environment
make riscv-setup

# Run timing experiments
./experiments/scripts/run_experiments.sh --timing --algorithm lcg --bits 1024

# Run energy experiments
./experiments/scripts/run_experiments.sh --energy --algorithm miller_rabin --bits 1024 --duration 600

# Optimize system for accurate measurements
./experiments/scripts/setup_riscv.sh --performance --stop-services

# Analyze results
./experiments/scripts/analyze_results.py --type timing --plot
```

For detailed instructions, see the [experiments README](experiments/README.md).

## Usage Examples

### Generating a Random Number
```cpp
#include "prng/xoshiro.h"
#include <gmp.h>

int main() {
    // Create a Xoshiro256++ generator with a seed
    Xoshiro256pp rng(12345);
    
    // Generate a 1024-bit random number
    mpz_t random_num;
    mpz_init(random_num);
    rng.randbits(random_num, 1024);
    
    // Use the random number...
    gmp_printf("Random number: %Zd\n", random_num);
    
    mpz_clear(random_num);
    return 0;
}
```

### Testing a Number for Primality
```cpp
#include "primality/miller_rabin.h"
#include <gmp.h>

int main() {
    mpz_t number;
    mpz_init_set_str(number, "104729", 10);  // A prime number
    
    // Test with Miller-Rabin (40 rounds for high certainty)
    bool is_prime = miller_rabin_test(number, 40);
    
    printf("The number is %s.\n", is_prime ? "probably prime" : "composite");
    
    mpz_clear(number);
    return 0;
}
```

## Documentation

The documentation includes:
- Algorithm explanations and references
- UML diagrams showing architecture and workflows
- Performance benchmarks and analysis

To view the PlantUML diagrams, you can use:
- VS Code with the PlantUML extension
- Online PlantUML editor (http://www.plantuml.com/plantuml/uml/)
- Command line: `plantuml docs/diagrams/*.puml`

## Performance Results

For detailed performance results and analysis, see the [results](results/README.md) directory.

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## References

- Knuth, D. E. (1997). *The Art of Computer Programming, Vol 2*.
- Blackman, D., & Vigna, S. (2019). Scrambled Linear Pseudorandom Number Generators. *arXiv preprint arXiv:1805.01407v5*.
- Baillie, R., & Wagstaff Jr, S. S. (1980). Lucas pseudoprimes. *Mathematics of Computation*, 35(152), 1391-1417.
- Crandall, R., & Pomerance, C. (2005). *Prime Numbers: A Computational Perspective*. Springer. 