#!/bin/bash

# Energy experiment script
# Runs algorithms continuously for a specified time period for energy measurements

set -e  # Exit on error

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
EXPERIMENTS_DIR="$REPO_ROOT/experiments"
CONFIG_DIR="$EXPERIMENTS_DIR/configs"
RESULTS_DIR="$EXPERIMENTS_DIR/results/energy"

# Default values
DEFAULT_CONFIG="$CONFIG_DIR/default_config.json"
DEFAULT_DURATION=600  # 10 minutes
DEFAULT_OUTPUT_DIR="$RESULTS_DIR"

function print_help() {
    echo "Usage: $0 [OPTIONS]"
    echo "Run algorithms continuously for energy consumption measurement."
    echo ""
    echo "Options:"
    echo "  -a, --algorithm ALGO     Algorithm to test (lcg, xoshiro, miller_rabin, baillie_psw, all)"
    echo "  -b, --bits BITS          Bit size to test (40, 56, 80, ..., 4096, or 'all')"
    echo "  -d, --duration SECONDS   Duration in seconds (default: $DEFAULT_DURATION)"
    echo "  -c, --config FILE        Config file to use (default: default_config.json)"
    echo "  -o, --output DIR         Output directory (default: $DEFAULT_OUTPUT_DIR)"
    echo "  -h, --help               Show this help message"
    exit 0
}

# Parse command line arguments
ALGORITHM="all"
BITS="all"
DURATION=$DEFAULT_DURATION
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
        -d|--duration)
            if [ -n "$2" ] && [ ${2:0:1} != "-" ]; then
                DURATION=$2
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
    # For energy tests, using a single representative bit size is usually sufficient
    BIT_SIZES=(1024)
else
    BIT_SIZES=($BITS)
fi

# Define the algorithms to test
if [ "$ALGORITHM" == "all" ]; then
    ALGOS=("lcg" "xoshiro" "miller_rabin" "baillie_psw")
elif [ "$ALGORITHM" == "lcg" ] || [ "$ALGORITHM" == "xoshiro" ] || [ "$ALGORITHM" == "miller_rabin" ] || [ "$ALGORITHM" == "baillie_psw" ]; then
    ALGOS=("$ALGORITHM")
else
    echo "Error: Unknown algorithm '$ALGORITHM'" >&2
    exit 1
fi

# Get timestamp for this experiment run
TIMESTAMP=$(date +"%Y%m%d_%H%M%S")

# For each algorithm and bit size
for algo in "${ALGOS[@]}"; do
    for bits in "${BIT_SIZES[@]}"; do
        echo "Running energy experiment for $algo with $bits bits..."
        log_file="$OUTPUT_DIR/${algo}_${bits}bits_${TIMESTAMP}.log"
        
        echo "Starting experiment at $(date)" > "$log_file"
        echo "Algorithm: $algo" >> "$log_file"
        echo "Bit size: $bits" >> "$log_file"
        echo "Duration: $DURATION seconds" >> "$log_file"
        echo "------------------------------" >> "$log_file"
        
        # Display instructions for the user
        echo ""
        echo "========================================================================================="
        echo "ENERGY MEASUREMENT FOR: $algo with $bits bits"
        echo "========================================================================================="
        echo "Start capturing energy consumption with your USB power meter NOW."
        echo "This test will run for $DURATION seconds."
        echo "Press Ctrl+C to stop the test early (not recommended)."
        echo ""
        echo "Test will start in 5 seconds..."
        sleep 5
        
        # Run the continuous operation program
        start_time=$(date +%s)
        end_time=$((start_time + DURATION))
        
        "$REPO_ROOT/experiments/bin/continuous_operation" "$algo" "$bits" "$DURATION" | tee -a "$log_file"
        
        echo "" >> "$log_file"
        echo "Experiment completed at $(date)" >> "$log_file"
        
        echo ""
        echo "========================================================================================="
        echo "ENERGY MEASUREMENT COMPLETE"
        echo "========================================================================================="
        echo "Stop capturing energy consumption with your USB power meter NOW."
        echo "Results logged to: $log_file"
        echo ""
        
        # If not the last experiment, wait for user to prepare for the next one
        if [ "$algo" != "${ALGOS[-1]}" ] || [ "$bits" != "${BIT_SIZES[-1]}" ]; then
            echo "Press Enter when ready to continue to the next experiment..."
            read
        fi
    done
done

echo "All energy experiments completed successfully!" 