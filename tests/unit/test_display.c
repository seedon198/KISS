/**
 * @file test_display.c
 * @brief Unit tests for display module
 * @author KISS Fuzzer Team
 * @date 2025
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "unity.h"
#include "../mocks/mock_hardware.h"

// Include the module under test
#define PICO_TESTING
#include "../../src/display.h"
#include "../../src/display.c"

void setUp(void) {
    mock_hardware_reset();
    display_reset_state();
}

void tearDown(void) {
    // Cleanup after each test
}

void test_display_init_success(void) {
    // Setup mock expectations
    mock_i2c_expect_init(DISPLAY_I2C_PORT, DISPLAY_I2C_FREQ);
    mock_i2c_expect_write_sequence(); // Expect initialization sequence
    
    // Test initialization
    display_result_t result = display_init();
    
    TEST_ASSERT_EQUAL(DISPLAY_OK, result);
    TEST_ASSERT_TRUE(mock_i2c_verify_expectations());
}

void test_display_init_failure(void) {
    // Setup mock to fail I2C init
    mock_i2c_set_init_result(false);
    
    // Test initialization failure
    display_result_t result = display_init();
    
    TEST_ASSERT_EQUAL(DISPLAY_ERROR_I2C, result);
}

void test_display_clear(void) {
    // Initialize display first
    test_display_init_success();
    
    // Test clear function
    display_result_t result = display_clear();
    
    TEST_ASSERT_EQUAL(DISPLAY_OK, result);
    
    // Verify buffer is cleared
    uint8_t* buffer = display_get_buffer();
    for (int i = 0; i < DISPLAY_BUFFER_SIZE; i++) {
        TEST_ASSERT_EQUAL(0x00, buffer[i]);
    }
}

void test_display_set_line_normal(void) {
    test_display_init_success();
    
    const char* test_text = "Test Message";
    display_result_t result = display_set_line(test_text);
    
    TEST_ASSERT_EQUAL(DISPLAY_OK, result);
    
    // Verify text was rendered to buffer
    uint8_t* buffer = display_get_buffer();
    TEST_ASSERT_NOT_EQUAL(0x00, buffer[0]); // Should have some pixels set
}

void test_display_set_line_null(void) {
    test_display_init_success();
    
    display_result_t result = display_set_line(NULL);
    
    TEST_ASSERT_EQUAL(DISPLAY_ERROR_INVALID_PARAM, result);
}

void test_display_set_line_too_long(void) {
    test_display_init_success();
    
    char long_text[256];
    memset(long_text, 'A', sizeof(long_text) - 1);
    long_text[sizeof(long_text) - 1] = '\0';
    
    display_result_t result = display_set_line(long_text);
    
    // Should truncate gracefully, not fail
    TEST_ASSERT_EQUAL(DISPLAY_OK, result);
}

void test_display_update(void) {
    test_display_init_success();
    
    // Expect I2C write for display update
    mock_i2c_expect_write(DISPLAY_I2C_ADDR, NULL, DISPLAY_BUFFER_SIZE + 1);
    
    display_result_t result = display_update();
    
    TEST_ASSERT_EQUAL(DISPLAY_OK, result);
    TEST_ASSERT_TRUE(mock_i2c_verify_expectations());
}

void test_display_set_brightness(void) {
    test_display_init_success();
    
    uint8_t brightness = 128;
    
    // Expect I2C command for brightness
    mock_i2c_expect_write_command(SSD1306_SETCONTRAST);
    mock_i2c_expect_write_command(brightness);
    
    display_result_t result = display_set_brightness(brightness);
    
    TEST_ASSERT_EQUAL(DISPLAY_OK, result);
    TEST_ASSERT_TRUE(mock_i2c_verify_expectations());
}

void test_display_scroll_text_long(void) {
    test_display_init_success();
    
    const char* long_text = "This is a very long message that should scroll across the banner display";
    
    display_result_t result = display_set_scrolling_text(long_text, 100); // 100ms per step
    
    TEST_ASSERT_EQUAL(DISPLAY_OK, result);
    
    // Simulate some scroll steps
    for (int i = 0; i < 10; i++) {
        display_scroll_step();
        // Verify different content each step
    }
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_display_init_success);
    RUN_TEST(test_display_init_failure);
    RUN_TEST(test_display_clear);
    RUN_TEST(test_display_set_line_normal);
    RUN_TEST(test_display_set_line_null);
    RUN_TEST(test_display_set_line_too_long);
    RUN_TEST(test_display_update);
    RUN_TEST(test_display_set_brightness);
    RUN_TEST(test_display_scroll_text_long);
    
    return UNITY_END();
}
