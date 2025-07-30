#!/bin/bash
# KISS Fuzzer Development Environment Setup
# macOS Apple Silicon (M1/M2/M3) - ARM64
# 
# This script installs all required dependencies for KISS Fuzzer development
# on macOS systems with Apple Silicon processors.

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

# Check if running on Apple Silicon
check_architecture() {
    ARCH=$(uname -m)
    if [[ "$ARCH" != "arm64" ]]; then
        log_error "This script is designed for Apple Silicon Macs (arm64)"
        log_error "Detected architecture: $ARCH"
        log_error "Please use the appropriate setup script for your platform"
        exit 1
    fi
    log_success "Detected Apple Silicon Mac ($ARCH)"
}

# Check macOS version
check_macos_version() {
    MACOS_VERSION=$(sw_vers -productVersion)
    MACOS_MAJOR=$(echo $MACOS_VERSION | cut -d. -f1)
    
    if [[ $MACOS_MAJOR -lt 12 ]]; then
        log_warning "macOS version $MACOS_VERSION detected"
        log_warning "macOS 12.0+ recommended for best compatibility"
    else
        log_success "macOS $MACOS_VERSION is supported"
    fi
}

# Install Homebrew if not present
install_homebrew() {
    if command -v brew >/dev/null 2>&1; then
        log_success "Homebrew is already installed"
        brew --version
    else
        log_info "Installing Homebrew..."
        /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
        
        # Add Homebrew to PATH for Apple Silicon
        echo 'eval "$(/opt/homebrew/bin/brew shellenv)"' >> ~/.zprofile
        eval "$(/opt/homebrew/bin/brew shellenv)"
        
        log_success "Homebrew installed successfully"
    fi
}

# Install development tools
install_dev_tools() {
    log_info "Installing development tools..."
    
    # Update Homebrew
    brew update
    
    # Install essential development tools
    log_info "Installing CMake and build tools..."
    brew install cmake ninja pkg-config
    
    # Install ARM cross-compilation toolchain
    log_info "Installing ARM GCC toolchain..."
    brew install --cask gcc-arm-embedded
    
    # Install LLVM tools (clang-format, clang-tidy)
    log_info "Installing LLVM tools..."
    brew install llvm
    
    # Install static analysis tools
    log_info "Installing static analysis tools..."
    brew install cppcheck
    
    # Install Git LFS for large files
    log_info "Installing Git LFS..."
    brew install git-lfs
    git lfs install
    
    log_success "Development tools installed"
}

# Setup LLVM tools in PATH
setup_llvm_path() {
    log_info "Setting up LLVM tools in PATH..."
    
    # Add LLVM to PATH in shell profiles
    LLVM_PATH="/opt/homebrew/opt/llvm/bin"
    
    for PROFILE in ~/.zshrc ~/.bash_profile ~/.bashrc; do
        if [[ -f "$PROFILE" ]]; then
            if ! grep -q "$LLVM_PATH" "$PROFILE"; then
                echo "export PATH=\"$LLVM_PATH:\$PATH\"" >> "$PROFILE"
                log_info "Added LLVM to PATH in $PROFILE"
            fi
        fi
    done
    
    # Export for current session
    export PATH="$LLVM_PATH:$PATH"
    
    log_success "LLVM tools configured"
}

