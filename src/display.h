/**
 * @file display.h
 * @brief Simple OLED display driver for KISS Fuzzer
 * @author KISS Fuzzer Team
 * @date 2025
 * @version 0.2.0
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// Hardware configuration
#define DISPLAY_I2C_PORT     i2c0
#define DISPLAY_SDA_PIN      8
#define DISPLAY_SCL_PIN      9
#define DISPLAY_I2C_ADDR     0x3C
#define DISPLAY_I2C_FREQ     400000

// Display dimensions (assuming SSD1306 128x64)
#define DISPLAY_WIDTH        128
#define DISPLAY_HEIGHT       64
#define DISPLAY_PAGES        8

/**
 * @brief Initialize the OLED display
 * @return true if successful, false otherwise
 */
bool display_init(void);

/**
 * @brief Clear the entire display buffer
 */
void display_clear(void);

/**
 * @brief Update the physical display from buffer
 */
void display_update(void);

/**
 * @brief Print text at specified position
 * @param x X coordinate (0-127)
 * @param y Y page (0-7, each page is 8 pixels tall)
 * @param text Text to display
 */
void display_print(uint8_t x, uint8_t y, const char* text);

/**
 * @brief Print centered text on a page
 * @param y Y page (0-7)
 * @param text Text to display
 */
void display_print_centered(uint8_t y, const char* text);

/**
 * @brief Show system status
 * @param status Status message
 */
void display_show_status(const char* status);

/**
 * @brief Test display functionality
 */
void display_test(void);

#endif // DISPLAY_H
