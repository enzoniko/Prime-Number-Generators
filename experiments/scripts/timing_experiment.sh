#!/bin/bash

# Timing experiment script
# Measures execution time with high precision for crypto algorithms

set -e  # Exit on error

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
EXPERIMENTS_DIR="$REPO_ROOT/experiments"
CONFIG_DIR="$EXPERIMENTS_DIR/configs"
RESULTS_DIR="$EXPERIMENTS_DIR/results/timing"

# Default values
DEFAULT_CONFIG="$CONFIG_DIR/default_config.json"
DEFAULT_ITERATIONS=10
DEFAULT_OUTPUT_DIR="$RESULTS_DIR"

function print_help() {
    echo "Usage: $0 [OPTIONS]"
    echo "Run precise timing measurements for crypto algorithms."
    echo ""
    echo "Options:"
    echo "  -a, --algorithm ALGO     Algorithm to test (lcg, xoshiro, miller_rabin, baillie_psw, all)"
    echo "  -b, --bits BITS          Bit size to test (40, 56, 80, ..., 4096, or 'all')"
    echo "  -i, --iterations COUNT   Number of iterations (default: $DEFAULT_ITERATIONS)"
    echo "  -c, --config FILE        Config file to use (default: default_config.json)"
    echo "  -o, --output DIR         Output directory (default: $DEFAULT_OUTPUT_DIR)"
    echo "  -h, --help               Show this help message"
    exit 0
}

# Parse command line arguments
ALGORITHM="all"
BITS="all"
ITERATIONS=$DEFAULT_ITERATIONS
CONFIG_FILE=$DEFAULT_CONFIG
OUTPUT_DIR=$DEFAULT_OUTPUT_DIR

while (( "$#" )); do
    case "$1" in
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
                CONFIG_FILE=$2
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

# Ensure output directory exists
mkdir -p "$OUTPUT_DIR"

# Define bit sizes array
if [ "$BITS" == "all" ]; then
    BIT_SIZES=(40 56 80 128 168 224 256 512 1024 2048 4096)
else
    BIT_SIZES=($BITS)
fi

# Define the algorithms to test
if [ "$ALGORITHM" == "all" ]; then
    PRNG_ALGOS=("lcg" "xoshiro")
    PRIMALITY_ALGOS=("miller_rabin" "baillie_psw")
elif [ "$ALGORITHM" == "lcg" ] || [ "$ALGORITHM" == "xoshiro" ]; then
    PRNG_ALGOS=("$ALGORITHM")
    PRIMALITY_ALGOS=()
elif [ "$ALGORITHM" == "miller_rabin" ] || [ "$ALGORITHM" == "baillie_psw" ]; then
    PRNG_ALGOS=()
    PRIMALITY_ALGOS=("$ALGORITHM")
else
    echo "Error: Unknown algorithm '$ALGORITHM'" >&2
    exit 1
fi

# Get timestamp for this experiment run
TIMESTAMP=$(date +"%Y%m%d_%H%M%S")

# Run PRNG timing experiments
for algo in "${PRNG_ALGOS[@]}"; do
    echo "Running timing experiments for $algo PRNG..."
    output_file="$OUTPUT_DIR/${algo}_timing_${TIMESTAMP}.csv"
    
    # CSV header
    echo "algorithm,bits,iteration,time_ns" > "$output_file"
    
    for bits in "${BIT_SIZES[@]}"; do
        echo "  Testing $bits bits..."
        for ((i=1; i<=$ITERATIONS; i++)); do
            # Call the timing measurement program
            time_ns=$("$REPO_ROOT/experiments/bin/measure_prng_time" "$algo" "$bits")
            
            # Append result to CSV
            echo "$algo,$bits,$i,$time_ns" >> "$output_file"
        done
    done
    
    echo "Results saved to $output_file"
done

# Run primality testing timing experiments
for algo in "${PRIMALITY_ALGOS[@]}"; do
    echo "Running timing experiments for $algo primality test..."
    output_file="$OUTPUT_DIR/${algo}_timing_${TIMESTAMP}.csv"
    
    # CSV header
    echo "algorithm,bits,iteration,time_ns" > "$output_file"
    
    for bits in "${BIT_SIZES[@]}"; do
        echo "  Testing $bits bits..."
        for ((i=1; i<=$ITERATIONS; i++)); do
            # Call the timing measurement program
            time_ns=$("$REPO_ROOT/experiments/bin/measure_primality_time" "$algo" "$bits")
            
            # Append result to CSV
            echo "$algo,$bits,$i,$time_ns" >> "$output_file"
        done
    done
    
    echo "Results saved to $output_file"
done

echo "All timing experiments completed successfully!" 