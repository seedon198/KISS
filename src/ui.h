/**
 * @file ui.h
 * @brief User interface and menu system with 5-way joystick input
 * @author KISS Fuzzer Team
 * @date 2025
 */

#ifndef UI_H
#define UI_H

#include "kiss_fuzzer.h"

// UI event types
typedef enum {
    UI_EVENT_UP,
    UI_EVENT_DOWN,
    UI_EVENT_LEFT,
    UI_EVENT_RIGHT,
    UI_EVENT_OK,
    UI_EVENT_BACK,
    UI_EVENT_TIMEOUT
} ui_event_type_t;

// UI event structure
typedef struct {
    ui_event_type_t type;
    uint32_t timestamp;
} ui_event_t;

// Menu states
typedef enum {
    MENU_MAIN,
    MENU_JTAG_SCAN,
    MENU_JTAG_OPTIONS,
    MENU_GLITCH_TOOL,
    MENU_LOGS,
    MENU_SETTINGS,
    MENU_WIFI_CONFIG
} menu_state_t;

// Menu item structure
typedef struct {
    const char* text;
    menu_state_t next_state;
    void (*action_func)(void);
} menu_item_t;

/**
 * @brief Initialize UI system and input handlers
 * @return void
 */
void ui_init(void);

/**
 * @brief Main UI task function
 * @param pvParameters FreeRTOS task parameters
 * @return void
 */
void ui_task(void* pvParameters);

/**
 * @brief Handle joystick input and generate UI events
 * @return void
 */
void ui_handle_input(void);

/**
 * @brief Process UI event and update menu state
 * @param event UI event to process
 * @return void
 */
void ui_process_event(const ui_event_t* event);

/**
 * @brief Update display with current menu
 * @return void
 */
void ui_update_display(void);

/**
 * @brief Show status message temporarily
 * @param message Status message to display
 * @param duration_ms Duration to show message (milliseconds)
 * @return void
 */
void ui_show_status(const char* message, uint32_t duration_ms);

/**
 * @brief Show confirmation dialog
 * @param message Confirmation message
 * @return true if confirmed, false if cancelled
 */
bool ui_confirm_dialog(const char* message);

/**
 * @brief Show progress bar
 * @param title Progress title
 * @param percent Progress percentage (0-100)
 * @return void
 */
void ui_show_progress(const char* title, uint8_t percent);

/**
 * @brief GPIO interrupt handler for joystick inputs
 * @param gpio GPIO pin number
 * @param events GPIO event mask
 * @return void
 */
void ui_gpio_callback(uint gpio, uint32_t events);

// Menu action functions
void action_jtag_scan(void);
void action_jtag_detect_pins(void);
void action_jtag_read_idcode(void);
void action_memory_dump(void);
void action_glitch_test(void);
void action_view_logs(void);
void action_clear_logs(void);
void action_wifi_enable(void);
void action_settings_menu(void);

#endif // UI_H
