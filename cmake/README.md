# KISS Fuzzer Code Quality System

This directory contains configuration and tools for maintaining high code quality in the KISS Fuzzer project.

## Overview

The code quality system enforces:
- **Consistent formatting** with clang-format and Black
- **Static analysis** with cppcheck and clang-tidy  
- **Style compliance** with custom rules for embedded C
- **Import organization** with isort for Python files
- **Automated checks** via pre-commit hooks and CI/CD

## Quick Start

1. **Install tools automatically:**
   ```bash
   python3 scripts/setup-dev-tools.py
   ```

2. **Check code quality:**
   ```bash
   ./scripts/check-quality.sh
   ```

3. **Auto-fix issues:**
   ```bash
   ./scripts/fix-quality.sh
   ```

## Files in this Directory

### Configuration Files
- **`clang-format-config.yaml`** - C/C++ formatting rules template
- **`clang-tidy-config.yaml`** - Static analysis configuration template
- **`CodeQuality.cmake`** - CMake integration for quality tools

### Generated Files (auto-created)
- **`.clang-format`** - Active C/C++ formatting configuration
- **`.clang-tidy`** - Active static analysis configuration
- **`.flake8`** - Python linting configuration
- **`pyproject.toml`** - Python tool settings
- **`.pre-commit-config.yaml`** - Git hook configuration

## CMake Integration

The quality system integrates with CMake through build options:

```bash
# Enable all quality checks (default)
cmake -DENABLE_STATIC_ANALYSIS=ON -DENABLE_FORMAT_CHECK=ON ..

# Additional options
cmake -DQUALITY_CHECK_ON_BUILD=ON ..      # Check before build
cmake -DAUTO_FORMAT=ON ..                  # Auto-format during build
cmake -DENABLE_CPPCHECK=OFF ..            # Disable specific tools
```

## Available Make Targets

| Target | Description |
|--------|-------------|
| `check-all` | Run all quality checks |
| `fix-all` | Auto-fix all fixable issues |
| `format-check` | Check C/C++ formatting |
| `format-fix` | Fix C/C++ formatting |
| `cppcheck` | Run cppcheck static analysis |
| `clang-tidy` | Run clang-tidy analysis |
| `black-check` | Check Python formatting |
| `black-fix` | Fix Python formatting |
| `isort-check` | Check Python import order |
| `isort-fix` | Fix Python import order |
| `flake8` | Run Python linting |

## Coding Standards Enforced

### C/C++ Standards
- **Indentation**: 4 spaces, no tabs
- **Line length**: 100 characters maximum
- **Naming**: snake_case for functions/variables, UPPER_CASE for constants
- **Braces**: Linux kernel style (K&R variant)
- **Documentation**: Doxygen comments for all public functions

### Python Standards
- **Formatting**: Black (88 character lines)
- **Import order**: isort with black profile
- **Style**: PEP 8 compliance via flake8
- **Type hints**: Encouraged, checked with mypy

## CI/CD Integration

The quality system runs automatically in GitHub Actions:
- **Pull requests**: Full quality checks required
- **Main branch**: Quality checks + build verification
- **Artifacts**: Quality reports uploaded for review

## Troubleshooting

### Common Issues

**Tool not found:**
```bash
# Re-run setup script
python3 scripts/setup-dev-tools.py

# Or install manually
brew install llvm cppcheck  # macOS
sudo apt install clang-format clang-tidy cppcheck  # Ubuntu
```

**Format check fails:**
```bash
# Auto-fix formatting
make format-fix
make black-fix
make isort-fix
```

**Static analysis warnings:**
```bash
# Review clang-tidy output
make clang-tidy

# Check cppcheck results
make cppcheck
```

### Configuration Customization

To modify quality rules:

1. Edit configuration templates in `cmake/`
2. Delete generated config files (`.clang-format`, etc.)
3. Re-run CMake to regenerate configurations

### Disabling Checks

For emergency situations:

```bash
# Skip pre-commit hooks
git commit --no-verify

# Disable specific CMake checks
cmake -DENABLE_STATIC_ANALYSIS=OFF ..

# Skip individual tools
cmake -DENABLE_CPPCHECK=OFF -DENABLE_CLANG_TIDY=OFF ..
```

## Contributing

When contributing to the quality system:

1. Test changes on multiple platforms (macOS, Ubuntu, Fedora)
2. Verify all tools install and run correctly
3. Update documentation for new features
4. Ensure CI/CD pipeline passes all checks

For questions or issues, please create a GitHub issue with the `quality` label.
