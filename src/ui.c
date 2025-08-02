/**
 * @file ui.c
 * @brief Basic UI and menu system implementation
 * @author KISS Fuzzer Team
 * @date 2025
 * @version 0.4.0
 */

#include "ui.h"

// Forward declarations
static void ui_handle_menu_input(input_event_t event);
static void ui_render_menu(void);

// Static variables for UI state
static bool ui_ready = false;
static ui_state_t current_state = UI_STATE_SPLASH;
static ui_menu_t* current_menu = NULL;
static ui_menu_t main_menu = {0};
static char status_message[UI_MAX_STATUS_LEN] = {0};
static uint32_t status_timeout = 0;
static uint32_t status_start_time = 0;

/**
 * @brief Initialize the UI system
 * @return true if successful, false otherwise
 */
bool ui_init(void) {
    printf("Initializing UI system...\n");
    
    // Initialize dependencies first
    if (!display_init()) {
        printf("UI init failed: display initialization failed\n");
        return false;
    }
    
    if (!input_init()) {
        printf("UI init failed: input initialization failed\n");
        return false;
    }
    
    if (!power_init()) {
        printf("UI init failed: power initialization failed\n");
        return false;
    }
    
    // Initialize main menu
    ui_init_main_menu();
    
    // Show splash screen
    ui_set_state(UI_STATE_SPLASH);
    ui_show_status("KISS Fuzzer v0.4.0", 2000);
    
    ui_ready = true;
    printf("UI system initialized successfully\n");
    return true;
}

/**
 * @brief Main UI loop - processes input and updates display
 */
void ui_update(void) {
    if (!ui_ready) return;
    
    // Update power status
    power_update();
    
    // Check status timeout
    if (status_timeout > 0) {
        uint32_t elapsed = to_ms_since_boot(get_absolute_time()) - status_start_time;
        if (elapsed >= status_timeout) {
            status_timeout = 0;
            if (current_state == UI_STATE_STATUS) {
                ui_set_state(UI_STATE_MAIN_MENU);
            }
        }
    }
    
    // Get input event (non-blocking)
    input_event_t event = input_get_event(50);
    
    // Handle input based on current state
    switch (current_state) {
        case UI_STATE_SPLASH:
            if (event != INPUT_NONE) {
                ui_set_state(UI_STATE_MAIN_MENU);
            }
            break;
            
        case UI_STATE_MAIN_MENU:
        case UI_STATE_SUBMENU:
            ui_handle_menu_input(event);
            break;
            
        case UI_STATE_ACTION:
            if (event == INPUT_BACK) {
                ui_set_state(UI_STATE_MAIN_MENU);
            }
            break;
            
        case UI_STATE_STATUS:
            if (event == INPUT_BACK || event == INPUT_OK) {
                ui_set_state(UI_STATE_MAIN_MENU);
            }
            break;
    }
}

/**
 * @brief Handle input for menu navigation
 * @param event Input event
 */
static void ui_handle_menu_input(input_event_t event) {
    if (!current_menu) return;
    
    switch (event) {
        case INPUT_UP:
            if (current_menu->selected_item > 0) {
                current_menu->selected_item--;
                ui_render_menu();
            }
            break;
            
        case INPUT_DOWN:
            if (current_menu->selected_item < current_menu->item_count - 1) {
                current_menu->selected_item++;
                ui_render_menu();
            }
            break;
            
        case INPUT_OK:
            {
                ui_menu_item_t* item = &current_menu->items[current_menu->selected_item];
                if (item->enabled && item->callback) {
                    printf("Executing menu item: %s\n", item->name);
                    item->callback();
                } else {
                    ui_show_status("Not available", 1000);
                }
            }
            break;
            
        case INPUT_BACK:
            ui_set_state(UI_STATE_MAIN_MENU);
            break;
            
        default:
            break;
    }
}

/**
 * @brief Render current menu to display
 */
static void ui_render_menu(void) {
    if (!current_menu) return;
    
    display_clear();
    
    // Show menu title with power status
    char title_with_power[32];
    char power_status[16];
    power_get_status_string(power_status, sizeof(power_status));
    snprintf(title_with_power, sizeof(title_with_power), "%s [%s]", current_menu->title, power_status);
    display_print_centered(0, title_with_power);
    
    // Show menu items
    for (uint8_t i = 0; i < current_menu->item_count && i < 6; i++) {
        char line[32];
        ui_menu_item_t* item = &current_menu->items[i];
        
        if (i == current_menu->selected_item) {
            snprintf(line, sizeof(line), "> %s", item->name);
        } else {
            snprintf(line, sizeof(line), "  %s", item->name);
        }
        
        if (!item->enabled) {
            // Add visual indicator for disabled items
            strcat(line, " (-)");
        }
        
        display_print(0, i + 2, line);
    }
    
    display_update();
}

/**
 * @brief Set current UI state
 * @param state New UI state
 */
void ui_set_state(ui_state_t state) {
    printf("UI state change: %d -> %d\n", current_state, state);
    current_state = state;
    
    switch (state) {
        case UI_STATE_SPLASH:
            display_clear();
            display_print_centered(3, "KISS Fuzzer");
            display_print_centered(4, "v0.4.0");
            display_update();
            break;
            
        case UI_STATE_MAIN_MENU:
            current_menu = &main_menu;
            ui_render_menu();
            break;
            
        case UI_STATE_STATUS:
            display_clear();
            display_print_centered(3, status_message);
            display_update();
            break;
            
        default:
            break;
    }
}

