/**
 * @file display.c
 * @brief Simple OLED display driver implementation
 * @author KISS Fuzzer Team  
 * @date 2025
 * @version 0.2.1
 */

#include "display.h"

// Forward declarations
static void display_draw_char(uint8_t x, uint8_t y, char c);

// Static flag to track initialization
static bool display_ready = false;

// Display buffer for SSD1306 - 128x64 pixels = 1024 bytes
static uint8_t display_buffer[DISPLAY_WIDTH * DISPLAY_HEIGHT / 8];

// Simple 5x8 bitmap font for ASCII characters 32-126
// Each character is 5 bytes wide, 8 pixels tall
static const uint8_t font_5x8[][5] = {
    {0x00, 0x00, 0x00, 0x00, 0x00}, // ' ' (32)
    {0x00, 0x00, 0x5F, 0x00, 0x00}, // '!' (33)
    {0x00, 0x07, 0x00, 0x07, 0x00}, // '"' (34)
    {0x14, 0x7F, 0x14, 0x7F, 0x14}, // '#' (35)
    {0x24, 0x2A, 0x7F, 0x2A, 0x12}, // '$' (36)
    {0x23, 0x13, 0x08, 0x64, 0x62}, // '%' (37)
    {0x36, 0x49, 0x55, 0x22, 0x50}, // '&' (38)
    {0x00, 0x05, 0x03, 0x00, 0x00}, // '\'' (39)
    {0x00, 0x1C, 0x22, 0x41, 0x00}, // '(' (40)
    {0x00, 0x41, 0x22, 0x1C, 0x00}, // ')' (41)
    {0x08, 0x2A, 0x1C, 0x2A, 0x08}, // '*' (42)
    {0x08, 0x08, 0x3E, 0x08, 0x08}, // '+' (43)
    {0x00, 0x50, 0x30, 0x00, 0x00}, // ',' (44)
    {0x08, 0x08, 0x08, 0x08, 0x08}, // '-' (45)
    {0x00, 0x60, 0x60, 0x00, 0x00}, // '.' (46)
    {0x20, 0x10, 0x08, 0x04, 0x02}, // '/' (47)
    {0x3E, 0x51, 0x49, 0x45, 0x3E}, // '0' (48)
    {0x00, 0x42, 0x7F, 0x40, 0x00}, // '1' (49)
    {0x42, 0x61, 0x51, 0x49, 0x46}, // '2' (50)
    {0x21, 0x41, 0x45, 0x4B, 0x31}, // '3' (51)
    {0x18, 0x14, 0x12, 0x7F, 0x10}, // '4' (52)
    {0x27, 0x45, 0x45, 0x45, 0x39}, // '5' (53)
    {0x3C, 0x4A, 0x49, 0x49, 0x30}, // '6' (54)
    {0x01, 0x71, 0x09, 0x05, 0x03}, // '7' (55)
    {0x36, 0x49, 0x49, 0x49, 0x36}, // '8' (56)
    {0x06, 0x49, 0x49, 0x29, 0x1E}, // '9' (57)
    {0x00, 0x36, 0x36, 0x00, 0x00}, // ':' (58)
    {0x00, 0x56, 0x36, 0x00, 0x00}, // ';' (59)
    {0x00, 0x08, 0x14, 0x22, 0x41}, // '<' (60)
    {0x14, 0x14, 0x14, 0x14, 0x14}, // '=' (61)
    {0x41, 0x22, 0x14, 0x08, 0x00}, // '>' (62)
    {0x02, 0x01, 0x51, 0x09, 0x06}, // '?' (63)
    {0x32, 0x49, 0x79, 0x41, 0x3E}, // '@' (64)
    {0x7E, 0x11, 0x11, 0x11, 0x7E}, // 'A' (65)
    {0x7F, 0x49, 0x49, 0x49, 0x36}, // 'B' (66)
    {0x3E, 0x41, 0x41, 0x41, 0x22}, // 'C' (67)
    {0x7F, 0x41, 0x41, 0x22, 0x1C}, // 'D' (68)
    {0x7F, 0x49, 0x49, 0x49, 0x41}, // 'E' (69)
    {0x7F, 0x09, 0x09, 0x01, 0x01}, // 'F' (70)
    {0x3E, 0x41, 0x41, 0x51, 0x32}, // 'G' (71)
    {0x7F, 0x08, 0x08, 0x08, 0x7F}, // 'H' (72)
    {0x00, 0x41, 0x7F, 0x41, 0x00}, // 'I' (73)
    {0x20, 0x40, 0x41, 0x3F, 0x01}, // 'J' (74)
    {0x7F, 0x08, 0x14, 0x22, 0x41}, // 'K' (75)
    {0x7F, 0x40, 0x40, 0x40, 0x40}, // 'L' (76)
    {0x7F, 0x02, 0x04, 0x02, 0x7F}, // 'M' (77)
    {0x7F, 0x04, 0x08, 0x10, 0x7F}, // 'N' (78)
    {0x3E, 0x41, 0x41, 0x41, 0x3E}, // 'O' (79)
    {0x7F, 0x09, 0x09, 0x09, 0x06}, // 'P' (80)
    {0x3E, 0x41, 0x51, 0x21, 0x5E}, // 'Q' (81)
    {0x7F, 0x09, 0x19, 0x29, 0x46}, // 'R' (82)
    {0x46, 0x49, 0x49, 0x49, 0x31}, // 'S' (83)
    {0x01, 0x01, 0x7F, 0x01, 0x01}, // 'T' (84)
    {0x3F, 0x40, 0x40, 0x40, 0x3F}, // 'U' (85)
    {0x1F, 0x20, 0x40, 0x20, 0x1F}, // 'V' (86)
    {0x7F, 0x20, 0x18, 0x20, 0x7F}, // 'W' (87)
    {0x63, 0x14, 0x08, 0x14, 0x63}, // 'X' (88)
    {0x03, 0x04, 0x78, 0x04, 0x03}, // 'Y' (89)
    {0x61, 0x51, 0x49, 0x45, 0x43}, // 'Z' (90)
    {0x00, 0x00, 0x7F, 0x41, 0x41}, // '[' (91)
    {0x02, 0x04, 0x08, 0x10, 0x20}, // '\' (92)
    {0x41, 0x41, 0x7F, 0x00, 0x00}, // ']' (93)
    {0x04, 0x02, 0x01, 0x02, 0x04}, // '^' (94)
    {0x40, 0x40, 0x40, 0x40, 0x40}, // '_' (95)
    {0x00, 0x01, 0x02, 0x04, 0x00}, // '`' (96)
    {0x20, 0x54, 0x54, 0x54, 0x78}, // 'a' (97)
    {0x7F, 0x48, 0x44, 0x44, 0x38}, // 'b' (98)
    {0x38, 0x44, 0x44, 0x44, 0x20}, // 'c' (99)
    {0x38, 0x44, 0x44, 0x48, 0x7F}, // 'd' (100)
    {0x38, 0x54, 0x54, 0x54, 0x18}, // 'e' (101)
    {0x08, 0x7E, 0x09, 0x01, 0x02}, // 'f' (102)
    {0x08, 0x14, 0x54, 0x54, 0x3C}, // 'g' (103)
    {0x7F, 0x08, 0x04, 0x04, 0x78}, // 'h' (104)
    {0x00, 0x44, 0x7D, 0x40, 0x00}, // 'i' (105)
    {0x20, 0x40, 0x44, 0x3D, 0x00}, // 'j' (106)
    {0x00, 0x7F, 0x10, 0x28, 0x44}, // 'k' (107)
    {0x00, 0x41, 0x7F, 0x40, 0x00}, // 'l' (108)
    {0x7C, 0x04, 0x18, 0x04, 0x78}, // 'm' (109)
    {0x7C, 0x08, 0x04, 0x04, 0x78}, // 'n' (110)
    {0x38, 0x44, 0x44, 0x44, 0x38}, // 'o' (111)
    {0x7C, 0x14, 0x14, 0x14, 0x08}, // 'p' (112)
    {0x08, 0x14, 0x14, 0x18, 0x7C}, // 'q' (113)
    {0x7C, 0x08, 0x04, 0x04, 0x08}, // 'r' (114)
    {0x48, 0x54, 0x54, 0x54, 0x20}, // 's' (115)
    {0x04, 0x3F, 0x44, 0x40, 0x20}, // 't' (116)
    {0x3C, 0x40, 0x40, 0x20, 0x7C}, // 'u' (117)
    {0x1C, 0x20, 0x40, 0x20, 0x1C}, // 'v' (118)
    {0x3C, 0x40, 0x30, 0x40, 0x3C}, // 'w' (119)
    {0x44, 0x28, 0x10, 0x28, 0x44}, // 'x' (120)
    {0x0C, 0x50, 0x50, 0x50, 0x3C}, // 'y' (121)
    {0x44, 0x64, 0x54, 0x4C, 0x44}, // 'z' (122)
    {0x00, 0x08, 0x36, 0x41, 0x00}, // '{' (123)
    {0x00, 0x00, 0x7F, 0x00, 0x00}, // '|' (124)
    {0x00, 0x41, 0x36, 0x08, 0x00}, // '}' (125)
    {0x08, 0x04, 0x08, 0x10, 0x08}, // '~' (126)
};

