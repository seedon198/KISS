#!/usr/bin/env bash
#
# Build script for KISS Fuzzer
# Supports debug, release, and clean builds
#

set -e

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$PROJECT_ROOT/build"
PICO_SDK_PATH="${PICO_SDK_PATH:-/opt/pico-sdk}"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Logging functions
log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

# Check prerequisites
check_prereqs() {
    log_info "Checking prerequisites..."
    
    # Check for Pico SDK
    if [ ! -d "$PICO_SDK_PATH" ]; then
        log_error "Pico SDK not found at: $PICO_SDK_PATH"
        log_error "Please set PICO_SDK_PATH environment variable or install SDK to /opt/pico-sdk"
        exit 1
    fi
    
    # Check for CMake
    if ! command -v cmake &> /dev/null; then
        log_error "CMake not found. Please install CMake."
        exit 1
    fi
    
    # Check for ARM GCC
    if ! command -v arm-none-eabi-gcc &> /dev/null; then
        log_error "ARM GCC not found. Please install arm-none-eabi-gcc toolchain."
        exit 1
    fi
    
    log_success "All prerequisites found"
}

# Clean build directory
clean_build() {
    log_info "Cleaning build directory..."
    if [ -d "$BUILD_DIR" ]; then
        rm -rf "$BUILD_DIR"
        log_success "Build directory cleaned"
    else
        log_info "Build directory doesn't exist"
    fi
}

# Configure build
configure_build() {
    local build_type="$1"
    
    log_info "Configuring $build_type build..."
    
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    
    cmake -DCMAKE_BUILD_TYPE="$build_type" \
          -DPICO_SDK_PATH="$PICO_SDK_PATH" \
          -DPICO_BOARD=pico_w \
          "$PROJECT_ROOT"
    
    if [ $? -eq 0 ]; then
        log_success "Configuration complete"
    else
        log_error "Configuration failed"
        exit 1
    fi
}

# Build project
build_project() {
    log_info "Building project..."
    
    cd "$BUILD_DIR"
    make -j$(nproc)
    
    if [ $? -eq 0 ]; then
        log_success "Build complete"
        
        # Show build artifacts
        log_info "Build artifacts:"
        ls -la *.uf2 *.elf *.map 2>/dev/null || log_warn "No build artifacts found"
        
        # Show size information
        if [ -f "kiss_fuzzer.elf" ]; then
            log_info "Binary size information:"
            arm-none-eabi-size kiss_fuzzer.elf
        fi
    else
        log_error "Build failed"
        exit 1
    fi
}

# Flash to device
flash_device() {
    log_info "Looking for Pico in BOOTSEL mode..."
    
    local uf2_file="$BUILD_DIR/kiss_fuzzer.uf2"
    
    if [ ! -f "$uf2_file" ]; then
        log_error "UF2 file not found: $uf2_file"
        log_error "Please build the project first"
        exit 1
    fi
    
    # Look for Pico mount point
    local pico_mount=""
    
    # macOS
    if [[ "$OSTYPE" == "darwin"* ]]; then
        pico_mount=$(mount | grep "RPI-RP2" | cut -d' ' -f3 | head -n1)
    # Linux
    elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
        pico_mount="/media/$USER/RPI-RP2"
        if [ ! -d "$pico_mount" ]; then
            pico_mount="/mnt/RPI-RP2"
        fi
    fi
    
    if [ -z "$pico_mount" ] || [ ! -d "$pico_mount" ]; then
        log_error "Pico not found in BOOTSEL mode"
        log_error "Please hold BOOTSEL button while connecting USB"
        exit 1
    fi
    
    log_info "Found Pico at: $pico_mount"
    log_info "Copying UF2 file..."
    
    cp "$uf2_file" "$pico_mount/"
    
    if [ $? -eq 0 ]; then
        log_success "Firmware flashed successfully!"
        log_info "Device should reboot automatically"
    else
        log_error "Failed to copy UF2 file"
        exit 1
    fi
}

# Show help
show_help() {
    echo "KISS Fuzzer Build Script"
    echo ""
    echo "Usage: $0 [COMMAND] [OPTIONS]"
    echo ""
    echo "Commands:"
    echo "  debug      Build debug version (default)"
    echo "  release    Build release version"
    echo "  clean      Clean build directory"
    echo "  flash      Flash firmware to device"
    echo "  help       Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0                 # Build debug version"
    echo "  $0 release         # Build release version"
    echo "  $0 clean debug     # Clean and build debug"
    echo "  $0 flash           # Flash existing firmware"
    echo ""
    echo "Environment Variables:"
    echo "  PICO_SDK_PATH      Path to Pico SDK (default: /opt/pico-sdk)"
}

# Main script logic
main() {
    log_info "KISS Fuzzer Build Script"
    log_info "========================"
    
    case "${1:-debug}" in
        "help"|"-h"|"--help")
            show_help
            exit 0
            ;;
        "clean")
            check_prereqs
            clean_build
            
            # If additional command provided after clean
            if [ -n "$2" ]; then
                configure_build "$2"
                build_project
            fi
            ;;
        "debug")
            check_prereqs
            configure_build "Debug"
            build_project
            ;;
        "release")
            check_prereqs
            configure_build "Release"
            build_project
            ;;
        "flash")
            flash_device
            ;;
        *)
            log_error "Unknown command: $1"
            show_help
            exit 1
            ;;
    esac
    
    log_success "Script completed successfully!"
}

# Run main function with all arguments
main "$@"