/**
 * @brief Get current UI state
 * @return Current UI state
 */
ui_state_t ui_get_state(void) {
    return current_state;
}

/**
 * @brief Show a status message
 * @param message Status message to display
 * @param timeout_ms Timeout in milliseconds (0 = permanent)
 */
void ui_show_status(const char* message, uint32_t timeout_ms) {
    strncpy(status_message, message, UI_MAX_STATUS_LEN - 1);
    status_message[UI_MAX_STATUS_LEN - 1] = '\0';
    
    status_timeout = timeout_ms;
    status_start_time = to_ms_since_boot(get_absolute_time());
    
    if (timeout_ms > 0) {
        ui_set_state(UI_STATE_STATUS);
    }
    
    printf("Status: %s (timeout: %lu ms)\n", message, timeout_ms);
}

/**
 * @brief Show a menu
 * @param menu Pointer to menu structure
 */
void ui_show_menu(ui_menu_t* menu) {
    current_menu = menu;
    ui_render_menu();
}

/**
 * @brief Add item to menu
 * @param menu Pointer to menu structure
 * @param name Item name
 * @param callback Callback function (can be NULL)
 * @param enabled Whether item is enabled
 * @return true if added successfully
 */
bool ui_menu_add_item(ui_menu_t* menu, const char* name, ui_menu_callback_t callback, bool enabled) {
    if (!menu || menu->item_count >= UI_MAX_MENU_ITEMS) {
        return false;
    }
    
    ui_menu_item_t* item = &menu->items[menu->item_count];
    strncpy(item->name, name, UI_MAX_ITEM_NAME_LEN - 1);
    item->name[UI_MAX_ITEM_NAME_LEN - 1] = '\0';
    item->callback = callback;
    item->enabled = enabled;
    
    menu->item_count++;
    return true;
}

/**
 * @brief Get main menu
 * @return Pointer to main menu structure
 */
ui_menu_t* ui_get_main_menu(void) {
    return &main_menu;
}

/**
 * @brief Initialize main menu with default items
 */
void ui_init_main_menu(void) {
    memset(&main_menu, 0, sizeof(main_menu));
    strcpy(main_menu.title, "Main Menu");
    
    ui_menu_add_item(&main_menu, "JTAG Scan", ui_callback_jtag_scan, false);
    ui_menu_add_item(&main_menu, "System Info", ui_callback_system_info, true);
    ui_menu_add_item(&main_menu, "Power Info", ui_callback_power_info, true);
    ui_menu_add_item(&main_menu, "Input Test", ui_callback_input_test, true);
    ui_menu_add_item(&main_menu, "Display Test", ui_callback_display_test, true);
    ui_menu_add_item(&main_menu, "Settings", ui_callback_settings, false);
    
    main_menu.selected_item = 0;
}

// Menu callback implementations
void ui_callback_jtag_scan(void) {
    ui_show_status("JTAG scan not implemented", 2000);
}

void ui_callback_system_info(void) {
    ui_set_state(UI_STATE_ACTION);
    display_clear();
    display_print(0, 0, "System Info");
    display_print(0, 1, "KISS Fuzzer v0.5.0");
    display_print(0, 2, "Display: OK");
    display_print(0, 3, "Input: OK");
    display_print(0, 4, "Power: OK");
    display_print(0, 5, "Memory: Available");
    display_print(0, 6, "Press BACK to exit");
    display_update();
}

void ui_callback_power_info(void) {
    ui_set_state(UI_STATE_ACTION);
    display_clear();
    display_print(0, 0, "Power Information");
    
    power_status_t status = power_get_status();
    char line[32];
    
    snprintf(line, sizeof(line), "Battery: %dmV", status.battery_mv);
    display_print(0, 1, line);
    
    snprintf(line, sizeof(line), "Percent: %d%%", status.battery_percent);
    display_print(0, 2, line);
    
    snprintf(line, sizeof(line), "USB: %s", status.usb_connected ? "Connected" : "Disconnected");
    display_print(0, 3, line);
    
    snprintf(line, sizeof(line), "Charging: %s", status.is_charging ? "Yes" : "No");
    display_print(0, 4, line);
    
    snprintf(line, sizeof(line), "State: %s", power_state_name(power_get_state()));
    display_print(0, 5, line);
    
    display_print(0, 6, "Press BACK to exit");
    display_update();
}

void ui_callback_input_test(void) {
    ui_set_state(UI_STATE_ACTION);
    display_clear();
    display_print(0, 0, "Input Test Mode");
    display_print(0, 1, "Press any button:");
    display_print(0, 6, "BACK to exit");
    display_update();
    
    // Simple input test loop
    while (ui_get_state() == UI_STATE_ACTION) {
        input_event_t event = input_get_event(100);
        if (event != INPUT_NONE) {
            if (event == INPUT_BACK) {
                break;
            }
            
            char msg[32];
            snprintf(msg, sizeof(msg), "Pressed: %s", input_event_name(event));
            display_print(0, 3, msg);
            display_update();
        }
    }
    
    ui_set_state(UI_STATE_MAIN_MENU);
}

void ui_callback_display_test(void) {
    ui_show_status("Running display test", 1000);
    display_test();
    ui_show_status("Display test complete", 2000);
}

void ui_callback_settings(void) {
    ui_show_status("Settings not implemented", 2000);
}
