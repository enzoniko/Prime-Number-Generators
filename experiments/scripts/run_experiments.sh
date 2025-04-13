#!/bin/bash

# Main experiment runner for RISC-V board measurements
# This script coordinates all experiments for timing and energy measurements

set -e  # Exit on error

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
EXPERIMENTS_DIR="$REPO_ROOT/experiments"
CONFIG_DIR="$EXPERIMENTS_DIR/configs"
RESULTS_DIR="$EXPERIMENTS_DIR/results"

# Default values
DEFAULT_CONFIG="default_config.json"
DEFAULT_DURATION=600  # 10 minutes in seconds
DEFAULT_ITERATIONS=10
DEFAULT_OUTPUT_DIR="$RESULTS_DIR/timing"

function print_help() {
    echo "Usage: $0 [OPTIONS]"
    echo "Run timing or energy consumption experiments on crypto algorithms."
    echo ""
    echo "Options:"
    echo "  -t, --timing              Run timing experiments (default)"
    echo "  -e, --energy              Run energy consumption experiments"
    echo "  -a, --algorithm ALGO      Algorithm to test (lcg, xoshiro, miller_rabin, baillie_psw, all)"
    echo "  -b, --bits BITS           Bit size to test (40, 56, 80, ..., 4096, or 'all')"
    echo "  -d, --duration SECONDS    Duration for energy tests in seconds (default: $DEFAULT_DURATION)"
    echo "  -i, --iterations COUNT    Number of iterations for timing tests (default: $DEFAULT_ITERATIONS)"
    echo "  -c, --config FILE         Config file to use (default: $DEFAULT_CONFIG)"
    echo "  -o, --output DIR          Output directory (default based on experiment type)"
    echo "  -h, --help                Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0 --timing --algorithm lcg --bits 1024 --iterations 100"
    echo "  $0 --energy --algorithm all --duration 1800"
    exit 0
}

# Parse command line arguments
EXPERIMENT_TYPE="timing"
ALGORITHM="all"
BITS="all"
DURATION=$DEFAULT_DURATION
ITERATIONS=$DEFAULT_ITERATIONS
CONFIG_FILE="$CONFIG_DIR/$DEFAULT_CONFIG"
OUTPUT_DIR=""

while (( "$#" )); do
    case "$1" in
        -t|--timing)
            EXPERIMENT_TYPE="timing"
            shift
            ;;
        -e|--energy)
            EXPERIMENT_TYPE="energy"
            shift
            ;;
        -a|--algorithm)
            if [ -n "$2" ] && [ ${2:0:1} != "-" ]; then
                ALGORITHM=$2
                shift 2
            else
                echo "Error: Argument for $1 is missing" >&2
                exit 1
            fi
            ;;
        -b|--bits)
            if [ -n "$2" ] && [ ${2:0:1} != "-" ]; then
                BITS=$2
                shift 2
            else
                echo "Error: Argument for $1 is missing" >&2
                exit 1
            fi
            ;;
        -d|--duration)
            if [ -n "$2" ] && [ ${2:0:1} != "-" ]; then
                DURATION=$2
                shift 2
            else
                echo "Error: Argument for $1 is missing" >&2
                exit 1
            fi
            ;;
        -i|--iterations)
            if [ -n "$2" ] && [ ${2:0:1} != "-" ]; then
                ITERATIONS=$2
                shift 2
            else
                echo "Error: Argument for $1 is missing" >&2
                exit 1
            fi
            ;;
        -c|--config)
            if [ -n "$2" ] && [ ${2:0:1} != "-" ]; then
                CONFIG_FILE="$CONFIG_DIR/$2"
                shift 2
            else
                echo "Error: Argument for $1 is missing" >&2
                exit 1
            fi
            ;;
        -o|--output)
            if [ -n "$2" ] && [ ${2:0:1} != "-" ]; then
                OUTPUT_DIR=$2
                shift 2
            else
                echo "Error: Argument for $1 is missing" >&2
                exit 1
            fi
            ;;
        -h|--help)
            print_help
            ;;
        *)
            echo "Error: Unsupported flag $1" >&2
            print_help
            ;;
    esac
done

# Set default output directory if not specified
if [ -z "$OUTPUT_DIR" ]; then
    if [ "$EXPERIMENT_TYPE" == "timing" ]; then
        OUTPUT_DIR="$RESULTS_DIR/timing"
    else
        OUTPUT_DIR="$RESULTS_DIR/energy"
    fi
fi

# Ensure output directory exists
mkdir -p "$OUTPUT_DIR"

# Run the appropriate experiment
if [ "$EXPERIMENT_TYPE" == "timing" ]; then
    echo "Running timing experiments:"
    echo "- Algorithm: $ALGORITHM"
    echo "- Bits: $BITS"
    echo "- Iterations: $ITERATIONS"
    echo "- Output: $OUTPUT_DIR"
    
    # Call the timing experiment script
    "$SCRIPT_DIR/timing_experiment.sh" \
        --algorithm "$ALGORITHM" \
        --bits "$BITS" \
        --iterations "$ITERATIONS" \
        --config "$CONFIG_FILE" \
        --output "$OUTPUT_DIR"
else
    echo "Running energy experiments:"
    echo "- Algorithm: $ALGORITHM"
    echo "- Bits: $BITS"
    echo "- Duration: $DURATION seconds"
    echo "- Output: $OUTPUT_DIR"
    
    # Call the energy experiment script
    "$SCRIPT_DIR/energy_experiment.sh" \
        --algorithm "$ALGORITHM" \
        --bits "$BITS" \
        --duration "$DURATION" \
        --config "$CONFIG_FILE" \
        --output "$OUTPUT_DIR"
fi

echo "Experiments completed successfully!" 