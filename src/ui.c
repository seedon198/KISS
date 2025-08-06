/**
 * @file ui.c
 * @brief Basic UI and menu system implementation
 * @author KISS Fuzzer Team
 * @date 2025
 * @version 0.7.0
 */

#include "ui.h"
#include "storage.h"
#include "fuzz_commands.h"

// Forward declarations
static void ui_handle_menu_input(input_event_t event);
static void ui_handle_command_input(input_event_t event);
static void ui_render_menu(void);

// Static variables for UI state
static bool ui_ready = false;
static ui_state_t current_state = UI_STATE_SPLASH;
static ui_menu_t* current_menu = NULL;
static ui_menu_t main_menu = {0};
static ui_menu_t fuzzing_menu = {0};
static char status_message[UI_MAX_STATUS_LEN] = {0};
static uint32_t status_timeout = 0;
static uint32_t status_start_time = 0;

// Command line predefined commands
static const char* predefined_commands[] = {
    "help",
    "status",
    "discover",
    "scan",
    "fuzz random 100",
    "fuzz sequential 50",
    "boundary",
    "stop",
    NULL
};
static size_t command_index = 0;

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
    
    if (!jtag_init(NULL)) {
        printf("UI init failed: JTAG initialization failed\n");
        return false;
    }
    
    // Initialize storage system (optional - continue without SD card)
    if (storage_init()) {
        printf("Storage system initialized successfully\n");
    } else {
        printf("Storage system initialization failed (continuing without SD)\n");
    }
    
    // Initialize fuzzing command system
    if (!fuzz_commands_init()) {
        printf("UI init: fuzzing commands initialization failed\n");
        return false;
    }
    
    // Initialize main menu
    ui_init_main_menu();
    ui_init_fuzzing_menu();
    
    // Show splash screen
    ui_set_state(UI_STATE_SPLASH);
    ui_show_status("KISS Fuzzer v0.7.0", 2000);
    
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
        case UI_STATE_FUZZING_MENU:
            ui_handle_menu_input(event);
            break;
            
        case UI_STATE_ACTION:
        case UI_STATE_FUZZING_ACTION:
            if (event == INPUT_BACK) {
                ui_set_state(UI_STATE_MAIN_MENU);
            }
            break;
            
        case UI_STATE_COMMAND_LINE:
            ui_handle_command_input(event);
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
            
        case UI_STATE_FUZZING_MENU:
            current_menu = &fuzzing_menu;
            ui_render_menu();
            break;
            
        case UI_STATE_STATUS:
            display_clear();
            display_print_centered(3, status_message);
            display_update();
            break;
            
        case UI_STATE_COMMAND_LINE:
            display_clear();
            display_print(0, 0, "Command Line");
            display_print(0, 1, "Select Command:");
            display_print(0, 2, predefined_commands[command_index]);
            display_print(0, 5, "UP/DOWN: Navigate");
            display_print(0, 6, "OK: Execute");
            display_print(0, 7, "BACK: Exit");
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
    
    ui_menu_add_item(&main_menu, "JTAG Scan", ui_callback_jtag_scan, true);
    ui_menu_add_item(&main_menu, "Fuzzing", ui_callback_fuzzing_menu, true);
    ui_menu_add_item(&main_menu, "System Info", ui_callback_system_info, true);
    ui_menu_add_item(&main_menu, "Power Info", ui_callback_power_info, true);
    ui_menu_add_item(&main_menu, "Storage Info", ui_callback_storage_info, true);
    ui_menu_add_item(&main_menu, "Input Test", ui_callback_input_test, true);
    ui_menu_add_item(&main_menu, "Display Test", ui_callback_display_test, true);
    ui_menu_add_item(&main_menu, "Settings", ui_callback_settings, false);
    
    main_menu.selected_item = 0;
}

