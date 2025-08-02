/**
 * @file main.c
 * @brief KISS Fuzzer v0.5.0 - Power Management System
 * @author KISS Fuzzer Team
 * @date 2025
 */

#include <stdio.h>
#include <stdarg.h>
#include "pico/stdlib.h"
#include "display.h"
#include "input.h"
#include "ui.h"
#include "power.h"

/**
 * @brief Main entry point
 * @return Never returns
 */
int main(void) {
    // Initialize stdio for USB output
    stdio_init_all();
    
    // Initialize LED
    const uint LED_PIN = 25;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    
    printf("KISS Fuzzer v0.5.0 - Power Management System\n");
    printf("System initializing...\n");
    
    // Initialize UI system (includes display, input, and power)
    if (ui_init()) {
        printf("UI system initialized successfully\n");
    } else {
        printf("UI system initialization failed\n");
        // Fall back to basic operation
        display_show_status("UI Init Failed");
        while (true) {
            gpio_put(LED_PIN, 1);
            sleep_ms(500);
            gpio_put(LED_PIN, 0);
            sleep_ms(500);
        }
    }
    
    uint32_t counter = 0;
    
    // Main loop with UI updates and LED heartbeat
    while (true) {
        // Update UI system (handles input and display)
        ui_update();
        
        // LED heartbeat every second
        if (counter % 20 == 0) { // Every 1 second (50ms * 20)
            gpio_put(LED_PIN, 1);
        } else if (counter % 20 == 1) {
            gpio_put(LED_PIN, 0);
        }
        
        sleep_ms(50); // 20Hz update rate
        counter++;
    }
    
    return 0;
}
