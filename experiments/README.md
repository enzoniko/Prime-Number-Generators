# RISC-V Performance and Energy Experiments

This directory contains tools and scripts for conducting performance and energy consumption measurements of cryptographic algorithms on RISC-V platforms.

## Documentation and Diagrams

Detailed documentation and architectural diagrams for these experiments are available in:
- [Experiments Documentation](../docs/experiments.md)
- [Experiment Architecture Diagram](../docs/diagrams/experiment_architecture.puml)
- [Timing Workflow Diagram](../docs/diagrams/timing_workflow.puml)
- [Energy Workflow Diagram](../docs/diagrams/energy_workflow.puml)
- [Experiment Classes Diagram](../docs/diagrams/experiment_classes.puml)

## Setup

Before running experiments, ensure you have compiled the necessary tools:

```bash
# From the repository root
make riscv-setup
```

This will:
1. Build all the necessary experiment programs
2. Make the script files executable
3. Create the required directories for results

## Directory Structure

- `bin/` - Compiled experiment binaries
- `configs/` - JSON configuration files for experiments
- `results/` - Where experimental results are stored
  - `timing/` - Timing measurement results
  - `energy/` - Energy consumption experiment logs
- `scripts/` - Shell scripts to run experiments

## Running Experiments

### Timing Experiments

To measure the precise execution time of different algorithms:

```bash
# Run a specific algorithm at a specific bit size
./scripts/run_experiments.sh --timing --algorithm lcg --bits 1024 --iterations 100

# Run all algorithms with all bit sizes with default iterations
./scripts/run_experiments.sh --timing
```

The timing results are stored in CSV files in the `results/timing/` directory.

### Energy Consumption Experiments

To measure energy consumption, you'll need a USB power meter connected to your RISC-V board. The script will guide you through the process:

```bash
# Run a specific algorithm for 10 minutes
./scripts/run_experiments.sh --energy --algorithm miller_rabin --bits 1024 --duration 600

# Run all algorithms for the default duration (10 minutes each)
./scripts/run_experiments.sh --energy
```

The script will:
1. Prompt you to start recording measurements with your USB power meter
2. Run the algorithm continuously for the specified duration
3. Prompt you to stop recording when complete
4. Log operation statistics to `results/energy/`

## Collecting Results

Results are automatically saved to CSV files (for timing) and log files (for energy measurements). You can collect these files for further analysis.

### Timing Results Format

The timing results CSV files contain the following columns:
- `algorithm` - Name of the algorithm
- `bits` - Bit size of the operation
- `iteration` - Iteration number
- `time_ns` - Time taken in nanoseconds (or CPU cycles, depending on the platform)

### Energy Results Format

The energy log files contain:
- Experiment metadata (algorithm, bit size, duration)
- Operation statistics (operations per second)
- Progress reports

## Analyzing Results

You can analyze the collected results using the provided Python script:

```bash
# Analyze timing results and generate plots
./scripts/analyze_results.py --type timing --plot

# Analyze energy results and generate plots
./scripts/analyze_results.py --type energy --plot
```

## Tips for Accurate Measurements

1. **CPU Frequency**: For accurate measurements, disable dynamic CPU frequency scaling:
   ```bash
   ./scripts/setup_riscv.sh --performance
   ```

2. **Process Isolation**: Run experiments with minimal other processes:
   ```bash
   ./scripts/setup_riscv.sh --stop-services
   ```

3. **Multiple Runs**: Perform multiple experiment runs to ensure statistical significance.

4. **Temperature**: Monitor system temperature, as thermal throttling can affect results.

5. **Power Source**: For energy measurements, ensure the RISC-V board is powered directly through the USB power meter. 