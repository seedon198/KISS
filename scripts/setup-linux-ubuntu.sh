#!/bin/bash
# KISS Fuzzer Development Environment Setup
# Linux Ubuntu/Debian-based Systems
# 
# This script installs all required dependencies for KISS Fuzzer development
# on Ubuntu, Debian, and compatible Linux distributions.

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

# Check if running on Linux
check_system() {
    if [[ "$(uname -s)" != "Linux" ]]; then
        log_error "This script is designed for Linux systems"
        log_error "Detected system: $(uname -s)"
        exit 1
    fi
    
    # Check if it's Ubuntu/Debian-based
    if [[ -f /etc/os-release ]]; then
        source /etc/os-release
        log_success "Detected: $NAME $VERSION"
        
        if [[ "$ID" != "ubuntu" && "$ID_LIKE" != *"ubuntu"* && "$ID" != "debian" && "$ID_LIKE" != *"debian"* ]]; then
            log_warning "This script is optimized for Ubuntu/Debian systems"
            log_warning "It may work on other distributions but is not tested"
        fi
    else
        log_warning "Could not detect Linux distribution"
    fi
    
    # Check architecture
    ARCH=$(uname -m)
    log_info "Architecture: $ARCH"
}

# Update package manager
update_system() {
    log_info "Updating package manager..."
    sudo apt-get update
    sudo apt-get upgrade -y
    log_success "System updated"
}

# Install essential development tools
install_essential_tools() {
    log_info "Installing essential development tools..."
    
    sudo apt-get install -y \
        build-essential \
        cmake \
        ninja-build \
        pkg-config \
        git \
        curl \
        wget \
        unzip \
        software-properties-common \
        apt-transport-https \
        ca-certificates \
        gnupg \
        lsb-release
    
    log_success "Essential tools installed"
}

# Install ARM cross-compilation toolchain
install_arm_toolchain() {
    log_info "Installing ARM GCC toolchain..."
    
    # Install from package manager
    sudo apt-get install -y gcc-arm-none-eabi
    
    # Verify installation
    if command -v arm-none-eabi-gcc >/dev/null 2>&1; then
        ARM_GCC_VERSION=$(arm-none-eabi-gcc --version | head -n1)
        log_success "ARM GCC installed: $ARM_GCC_VERSION"
    else
        log_error "ARM GCC installation failed"
        exit 1
    fi
}

# Install LLVM tools
install_llvm_tools() {
    log_info "Installing LLVM tools..."
    
    # Get Ubuntu/Debian version for LLVM repository
    UBUNTU_CODENAME=$(lsb_release -cs)
    
    # Add LLVM repository for latest tools
    if ! grep -q "llvm-toolchain" /etc/apt/sources.list.d/llvm.list 2>/dev/null; then
        wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add -
        echo "deb http://apt.llvm.org/$UBUNTU_CODENAME/ llvm-toolchain-$UBUNTU_CODENAME main" | sudo tee /etc/apt/sources.list.d/llvm.list
        sudo apt-get update
    fi
    
    # Install LLVM tools
    sudo apt-get install -y \
        clang-format \
        clang-tidy \
        clang \
        lldb
    
    log_success "LLVM tools installed"
}

# Install static analysis tools
install_analysis_tools() {
    log_info "Installing static analysis tools..."
    
    sudo apt-get install -y \
        cppcheck \
        valgrind \
        gdb \
        doxygen \
        graphviz
    
    log_success "Static analysis tools installed"
}

