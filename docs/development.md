# Development Guide

```{note}
**Navigation:** [Home](index.md) | [Getting Started](getting-started.md) | [Hardware](hardware.md) | [Software](software.md) | [API Reference](api-reference.md) | [Tutorials](tutorials.md) | [Troubleshooting](troubleshooting.md)
```

Contributing to KISS Fuzzer development and extending functionality.

## Development Environment Setup

### Prerequisites

- **Git**: Version control
- **CMake**: Build system (3.13+)
- **ARM GCC**: Cross-compiler toolchain
- **Python**: For scripts and documentation (3.8+)
- **VSCode**: Recommended IDE with C/C++ extension

### Pico SDK Installation

```bash
# Clone Pico SDK
git clone https://github.com/raspberrypi/pico-sdk.git
cd pico-sdk
git submodule update --init

# Set environment variable
export PICO_SDK_PATH=/path/to/pico-sdk
```

### Project Setup

```bash
# Clone KISS Fuzzer
git clone https://github.com/your-username/KISS.git
cd KISS

# Initialize submodules
git submodule update --init --recursive

# Build
mkdir build && cd build
cmake ..
make -j$(nproc)
```

## Project Structure

```
KISS/
├── src/                    # Source code
│   ├── main.c             # Application entry point
│   ├── kiss_fuzzer.h      # Common definitions
│   ├── display.*          # Display driver
│   ├── ui.*               # User interface
│   ├── jtag.*             # JTAG/SWD engine
│   ├── wifi.*             # Wi-Fi and web server
│   ├── power.*            # Power management
│   ├── storage.*          # SD card operations
│   └── *.pio              # PIO assembly programs
├── docs/                  # Documentation
├── hardware/              # PCB design files
├── case/                  # 3D printable case
├── tests/                 # Unit and integration tests
├── scripts/               # Build and utility scripts
└── CMakeLists.txt         # Build configuration
```

## Coding Standards

### Style Guide

- **Indentation**: 4 spaces (no tabs)
- **Line Length**: 100 characters maximum
- **Naming**: snake_case for functions and variables
- **Constants**: UPPER_SNAKE_CASE
- **Braces**: K&R style (opening brace on same line)

### Example Function
```c
/**
 * @brief Initialize the JTAG subsystem
 * @param config Pointer to configuration structure
 * @return KISS_OK on success, error code on failure
 */
kiss_error_t jtag_init(const jtag_config_t* config) {
    if (config == NULL) {
        return KISS_ERROR_INVALID_PARAM;
    }
    
    // Implementation here...
    
    return KISS_OK;
}
```

### Documentation

All public functions must have Doxygen comments:

```c
/**
 * @brief Brief description of function
 * @param param1 Description of first parameter
 * @param param2 Description of second parameter
 * @return Description of return value
 * @note Any special notes about usage
 * @warning Any warnings about dangerous usage
 */
```

## Adding New Features

### 1. Planning Phase

1. **Create Issue**: Describe the feature request
2. **Design Review**: Discuss architecture implications
3. **API Design**: Define public interfaces
4. **Documentation**: Update design docs

### 2. Implementation Phase

1. **Create Branch**: `feature/description-of-feature`
2. **Write Tests**: Test-driven development preferred
3. **Implement Code**: Follow coding standards
4. **Update Docs**: Keep documentation current

### 3. Integration Phase

1. **Code Review**: Submit pull request
2. **Testing**: Automated and manual testing
3. **Documentation**: Final doc updates
4. **Merge**: After approval and testing

## Testing Framework

### Unit Tests

Location: `tests/unit/`

Example test:
```c
#include "unity.h"
#include "jtag.h"

void test_jtag_idcode_parsing(void) {
    uint32_t idcode = 0x4BA00477;
    uint32_t manufacturer = jtag_extract_manufacturer(idcode);
    
    TEST_ASSERT_EQUAL_UINT32(0x23B, manufacturer);
}

void setUp(void) {
    // Setup before each test
}

void tearDown(void) {
    // Cleanup after each test
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_jtag_idcode_parsing);
    return UNITY_END();
}
```

### Integration Tests

Location: `tests/integration/`

Tests that verify component interactions:
- Display + UI integration
- JTAG + Storage integration
- Wi-Fi + UI integration

### Hardware-in-Loop Tests

Location: `tests/hil/`

Tests that require actual hardware:
- JTAG communication with real targets
- SD card operations
- Battery charging behavior

## Build System

### CMake Configuration

The build system supports multiple configurations:

```bash
# Debug build (default)
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Release build
cmake -DCMAKE_BUILD_TYPE=Release ..

# Release with debug info
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
```

### Build Targets

```bash
# Build main firmware
make kiss_fuzzer

# Build and run unit tests
make test

# Generate documentation
make docs

# Flash to device
make flash

# Create release package
make package
```

### Custom CMake Options

```bash
# Enable additional debugging
cmake -DENABLE_DEBUG_LOGGING=ON ..

# Build without Wi-Fi support
cmake -DENABLE_WIFI=OFF ..

# Use custom display driver
cmake -DDISPLAY_DRIVER=ssd1306_custom ..
```

## Debugging

### Serial Debugging

Enable debug output via USB:
```c
#define DEBUG_LEVEL LOG_DEBUG
#include "debug.h"

void some_function(void) {
    DEBUG_LOG("Starting operation");
    // ... code ...
    DEBUG_LOG("Operation completed");
}
```

### Hardware Debugging

Using SWD with external debugger:

