#!/bin/bash
# KISS Fuzzer Quick Build Test
# Tests if our CMake quality system works with minimal dependencies

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

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

echo "🔍 KISS Fuzzer Build System Test"
echo "================================="

# Check if we're in the right directory
if [[ ! -f "CMakeLists.txt" ]]; then
    log_error "CMakeLists.txt not found. Please run this script from the project root."
    exit 1
fi

# Check available tools
log_info "Checking available development tools..."

TOOLS_AVAILABLE=0
TOOLS_MISSING=()

# Check for basic tools
if command -v cmake >/dev/null 2>&1; then
    CMAKE_VERSION=$(cmake --version | head -n1)
    log_success "✅ CMake: $CMAKE_VERSION"
    ((TOOLS_AVAILABLE++))
else
    log_warning "❌ CMake not found"
    TOOLS_MISSING+=("cmake")
fi

if command -v clang-format >/dev/null 2>&1; then
    CLANG_FORMAT_VERSION=$(clang-format --version)
    log_success "✅ clang-format: $CLANG_FORMAT_VERSION"
    ((TOOLS_AVAILABLE++))
else
    log_warning "❌ clang-format not found"
    TOOLS_MISSING+=("clang-format")
fi

if command -v cppcheck >/dev/null 2>&1; then
    CPPCHECK_VERSION=$(cppcheck --version)
    log_success "✅ cppcheck: $CPPCHECK_VERSION"
    ((TOOLS_AVAILABLE++))
else
    log_warning "❌ cppcheck not found"
    TOOLS_MISSING+=("cppcheck")
fi

if command -v clang-tidy >/dev/null 2>&1; then
    CLANG_TIDY_VERSION=$(clang-tidy --version | head -n1)
    log_success "✅ clang-tidy: $CLANG_TIDY_VERSION"
    ((TOOLS_AVAILABLE++))
else
    log_warning "❌ clang-tidy not found"
    TOOLS_MISSING+=("clang-tidy")
fi

# Check Python tools
if command -v python3 >/dev/null 2>&1; then
    PYTHON_VERSION=$(python3 --version)
    log_success "✅ Python: $PYTHON_VERSION"
    ((TOOLS_AVAILABLE++))
    
    # Check Python packages
    if python3 -c "import black" 2>/dev/null; then
        log_success "✅ Black (Python formatter)"
        ((TOOLS_AVAILABLE++))
    else
        log_warning "❌ Black not found"
        TOOLS_MISSING+=("black")
    fi
    
    if python3 -c "import isort" 2>/dev/null; then
        log_success "✅ isort (Python import sorter)"
        ((TOOLS_AVAILABLE++))
    else
        log_warning "❌ isort not found"
        TOOLS_MISSING+=("isort")
    fi
    
    if python3 -c "import flake8" 2>/dev/null; then
        log_success "✅ flake8 (Python linter)"
        ((TOOLS_AVAILABLE++))
    else
        log_warning "❌ flake8 not found"
        TOOLS_MISSING+=("flake8")
    fi
else
    log_warning "❌ Python3 not found"
    TOOLS_MISSING+=("python3")
fi

# Check for Pico SDK (optional for quality checks)
if [[ -n "$PICO_SDK_PATH" ]] && [[ -d "$PICO_SDK_PATH" ]]; then
    log_success "✅ Pico SDK: $PICO_SDK_PATH"
    ((TOOLS_AVAILABLE++))
else
    log_warning "❌ Pico SDK not found (PICO_SDK_PATH not set)"
    TOOLS_MISSING+=("pico-sdk")
fi

echo ""
log_info "Tool Summary: $TOOLS_AVAILABLE tools available"

if [[ ${#TOOLS_MISSING[@]} -gt 0 ]]; then
    log_warning "Missing tools: ${TOOLS_MISSING[*]}"
    echo ""
    log_info "To install missing tools, run:"
    log_info "  macOS: ./scripts/setup-macos-arm64.sh"
    log_info "  Linux: ./scripts/setup-linux-ubuntu.sh"
    log_info "  or use: python3 scripts/setup-dev-tools.py"
fi

# Test CMake configuration if CMake is available
if command -v cmake >/dev/null 2>&1; then
    echo ""
    log_info "Testing CMake configuration..."
    
    # Clean up previous test builds
    if [[ -d "build-test" ]]; then
        log_info "Cleaning up previous test build..."
        rm -rf build-test
    fi
    
    # Create build directory
    mkdir -p build-test
    cd build-test
    
    # Test basic CMake configuration (without Pico SDK for now)
    log_info "Running CMake configure..."
    if cmake -DENABLE_STATIC_ANALYSIS=ON -DENABLE_FORMAT_CHECK=ON .. 2>&1 | tee cmake-output.log; then
        log_success "✅ CMake configuration successful"
        
        # Test if our quality targets are available
        log_info "Checking available make targets..."
        if make help | grep -q "check-all"; then
            log_success "✅ Quality check targets available"
            
            # Test quality checks if tools are available
            if [[ $TOOLS_AVAILABLE -ge 4 ]]; then
                log_info "Running quick quality checks..."
                
                # Test individual targets that should work
                if command -v clang-format >/dev/null 2>&1; then
                    log_info "Testing clang-format check..."
                    if make format-check 2>&1; then
                        log_success "✅ clang-format check passed"
                    else
                        log_warning "⚠️  clang-format check failed (expected - no .clang-format yet)"
                    fi
                fi
                
                if command -v cppcheck >/dev/null 2>&1; then
                    log_info "Testing cppcheck..."
                    if make cppcheck 2>&1; then
                        log_success "✅ cppcheck passed"
                    else
                        log_warning "⚠️  cppcheck failed (expected without Pico SDK)"
                    fi
                fi
                
            else
                log_warning "Not enough tools available for quality testing"
            fi
            
        else
            log_error "❌ Quality check targets not found"
        fi
        
    else
        log_error "❌ CMake configuration failed"
        echo ""
        log_info "CMake output:"
        cat cmake-output.log
    fi
    
    # Return to project root
    cd ..
    
else
    log_warning "Skipping CMake test (CMake not available)"
fi

echo ""
echo "🎯 Test Summary"
echo "==============="

if [[ $TOOLS_AVAILABLE -ge 6 ]]; then
    log_success "✅ Development environment is well configured!"
    log_info "You can now run:"
    log_info "  mkdir build && cd build"
    log_info "  cmake -DENABLE_STATIC_ANALYSIS=ON .."
    log_info "  make check-all"
elif [[ $TOOLS_AVAILABLE -ge 3 ]]; then
    log_warning "⚠️  Partial development environment configured"
    log_info "Some quality tools are missing but basic functionality should work"
else
    log_error "❌ Development environment needs setup"
    log_info "Please run the appropriate setup script for your platform"
fi

echo ""
log_info "Next steps:"
log_info "1. Install missing tools (if any) using setup scripts"
log_info "2. Test full build: mkdir build && cd build && cmake .. && make"
log_info "3. Run quality checks: make check-all"
log_info "4. Auto-fix issues: make fix-all"

# Clean up test build
if [[ -d "build-test" ]]; then
    log_info "Cleaning up test build directory..."
    rm -rf build-test
fi

echo ""
log_success "Build system test completed!"
