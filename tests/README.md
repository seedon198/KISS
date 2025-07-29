# Test Framework for KISS Fuzzer

This directory contains unit tests and integration tests for the KISS Fuzzer project.

## Test Structure

- `unit/` - Unit tests for individual modules
- `integration/` - Integration tests for combined functionality  
- `mocks/` - Mock implementations for testing
- `fixtures/` - Test data and fixtures
- `scripts/` - Test automation scripts

## Running Tests

```bash
# Build and run all tests
mkdir build_test && cd build_test
cmake -DBUILD_TESTS=ON ..
make
ctest

# Run specific test suite
./test_display
./test_jtag
./test_power
```

## Test Coverage

Each module should have comprehensive unit tests covering:
- Initialization and cleanup
- Normal operation
- Error conditions
- Edge cases
- Resource management

## Mock Framework

Tests use a lightweight mock framework to simulate hardware interactions without requiring physical hardware.