// Menu callback implementations
void ui_callback_jtag_scan(void) {
    ui_set_state(UI_STATE_ACTION);
    display_clear();
    display_print(0, 0, "JTAG Scan");
    display_print(0, 1, "Scanning...");
    display_update();
    
    // Perform JTAG scan
    jtag_chain_t chain;
    uint8_t device_count = jtag_scan_chain(&chain);
    
    // Log scan results to storage
    if (storage_is_ready()) {
        char log_data[256];
        snprintf(log_data, sizeof(log_data), 
                "JTAG Scan: %d device(s) found\n", device_count);
        
        if (device_count > 0) {
            char device_info[128];
            snprintf(device_info, sizeof(device_info),
                    "Device 0: ID=0x%08lX, %s %s\n",
                    chain.devices[0].idcode,
                    chain.devices[0].identified ? chain.devices[0].manufacturer : "Unknown",
                    chain.devices[0].identified ? chain.devices[0].device_name : "Device");
            strncat(log_data, device_info, sizeof(log_data) - strlen(log_data) - 1);
        }
        
        storage_log_jtag_scan(log_data, strlen(log_data));
        storage_log_system_event("JTAG scan completed via UI", 6);
    }
    
    display_clear();
    display_print(0, 0, "JTAG Scan Results");
    
    if (device_count > 0) {
        char result_line[32];
        snprintf(result_line, sizeof(result_line), "Found %d device(s)", device_count);
        display_print(0, 1, result_line);
        
        // Show first device info
        if (chain.devices[0].identified) {
            display_print(0, 2, chain.devices[0].manufacturer);
            display_print(0, 3, chain.devices[0].device_name);
        }
        
        char idcode_line[32];
        snprintf(idcode_line, sizeof(idcode_line), "ID: 0x%08lX", chain.devices[0].idcode);
        display_print(0, 4, idcode_line);
    } else {
        display_print(0, 1, "No devices found");
        display_print(0, 2, "Check connections");
    }
    
    display_print(0, 6, "Press BACK to exit");
    display_update();
}

