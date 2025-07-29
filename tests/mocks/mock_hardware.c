/**
 * @file mock_hardware.c
 * @brief Hardware abstraction layer mock implementations
 * @author KISS Fuzzer Team
 * @date 2025
 */

#include "mock_hardware.h"
#include <string.h>

// Global mock state
mock_hardware_state_t mock_hw_state;

// Mock timer state
static uint32_t mock_timer_ticks = 0;

void mock_hardware_reset(void) {
    memset(&mock_hw_state, 0, sizeof(mock_hw_state));
    mock_timer_ticks = 0;
}

// I2C mock functions
void mock_i2c_expect_init(uint8_t port, uint32_t freq) {
    mock_hw_state.expected_calls++;
}

void mock_i2c_expect_write(uint8_t addr, const uint8_t* data, size_t len) {
    mock_hw_state.expected_calls++;
}

void mock_i2c_expect_write_command(uint8_t cmd) {
    mock_hw_state.expected_calls++;
}

void mock_i2c_expect_write_sequence(void) {
    // Expect a sequence of initialization commands
    mock_hw_state.expected_calls += 10; // Typical init sequence length
}

void mock_i2c_set_init_result(bool success) {
    mock_hw_state.i2c_should_fail = !success;
}

bool mock_i2c_verify_expectations(void) {
    return mock_hw_state.actual_calls >= mock_hw_state.expected_calls;
}

// GPIO mock functions
void mock_gpio_set_state(uint8_t pin, bool state) {
    if (state) {
        mock_hw_state.gpio_states |= (1 << pin);
    } else {
        mock_hw_state.gpio_states &= ~(1 << pin);
    }
}

bool mock_gpio_get_state(uint8_t pin) {
    return (mock_hw_state.gpio_states >> pin) & 1;
}

void mock_gpio_expect_init(uint8_t pin) {
    mock_hw_state.expected_calls++;
}

// ADC mock functions
void mock_adc_set_value(uint16_t value) {
    mock_hw_state.adc_value = value;
}

uint16_t mock_adc_get_last_read(void) {
    return mock_hw_state.adc_value;
}

// Timer mock functions
void mock_timer_advance(uint32_t ms) {
    mock_timer_ticks += ms;
}

uint32_t mock_timer_get_ticks(void) {
    return mock_timer_ticks;
}