#define FONT_CHAR_WIDTH  6  // 5 pixels + 1 spacing
#define FONT_CHAR_HEIGHT 8

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
    
    uint8_t pos_x = x;
    uint8_t pos_y = y * 8; // Convert page to pixel Y coordinate
    
    while (*text && pos_x < DISPLAY_WIDTH - 5) {
        display_draw_char(pos_x, pos_y, *text);
        pos_x += FONT_CHAR_WIDTH;
        text++;
    }
    
    printf("Display print (%d,%d): rendered text\n", x, y);
}

/**
 * @brief Print centered text on a page
 * @param y Y page (0-7)
 * @param text Text to display
 */
void display_print_centered(uint8_t y, const char* text) {
    if (!display_ready) return;
    
    uint8_t text_width = strlen(text) * FONT_CHAR_WIDTH;
    uint8_t x = (DISPLAY_WIDTH - text_width) / 2;
    
    display_print(x / FONT_CHAR_WIDTH, y, text);
    
    printf("Display centered (%d): %s\n", y, text);
}

/**
 * @brief Show system status
 * @param status Status message
 */
void display_show_status(const char* status) {
    if (!display_ready) return;
    
    // Clear display and show title + status
    display_clear();
    display_print_centered(0, "KISS FUZZER");
    display_print_centered(2, status);
    display_update();
    
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

/**
 * @brief Set a pixel in the display buffer
 * @param x X coordinate (0-127)
 * @param y Y coordinate (0-63)
 * @param on true to turn pixel on, false to turn off
 */
static void display_set_pixel(uint8_t x, uint8_t y, bool on) {
    if (x >= DISPLAY_WIDTH || y >= DISPLAY_HEIGHT) return;
    
    uint16_t byte_index = x + (y / 8) * DISPLAY_WIDTH;
    uint8_t bit_index = y % 8;
    
    if (on) {
        display_buffer[byte_index] |= (1 << bit_index);
    } else {
        display_buffer[byte_index] &= ~(1 << bit_index);
    }
}

/**
 * @brief Draw a character at specified position
 * @param x X coordinate (0-127)
 * @param y Y coordinate (0-63)
 * @param c Character to draw
 */
static void display_draw_char(uint8_t x, uint8_t y, char c) {
    if (c < 32 || c > 126) c = '?'; // Replace unknown chars with '?'
    
    const uint8_t* char_data = font_5x8[c - 32];
    
    for (int col = 0; col < 5; col++) {
        uint8_t column = char_data[col];
        for (int row = 0; row < 8; row++) {
            if (column & (1 << row)) {
                display_set_pixel(x + col, y + row, true);
            }
        }
    }
}
