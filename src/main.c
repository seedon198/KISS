/**
 * @file main.c
 * @brief Minimal KISS Fuzzer implementation - step by step build
 * @author KISS Fuzzer Team
 * @date 2025
 */

#include <stdio.h>
#include "pico/stdlib.h"

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
    
    printf("KISS Fuzzer v0.1.0 - Foundation Release\n");
    printf("System initialized successfully!\n");
    
    uint32_t counter = 0;
    
    // Simple blink loop with status
    while (true) {
        gpio_put(LED_PIN, 1);
        printf("Tick %lu: LED ON\n", counter);
        sleep_ms(1000);
        
        gpio_put(LED_PIN, 0);
        printf("Tick %lu: LED OFF\n", counter);
        sleep_ms(1000);
        
        counter++;
    }
    
    return 0;
}