# Install Python development tools
install_python_tools() {
    log_info "Installing Python development tools..."
    
    # Check Python version
    PYTHON_VERSION=$(python3 --version | cut -d' ' -f2)
    log_info "Python version: $PYTHON_VERSION"
    
    # Install pipx for Python application management
    if ! command -v pipx >/dev/null 2>&1; then
        log_info "Installing pipx..."
        brew install pipx
        pipx ensurepath
    fi
    
    # Install Python quality tools using pipx (preferred method)
    if command -v pipx >/dev/null 2>&1; then
        log_info "Using pipx for Python package management..."
        pipx install black || log_warning "Failed to install black with pipx"
        pipx install isort || log_warning "Failed to install isort with pipx"
        pipx install flake8 || log_warning "Failed to install flake8 with pipx"
        pipx install mypy || log_warning "Failed to install mypy with pipx"
        pipx install pre-commit || log_warning "Failed to install pre-commit with pipx"
        pipx install cmake-format || log_warning "Failed to install cmake-format with pipx"
        
        # Documentation tools
        pipx install sphinx || log_warning "Failed to install sphinx with pipx"
        
        # Ensure pipx packages are in PATH
        export PATH="$HOME/.local/bin:$PATH"
        
        # Add to shell profiles
        for PROFILE in ~/.zshrc ~/.bash_profile ~/.bashrc; do
            if [[ -f "$PROFILE" ]]; then
                if ! grep -q "HOME/.local/bin" "$PROFILE"; then
                    echo 'export PATH="$HOME/.local/bin:$PATH"' >> "$PROFILE"
                    log_info "Added ~/.local/bin to PATH in $PROFILE"
                fi
            fi
        done
        
    else
        # Fallback to pip3 with user install
        log_warning "pipx not available, using pip3 with --user flag..."
        
        python3 -m pip install --user \
            black \
            isort \
            flake8 \
            mypy \
            pre-commit \
            cmake-format \
            sphinx \
            sphinx-rtd-theme \
            myst-parser \
            sphinx-copybutton \
            sphinxcontrib-mermaid \
            breathe || {
            
            log_warning "Failed with --user, trying with --break-system-packages"
            python3 -m pip install --break-system-packages \
                black \
                isort \
                flake8 \
                mypy \
                pre-commit \
                cmake-format \
                sphinx \
                sphinx-rtd-theme \
                myst-parser \
                sphinx-copybutton \
                sphinxcontrib-mermaid \
                breathe || {
                log_error "Failed to install Python packages"
                return 1
            }
        }
        
        # Ensure user packages are in PATH
        USER_BASE=$(python3 -m site --user-base)
        USER_BIN="$USER_BASE/bin"
        export PATH="$USER_BIN:$PATH"
        
        # Add to shell profiles
        for PROFILE in ~/.zshrc ~/.bash_profile ~/.bashrc; do
            if [[ -f "$PROFILE" ]]; then
                if ! grep -q "$USER_BIN" "$PROFILE"; then
                    echo "export PATH=\"$USER_BIN:\$PATH\"" >> "$PROFILE"
                    log_info "Added $USER_BIN to PATH in $PROFILE"
                fi
            fi
        done
    fi
    
    log_success "Python tools installed"
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
    for PROFILE in ~/.zshrc ~/.bash_profile ~/.bashrc; do
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
    make help | grep -E "(check-all|format-check|cppcheck)"
    
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
1. ${YELLOW}Restart your terminal${NC} or run: source ~/.zshrc
2. ${YELLOW}Navigate to project directory:${NC} cd $PROJECT_ROOT
3. ${YELLOW}Test the build system:${NC}
   mkdir -p build && cd build
   cmake -DENABLE_STATIC_ANALYSIS=ON -DENABLE_FORMAT_CHECK=ON ..
   make check-all

${BLUE}Available Development Commands:${NC}
• ${YELLOW}make check-all${NC}     - Run all quality checks
• ${YELLOW}make fix-all${NC}       - Auto-fix code formatting
• ${YELLOW}make format-check${NC}  - Check C/C++ formatting
• ${YELLOW}make cppcheck${NC}      - Run static analysis
• ${YELLOW}make${NC}               - Build firmware

${BLUE}Documentation:${NC}
• Project Docs: https://kiss-fuzz.readthedocs.io/
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
║              macOS Apple Silicon (ARM64)                      ║
║                                                               ║
║  This script will install all development dependencies        ║
║  for KISS Fuzzer embedded development on macOS.              ║
╚═══════════════════════════════════════════════════════════════╝
${NC}

EOF
    
    log_info "Starting KISS Fuzzer development environment setup..."
    
    # Run all setup steps
    check_architecture
    check_macos_version
    install_homebrew
    install_dev_tools
    setup_llvm_path
    install_python_tools
    install_pico_sdk
    install_freertos
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
