/**
 * @file ui.c
 * @brief User interface and menu system implementation with 5-way joystick input
 * @author KISS Fuzzer Team
 * @date 2025
 */

#include "ui.h"
#include "jtag.h"

// Menu definitions
static const menu_item_t main_menu[] = {
    {"Scan JTAG", MENU_JTAG_SCAN, action_jtag_scan},
    {"Manual Mode", MENU_JTAG_OPTIONS, NULL},
    {"Glitch Tool", MENU_GLITCH_TOOL, action_glitch_test},
    {"Logs", MENU_LOGS, action_view_logs},
    {"Settings", MENU_SETTINGS, action_settings_menu},
    {"Wi-Fi", MENU_WIFI_CONFIG, action_wifi_enable}
};

static const menu_item_t jtag_menu[] = {
    {"Auto Scan", MENU_MAIN, action_jtag_scan},
    {"Detect Pins", MENU_MAIN, action_jtag_detect_pins},
    {"Read IDCODE", MENU_MAIN, action_jtag_read_idcode},
    {"Memory Dump", MENU_MAIN, action_memory_dump},
    {"Back", MENU_MAIN, NULL}
};

static const menu_item_t logs_menu[] = {
    {"View Logs", MENU_MAIN, action_view_logs},
    {"Clear Logs", MENU_MAIN, action_clear_logs},
    {"Export to SD", MENU_MAIN, NULL},
    {"Back", MENU_MAIN, NULL}
};

// UI state variables
static menu_state_t current_menu = MENU_MAIN;
static uint8_t selected_item = 0;
static uint32_t last_input_time = 0;
static bool status_message_active = false;
static uint32_t status_message_end = 0;
static char status_buffer[64] = {0};

// GPIO debounce variables
static uint32_t last_button_time[6] = {0};
static const uint32_t DEBOUNCE_TIME_MS = 50;

/**
 * @brief Initialize UI system and input handlers
 * @return void
 */
