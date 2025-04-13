# Performance Results

This directory contains the performance benchmark results for the various algorithms implemented in this project.

## PRNG Benchmark Results

The file `prng_benchmark.csv` contains the results of benchmarking the LCG and Xoshiro256++ pseudo-random number generators for generating numbers of different bit sizes.

The CSV format is:
```
Algorithm,BitSize,TimeMs
```

Where:
- `Algorithm` is the name of the PRNG algorithm (LCG or Xoshiro256++)
- `BitSize` is the size of the generated number in bits
- `TimeMs` is the average time to generate a number of that size in milliseconds

## Prime Finding Benchmark Results

The file `find_prime_benchmark.csv` contains the results of benchmarking the Miller-Rabin and Baillie-PSW algorithms for finding prime numbers of different bit sizes.

The CSV format is:
```
Algorithm,BitSize,TimeMs,Prime
```

Where:
- `Algorithm` is the name of the primality testing algorithm (Miller-Rabin or Baillie-PSW)
- `BitSize` is the size of the prime number in bits
- `TimeMs` is the time taken to find a prime of that size in milliseconds
- `Prime` is the actual prime number found (truncated for very large primes)

## Primality Testing Benchmark Results

The file `test_prime_benchmark.csv` contains the results of benchmarking the Miller-Rabin and Baillie-PSW algorithms for testing the primality of numbers of different bit sizes.

The CSV format is:
```
Algorithm,BitSize,TimeMs
```

Where:
- `Algorithm` is the name of the primality testing algorithm (Miller-Rabin or Baillie-PSW)
- `BitSize` is the size of the tested number in bits
- `TimeMs` is the average time to test a number of that size in milliseconds

## Analyzing Results

You can import these CSV files into a spreadsheet program like Microsoft Excel or Google Sheets to generate charts and perform further analysis. The data is intentionally provided in a simple format to facilitate analysis and visualization.

For example, you might want to create charts showing:
- How generation time increases with bit size for each PRNG
- How prime finding time increases with bit size for each primality test
- A comparison of Miller-Rabin vs. Baillie-PSW performance for various bit sizes 