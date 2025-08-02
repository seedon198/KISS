/**
 * @file main.c
 * @brief KISS Fuzzer v0.3.0 - Input Module
 * @author KISS Fuzzer Team
 * @date 2025
 */

#include <stdio.h>
#include <stdarg.h>
#include "pico/stdlib.h"
#include "display.h"
#include "input.h"

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
    
    printf("KISS Fuzzer v0.3.0 - Input Module Testing\n");
    printf("System initializing...\n");
    
    // Initialize display
    if (display_init()) {
        printf("Display initialized successfully\n");
        display_test();
    } else {
        printf("Display initialization failed\n");
    }
    
    // Initialize input system
    if (input_init()) {
        printf("Input system initialized successfully\n");
    } else {
        printf("Input system initialization failed\n");
    }
    
    uint32_t counter = 0;
    uint32_t display_update_counter = 0;
    
    // Initial display message
    display_show_status("Input Test Mode");
    
    // Main loop with LED blink, display updates, and input testing
    while (true) {
        gpio_put(LED_PIN, 1);
        
        // Check for input events (non-blocking)
        input_event_t event = input_get_event(100); // 100ms timeout
        if (event != INPUT_NONE) {
            printf("Input detected: %s\n", input_event_name(event));
            
            // Update display with input event
            char input_msg[32];
            snprintf(input_msg, sizeof(input_msg), "Input: %s", input_event_name(event));
            display_show_status(input_msg);
            
            // Special handling for different inputs
            switch (event) {
                case INPUT_OK:
                    printf("OK button pressed - System OK!\n");
                    break;
                case INPUT_BACK:
                    printf("Back button pressed\n");
                    break;
                case INPUT_MENU:
                    printf("Menu button pressed\n");
                    break;
                case INPUT_UP:
                    printf("Joystick UP\n");
                    break;
                case INPUT_DOWN:
                    printf("Joystick DOWN\n");
                    break;
                case INPUT_LEFT:
                    printf("Joystick LEFT\n");
                    break;
                case INPUT_RIGHT:
                    printf("Joystick RIGHT\n");
                    break;
                default:
                    break;
            }
        } else {
            // No input - update display with system status
            if (display_update_counter % 50 == 0) { // Every 5 seconds
                if (counter % 2 == 0) {
                    display_show_status("System Running");
                } else {
                    display_show_status("Press any button");
                }
            }
        }
        
        sleep_ms(50);
        
        gpio_put(LED_PIN, 0);
        sleep_ms(50);
        
        counter++;
        display_update_counter++;
    }
    
    return 0;
}
