#!/usr/bin/env python3

"""
Result analysis script for cryptographic algorithm experiments
"""

import os
import sys
import csv
import argparse
import statistics
import glob
import re
from datetime import datetime
import matplotlib.pyplot as plt
import numpy as np

def parse_args():
    """Parse command line arguments"""
    parser = argparse.ArgumentParser(description='Analyze experiment results')
    parser.add_argument('--type', choices=['timing', 'energy'], required=True,
                        help='Type of results to analyze')
    parser.add_argument('--dir', default=None,
                        help='Directory containing result files')
    parser.add_argument('--output', default='analysis_results',
                        help='Output directory for analysis results')
    parser.add_argument('--plot', action='store_true',
                        help='Generate plots')
    return parser.parse_args()

def analyze_timing_results(directory, output_dir):
    """Analyze timing experiment results"""
    if not directory:
        directory = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), 
                                 'results', 'timing')
    
    # Create output directory if it doesn't exist
    os.makedirs(output_dir, exist_ok=True)
    
    # Find all CSV files in the directory
    csv_files = glob.glob(os.path.join(directory, '*.csv'))
    
    if not csv_files:
        print(f"No CSV files found in {directory}")
        return
    
    print(f"Found {len(csv_files)} CSV files in {directory}")
    
    # Dictionary to store results by algorithm and bit size
    results = {}
    
    # Read all CSV files
    for csv_file in csv_files:
        filename = os.path.basename(csv_file)
        print(f"Processing {filename}...")
        
        with open(csv_file, 'r') as f:
            reader = csv.DictReader(f)
            for row in reader:
                algorithm = row['algorithm']
                bits = int(row['bits'])
                time = float(row['time_ns'])
                
                if algorithm not in results:
                    results[algorithm] = {}
                
                if bits not in results[algorithm]:
                    results[algorithm][bits] = []
                
                results[algorithm][bits].append(time)
    
    # Calculate statistics
    stats = {}
    for algorithm in results:
        stats[algorithm] = {}
        for bits in results[algorithm]:
            times = results[algorithm][bits]
            stats[algorithm][bits] = {
                'min': min(times),
                'max': max(times),
                'mean': statistics.mean(times),
                'median': statistics.median(times),
                'stdev': statistics.stdev(times) if len(times) > 1 else 0,
                'cv': statistics.stdev(times) / statistics.mean(times) if len(times) > 1 else 0,
                'samples': len(times)
            }
    
    # Write summary to CSV
    summary_file = os.path.join(output_dir, 'timing_summary.csv')
    with open(summary_file, 'w', newline='') as f:
        fieldnames = ['algorithm', 'bits', 'min', 'max', 'mean', 'median', 'stdev', 'cv', 'samples']
        writer = csv.DictWriter(f, fieldnames=fieldnames)
        writer.writeheader()
        
        for algorithm in sorted(stats.keys()):
            for bits in sorted(stats[algorithm].keys()):
                writer.writerow({
                    'algorithm': algorithm,
                    'bits': bits,
                    **{k: stats[algorithm][bits][k] for k in ['min', 'max', 'mean', 'median', 'stdev', 'cv', 'samples']}
                })
    
    print(f"Timing summary written to {summary_file}")
    
    return stats

