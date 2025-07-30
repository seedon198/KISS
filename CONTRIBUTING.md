# Contributing to KISS Fuzzer 🤝

Thank you for considering contributing to KISS Fuzzer! We welcome contributions from the community and are excited to work with you.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [How to Contribute](#how-to-contribute)
- [Development Setup](#development-setup)
- [Code Standards](#code-standards)
- [Testing Guidelines](#testing-guidelines)
- [Documentation](#documentation)
- [Security](#security)
- [Bug Bounty Program](#bug-bounty-program)

## Code of Conduct

This project and everyone participating in it is governed by our [Code of Conduct](CODE_OF_CONDUCT.md). By participating, you are expected to uphold this code.

## How to Contribute

### Reporting Bugs 🐛

Before creating bug reports, please check existing issues to avoid duplicates. When you create a bug report, include as many details as possible:

- Use the [bug report template](.github/ISSUE_TEMPLATE/bug_report.md)
- Describe the exact steps to reproduce the problem
- Include hardware specifications and firmware version
- Provide logs, error messages, or screenshots if applicable

### Suggesting Features 💡

We welcome feature suggestions! Please:

- Use the [feature request template](.github/ISSUE_TEMPLATE/feature_request.md)
- Explain the use case and benefits
- Consider backward compatibility
- Provide mockups or examples if applicable

### Pull Requests 🔄

1. **Fork** the repository
2. **Create** a feature branch (`git checkout -b feature/amazing-feature`)
3. **Follow** our [code standards](#code-standards)
4. **Write** or update tests as needed
5. **Update** documentation if required
6. **Ensure** all tests pass
7. **Commit** your changes with descriptive messages
8. **Push** to your branch (`git push origin feature/amazing-feature`)
9. **Open** a Pull Request

#### Pull Request Guidelines

- Keep changes focused and atomic
- Write clear, descriptive commit messages
- Include tests for new functionality
- Update documentation as needed
- Ensure CI/CD checks pass
- Reference related issues using keywords (e.g., "Fixes #123")

## Development Setup

### Prerequisites

- **Hardware**: Raspberry Pi Pico W
- **Toolchain**: ARM GCC, CMake 3.13+
- **SDK**: Pico SDK
- **Python**: 3.8+ (for scripts and tools)

### Quick Start

```bash
# Clone the repository
git clone https://github.com/seedon198/KISS.git
cd KISS

# Setup environment
cp config.env.template config.env
# Edit config.env with your settings

# Build the project
./build.sh

# Flash to device
# Connect Pico W in BOOTSEL mode
cp build/kiss_fuzzer.uf2 /Volumes/RPI-RP2/
```

### Detailed Setup

See our comprehensive [Development Guide](DEVELOPMENT.md) for detailed setup instructions.

## Code Standards

### C/C++ Standards

- **Standard**: C11 with GNU extensions
- **Style**: Modified K&R with 4-space indentation
- **Formatter**: clang-format (configuration in `.clang-format`)
- **Linter**: clang-tidy, cppcheck

#### Code Style Example

```c
// Function naming: snake_case
int initialize_target_interface(target_config_t *config) {
    if (!config) {
        log_error("Invalid configuration pointer");
        return -1;
    }
    
    // Variable naming: snake_case
    uint32_t clock_frequency = config->clock_freq;
    bool voltage_enabled = false;
    
    // Constants: UPPER_SNAKE_CASE
    if (clock_frequency > MAX_CLOCK_FREQUENCY) {
        clock_frequency = DEFAULT_CLOCK_FREQUENCY;
    }
    
    return 0;
}
```

### Documentation Standards

- **API Documentation**: Doxygen comments for all public functions
- **Comments**: Clear, concise explanations of complex logic
- **README**: Keep up-to-date with latest features
- **Changelog**: Document all notable changes

#### Documentation Example

```c
/**
 * @brief Initialize the JTAG/SWD target interface
 * 
 * Sets up the target interface with the specified configuration,
 * including clock frequency, voltage levels, and pin mappings.
 * 
 * @param config Pointer to target configuration structure
 * @return 0 on success, negative error code on failure
 * 
 * @note This function must be called before any JTAG/SWD operations
 * @warning Ensure target power is stable before calling
 * 
 * @see target_config_t
 * @see cleanup_target_interface()
 */
int initialize_target_interface(target_config_t *config);
```

### Commit Message Standards

We follow the [Conventional Commits](https://conventionalcommits.org/) specification:

```
<type>[optional scope]: <description>

[optional body]

[optional footer(s)]
```

#### Types

- **feat**: New feature
- **fix**: Bug fix
- **docs**: Documentation changes
- **style**: Code style changes (formatting, etc.)
- **refactor**: Code refactoring
- **test**: Adding or updating tests
- **chore**: Maintenance tasks
- **perf**: Performance improvements
- **ci**: CI/CD changes

#### Examples

```
feat(jtag): add IEEE 1149.1 boundary scan support

Implement full boundary scan functionality with support for
instruction register capture and device chain enumeration.

Closes #42
```

```
fix(power): resolve voltage glitch timing issue

The voltage glitch pulse width was incorrectly calculated,
causing inconsistent fault injection results.

Fixes #67
```

## Testing Guidelines

### Unit Tests

- **Framework**: Unity test framework
- **Coverage**: Aim for >90% code coverage
- **Mocking**: Use CMock for hardware dependencies
- **Naming**: `test_<module>_<function>_<scenario>()`

#### Test Example

```c
void test_jtag_scan_chain_single_device(void) {
    // Setup
    target_config_t config = {
        .clock_freq = 1000000,  // 1MHz
        .voltage = TARGET_3V3
    };
    
    // Mock expectations
    gpio_set_level_ExpectAndReturn(TCK_PIN, false, true);
    gpio_set_level_ExpectAndReturn(TMS_PIN, true, true);
    
    // Execute
    int result = jtag_scan_chain(&config);
    
    // Verify
    TEST_ASSERT_EQUAL(1, result);
    TEST_ASSERT_EQUAL(0x12345678, detected_devices[0].idcode);
}
```

### Integration Tests

- Test complete workflows
- Use real hardware when possible
- Validate timing-critical operations
- Test error conditions and recovery

### Hardware-in-the-Loop Tests

- Automated testing with real Pico W hardware
- Validate JTAG/SWD protocols with target devices
- Performance benchmarking
- Power consumption validation

## Documentation

### Building Documentation

```bash
cd docs
pip install -r requirements.txt
make html
```

### Multi-language Support

We support 9 languages. To add or update translations:

```bash
cd docs
./setup-i18n.sh
# Edit translation files in locales/<lang>/LC_MESSAGES/
make html
```

### Documentation Structure

```
docs/
├── getting-started/     # Quick start guides
├── hardware/           # Hardware documentation
├── tutorials/          # Step-by-step tutorials
├── api-reference/      # API documentation
├── troubleshooting/    # Common issues
└── locales/           # Translations
```

## Security

### Reporting Security Issues

Please **DO NOT** report security vulnerabilities through public GitHub issues.

Instead, email us at: **security@kiss-fuzzer.org**

### Security Guidelines

- Keep dependencies updated
- Validate all input data
- Use secure coding practices
- Follow principle of least privilege
- Document security considerations

### Code Review

All code changes require review by at least one maintainer:

- Security-sensitive changes require additional review
- External contributions require maintainer approval
- Automated security scans must pass

## Bug Bounty Program

We run a swag-based bug bounty program! See our [Bug Bounty Program](https://github.com/seedon198/KISS/wiki/Bug-Bounty-Program) for details.

### Rewards

- **Critical**: Custom KISS Fuzzer + Swag Pack
- **High**: Premium Swag Pack + Recognition
- **Medium**: Standard Swag Pack
- **Low**: Digital Recognition

## Recognition

Contributors are recognized in:

- [Contributors section](README.md#contributors) in README
- [Hall of Fame](https://github.com/seedon198/KISS/wiki/Hall-of-Fame) wiki page
- Release notes for significant contributions
- Annual contributor appreciation post

## Questions?

- 💬 **Discussions**: [GitHub Discussions](https://github.com/seedon198/KISS/discussions)
- 🐛 **Issues**: [GitHub Issues](https://github.com/seedon198/KISS/issues)
- 📧 **Email**: support@kiss-fuzzer.org
- 📖 **Wiki**: [Project Wiki](https://github.com/seedon198/KISS/wiki)

Thank you for contributing to KISS Fuzzer! 🚀

---

**Happy Contributing!** 🎉
