# RISC-V Performance and Energy Experiments

This document provides detailed information about the RISC-V performance and energy measurement tools included in this repository.

## Overview

The experiment tools allow for precise measurement of:
1. **Execution Time**: CPU cycles or nanoseconds required for cryptographic operations
2. **Energy Consumption**: Energy used during continuous algorithm operation

These measurements are critical for evaluating the efficiency of cryptographic algorithms on resource-constrained RISC-V platforms.

## Architecture

The experiment system is organized into several components:

![Experiment Architecture](diagrams/experiment_architecture.png)

### Main Components

- **Experiment Scripts** - Shell scripts that coordinate and run experiments
- **Measurement Programs** - C++ programs that perform the actual measurements
- **Configuration Files** - JSON files containing experiment parameters
- **Results** - CSV and log files storing experiment results
- **Analysis Tools** - Python script for statistical analysis and visualization

## Experiment Types

### Timing Measurements

![Timing Workflow](diagrams/timing_workflow.png)

Timing experiments measure the precise execution time of cryptographic operations:

1. The system measures how long it takes to:
   - Generate a random number of a specific bit size using a PRNG
   - Test the primality of a number using a primality test

2. Multiple iterations are performed to ensure statistical significance

3. Results are stored in CSV files with the format:
   ```
   algorithm,bits,iteration,time_ns
   lcg,1024,1,123456
   lcg,1024,2,123789
   ...
   ```

### Energy Measurements

![Energy Workflow](diagrams/energy_workflow.png)

Energy experiments run algorithms continuously while energy consumption is measured:

1. The system runs algorithms in a loop for a specified duration
2. An external USB power meter records energy consumption
3. The program logs operations-per-second at regular intervals
4. Results can be correlated with power meter readings to calculate energy per operation

## Implementation Details

### Measurement Programs

The measurement programs are implemented in C++ and use the GMP library:

1. **measure_prng_time** - Measures PRNG execution time
   - Uses RDTSC instruction for high-precision timing on x86
   - Falls back to std::chrono for other architectures

2. **measure_primality_time** - Measures primality testing execution time
   - Generates test prime numbers of specified sizes
   - Measures verification time with multiple algorithms

3. **continuous_operation** - Runs algorithms continuously for energy measurements
   - Handles both PRNG and primality testing algorithms
   - Reports statistics periodically for correlation with power readings

### Environment Setup

The `setup_riscv.sh` script prepares the system for accurate measurements by:

1. Setting the CPU governor to performance mode (fixed frequency)
2. Stopping non-essential system services
3. Disabling Wi-Fi and other power-consuming peripherals (optional)
4. Providing system information for experiment documentation

## Usage Workflow

A typical workflow for performance experiments:

1. **Prepare the environment**:
   ```bash
   ./experiments/scripts/setup_riscv.sh --performance --stop-services
   ```

2. **Run timing experiments**:
   ```bash
   ./experiments/scripts/run_experiments.sh --timing --algorithm all
   ```

3. **Run energy experiments**:
   ```bash
   ./experiments/scripts/run_experiments.sh --energy --algorithm miller_rabin --bits 1024
   ```

4. **Analyze results**:
   ```bash
   ./experiments/scripts/analyze_results.py --type timing --plot
   ```

5. **Restore system settings**:
   ```bash
   ./experiments/scripts/setup_riscv.sh --powersave --start-services
   ```

## Configuration Options

The experiments can be configured through `default_config.json` with parameters such as:

- General settings (seed values, CPU affinity)
- Timing parameters (warmup runs, iterations)
- Energy measurement settings (interval time)
- Algorithm-specific parameters (Miller-Rabin iterations)
- Bit sizes to test

## Best Practices

For accurate measurements:

1. **Control Variables**:
   - Use a consistent power supply
   - Maintain stable temperature
   - Disable dynamic frequency scaling

2. **Statistical Significance**:
   - Run multiple iterations (10+ recommended)
   - Calculate mean, median, and coefficient of variation

3. **Documentation**:
   - Record all system settings and environment variables
   - Include hardware specifications in reports
   - Note any anomalies or interruptions

## Extending the Framework

The experiment framework can be extended to:

1. **Support Additional Algorithms**:
   - Add new algorithm implementations in `src/`
   - Update measurement programs to support the new algorithm
   - Modify experiment scripts to include the new algorithm

2. **Add New Measurement Types**:
   - Implement new measurement programs in `src/experiments/`
   - Create corresponding scripts in `experiments/scripts/`
   - Update the analysis script to handle the new data format 