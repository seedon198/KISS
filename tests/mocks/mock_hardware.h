/**
 * @file mock_hardware.h
 * @brief Hardware abstraction layer mocks for testing
 * @author KISS Fuzzer Team
 * @date 2025
 */

#ifndef MOCK_HARDWARE_H
#define MOCK_HARDWARE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// Mock hardware state
typedef struct {
    bool i2c_initialized;
    bool spi_initialized;
    bool adc_initialized;
    uint32_t gpio_states;
    uint16_t adc_value;
    bool i2c_should_fail;
    int expected_calls;
    int actual_calls;
} mock_hardware_state_t;

extern mock_hardware_state_t mock_hw_state;

// Reset mock state
void mock_hardware_reset(void);

// I2C mocks
void mock_i2c_expect_init(uint8_t port, uint32_t freq);
void mock_i2c_expect_write(uint8_t addr, const uint8_t* data, size_t len);
void mock_i2c_expect_write_command(uint8_t cmd);
void mock_i2c_expect_write_sequence(void);
void mock_i2c_set_init_result(bool success);
bool mock_i2c_verify_expectations(void);

// GPIO mocks
void mock_gpio_set_state(uint8_t pin, bool state);
bool mock_gpio_get_state(uint8_t pin);
void mock_gpio_expect_init(uint8_t pin);

// ADC mocks
void mock_adc_set_value(uint16_t value);
uint16_t mock_adc_get_last_read(void);

// Timer mocks
void mock_timer_advance(uint32_t ms);
uint32_t mock_timer_get_ticks(void);

// Mock Pico SDK functions when testing
#ifdef PICO_TESTING

// I2C functions
static inline void i2c_init(void* i2c, uint32_t baudrate) {
    mock_hw_state.actual_calls++;
    if (mock_hw_state.i2c_should_fail) return;
    mock_hw_state.i2c_initialized = true;
}

static inline int i2c_write_blocking(void* i2c, uint8_t addr, const uint8_t* src, size_t len, bool nostop) {
    mock_hw_state.actual_calls++;
    return mock_hw_state.i2c_should_fail ? -1 : len;
}

// GPIO functions
static inline void gpio_init(uint8_t pin) {
    mock_hw_state.actual_calls++;
}

static inline void gpio_set_dir(uint8_t pin, bool out) {
    // Mock implementation
}

static inline void gpio_put(uint8_t pin, bool value) {
    if (value) {
        mock_hw_state.gpio_states |= (1 << pin);
    } else {
        mock_hw_state.gpio_states &= ~(1 << pin);
    }
}

static inline bool gpio_get(uint8_t pin) {
    return (mock_hw_state.gpio_states >> pin) & 1;
}

// ADC functions
static inline void adc_init(void) {
    mock_hw_state.adc_initialized = true;
}

static inline uint16_t adc_read(void) {
    return mock_hw_state.adc_value;
}

// Time functions
static inline uint32_t time_us_32(void) {
    return mock_timer_get_ticks() * 1000;
}

static inline void sleep_ms(uint32_t ms) {
    mock_timer_advance(ms);
}

// FreeRTOS mocks
static inline uint32_t xTaskGetTickCount(void) {
    return mock_timer_get_ticks();
}

static inline void vTaskDelay(uint32_t ticks) {
    mock_timer_advance(ticks);
}

#endif // PICO_TESTING

#endif // MOCK_HARDWARE_H
