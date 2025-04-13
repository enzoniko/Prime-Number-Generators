# Project Documentation

This directory contains the documentation for the Prime Number Generation and Testing project.

## Diagrams

The `diagrams` directory contains PlantUML diagrams that illustrate the architecture and workflow of the project:

### Architecture Diagram

The `architecture.puml` file contains a class diagram showing the main components of the system and their relationships. This diagram provides a high-level overview of the code structure.

To view the diagram, you can use:
- The PlantUML online server: http://www.plantuml.com/plantuml/
- A PlantUML plugin for your IDE (available for VSCode, IntelliJ, etc.)
- The PlantUML command-line tool: `plantuml architecture.puml`

### Workflow Diagram

The `workflow.puml` file contains a sequence diagram showing the workflow of prime number generation and testing. This diagram illustrates the interactions between different components during the process.

## Implementation Details

### Pseudo-Random Number Generators

#### Linear Congruential Generator (LCG)

The LCG is one of the oldest and simplest pseudo-random number generators. It works by using the recurrence relation:

```
X_{n+1} = (a * X_n + c) mod m
```

Where:
- X is the sequence of pseudo-random values
- a is the multiplier
- c is the increment
- m is the modulus

Our implementation uses:
- a = 6364136223846793005 (from the POSIX standard)
- c = 1
- m = 2^64 (implicit due to 64-bit integer overflow)

#### Xoshiro256++

Xoshiro256++ is a modern, high-quality PRNG with excellent statistical properties and a long period of 2^256 - 1. It's designed by David Blackman and Sebastiano Vigna as an improvement over earlier xorshift-based generators.

The algorithm maintains a state of 256 bits (as four 64-bit integers) and uses a combination of bitwise operations (XOR, shifts, rotations) to update the state and generate outputs.

### Primality Tests

#### Miller-Rabin Primality Test

The Miller-Rabin test is a probabilistic primality test based on Fermat's little theorem and properties of square roots of unity in finite fields. It works as follows:

1. Express n-1 as 2^s * d where d is odd
2. Choose a random base a in the range [2, n-2]
3. Compute a^d mod n
4. If a^d ≡ 1 (mod n) or a^(d*2^r) ≡ -1 (mod n) for some 0 ≤ r < s, then the test passes
5. If the test passes for k different bases, then n is probably prime with a probability of at least 1 - 4^(-k)

#### Baillie-PSW Primality Test

The Baillie-PSW test is a combination of:
1. Trial division by small primes
2. A base-2 Miller-Rabin test
3. A strong Lucas probable prime test

No composite number is currently known to pass the Baillie-PSW test, making it very reliable for practical purposes.

The strong Lucas test involves finding a parameter D with Jacobi symbol (D/n) = -1, computing Lucas sequences, and checking specific conditions on sequence values. The details are complex but provide a powerful complement to the Miller-Rabin test. 