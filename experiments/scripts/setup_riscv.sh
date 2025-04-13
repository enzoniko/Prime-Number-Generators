#!/bin/bash

# RISC-V Environment Setup Script
# Prepares the system for accurate performance and energy measurements

set -e  # Exit on error

function print_help() {
    echo "RISC-V Environment Setup for Performance and Energy Measurements"
    echo ""
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  --performance       Set CPU governor to performance mode"
    echo "  --powersave         Set CPU governor to powersave mode"
    echo "  --stop-services     Stop non-essential services"
    echo "  --start-services    Restart previously stopped services"
    echo "  --disable-wifi      Disable WiFi (reduces power consumption)"
    echo "  --enable-wifi       Enable WiFi"
    echo "  --check             Check current system settings"
    echo "  --help              Show this help message"
    exit 0
}

# Check if cpupower is installed
check_cpupower() {
    if ! command -v cpupower &> /dev/null; then
        echo "cpupower is not installed. Please install the linux-tools-common package:"
        echo "  sudo apt install linux-tools-common linux-tools-generic"
        return 1
    fi
    return 0
}

# Set CPU governor
set_governor() {
    if ! check_cpupower; then
        echo "Cannot set CPU governor without cpupower."
        return 1
    fi
    
    echo "Setting CPU governor to $1..."
    sudo cpupower frequency-set --governor $1
    echo "Current CPU information:"
    sudo cpupower frequency-info | grep "current CPU"
}

# Stop non-essential services
stop_services() {
    echo "Stopping non-essential services..."
    
    # List of services to stop
    SERVICES=(
        "bluetooth"
        "cups"
        "avahi-daemon"
        "ModemManager"
        "wpa_supplicant"
        "NetworkManager"
        "apache2"
        "mysql"
        "postgresql"
    )
    
    for service in "${SERVICES[@]}"; do
        if systemctl is-active --quiet $service; then
            echo "  Stopping $service..."
            sudo systemctl stop $service
            echo "$service" >> /tmp/stopped_services.txt
        else
            echo "  $service is already stopped or not installed."
        fi
    done
    
    echo "Non-essential services stopped."
}

# Restart previously stopped services
start_services() {
    echo "Restarting previously stopped services..."
    
    if [ ! -f /tmp/stopped_services.txt ]; then
        echo "No record of stopped services found."
        return
    fi
    
    while IFS= read -r service; do
        echo "  Starting $service..."
        sudo systemctl start $service
    done < /tmp/stopped_services.txt
    
    rm /tmp/stopped_services.txt
    echo "Services restarted."
}

# Disable WiFi
disable_wifi() {
    echo "Disabling WiFi..."
    
    # Check if rfkill is available
    if ! command -v rfkill &> /dev/null; then
        echo "rfkill is not installed. Please install it:"
        echo "  sudo apt install rfkill"
        return 1
    fi
    
    # Disable all wireless
    sudo rfkill block wifi
    echo "WiFi disabled."
}

# Enable WiFi
enable_wifi() {
    echo "Enabling WiFi..."
    
    # Check if rfkill is available
    if ! command -v rfkill &> /dev/null; then
        echo "rfkill is not installed. Please install it:"
        echo "  sudo apt install rfkill"
        return 1
    fi
    
    # Enable all wireless
    sudo rfkill unblock wifi
    echo "WiFi enabled."
}

# Check current system settings
check_settings() {
    echo "Checking current system settings..."
    echo ""
    
    echo "CPU Information:"
    if command -v cpupower &> /dev/null; then
        sudo cpupower frequency-info | grep -E "hardware limits|current CPU frequency|current policy|driver"
    else
        echo "  cpupower not installed."
        cat /proc/cpuinfo | grep -E "model name|cpu MHz" | head -2
    fi
    echo ""
    
    echo "Running Services:"
    systemctl list-units --type=service --state=running | head -10
    echo "  ... (showing only 10 services)"
    echo ""
    
    echo "Wireless Status:"
    if command -v rfkill &> /dev/null; then
        rfkill list
    else
        echo "  rfkill not installed."
    fi
    echo ""
    
    echo "Memory Usage:"
    free -h
    echo ""
    
    echo "System Load:"
    uptime
    echo ""
    
    echo "Running Processes (top 5 by CPU):"
    ps aux --sort=-%cpu | head -6
}

# Parse command-line arguments
if [ $# -eq 0 ]; then
    print_help
fi

while (( "$#" )); do
    case "$1" in
        --performance)
            set_governor performance
            shift
            ;;
        --powersave)
            set_governor powersave
            shift
            ;;
        --stop-services)
            stop_services
            shift
            ;;
        --start-services)
            start_services
            shift
            ;;
        --disable-wifi)
            disable_wifi
            shift
            ;;
        --enable-wifi)
            enable_wifi
            shift
            ;;
        --check)
            check_settings
            shift
            ;;
        --help)
            print_help
            ;;
        *)
            echo "Error: Unknown option: $1"
            print_help
            ;;
    esac
done

echo "Setup complete." 