void ui_init(void)
{
    // Initialize joystick GPIO pins
    gpio_init(JOYSTICK_UP);
    gpio_init(JOYSTICK_DOWN);
    gpio_init(JOYSTICK_LEFT);
    gpio_init(JOYSTICK_RIGHT);
    gpio_init(JOYSTICK_OK);
    gpio_init(JOYSTICK_BACK);
    
    // Set as inputs with pull-up resistors
    gpio_set_dir(JOYSTICK_UP, GPIO_IN);
    gpio_set_dir(JOYSTICK_DOWN, GPIO_IN);
    gpio_set_dir(JOYSTICK_LEFT, GPIO_IN);
    gpio_set_dir(JOYSTICK_RIGHT, GPIO_IN);
    gpio_set_dir(JOYSTICK_OK, GPIO_IN);
    gpio_set_dir(JOYSTICK_BACK, GPIO_IN);
    
    gpio_pull_up(JOYSTICK_UP);
    gpio_pull_up(JOYSTICK_DOWN);
    gpio_pull_up(JOYSTICK_LEFT);
    gpio_pull_up(JOYSTICK_RIGHT);
    gpio_pull_up(JOYSTICK_OK);
    gpio_pull_up(JOYSTICK_BACK);
    
    // Enable interrupts on falling edge (button press)
    gpio_set_irq_enabled_with_callback(JOYSTICK_UP, GPIO_IRQ_EDGE_FALL, true, &ui_gpio_callback);
    gpio_set_irq_enabled(JOYSTICK_DOWN, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(JOYSTICK_LEFT, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(JOYSTICK_RIGHT, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(JOYSTICK_OK, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(JOYSTICK_BACK, GPIO_IRQ_EDGE_FALL, true);
    
    // Initialize menu state
    current_menu = MENU_MAIN;
    selected_item = 0;
    
    printf("UI system initialized\n");
}

/**
 * @brief Main UI task function
 * @param pvParameters FreeRTOS task parameters
 * @return void
 */
void ui_task(void* pvParameters)
{
    (void)pvParameters;
    
    ui_event_t event;
    TickType_t last_wake_time = xTaskGetTickCount();
    
    printf("UI task started\n");
    
    // Initial display update
    ui_update_display();
    
    while (1) {
        // Check for UI events
        if (xQueueReceive(ui_event_queue, &event, pdMS_TO_TICKS(100)) == pdTRUE) {
            ui_process_event(&event);
            ui_update_display();
        }
        
        // Check for status message timeout
        if (status_message_active && (xTaskGetTickCount() - status_message_end) > 0) {
            status_message_active = false;
            ui_update_display();
        }
        
        // Update battery status periodically
        static uint32_t last_battery_update = 0;
        uint32_t now = xTaskGetTickCount();
        if (now - last_battery_update > pdMS_TO_TICKS(5000)) { // Every 5 seconds
            power_status_t power_status;
            power_get_status(&power_status);
            
            xSemaphoreTake(display_mutex, portMAX_DELAY);
            display_battery_status(power_status.battery_percent, power_status.is_charging);
            display_refresh();
            xSemaphoreGive(display_mutex);
            
            last_battery_update = now;
        }
        
        // Sleep until next cycle
        vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(100));
    }
}

/**
 * @brief Handle joystick input and generate UI events
 * @return void
 */
void ui_handle_input(void)
{
    // This function is called from GPIO interrupt
    // Actual processing is done in ui_gpio_callback
}

/**
 * @brief Process UI event and update menu state
 * @param event UI event to process
 * @return void
 */
void ui_process_event(const ui_event_t* event)
{
    if (!event) return;
    
    uint8_t menu_size = 0;
    
    // Determine current menu size
    switch (current_menu) {
        case MENU_MAIN:
            menu_size = sizeof(main_menu) / sizeof(menu_item_t);
            break;
        case MENU_JTAG_OPTIONS:
            menu_size = sizeof(jtag_menu) / sizeof(menu_item_t);
            break;
        case MENU_LOGS:
            menu_size = sizeof(logs_menu) / sizeof(menu_item_t);
            break;
        default:
            menu_size = 1;
            break;
    }
    
    switch (event->type) {
        case UI_EVENT_UP:
            if (selected_item > 0) {
                selected_item--;
            } else {
                selected_item = menu_size - 1; // Wrap around
            }
            break;
            
        case UI_EVENT_DOWN:
            if (selected_item < menu_size - 1) {
                selected_item++;
            } else {
                selected_item = 0; // Wrap around
            }
            break;
            
        case UI_EVENT_LEFT:
            // Could be used for horizontal navigation in future
            break;
            
        case UI_EVENT_RIGHT:
            // Could be used for horizontal navigation in future
            break;
            
        case UI_EVENT_OK:
            // Execute selected menu item
            switch (current_menu) {
                case MENU_MAIN:
                    if (main_menu[selected_item].action_func) {
                        main_menu[selected_item].action_func();
                    } else if (main_menu[selected_item].next_state != MENU_MAIN) {
                        current_menu = main_menu[selected_item].next_state;
                        selected_item = 0;
                    }
                    break;
                    
                case MENU_JTAG_OPTIONS:
                    if (jtag_menu[selected_item].action_func) {
                        jtag_menu[selected_item].action_func();
                    }
                    current_menu = jtag_menu[selected_item].next_state;
                    selected_item = 0;
                    break;
                    
                case MENU_LOGS:
                    if (logs_menu[selected_item].action_func) {
                        logs_menu[selected_item].action_func();
                    }
                    current_menu = logs_menu[selected_item].next_state;
                    selected_item = 0;
                    break;
                    
                default:
                    current_menu = MENU_MAIN;
                    selected_item = 0;
                    break;
            }
            break;
            
        case UI_EVENT_BACK:
            // Return to main menu
            current_menu = MENU_MAIN;
            selected_item = 0;
            break;
            
        default:
            break;
    }
    
    last_input_time = xTaskGetTickCount();
}

/**
 * @brief Update display with current menu
 * @return void
 */
void ui_update_display(void)
{
    if (xSemaphoreTake(display_mutex, pdMS_TO_TICKS(100)) != pdTRUE) {
        return; // Skip update if can't get mutex
    }
    
    display_clear();
    
    // Show status message if active
    if (status_message_active) {
        display_write_line(0, "KISS Fuzzer", false);
        display_write_line(2, status_buffer, false);
        display_write_line(4, "Press any key...", false);
    } else {
        // Show current menu
        const menu_item_t* menu_items = NULL;
        uint8_t menu_size = 0;
        const char* menu_title = "";
        
        switch (current_menu) {
            case MENU_MAIN:
                menu_items = main_menu;
                menu_size = sizeof(main_menu) / sizeof(menu_item_t);
                menu_title = "KISS Fuzzer - Main";
                break;
                
            case MENU_JTAG_OPTIONS:
                menu_items = jtag_menu;
                menu_size = sizeof(jtag_menu) / sizeof(menu_item_t);
                menu_title = "JTAG Operations";
                break;
                
            case MENU_LOGS:
                menu_items = logs_menu;
                menu_size = sizeof(logs_menu) / sizeof(menu_item_t);
                menu_title = "Log Management";
                break;
                
            default:
                menu_title = "Unknown Menu";
                break;
        }
        
        // Show title
        display_write_line(0, menu_title, false);
        
        // Show menu items (max 6 items visible)
        uint8_t start_item = 0;
        if (selected_item > 5) {
            start_item = selected_item - 5;
        }
        
        for (uint8_t i = 0; i < 6 && (start_item + i) < menu_size; i++) {
            char line_buffer[64];
            uint8_t item_index = start_item + i;
            
            if (item_index == selected_item) {
                snprintf(line_buffer, sizeof(line_buffer), "> %s", menu_items[item_index].text);
            } else {
                snprintf(line_buffer, sizeof(line_buffer), "  %s", menu_items[item_index].text);
            }
            
            display_write_line(i + 2, line_buffer, false);
        }
    }
    
    display_refresh();
    xSemaphoreGive(display_mutex);
}

/**
 * @brief Show status message temporarily
 * @param message Status message to display
 * @param duration_ms Duration to show message (milliseconds)
 * @return void
 */
void ui_show_status(const char* message, uint32_t duration_ms)
{
    if (!message) return;
    
    strncpy(status_buffer, message, sizeof(status_buffer) - 1);
    status_buffer[sizeof(status_buffer) - 1] = '\0';
    
    status_message_active = true;
    status_message_end = xTaskGetTickCount() + pdMS_TO_TICKS(duration_ms);
    
    ui_update_display();
}

/**
 * @brief Show confirmation dialog
 * @param message Confirmation message
 * @return true if confirmed, false if cancelled
 */
bool ui_confirm_dialog(const char* message)
{
    // TODO: Implement confirmation dialog
    // For now, always return true
    ui_show_status(message, 2000);
    return true;
}

/**
 * @brief Show progress bar
 * @param title Progress title
 * @param percent Progress percentage (0-100)
 * @return void
 */
void ui_show_progress(const char* title, uint8_t percent)
{
    char progress_msg[64];
    snprintf(progress_msg, sizeof(progress_msg), "%s: %d%%", title, percent);
    ui_show_status(progress_msg, 100); // Short duration for frequent updates
}

/**
 * @brief GPIO interrupt handler for joystick inputs
 * @param gpio GPIO pin number
 * @param events GPIO event mask
 * @return void
 */
void ui_gpio_callback(uint gpio, uint32_t events)
{
    uint32_t now = to_ms_since_boot(get_absolute_time());
    ui_event_t event;
    uint8_t button_index = 0;
    
    // Determine which button was pressed
    switch (gpio) {
        case JOYSTICK_UP:
            event.type = UI_EVENT_UP;
            button_index = 0;
            break;
        case JOYSTICK_DOWN:
            event.type = UI_EVENT_DOWN;
            button_index = 1;
            break;
        case JOYSTICK_LEFT:
            event.type = UI_EVENT_LEFT;
            button_index = 2;
            break;
        case JOYSTICK_RIGHT:
            event.type = UI_EVENT_RIGHT;
            button_index = 3;
            break;
        case JOYSTICK_OK:
            event.type = UI_EVENT_OK;
            button_index = 4;
            break;
        case JOYSTICK_BACK:
            event.type = UI_EVENT_BACK;
            button_index = 5;
            break;
        default:
            return; // Unknown GPIO
    }
    
    // Debounce check
    if (now - last_button_time[button_index] < DEBOUNCE_TIME_MS) {
        return;
    }
    last_button_time[button_index] = now;
    
    // Clear status message on any input
    if (status_message_active) {
        status_message_active = false;
    }
    
    event.timestamp = now;
    
    // Send event to UI task (from ISR context)
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xQueueSendFromISR(ui_event_queue, &event, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

// Menu action functions implementation

/**
 * @brief Start JTAG scan operation
 * @return void
 */
void action_jtag_scan(void)
{
    ui_show_status("Starting JTAG scan...", 2000);
    printf("JTAG scan requested\n");
    
    // Perform JTAG scan
    jtag_scan_result_t scan_result;
    if (jtag_scan_chain(&scan_result)) {
        char status_msg[64];
        if (scan_result.device_count > 0) {
            snprintf(status_msg, sizeof(status_msg), "Found %d JTAG device%s", 
                    scan_result.device_count, scan_result.device_count > 1 ? "s" : "");
            ui_show_status(status_msg, 3000);
            
            // Log detailed results
            for (uint8_t i = 0; i < scan_result.device_count; i++) {
                printf("Device %d: %s %s (IDCODE: 0x%08lX)\n", 
                       i, scan_result.devices[i].manufacturer, 
                       scan_result.devices[i].device_name,
                       scan_result.devices[i].idcode);
            }
        } else {
            ui_show_status("No JTAG devices found", 3000);
        }
    } else {
        ui_show_status("JTAG scan failed", 3000);
    }
}

/**
 * @brief Detect JTAG pins automatically
 * @return void
 */
void action_jtag_detect_pins(void)
{
    ui_show_status("Detecting JTAG pins...", 2000);
    printf("JTAG pin detection requested\n");
    
    uint8_t detected_pins[4];
    if (jtag_detect_pins(detected_pins)) {
        char status_msg[64];
        snprintf(status_msg, sizeof(status_msg), "Pins: TCK=%d TMS=%d TDI=%d TDO=%d", 
                detected_pins[0], detected_pins[1], detected_pins[2], detected_pins[3]);
        ui_show_status(status_msg, 4000);
        
        printf("Detected JTAG pins - TCK:%d TMS:%d TDI:%d TDO:%d\n",
               detected_pins[0], detected_pins[1], detected_pins[2], detected_pins[3]);
    } else {
        ui_show_status("Pin detection failed", 3000);
    }
}

/**
 * @brief Read IDCODE from detected devices
 * @return void
 */
void action_jtag_read_idcode(void)
{
    ui_show_status("Reading IDCODE...", 2000);
    printf("IDCODE read requested\n");
    
    // First scan to find devices
    jtag_scan_result_t scan_result;
    if (jtag_scan_chain(&scan_result) && scan_result.device_count > 0) {
        // Read IDCODE from first device
        uint32_t idcode;
        if (jtag_read_idcode(0, &idcode)) {
            char status_msg[32];
            snprintf(status_msg, sizeof(status_msg), "IDCODE: 0x%08lX", idcode);
            ui_show_status(status_msg, 4000);
            printf("Read IDCODE: 0x%08lX\n", idcode);
        } else {
            ui_show_status("IDCODE read failed", 3000);
        }
    } else {
        ui_show_status("No devices found", 3000);
    }
}

/**
 * @brief Perform memory dump operation
 * @return void
 */
void action_memory_dump(void)
{
    ui_show_status("Starting memory dump...", 2000);
    printf("Memory dump requested\n");
    
    // Simplified memory dump - dump 256 bytes from address 0x08000000 (common flash start)
    const uint32_t start_addr = 0x08000000;
    const uint32_t dump_size = 256;
    uint8_t dump_data[256];
    
    uint32_t bytes_read = jtag_memory_dump(start_addr, dump_size, dump_data);
    if (bytes_read > 0) {
        char status_msg[48];
        snprintf(status_msg, sizeof(status_msg), "Dumped %lu bytes from 0x%08lX", bytes_read, start_addr);
        ui_show_status(status_msg, 4000);
        
        printf("Memory dump successful: %lu bytes from 0x%08lX\n", bytes_read, start_addr);
        
        // Log first few bytes for verification
        printf("First 16 bytes: ");
        for (int i = 0; i < 16 && i < bytes_read; i++) {
            printf("%02X ", dump_data[i]);
        }
        printf("\n");
    } else {
        ui_show_status("Memory dump failed", 3000);
    }
}

/**
 * @brief Run power glitch test
 * @return void
 */
void action_glitch_test(void)
{
    ui_show_status("Glitch test mode", 2000);
    printf("Glitch test requested\n");
    
    // TODO: Implement glitch testing
}

/**
 * @brief View system logs
 * @return void
 */
void action_view_logs(void)
{
    ui_show_status("Loading logs...", 1000);
    printf("Log view requested\n");
    
    // TODO: Implement log viewing
}

/**
 * @brief Clear system logs
 * @return void
 */
void action_clear_logs(void)
{
    if (ui_confirm_dialog("Clear all logs?")) {
        ui_show_status("Logs cleared", 1000);
        printf("Logs cleared\n");
        
        // TODO: Implement log clearing
    }
}

/**
 * @brief Enable/configure Wi-Fi
 * @return void
 */
void action_wifi_enable(void)
{
    ui_show_status("Starting Wi-Fi...", 2000);
    printf("Wi-Fi enable requested\n");
    
    // TODO: Signal Wi-Fi task to start AP mode
}

/**
 * @brief Open settings menu
 * @return void
 */
void action_settings_menu(void)
{
    ui_show_status("Settings menu", 1000);
    printf("Settings menu requested\n");
    
    // TODO: Implement settings menu
}