# Install Python and development tools
install_python_tools() {
    log_info "Installing Python development tools..."
    
    # Install Python and pip
    sudo apt-get install -y \
        python3 \
        python3-pip \
        python3-venv \
        python3-dev
    
    # Upgrade pip
    python3 -m pip install --user --upgrade pip
    
    # Install Python quality tools
    python3 -m pip install --user \
        black \
        isort \
        flake8 \
        mypy \
        pre-commit \
        sphinx \
        sphinx-rtd-theme \
        myst-parser \
        sphinx-copybutton \
        sphinxcontrib-mermaid \
        breathe
    
    # Add user pip binaries to PATH
    PIP_USER_BASE=$(python3 -m site --user-base)
    USER_BIN_PATH="$PIP_USER_BASE/bin"
    
    # Add to shell profiles
    for PROFILE in ~/.bashrc ~/.zshrc ~/.profile; do
        if [[ -f "$PROFILE" ]]; then
            if ! grep -q "$USER_BIN_PATH" "$PROFILE"; then
                echo "export PATH=\"$USER_BIN_PATH:\$PATH\"" >> "$PROFILE"
                log_info "Added user pip path to $PROFILE"
            fi
        fi
    done
    
    # Export for current session
    export PATH="$USER_BIN_PATH:$PATH"
    
    log_success "Python tools installed"
}

# Install Git LFS
install_git_lfs() {
    log_info "Installing Git LFS..."
    
    # Install from package manager or download directly
    if apt-cache search git-lfs | grep -q git-lfs; then
        sudo apt-get install -y git-lfs
    else
        # Download and install manually
        curl -s https://packagecloud.io/install/repositories/github/git-lfs/script.deb.sh | sudo bash
        sudo apt-get install -y git-lfs
    fi
    
    git lfs install
    log_success "Git LFS installed"
}

# Install USB and serial tools
install_usb_tools() {
    log_info "Installing USB and serial communication tools..."
    
    sudo apt-get install -y \
        minicom \
        picocom \
        screen \
        usbutils \
        libusb-1.0-0-dev \
        libudev-dev
    
    # Add user to dialout group for serial access
    sudo usermod -a -G dialout $USER
    
    log_success "USB and serial tools installed"
    log_warning "You may need to log out and back in for group changes to take effect"
}

# Install Pico SDK
install_pico_sdk() {
    log_info "Installing Raspberry Pi Pico SDK..."
    
    PICO_SDK_DIR="$HOME/pico-sdk"
    
    if [[ -d "$PICO_SDK_DIR" ]]; then
        log_info "Pico SDK already exists, updating..."
        cd "$PICO_SDK_DIR"
        git pull
        git submodule update --init --recursive
    else
        log_info "Cloning Pico SDK..."
        git clone https://github.com/raspberrypi/pico-sdk.git "$PICO_SDK_DIR"
        cd "$PICO_SDK_DIR"
        git submodule update --init --recursive
    fi
    
    # Set environment variable
    PICO_SDK_PATH="$PICO_SDK_DIR"
    
    # Add to shell profiles
    for PROFILE in ~/.bashrc ~/.zshrc ~/.profile; do
        if [[ -f "$PROFILE" ]]; then
            if ! grep -q "PICO_SDK_PATH" "$PROFILE"; then
                echo "export PICO_SDK_PATH=\"$PICO_SDK_PATH\"" >> "$PROFILE"
                log_info "Added PICO_SDK_PATH to $PROFILE"
            fi
        fi
    done
    
    # Export for current session
    export PICO_SDK_PATH="$PICO_SDK_PATH"
    
    log_success "Pico SDK installed at $PICO_SDK_PATH"
}

# Install FreeRTOS kernel
install_freertos() {
    log_info "Installing FreeRTOS kernel..."
    
    FREERTOS_DIR="$HOME/FreeRTOS-Kernel"
    
    if [[ -d "$FREERTOS_DIR" ]]; then
        log_info "FreeRTOS kernel already exists, updating..."
        cd "$FREERTOS_DIR"
        git pull
    else
        log_info "Cloning FreeRTOS kernel..."
        git clone https://github.com/FreeRTOS/FreeRTOS-Kernel.git "$FREERTOS_DIR"
    fi
    
    log_success "FreeRTOS kernel installed"
}

