#!/usr/bin/env bash
# KISS Fuzzer Universal Development Environment Setup
# 
# This script automatically detects your platform and runs the appropriate
# setup script for KISS Fuzzer development environment.

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Logging functions
log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Detect platform and architecture
detect_platform() {
    local OS=$(uname -s)
    local ARCH=$(uname -m)
    
    case "$OS" in
        Darwin*)
            if [[ "$ARCH" == "arm64" ]]; then
                echo "macos-arm64"
            else
                echo "macos-intel"
            fi
            ;;
        Linux*)
            if command -v apt-get >/dev/null 2>&1; then
                echo "linux-ubuntu"
            elif command -v dnf >/dev/null 2>&1; then
                echo "linux-fedora"
            elif command -v yum >/dev/null 2>&1; then
                echo "linux-centos"
            else
                echo "linux-unknown"
            fi
            ;;
        CYGWIN*|MINGW*|MSYS*)
            echo "windows"
            ;;
        *)
            echo "unknown"
            ;;
    esac
}

# Show platform information
show_platform_info() {
    local PLATFORM=$(detect_platform)
    local OS=$(uname -s)
    local ARCH=$(uname -m)
    
    cat << EOF
${BLUE}
╔═══════════════════════════════════════════════════════════════╗
║                KISS Fuzzer Universal Setup                    ║
║              Platform Detection & Setup                       ║
╚═══════════════════════════════════════════════════════════════╝
${NC}

${BLUE}Platform Information:${NC}
• Operating System: $OS
• Architecture: $ARCH  
• Detected Platform: $PLATFORM

EOF
}

# Main execution
main() {
    show_platform_info
    
    local PLATFORM=$(detect_platform)
    local SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
    
    case "$PLATFORM" in
        macos-arm64)
            log_info "Running macOS Apple Silicon setup..."
            if [[ -f "$SCRIPT_DIR/setup-macos-arm64.sh" ]]; then
                exec "$SCRIPT_DIR/setup-macos-arm64.sh" "$@"
            else
                log_error "Setup script not found: setup-macos-arm64.sh"
                exit 1
            fi
            ;;
            
        macos-intel)
            log_warning "macOS Intel detected - using ARM64 script with Rosetta compatibility"
            if [[ -f "$SCRIPT_DIR/setup-macos-arm64.sh" ]]; then
                exec "$SCRIPT_DIR/setup-macos-arm64.sh" "$@"
            else
                log_error "Setup script not found: setup-macos-arm64.sh"
                exit 1
            fi
            ;;
            
        linux-ubuntu)
            log_info "Running Ubuntu/Debian setup..."
            if [[ -f "$SCRIPT_DIR/setup-linux-ubuntu.sh" ]]; then
                exec "$SCRIPT_DIR/setup-linux-ubuntu.sh" "$@"
            else
                log_error "Setup script not found: setup-linux-ubuntu.sh"
                exit 1
            fi
            ;;
            
        linux-fedora|linux-centos)
            log_warning "Fedora/CentOS detected - using Ubuntu script as base"
            log_warning "You may need to manually adjust package manager commands"
            if [[ -f "$SCRIPT_DIR/setup-linux-ubuntu.sh" ]]; then
                exec "$SCRIPT_DIR/setup-linux-ubuntu.sh" "$@"
            else
                log_error "Setup script not found: setup-linux-ubuntu.sh"
                exit 1
            fi
            ;;
            
        windows)
            log_info "Windows detected - please use PowerShell script"
            log_info "Run in PowerShell as Administrator:"
            log_info "  Set-ExecutionPolicy Bypass -Scope Process -Force"
            log_info "  .\\scripts\\setup-windows.ps1"
            exit 1
            ;;
            
        *)
            log_error "Unsupported platform: $PLATFORM"
            log_error "Please use one of the platform-specific setup scripts:"
            log_error "  • macOS: scripts/setup-macos-arm64.sh"
            log_error "  • Linux: scripts/setup-linux-ubuntu.sh" 
            log_error "  • Windows: scripts/setup-windows.ps1"
            exit 1
            ;;
    esac
}

# Handle help flag
if [[ "$1" == "-h" || "$1" == "--help" ]]; then
    cat << EOF
KISS Fuzzer Universal Setup Script

This script automatically detects your platform and runs the appropriate
setup script for KISS Fuzzer development environment.

Usage:
  ./scripts/setup.sh [options]

Platform-specific scripts:
  • macOS Apple Silicon:  ./scripts/setup-macos-arm64.sh
  • Linux Ubuntu/Debian:  ./scripts/setup-linux-ubuntu.sh
  • Windows 10/11:        ./scripts/setup-windows.ps1

Options:
  -h, --help     Show this help message

For Windows users:
  Run in PowerShell as Administrator:
    Set-ExecutionPolicy Bypass -Scope Process -Force
    .\\scripts\\setup-windows.ps1

For more information:
  • Documentation: https://kiss-fuzz.readthedocs.io/
  • Development Guide: docs/development.md
  
EOF
    exit 0
fi

# Run main function
main "$@"
