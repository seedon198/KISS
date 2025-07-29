/**
 * @file display.h
 * @brief OLED display driver for ultra-wide banner-style display (240×64)
 * @author KISS Fuzzer Team
 * @date 2025
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include "kiss_fuzzer.h"

// Display specifications
#define DISPLAY_WIDTH   240
#define DISPLAY_HEIGHT  64
#define DISPLAY_ADDR    0x3C

// Font definitions
#define FONT_WIDTH      6
#define FONT_HEIGHT     8
#define MAX_CHARS_PER_LINE (DISPLAY_WIDTH / FONT_WIDTH)
#define MAX_LINES       (DISPLAY_HEIGHT / FONT_HEIGHT)

/**
 * @brief Initialize OLED display
 * @return true if initialization successful, false otherwise
 */
bool display_init(void);

/**
 * @brief Clear the entire display
 * @return void
 */
void display_clear(void);

/**
 * @brief Write text to display at specified position
 * @param x X coordinate (0-239)
 * @param y Y coordinate (0-63)
 * @param text Text string to display
 * @return void
 */
void display_write_text(uint16_t x, uint8_t y, const char* text);

/**
 * @brief Write a single line of text (banner-style)
 * @param line Line number (0-7)
 * @param text Text string to display
 * @param scroll Enable horizontal scrolling if text is too long
 * @return void
 */
void display_write_line(uint8_t line, const char* text, bool scroll);

/**
 * @brief Update status line with system information
 * @param status Status string to display
 * @return void
 */
void display_update_status(const char* status);

/**
 * @brief Display battery percentage as progress bar
 * @param percent Battery percentage (0-100)
 * @param charging True if charging
 * @return void
 */
void display_battery_status(uint8_t percent, bool charging);

/**
 * @brief Show menu items
 * @param items Array of menu item strings
 * @param item_count Number of menu items
 * @param selected_item Currently selected item index
 * @return void
 */
void display_menu(const char** items, uint8_t item_count, uint8_t selected_item);

/**
 * @brief Refresh display buffer to screen
 * @return void
 */
void display_refresh(void);

/**
 * @brief Set display brightness
 * @param brightness Brightness level (0-255)
 * @return void
 */
void display_set_brightness(uint8_t brightness);

#endif // DISPLAY_H
