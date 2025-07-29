/**
 * @file display.c
 * @brief OLED display driver implementation for ultra-wide banner-style display (240×64)
 * @author KISS Fuzzer Team
 * @date 2025
 */

#include "display.h"

// SSD1306 commands
#define SSD1306_SETCONTRAST         0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON        0xA5
#define SSD1306_NORMALDISPLAY       0xA6
#define SSD1306_INVERTDISPLAY       0xA7
#define SSD1306_DISPLAYOFF          0xAE
#define SSD1306_DISPLAYON           0xAF
#define SSD1306_SETDISPLAYOFFSET    0xD3
#define SSD1306_SETCOMPINS          0xDA
#define SSD1306_SETVCOMDETECT       0xDB
#define SSD1306_SETDISPLAYCLOCKDIV  0xD5
#define SSD1306_SETPRECHARGE        0xD9
#define SSD1306_SETMULTIPLEX        0xA8
#define SSD1306_SETLOWCOLUMN        0x00
#define SSD1306_SETHIGHCOLUMN       0x10
#define SSD1306_SETSTARTLINE        0x40
#define SSD1306_MEMORYMODE          0x20
#define SSD1306_COLUMNADDR          0x21
#define SSD1306_PAGEADDR            0x22
#define SSD1306_COMSCANINC          0xC0
#define SSD1306_COMSCANDEC          0xC8
#define SSD1306_SEGREMAP            0xA0
#define SSD1306_CHARGEPUMP          0x8D

// Display buffer
static uint8_t display_buffer[DISPLAY_WIDTH * DISPLAY_HEIGHT / 8];
static bool display_initialized = false;
static uint8_t current_brightness = 0x7F;

// Simple 6x8 font (ASCII 32-127)
static const uint8_t font_6x8[][6] = {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // space
    {0x00, 0x00, 0x5F, 0x00, 0x00, 0x00}, // !
    {0x00, 0x07, 0x00, 0x07, 0x00, 0x00}, // "
    {0x14, 0x7F, 0x14, 0x7F, 0x14, 0x00}, // #
    {0x24, 0x2A, 0x7F, 0x2A, 0x12, 0x00}, // $
    {0x23, 0x13, 0x08, 0x64, 0x62, 0x00}, // %
    {0x36, 0x49, 0x55, 0x22, 0x50, 0x00}, // &
    {0x00, 0x05, 0x03, 0x00, 0x00, 0x00}, // '
    {0x00, 0x1C, 0x22, 0x41, 0x00, 0x00}, // (
    {0x00, 0x41, 0x22, 0x1C, 0x00, 0x00}, // )
    {0x08, 0x2A, 0x1C, 0x2A, 0x08, 0x00}, // *
    {0x08, 0x08, 0x3E, 0x08, 0x08, 0x00}, // +
    {0x00, 0x50, 0x30, 0x00, 0x00, 0x00}, // ,
    {0x08, 0x08, 0x08, 0x08, 0x08, 0x00}, // -
    {0x00, 0x60, 0x60, 0x00, 0x00, 0x00}, // .
    {0x20, 0x10, 0x08, 0x04, 0x02, 0x00}, // /
    {0x3E, 0x51, 0x49, 0x45, 0x3E, 0x00}, // 0
    {0x00, 0x42, 0x7F, 0x40, 0x00, 0x00}, // 1
    {0x42, 0x61, 0x51, 0x49, 0x46, 0x00}, // 2
    {0x21, 0x41, 0x45, 0x4B, 0x31, 0x00}, // 3
    {0x18, 0x14, 0x12, 0x7F, 0x10, 0x00}, // 4
    {0x27, 0x45, 0x45, 0x45, 0x39, 0x00}, // 5
    {0x3C, 0x4A, 0x49, 0x49, 0x30, 0x00}, // 6
    {0x01, 0x71, 0x09, 0x05, 0x03, 0x00}, // 7
    {0x36, 0x49, 0x49, 0x49, 0x36, 0x00}, // 8
    {0x06, 0x49, 0x49, 0x29, 0x1E, 0x00}, // 9
    // ... continue with more characters as needed
};