# Install optional development tools
install_optional_tools() {
    log_info "Installing optional development tools..."
    
    # IDE and editors
    if command -v snap >/dev/null 2>&1; then
        log_info "Installing VS Code via snap..."
        sudo snap install code --classic
    else
        log_info "Installing VS Code via apt..."
        wget -qO- https://packages.microsoft.com/keys/microsoft.asc | gpg --dearmor > packages.microsoft.gpg
        sudo install -o root -g root -m 644 packages.microsoft.gpg /etc/apt/trusted.gpg.d/
        echo "deb [arch=amd64,arm64,armhf signed-by=/etc/apt/trusted.gpg.d/packages.microsoft.gpg] https://packages.microsoft.com/repos/code stable main" | sudo tee /etc/apt/sources.list.d/vscode.list
        sudo apt-get update
        sudo apt-get install -y code
    fi
    
    # OpenOCD for debugging
    sudo apt-get install -y openocd
    
    log_success "Optional tools installed"
}

# Setup udev rules for development boards
setup_udev_rules() {
    log_info "Setting up udev rules for development boards..."
    
    # Create udev rules for Raspberry Pi Pico
    sudo tee /etc/udev/rules.d/99-pico.rules > /dev/null << 'EOF'
# Raspberry Pi Pico
SUBSYSTEM=="usb", ATTRS{idVendor}=="2e8a", ATTRS{idProduct}=="0003", MODE="0666"
SUBSYSTEM=="usb", ATTRS{idVendor}=="2e8a", ATTRS{idProduct}=="0004", MODE="0666"
SUBSYSTEM=="usb", ATTRS{idVendor}=="2e8a", ATTRS{idProduct}=="0005", MODE="0666"
SUBSYSTEM=="usb", ATTRS{idVendor}=="2e8a", ATTRS{idProduct}=="000a", MODE="0666"

# Generic USB-to-serial adapters
SUBSYSTEM=="tty", ATTRS{idVendor}=="0403", ATTRS{idProduct}=="6001", MODE="0666", GROUP="dialout"
SUBSYSTEM=="tty", ATTRS{idVendor}=="10c4", ATTRS{idProduct}=="ea60", MODE="0666", GROUP="dialout"
EOF
    
    # Reload udev rules
    sudo udevadm control --reload-rules
    sudo udevadm trigger
    
    log_success "Udev rules configured"
}

# Setup project environment
setup_project_env() {
    log_info "Setting up project environment..."
    
    # Return to project directory
    cd "$PROJECT_ROOT"
    
    # Initialize git submodules if any
    if [[ -f ".gitmodules" ]]; then
        git submodule update --init --recursive
    fi
    
    # Create build directory
    mkdir -p build
    
    # Run Python setup script
    if [[ -f "scripts/setup-dev-tools.py" ]]; then
        log_info "Running Python development tools setup..."
        python3 scripts/setup-dev-tools.py
    fi
    
    log_success "Project environment configured"
}

# Test the build system
test_build_system() {
    log_info "Testing build system..."
    
    cd "$PROJECT_ROOT/build"
    
    # Test CMake configuration
    log_info "Testing CMake configuration..."
    if cmake -DENABLE_STATIC_ANALYSIS=ON -DENABLE_FORMAT_CHECK=ON ..; then
        log_success "CMake configuration successful"
    else
        log_error "CMake configuration failed"
        return 1
    fi
    
    # Test make targets
    log_info "Testing available make targets..."
    make help | grep -E "(check-all|format-check|cppcheck)" || true
    
    log_success "Build system test completed"
}

