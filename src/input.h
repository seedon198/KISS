/**
 * @file input.h
 * @brief Input system for 5-way joystick and buttons
 * @author KISS Fuzzer Team
 * @date 2025
 * @version 0.3.0
 */

#ifndef INPUT_H
#define INPUT_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

// Input pin assignments (configurable based on hardware design)
#define INPUT_JOYSTICK_UP     10
#define INPUT_JOYSTICK_DOWN   11
#define INPUT_JOYSTICK_LEFT   12
#define INPUT_JOYSTICK_RIGHT  13
#define INPUT_JOYSTICK_OK     14
#define INPUT_BUTTON_BACK     15
#define INPUT_BUTTON_MENU     16

// Debounce timing
#define INPUT_DEBOUNCE_MS     50

/**
 * @brief Input event types
 */
typedef enum {
    INPUT_NONE = 0,
    INPUT_UP,
    INPUT_DOWN,
    INPUT_LEFT,
    INPUT_RIGHT,
    INPUT_OK,
    INPUT_BACK,
    INPUT_MENU
} input_event_t;

/**
 * @brief Input state structure
 */
typedef struct {
    bool up;
    bool down;
    bool left;
    bool right;
    bool ok;
    bool back;
    bool menu;
    uint32_t last_change_time;
} input_state_t;

/**
 * @brief Initialize the input system
 * @return true if successful, false otherwise
 */
bool input_init(void);

/**
 * @brief Get current input state
 * @return Current input state structure
 */
input_state_t input_get_state(void);

/**
 * @brief Get next input event (blocking with timeout)
 * @param timeout_ms Timeout in milliseconds (0 = no timeout)
 * @return Input event type
 */
input_event_t input_get_event(uint32_t timeout_ms);

/**
 * @brief Check if any input is currently pressed
 * @return true if any input is active
 */
bool input_any_pressed(void);

/**
 * @brief Wait for all inputs to be released
 */
void input_wait_release(void);

/**
 * @brief Get human-readable name for input event
 * @param event Input event type
 * @return String name of the event
 */
const char* input_event_name(input_event_t event);

#endif // INPUT_H