void ui_callback_system_info(void) {
    ui_set_state(UI_STATE_ACTION);
    display_clear();
    display_print(0, 0, "System Info");
    display_print(0, 1, "KISS Fuzzer v0.7.0");
    display_print(0, 2, "Display: OK");
    display_print(0, 3, "Input: OK");
    display_print(0, 4, "Power: OK");
    display_print(0, 5, "JTAG: OK");
    
    char storage_line[32];
    snprintf(storage_line, sizeof(storage_line), "Storage: %s", 
             storage_is_ready() ? "OK" : "No SD");
    display_print(120, 2, storage_line); // Right side of display
    
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

void ui_callback_storage_info(void) {
    ui_set_state(UI_STATE_ACTION);
    display_clear();
    display_print(0, 0, "Storage Information");
    
    if (storage_is_ready()) {
        storage_status_t status = storage_get_status();
        char line[32];
        
        display_print(0, 1, "SD Card: Ready");
        
        snprintf(line, sizeof(line), "Label: %s", status.volume_label);
        display_print(0, 2, line);
        
        snprintf(line, sizeof(line), "Total: %luMB", status.total_size_mb);
        display_print(0, 3, line);
        
        snprintf(line, sizeof(line), "Free: %luMB", status.free_size_mb);
        display_print(0, 4, line);
        
        snprintf(line, sizeof(line), "Files: %lu", status.files_count);
        display_print(0, 5, line);
    } else {
        display_print(0, 1, "SD Card: Not Ready");
        display_print(0, 2, "Insert SD card");
        display_print(0, 3, "and restart");
    }
    
    display_print(0, 6, "Press BACK to exit");
    display_update();
}

void ui_callback_settings(void) {
    ui_show_status("Settings not implemented", 2000);
}

/**
 * @brief Initialize fuzzing menu with fuzzing commands
 */
void ui_init_fuzzing_menu(void) {
    memset(&fuzzing_menu, 0, sizeof(fuzzing_menu));
    strcpy(fuzzing_menu.title, "Fuzzing Menu");
    
    ui_menu_add_item(&fuzzing_menu, "Pin Discovery", ui_callback_fuzz_discovery, true);
    ui_menu_add_item(&fuzzing_menu, "Random Fuzz", ui_callback_fuzz_random, true);
    ui_menu_add_item(&fuzzing_menu, "Boundary Scan", ui_callback_fuzz_boundary, true);
    ui_menu_add_item(&fuzzing_menu, "Quick Scan", ui_callback_fuzz_scan, true);
    ui_menu_add_item(&fuzzing_menu, "Fuzz Status", ui_callback_fuzz_status, true);
    ui_menu_add_item(&fuzzing_menu, "Command Line", ui_callback_command_line, true);
    
    fuzzing_menu.selected_item = 0;
}

/**
 * @brief Show fuzzing menu
 */
void ui_show_fuzzing_menu(void) {
    ui_set_state(UI_STATE_FUZZING_MENU);
}

/**
 * @brief Handle command line input events
 */
static void ui_handle_command_input(input_event_t event) {
    switch (event) {
        case INPUT_UP:
            if (command_index > 0) {
                command_index--;
                ui_set_state(UI_STATE_COMMAND_LINE); // Refresh display
            }
            break;
            
        case INPUT_DOWN:
            if (predefined_commands[command_index + 1] != NULL) {
                command_index++;
                ui_set_state(UI_STATE_COMMAND_LINE); // Refresh display
            }
            break;
            
        case INPUT_OK:
            ui_process_command(predefined_commands[command_index]);
            break;
            
        case INPUT_BACK:
            ui_set_state(UI_STATE_FUZZING_MENU);
            command_index = 0;
            break;
            
        default:
            break;
    }
}

/**
 * @brief Process a command line command
 */
void ui_process_command(const char* command) {
    char response[256] = {0};
    cmd_result_t result = fuzz_commands_process(command, response, sizeof(response));
    
    // Display command result
    ui_set_state(UI_STATE_STATUS);
    display_clear();
    display_print(0, 0, "Command Result:");
    
    switch (result) {
        case CMD_RESULT_SUCCESS:
            display_print(0, 1, "SUCCESS");
            break;
        case CMD_RESULT_ERROR:
            display_print(0, 1, "ERROR");
            break;
        case CMD_RESULT_INVALID_ARGS:
            display_print(0, 1, "INVALID ARGS");
            break;
        case CMD_RESULT_NOT_READY:
            display_print(0, 1, "NOT READY");
            break;
        case CMD_RESULT_BUSY:
            display_print(0, 1, "BUSY");
            break;
    }
    
    // Show response text (truncated to fit display)
    if (strlen(response) > 0) {
        display_print(0, 3, response);
        if (strlen(response) > 24) {
            display_print(0, 4, response + 24);
        }
        if (strlen(response) > 48) {
            display_print(0, 5, response + 48);
        }
    }
    
    display_print(0, 7, "Press any key");
    display_update();
    
    // Log command to storage
    if (storage_is_ready()) {
        char log_entry[128];
        snprintf(log_entry, sizeof(log_entry), "CMD: %s -> %d", command, result);
        storage_log_system_event(log_entry, 4);
    }
}

// Fuzzing menu callback implementations
void ui_callback_fuzzing_menu(void) {
    ui_set_state(UI_STATE_FUZZING_MENU);
}

void ui_callback_fuzz_discovery(void) {
    ui_set_state(UI_STATE_FUZZING_ACTION);
    display_clear();
    display_print(0, 0, "Pin Discovery");
    display_print(0, 1, "Running...");
    display_update();
    
    char response[256] = {0};
    cmd_result_t result = fuzz_commands_process("discover", response, sizeof(response));
    
    display_clear();
    display_print(0, 0, "Pin Discovery");
    
    if (result == CMD_RESULT_SUCCESS) {
        display_print(0, 1, "Success!");
        // Show first line of response
        if (strlen(response) > 0) {
            display_print(0, 2, response);
        }
    } else {
        display_print(0, 1, "Failed");
        display_print(0, 2, response);
    }
    
    display_print(0, 6, "Press BACK to exit");
    display_update();
}

void ui_callback_fuzz_random(void) {
    ui_set_state(UI_STATE_FUZZING_ACTION);
    display_clear();
    display_print(0, 0, "Random Fuzzing");
    display_print(0, 1, "Starting...");
    display_update();
    
    char response[256] = {0};
    cmd_result_t result = fuzz_commands_process("fuzz random 100", response, sizeof(response));
    
    display_clear();
    display_print(0, 0, "Random Fuzzing");
    
    if (result == CMD_RESULT_SUCCESS) {
        display_print(0, 1, "Started!");
        display_print(0, 2, "100 iterations");
    } else {
        display_print(0, 1, "Failed to start");
        display_print(0, 2, response);
    }
    
    display_print(0, 6, "Press BACK to exit");
    display_update();
}

void ui_callback_fuzz_boundary(void) {
    ui_set_state(UI_STATE_FUZZING_ACTION);
    display_clear();
    display_print(0, 0, "Boundary Scan");
    display_print(0, 1, "Running...");
    display_update();
    
    char response[256] = {0};
    cmd_result_t result = fuzz_commands_process("boundary", response, sizeof(response));
    
    display_clear();
    display_print(0, 0, "Boundary Scan");
    
    if (result == CMD_RESULT_SUCCESS) {
        display_print(0, 1, "Complete!");
        if (strlen(response) > 0) {
            display_print(0, 2, response);
        }
    } else {
        display_print(0, 1, "Failed");
        display_print(0, 2, response);
    }
    
    display_print(0, 6, "Press BACK to exit");
    display_update();
}

void ui_callback_fuzz_scan(void) {
    ui_set_state(UI_STATE_FUZZING_ACTION);
    display_clear();
    display_print(0, 0, "Quick Fuzz Scan");
    display_print(0, 1, "Scanning...");
    display_update();
    
    char response[256] = {0};
    cmd_result_t result = fuzz_commands_process("scan", response, sizeof(response));
    
    display_clear();
    display_print(0, 0, "Quick Scan");
    
    if (result == CMD_RESULT_SUCCESS) {
        display_print(0, 1, "Complete!");
        if (strlen(response) > 0) {
            display_print(0, 2, response);
        }
    } else {
        display_print(0, 1, "Failed");
        display_print(0, 2, response);
    }
    
    display_print(0, 6, "Press BACK to exit");
    display_update();
}

void ui_callback_fuzz_status(void) {
    ui_set_state(UI_STATE_FUZZING_ACTION);
    display_clear();
    display_print(0, 0, "Fuzzing Status");
    display_print(0, 1, "Checking...");
    display_update();
    
    char response[256] = {0};
    cmd_result_t result = fuzz_commands_process("status", response, sizeof(response));
    
    display_clear();
    display_print(0, 0, "Fuzz Status");
    
    if (result == CMD_RESULT_SUCCESS) {
        // Parse and display status information
        display_print(0, 1, "Engine Ready");
        if (strlen(response) > 0) {
            display_print(0, 2, response);
            if (strlen(response) > 24) {
                display_print(0, 3, response + 24);
            }
            if (strlen(response) > 48) {
                display_print(0, 4, response + 48);
            }
        }
    } else {
        display_print(0, 1, "Not Ready");
        display_print(0, 2, response);
    }
    
    display_print(0, 6, "Press BACK to exit");
    display_update();
}

void ui_callback_command_line(void) {
    command_index = 0;
    ui_set_state(UI_STATE_COMMAND_LINE);
}
