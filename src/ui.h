/**
 * @file ui.h
 * @brief Basic UI and menu system for KISS Fuzzer
 * @author KISS Fuzzer Team
 * @date 2025
 * @version 0.4.0
 */

#ifndef UI_H
#define UI_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "pico/stdlib.h"
#include "display.h"
#include "input.h"

// Menu system constants
#define UI_MAX_MENU_ITEMS     8
#define UI_MAX_ITEM_NAME_LEN  20
#define UI_MAX_STATUS_LEN     24

/**
 * @brief Menu item callback function type
 */
typedef void (*ui_menu_callback_t)(void);

/**
 * @brief Menu item structure
 */
typedef struct {
    char name[UI_MAX_ITEM_NAME_LEN];
    ui_menu_callback_t callback;
    bool enabled;
} ui_menu_item_t;

/**
 * @brief Menu structure
 */
typedef struct {
    char title[UI_MAX_ITEM_NAME_LEN];
    ui_menu_item_t items[UI_MAX_MENU_ITEMS];
    uint8_t item_count;
    uint8_t selected_item;
} ui_menu_t;

/**
 * @brief UI state enumeration
 */
typedef enum {
    UI_STATE_SPLASH,
    UI_STATE_MAIN_MENU,
    UI_STATE_SUBMENU,
    UI_STATE_ACTION,
    UI_STATE_STATUS
} ui_state_t;

/**
 * @brief Initialize the UI system
 * @return true if successful, false otherwise
 */
bool ui_init(void);

/**
 * @brief Main UI loop - processes input and updates display
 * Should be called regularly from main loop
 */
void ui_update(void);

/**
 * @brief Set current UI state
 * @param state New UI state
 */
void ui_set_state(ui_state_t state);

/**
 * @brief Get current UI state
 * @return Current UI state
 */
ui_state_t ui_get_state(void);

/**
 * @brief Show a status message
 * @param message Status message to display
 * @param timeout_ms Timeout in milliseconds (0 = permanent)
 */
void ui_show_status(const char* message, uint32_t timeout_ms);

/**
 * @brief Create and show a menu
 * @param menu Pointer to menu structure
 */
void ui_show_menu(ui_menu_t* menu);

/**
 * @brief Add item to menu
 * @param menu Pointer to menu structure
 * @param name Item name
 * @param callback Callback function (can be NULL)
 * @param enabled Whether item is enabled
 * @return true if added successfully
 */
bool ui_menu_add_item(ui_menu_t* menu, const char* name, ui_menu_callback_t callback, bool enabled);

/**
 * @brief Create the main menu
 * @return Pointer to main menu structure
 */
ui_menu_t* ui_get_main_menu(void);

/**
 * @brief Initialize main menu with default items
 */
void ui_init_main_menu(void);

// Menu callback functions
void ui_callback_jtag_scan(void);
void ui_callback_system_info(void);
void ui_callback_input_test(void);
void ui_callback_display_test(void);
void ui_callback_settings(void);

#endif // UI_H
