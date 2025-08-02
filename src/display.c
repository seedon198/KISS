/**
 * @file display.c
 * @brief Simple OLED display driver implementation
 * @author KISS Fuzzer Team  
 * @date 2025
 * @version 0.2.0
 */

#include "display.h"

// Static flag to track initialization
static bool display_ready = false;

// Display buffer for SSD1306 - 128x64 pixels = 1024 bytes
static uint8_t display_buffer[DISPLAY_WIDTH * DISPLAY_HEIGHT / 8];

/**
 * @brief Send command to OLED controller
 * @param cmd Command byte
 */
static void oled_send_command(uint8_t cmd) {
    uint8_t buffer[2] = {0x00, cmd}; // Control byte (0x00) + command
    i2c_write_blocking(DISPLAY_I2C_PORT, DISPLAY_I2C_ADDR, buffer, 2, false);
}

/**
 * @brief Send data to OLED controller  
 * @param data Data buffer
 * @param length Number of bytes to send
 */
static void oled_send_data(const uint8_t* data, size_t length) {
    uint8_t control_byte = 0x40; // Data control byte
    i2c_write_blocking(DISPLAY_I2C_PORT, DISPLAY_I2C_ADDR, &control_byte, 1, true);
    i2c_write_blocking(DISPLAY_I2C_PORT, DISPLAY_I2C_ADDR, data, length, false);
}

/**
 * @brief Initialize the OLED display
 * @return true if successful, false otherwise
 */
bool display_init(void) {
    // Initialize I2C
    i2c_init(DISPLAY_I2C_PORT, DISPLAY_I2C_FREQ);
    gpio_set_function(DISPLAY_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(DISPLAY_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(DISPLAY_SDA_PIN);
    gpio_pull_up(DISPLAY_SCL_PIN);
    
    // Wait for display to power up
    sleep_ms(100);
    
    // Initialize SSD1306 OLED controller
    oled_send_command(0xAE); // Display off
    oled_send_command(0xD5); // Set display clock divide ratio
    oled_send_command(0x80); // Default ratio
    oled_send_command(0xA8); // Set multiplex ratio
    oled_send_command(0x3F); // 64 lines
    oled_send_command(0xD3); // Set display offset
    oled_send_command(0x00); // No offset
    oled_send_command(0x40); // Set start line
    oled_send_command(0x8D); // Enable charge pump
    oled_send_command(0x14); // Charge pump on
    oled_send_command(0x20); // Set memory addressing mode
    oled_send_command(0x00); // Horizontal addressing mode
    oled_send_command(0xA1); // Set segment re-map
    oled_send_command(0xC8); // Set COM output scan direction
    oled_send_command(0xDA); // Set COM pins hardware configuration
    oled_send_command(0x12); // Alternative COM pin config
    oled_send_command(0x81); // Set contrast
    oled_send_command(0xCF); // High contrast
    oled_send_command(0xD9); // Set pre-charge period
    oled_send_command(0xF1); // Pre-charge period
    oled_send_command(0xDB); // Set VCOMH deselect level
    oled_send_command(0x40); // VCOMH level
    oled_send_command(0xA4); // Entire display on (normal)
    oled_send_command(0xA6); // Set normal display (not inverted)
    oled_send_command(0xAF); // Display on
    
    // Clear display buffer and update
    display_clear();
    display_update();
    
    printf("Display I2C initialized on pins %d/%d\n", DISPLAY_SDA_PIN, DISPLAY_SCL_PIN);
    
    display_ready = true;
    return true;
}

/**
 * @brief Clear the entire display buffer
 */
void display_clear(void) {
    if (!display_ready) return;
    memset(display_buffer, 0, sizeof(display_buffer));
    printf("Display cleared\n");
}

/**
 * @brief Update the physical display from buffer
 */
void display_update(void) {
    if (!display_ready) return;
    
    // Set column address range
    oled_send_command(0x21); // Set column address
    oled_send_command(0x00); // Start column
    oled_send_command(0x7F); // End column (127)
    
    // Set page address range
    oled_send_command(0x22); // Set page address
    oled_send_command(0x00); // Start page
    oled_send_command(0x07); // End page (7)
    
    // Send display buffer
    oled_send_data(display_buffer, sizeof(display_buffer));
    
    printf("Display updated\n");
}

/**
 * @brief Print text at specified position
 * @param x X coordinate (0-127)
 * @param y Y page (0-7, each page is 8 pixels tall)
 * @param text Text to display
 */
void display_print(uint8_t x, uint8_t y, const char* text) {
    if (!display_ready) return;
    printf("Display print (%d,%d): %s\n", x, y, text);
}

/**
 * @brief Print centered text on a page
 * @param y Y page (0-7)
 * @param text Text to display
 */
void display_print_centered(uint8_t y, const char* text) {
    if (!display_ready) return;
    printf("Display centered (%d): %s\n", y, text);
}

/**
 * @brief Show system status
 * @param status Status message
 */
void display_show_status(const char* status) {
    if (!display_ready) return;
    printf("Display status: %s\n", status);
}

/**
 * @brief Test display functionality
 */
void display_test(void) {
    printf("Display test starting...\n");
    
    if (!display_ready) {
        printf("Display not initialized!\n");
        return;
    }
    
    display_show_status("Test Mode");
    sleep_ms(1000);
    
    display_print_centered(2, "KISS FUZZER");
    display_print_centered(3, "v0.2.0");
    sleep_ms(1000);
    
    printf("Display test complete\n");
}