/**
 * @brief Send command to SSD1306
 * @param cmd Command byte
 * @return void
 */
static void display_command(uint8_t cmd)
{
    uint8_t data[2] = {0x00, cmd}; // Co=0, D/C=0
    i2c_write_blocking(OLED_I2C_INST, DISPLAY_ADDR, data, 2, false);
}

/**
 * @brief Send data to SSD1306
 * @param data Data buffer
 * @param len Data length
 * @return void
 */
static void display_data(const uint8_t* data, size_t len)
{
    uint8_t cmd = 0x40; // Co=0, D/C=1
    i2c_write_blocking(OLED_I2C_INST, DISPLAY_ADDR, &cmd, 1, true);
    i2c_write_blocking(OLED_I2C_INST, DISPLAY_ADDR, data, len, false);
}

/**
 * @brief Initialize OLED display
 * @return true if initialization successful, false otherwise
 */
bool display_init(void)
{
    // Initialize I2C
    i2c_init(OLED_I2C_INST, 400000); // 400kHz
    gpio_set_function(OLED_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(OLED_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(OLED_SDA_PIN);
    gpio_pull_up(OLED_SCL_PIN);
    
    sleep_ms(100); // Allow display to stabilize
    
    // Initialization sequence for SSD1306 240x64
    display_command(SSD1306_DISPLAYOFF);
    display_command(SSD1306_SETDISPLAYCLOCKDIV);
    display_command(0x80);
    display_command(SSD1306_SETMULTIPLEX);
    display_command(0x3F); // 64-1
    display_command(SSD1306_SETDISPLAYOFFSET);
    display_command(0x00);
    display_command(SSD1306_SETSTARTLINE | 0x00);
    display_command(SSD1306_CHARGEPUMP);
    display_command(0x14); // Enable charge pump
    display_command(SSD1306_MEMORYMODE);
    display_command(0x00); // Horizontal addressing mode
    display_command(SSD1306_SEGREMAP | 0x01);
    display_command(SSD1306_COMSCANDEC);
    display_command(SSD1306_SETCOMPINS);
    display_command(0x12);
    display_command(SSD1306_SETCONTRAST);
    display_command(current_brightness);
    display_command(SSD1306_SETPRECHARGE);
    display_command(0xF1);
    display_command(SSD1306_SETVCOMDETECT);
    display_command(0x40);
    display_command(SSD1306_DISPLAYALLON_RESUME);
    display_command(SSD1306_NORMALDISPLAY);
    display_command(SSD1306_DISPLAYON);
    
    // Clear display buffer
    memset(display_buffer, 0, sizeof(display_buffer));
    display_refresh();
    
    display_initialized = true;
    return true;
}

/**
 * @brief Clear the entire display
 * @return void
 */
void display_clear(void)
{
    if (!display_initialized) return;
    
    memset(display_buffer, 0, sizeof(display_buffer));
}

/**
 * @brief Set pixel in display buffer
 * @param x X coordinate (0-239)
 * @param y Y coordinate (0-63)
 * @param color Pixel color (0 or 1)
 * @return void
 */
static void display_set_pixel(uint16_t x, uint8_t y, uint8_t color)
{
    if (x >= DISPLAY_WIDTH || y >= DISPLAY_HEIGHT) return;
    
    uint16_t index = x + (y / 8) * DISPLAY_WIDTH;
    uint8_t bit = y % 8;
    
    if (color) {
        display_buffer[index] |= (1 << bit);
    } else {
        display_buffer[index] &= ~(1 << bit);
    }
}

/**
 * @brief Draw character at specified position
 * @param x X coordinate
 * @param y Y coordinate 
 * @param c Character to draw
 * @return void
 */
static void display_draw_char(uint16_t x, uint8_t y, char c)
{
    if (c < 32 || c > 126) c = 32; // Default to space for unsupported chars
    
    const uint8_t* font_data = font_6x8[c - 32];
    
    for (int col = 0; col < FONT_WIDTH; col++) {
        uint8_t column = font_data[col];
        for (int row = 0; row < FONT_HEIGHT; row++) {
            if (column & (1 << row)) {
                display_set_pixel(x + col, y + row, 1);
            }
        }
    }
}

/**
 * @brief Write text to display at specified position
 * @param x X coordinate (0-239)
 * @param y Y coordinate (0-63)
 * @param text Text string to display
 * @return void
 */
void display_write_text(uint16_t x, uint8_t y, const char* text)
{
    if (!display_initialized || !text) return;
    
    uint16_t cursor_x = x;
    
    while (*text && cursor_x < DISPLAY_WIDTH) {
        display_draw_char(cursor_x, y, *text);
        cursor_x += FONT_WIDTH;
        text++;
    }
}

/**
 * @brief Write a single line of text (banner-style)
 * @param line Line number (0-7)
 * @param text Text string to display
 * @param scroll Enable horizontal scrolling if text is too long
 * @return void
 */
void display_write_line(uint8_t line, const char* text, bool scroll)
{
    if (!display_initialized || !text || line >= MAX_LINES) return;
    
    uint8_t y = line * FONT_HEIGHT;
    
    // Clear the line first
    for (uint16_t x = 0; x < DISPLAY_WIDTH; x++) {
        for (uint8_t row = 0; row < FONT_HEIGHT; row++) {
            display_set_pixel(x, y + row, 0);
        }
    }
    
    // TODO: Implement scrolling for long text
    display_write_text(0, y, text);
}

/**
 * @brief Update status line with system information
 * @param status Status string to display
 * @return void
 */
void display_update_status(const char* status)
{
    if (!display_initialized) return;
    
    xSemaphoreTake(display_mutex, portMAX_DELAY);
    display_write_line(0, status, false);
    display_refresh();
    xSemaphoreGive(display_mutex);
}

/**
 * @brief Display battery percentage as progress bar
 * @param percent Battery percentage (0-100)
 * @param charging True if charging
 * @return void
 */
void display_battery_status(uint8_t percent, bool charging)
{
    if (!display_initialized) return;
    
    // Draw battery icon and percentage on top right
    char battery_str[32];
    snprintf(battery_str, sizeof(battery_str), "%s%d%%", 
             charging ? "CHG " : "BAT ", percent);
    
    // Position on right side of first line
    uint16_t x = DISPLAY_WIDTH - (strlen(battery_str) * FONT_WIDTH);
    display_write_text(x, 0, battery_str);
}

/**
 * @brief Show menu items
 * @param items Array of menu item strings
 * @param item_count Number of menu items
 * @param selected_item Currently selected item index
 * @return void
 */
void display_menu(const char** items, uint8_t item_count, uint8_t selected_item)
{
    if (!display_initialized || !items) return;
    
    display_clear();
    
    // Show title on first line
    display_write_line(0, "KISS Fuzzer Menu", false);
    
    // Show menu items starting from line 2
    for (uint8_t i = 0; i < item_count && i < (MAX_LINES - 2); i++) {
        char line_text[64];
        
        if (i == selected_item) {
            snprintf(line_text, sizeof(line_text), "> %s", items[i]);
        } else {
            snprintf(line_text, sizeof(line_text), "  %s", items[i]);
        }
        
        display_write_line(i + 2, line_text, false);
    }
}

/**
 * @brief Refresh display buffer to screen
 * @return void
 */
void display_refresh(void)
{
    if (!display_initialized) return;
    
    // Set column address range (0 to 239)
    display_command(SSD1306_COLUMNADDR);
    display_command(0);
    display_command(DISPLAY_WIDTH - 1);
    
    // Set page address range (0 to 7 for 64 pixels / 8)
    display_command(SSD1306_PAGEADDR);
    display_command(0);
    display_command((DISPLAY_HEIGHT / 8) - 1);
    
    // Send display buffer
    display_data(display_buffer, sizeof(display_buffer));
}

/**
 * @brief Set display brightness
 * @param brightness Brightness level (0-255)
 * @return void
 */
void display_set_brightness(uint8_t brightness)
{
    if (!display_initialized) return;
    
    current_brightness = brightness;
    display_command(SSD1306_SETCONTRAST);
    display_command(brightness);
}