# Verify installation
verify_installation() {
    log_info "Verifying installation..."
    
    # Check required tools
    TOOLS=(
        "cmake:CMake"
        "arm-none-eabi-gcc:ARM GCC"
        "clang-format:clang-format"
        "clang-tidy:clang-tidy"
        "cppcheck:cppcheck"
        "python3:Python"
        "git:Git"
    )
    
    ALL_GOOD=true
    
    for tool_pair in "${TOOLS[@]}"; do
        IFS=':' read -r tool_cmd tool_name <<< "$tool_pair"
        if command -v "$tool_cmd" >/dev/null 2>&1; then
            VERSION=$(eval "$tool_cmd --version 2>/dev/null | head -n1" || echo "unknown")
            log_success "$tool_name: $VERSION"
        else
            log_error "$tool_name not found"
            ALL_GOOD=false
        fi
    done
    
    # Check Python packages
    PYTHON_PACKAGES=("black" "isort" "flake8" "mypy" "pre-commit")
    
    for package in "${PYTHON_PACKAGES[@]}"; do
        if python3 -c "import $package" 2>/dev/null; then
            VERSION=$(python3 -c "import $package; print($package.__version__)" 2>/dev/null || echo "unknown")
            log_success "Python $package: $VERSION"
        else
            log_error "Python package $package not found"
            ALL_GOOD=false
        fi
    done
    
    # Check environment variables
    if [[ -n "$PICO_SDK_PATH" && -d "$PICO_SDK_PATH" ]]; then
        log_success "PICO_SDK_PATH: $PICO_SDK_PATH"
    else
        log_error "PICO_SDK_PATH not set or invalid"
        ALL_GOOD=false
    fi
    
    if [[ "$ALL_GOOD" == true ]]; then
        log_success "All tools and dependencies verified successfully!"
        return 0
    else
        log_error "Some tools or dependencies are missing"
        return 1
    fi
}

# Print next steps
print_next_steps() {
    cat << EOF

${GREEN}🎉 KISS Fuzzer Development Environment Setup Complete!${NC}

${BLUE}Next Steps:${NC}
1. ${YELLOW}Restart your terminal${NC} or run: source ~/.bashrc
2. ${YELLOW}Log out and back in${NC} for group changes to take effect
3. ${YELLOW}Navigate to project directory:${NC} cd $PROJECT_ROOT
4. ${YELLOW}Test the build system:${NC}
   mkdir -p build && cd build
   cmake -DENABLE_STATIC_ANALYSIS=ON -DENABLE_FORMAT_CHECK=ON ..
   make check-all

${BLUE}Available Development Commands:${NC}
• ${YELLOW}make check-all${NC}     - Run all quality checks
• ${YELLOW}make fix-all${NC}       - Auto-fix code formatting
• ${YELLOW}make format-check${NC}  - Check C/C++ formatting
• ${YELLOW}make cppcheck${NC}      - Run static analysis
• ${YELLOW}make${NC}               - Build firmware

${BLUE}Hardware Development:${NC}
• Connect Raspberry Pi Pico via USB
• Use ${YELLOW}lsusb${NC} to verify device detection
• Flash firmware: drag .uf2 file to mounted Pico

${BLUE}Documentation:${NC}
• Project Docs: https://kiss-fuzzer.readthedocs.io/
• Development Guide: docs/development.md
• Quality System: cmake/README.md

${GREEN}Happy coding! 🚀${NC}

EOF
}

# Main execution
main() {
    # Store project root
    PROJECT_ROOT=$(pwd)
    
    cat << EOF
${BLUE}
╔═══════════════════════════════════════════════════════════════╗
║                    KISS Fuzzer Setup                          ║
║                Linux Ubuntu/Debian                            ║
║                                                               ║
║  This script will install all development dependencies        ║
║  for KISS Fuzzer embedded development on Linux.              ║
╚═══════════════════════════════════════════════════════════════╝
${NC}

EOF
    
    log_info "Starting KISS Fuzzer development environment setup..."
    
    # Run all setup steps
    check_system
    update_system
    install_essential_tools
    install_arm_toolchain
    install_llvm_tools
    install_analysis_tools
    install_python_tools
    install_git_lfs
    install_usb_tools
    install_pico_sdk
    install_freertos
    install_optional_tools
    setup_udev_rules
    setup_project_env
    
    # Test and verify
    if test_build_system && verify_installation; then
        print_next_steps
        log_success "Setup completed successfully!"
        exit 0
    else
        log_error "Setup encountered errors. Please check the output above."
        exit 1
    fi
}

# Run main function
main "$@"