1. **Connect** SWD debugger to debug header
2. **Start** OpenOCD or similar
3. **Attach** GDB
4. **Set** breakpoints and inspect variables

### PIO Debugging

Debug PIO programs with special tools:
```bash
# Analyze PIO assembly
pioasm -v jtag.pio jtag.pio.h

# View PIO state machine status
# (requires custom debug build)
```

## Performance Optimization

### Profiling

Use built-in profiling:
```c
#include "profiler.h"

void performance_critical_function(void) {
    PROFILE_START("operation_name");
    
    // Critical code here
    
    PROFILE_END("operation_name");
}
```

### Memory Optimization

- **Static Allocation**: Prefer static over dynamic allocation
- **Stack Usage**: Monitor stack high-water marks
- **Heap Fragmentation**: Use appropriate FreeRTOS heap algorithm

### Power Optimization

- **Clock Gating**: Disable unused peripherals
- **Sleep Modes**: Use appropriate sleep states
- **DMA**: Offload CPU for data transfers

## Release Process

### Version Management

Semantic versioning (MAJOR.MINOR.PATCH):
- **MAJOR**: Breaking API changes
- **MINOR**: New features, backward compatible
- **PATCH**: Bug fixes

### Release Checklist

1. **Code Complete**: All features implemented
2. **Testing**: All tests passing
3. **Documentation**: Updated and reviewed
4. **Performance**: Benchmarks acceptable
5. **Hardware**: Tested on physical devices
6. **Package**: Release artifacts created

### Distribution

1. **GitHub Release**: Create release with binaries
2. **Documentation**: Deploy to Read the Docs
3. **Announcement**: Update project communications
4. **Tagging**: Tag commit with version number

## Contributing Guidelines

### Pull Request Process

1. **Fork** the repository
2. **Create** feature branch from main
3. **Implement** changes with tests
4. **Update** documentation
5. **Submit** pull request with description

### Code Review Criteria

- **Functionality**: Does it work as intended?
- **Code Quality**: Follows style guidelines?
- **Testing**: Adequate test coverage?
- **Documentation**: Clear and complete?
- **Performance**: No significant regressions?

### Commit Message Format

```
type(scope): short description

Longer description explaining the change and why it was made.

Fixes: #123
```

Types: feat, fix, docs, style, refactor, test, chore

## Code Quality and Linting

KISS Fuzzer enforces strict code quality standards through automated tools integrated into the build system. This ensures consistent, maintainable, and bug-free code across all contributions.

### Automated Quality Checks

The project includes comprehensive quality checking through:

**C/C++ Tools:**
- **clang-format**: Enforces consistent code formatting
- **clang-tidy**: Advanced static analysis and linting  
- **cppcheck**: Additional static analysis for common bugs

**Python Tools:**
- **Black**: Automatic code formatting
- **isort**: Import statement organization
- **flake8**: Style guide enforcement and linting

**Build Integration:**
- **Pre-commit hooks**: Automatic checks before commits
- **CMake targets**: Integration with build system
- **CI/CD ready**: Supports automated testing pipelines

### Quick Setup

Install all development tools automatically:

```bash
# Run the automated setup script
python3 scripts/setup-dev-tools.py

# Or install manually (see Manual Installation section below)
```

### Using Quality Checks

**Check all code quality:**
```bash
# In build directory
make check-all

# Or use convenience script
./scripts/check-quality.sh
```

**Auto-fix issues:**
```bash
# In build directory  
make fix-all

# Or use convenience script
./scripts/fix-quality.sh
```

**Individual tool usage:**
```bash
make format-check      # Check C/C++ formatting
make format-fix        # Fix C/C++ formatting  
make cppcheck          # Run cppcheck analysis
make clang-tidy        # Run clang-tidy analysis
make black-check       # Check Python formatting
make black-fix         # Fix Python formatting
make isort-check       # Check Python imports
make isort-fix         # Fix Python imports
make flake8            # Run Python linting
```

### Build Options

Configure quality checking behavior:

```bash
# Enable all quality checks (default)
cmake -DENABLE_STATIC_ANALYSIS=ON -DENABLE_FORMAT_CHECK=ON ..

# Run quality checks before every build
cmake -DQUALITY_CHECK_ON_BUILD=ON ..

# Enable automatic formatting during build
cmake -DAUTO_FORMAT=ON ..

# Disable specific tools
cmake -DENABLE_CPPCHECK=OFF -DENABLE_CLANG_TIDY=OFF ..
```

### Pre-commit Hooks

Pre-commit hooks automatically run quality checks before each commit:

```bash
# Install hooks (done automatically by setup script)
pre-commit install

# Run hooks manually on all files
pre-commit run --all-files

# Skip hooks for emergency commits (not recommended)
git commit --no-verify -m "Emergency fix"
```

### Manual Tool Installation

**macOS (Homebrew):**
```bash
brew install llvm cppcheck
pip3 install black isort flake8 pre-commit
```

**Ubuntu/Debian:**
```bash
sudo apt-get install clang-format clang-tidy cppcheck
pip3 install black isort flake8 pre-commit
```

**Fedora:**
```bash
sudo dnf install clang-tools-extra cppcheck
pip3 install black isort flake8 pre-commit
```

### Configuration Files

Quality tools are configured through:

- **`.clang-format`**: C/C++ formatting rules (auto-generated)
- **`.clang-tidy`**: Static analysis configuration (auto-generated)  
- **`.flake8`**: Python linting configuration
- **`pyproject.toml`**: Python tool settings (Black, isort)
- **`.pre-commit-config.yaml`**: Git hook configuration (auto-generated)