def analyze_energy_results(directory, output_dir):
    """Analyze energy experiment results"""
    if not directory:
        directory = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), 
                                 'results', 'energy')
    
    # Create output directory if it doesn't exist
    os.makedirs(output_dir, exist_ok=True)
    
    # Find all log files in the directory
    log_files = glob.glob(os.path.join(directory, '*.log'))
    
    if not log_files:
        print(f"No log files found in {directory}")
        return
    
    print(f"Found {len(log_files)} log files in {directory}")
    
    # Dictionary to store results by algorithm and bit size
    results = {}
    
    # Regular expression to extract operation rates
    rate_regex = re.compile(r'STAT: (\d+)s - Rate:\s+(\d+) ops/sec')
    
    # Read all log files
    for log_file in log_files:
        filename = os.path.basename(log_file)
        print(f"Processing {filename}...")
        
        # Extract algorithm and bit size from filename
        match = re.match(r'([a-z_]+)_(\d+)bits_(\d{8}_\d{6})\.log', filename)
        if not match:
            print(f"  Skipping {filename}: does not match expected format")
            continue
        
        algorithm, bits_str, timestamp = match.groups()
        bits = int(bits_str)
        
        # Initialize dictionary entries
        if algorithm not in results:
            results[algorithm] = {}
        
        if bits not in results[algorithm]:
            results[algorithm][bits] = []
        
        # Extract operation rates from log file
        rates = []
        with open(log_file, 'r') as f:
            for line in f:
                match = rate_regex.search(line)
                if match:
                    seconds, rate = match.groups()
                    rates.append(int(rate))
        
        if rates:
            # Add this experiment's average rate to results
            avg_rate = sum(rates) / len(rates)
            results[algorithm][bits].append({
                'timestamp': timestamp,
                'avg_rate': avg_rate,
                'min_rate': min(rates),
                'max_rate': max(rates),
                'rates': rates
            })
    
    # Calculate statistics
    stats = {}
    for algorithm in results:
        stats[algorithm] = {}
        for bits in results[algorithm]:
            experiments = results[algorithm][bits]
            if experiments:
                avg_rates = [exp['avg_rate'] for exp in experiments]
                stats[algorithm][bits] = {
                    'min_rate': min(avg_rates),
                    'max_rate': max(avg_rates),
                    'mean_rate': statistics.mean(avg_rates) if avg_rates else 0,
                    'experiments': len(experiments)
                }
    
    # Write summary to CSV
    summary_file = os.path.join(output_dir, 'energy_summary.csv')
    with open(summary_file, 'w', newline='') as f:
        fieldnames = ['algorithm', 'bits', 'min_rate', 'max_rate', 'mean_rate', 'experiments']
        writer = csv.DictWriter(f, fieldnames=fieldnames)
        writer.writeheader()
        
        for algorithm in sorted(stats.keys()):
            for bits in sorted(stats[algorithm].keys()):
                writer.writerow({
                    'algorithm': algorithm,
                    'bits': bits,
                    **stats[algorithm][bits]
                })
    
    print(f"Energy summary written to {summary_file}")
    
    return stats

def generate_timing_plots(stats, output_dir):
    """Generate plots for timing results"""
    if not stats:
        return
    
    plt.figure(figsize=(12, 8))
    
    # Plot mean time vs bit size for each algorithm
    for algorithm in stats:
        x = sorted(stats[algorithm].keys())
        y = [stats[algorithm][bits]['mean'] for bits in x]
        
        plt.plot(x, y, marker='o', label=algorithm)
    
    plt.title('Mean Execution Time by Bit Size')
    plt.xlabel('Bit Size')
    plt.ylabel('Time (ns)')
    plt.xscale('log', base=2)
    plt.yscale('log', base=10)
    plt.grid(True, which='both', linestyle='--', alpha=0.5)
    plt.legend()
    
    plot_file = os.path.join(output_dir, 'timing_plot.png')
    plt.savefig(plot_file)
    print(f"Timing plot saved to {plot_file}")

def generate_energy_plots(stats, output_dir):
    """Generate plots for energy results"""
    if not stats:
        return
    
    plt.figure(figsize=(12, 8))
    
    # Plot mean operation rate vs bit size for each algorithm
    for algorithm in stats:
        x = sorted(stats[algorithm].keys())
        y = [stats[algorithm][bits]['mean_rate'] for bits in x]
        
        plt.plot(x, y, marker='o', label=algorithm)
    
    plt.title('Mean Operation Rate by Bit Size')
    plt.xlabel('Bit Size')
    plt.ylabel('Operations per Second')
    plt.xscale('log', base=2)
    plt.yscale('log', base=10)
    plt.grid(True, which='both', linestyle='--', alpha=0.5)
    plt.legend()
    
    plot_file = os.path.join(output_dir, 'energy_plot.png')
    plt.savefig(plot_file)
    print(f"Energy plot saved to {plot_file}")

def main():
    args = parse_args()
    
    # Set default output directory if not specified
    if not args.output:
        timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
        args.output = f'analysis_{args.type}_{timestamp}'
    
    if args.type == 'timing':
        stats = analyze_timing_results(args.dir, args.output)
        if args.plot:
            generate_timing_plots(stats, args.output)
    else:  # energy
        stats = analyze_energy_results(args.dir, args.output)
        if args.plot:
            generate_energy_plots(stats, args.output)

if __name__ == '__main__':
    main() 