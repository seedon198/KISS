/**
 * @file main.c
 * @brief KISS Fuzzer v0.2.0 - Display Module
 * @author KISS Fuzzer Team
 * @date 2025
 */

#include <stdio.h>
#include <stdarg.h>
#include "pico/stdlib.h"
#include "display.h"

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
    
    printf("KISS Fuzzer v0.2.0 - Display Module Complete\n");
    printf("System initializing...\n");
    
    // Initialize display
    if (display_init()) {
        printf("Display initialized successfully\n");
        display_test();
    } else {
        printf("Display initialization failed\n");
    }
    
    uint32_t counter = 0;
    uint32_t display_update_counter = 0;
    
    // Main loop with LED blink and display updates
    while (true) {
        gpio_put(LED_PIN, 1);
        printf("Tick %lu: LED ON\n", counter);
        
        // Update display every 5 seconds
        if (display_update_counter % 5 == 0) {
            if (counter % 2 == 0) {
                display_show_status("System Running");
            } else {
                display_show_status("All Systems OK");
            }
        }
        
        sleep_ms(1000);
        
        gpio_put(LED_PIN, 0);
        printf("Tick %lu: LED OFF\n", counter);
        sleep_ms(1000);
        
        counter++;
        display_update_counter++;
    }
    
    return 0;
}
