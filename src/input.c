/**
 * @file input.c
 * @brief Input system implementation for 5-way joystick and buttons
 * @author KISS Fuzzer Team
 * @date 2025
 * @version 0.3.0
 */

#include "input.h"

// Static variables for input state tracking
static bool input_ready = false;
static input_state_t current_state = {0};
static input_state_t previous_state = {0};

/**
 * @brief Initialize the input system
 * @return true if successful, false otherwise
 */
bool input_init(void) {
    printf("Initializing input system...\n");
    
    // Initialize all input pins as inputs with pull-up resistors
    // Assuming active-low inputs (pressed = 0, released = 1)
    
    gpio_init(INPUT_JOYSTICK_UP);
    gpio_set_dir(INPUT_JOYSTICK_UP, GPIO_IN);
    gpio_pull_up(INPUT_JOYSTICK_UP);
    
    gpio_init(INPUT_JOYSTICK_DOWN);
    gpio_set_dir(INPUT_JOYSTICK_DOWN, GPIO_IN);
    gpio_pull_up(INPUT_JOYSTICK_DOWN);
    
    gpio_init(INPUT_JOYSTICK_LEFT);
    gpio_set_dir(INPUT_JOYSTICK_LEFT, GPIO_IN);
    gpio_pull_up(INPUT_JOYSTICK_LEFT);
    
    gpio_init(INPUT_JOYSTICK_RIGHT);
    gpio_set_dir(INPUT_JOYSTICK_RIGHT, GPIO_IN);
    gpio_pull_up(INPUT_JOYSTICK_RIGHT);
    
    gpio_init(INPUT_JOYSTICK_OK);
    gpio_set_dir(INPUT_JOYSTICK_OK, GPIO_IN);
    gpio_pull_up(INPUT_JOYSTICK_OK);
    
    gpio_init(INPUT_BUTTON_BACK);
    gpio_set_dir(INPUT_BUTTON_BACK, GPIO_IN);
    gpio_pull_up(INPUT_BUTTON_BACK);
    
    gpio_init(INPUT_BUTTON_MENU);
    gpio_set_dir(INPUT_BUTTON_MENU, GPIO_IN);
    gpio_pull_up(INPUT_BUTTON_MENU);
    
    // Initialize state
    current_state.last_change_time = to_ms_since_boot(get_absolute_time());
    input_ready = true;
    
    printf("Input system initialized successfully\n");
    printf("Input pins: UP=%d, DOWN=%d, LEFT=%d, RIGHT=%d, OK=%d, BACK=%d, MENU=%d\n",
           INPUT_JOYSTICK_UP, INPUT_JOYSTICK_DOWN, INPUT_JOYSTICK_LEFT,
           INPUT_JOYSTICK_RIGHT, INPUT_JOYSTICK_OK, INPUT_BUTTON_BACK, INPUT_BUTTON_MENU);
    
    return true;
}

/**
 * @brief Read raw input states from GPIO pins
 * @param state Pointer to input state structure to fill
 */
static void input_read_raw(input_state_t* state) {
    if (!input_ready) return;
    
    // Read GPIO states (active low, so invert)
    state->up = !gpio_get(INPUT_JOYSTICK_UP);
    state->down = !gpio_get(INPUT_JOYSTICK_DOWN);
    state->left = !gpio_get(INPUT_JOYSTICK_LEFT);
    state->right = !gpio_get(INPUT_JOYSTICK_RIGHT);
    state->ok = !gpio_get(INPUT_JOYSTICK_OK);
    state->back = !gpio_get(INPUT_BUTTON_BACK);
    state->menu = !gpio_get(INPUT_BUTTON_MENU);
    state->last_change_time = to_ms_since_boot(get_absolute_time());
}

/**
 * @brief Compare two input states for differences
 * @param state1 First state
 * @param state2 Second state
 * @return true if states are different
 */
static bool input_states_different(const input_state_t* state1, const input_state_t* state2) {
    return (state1->up != state2->up) ||
           (state1->down != state2->down) ||
           (state1->left != state2->left) ||
           (state1->right != state2->right) ||
           (state1->ok != state2->ok) ||
           (state1->back != state2->back) ||
           (state1->menu != state2->menu);
}

/**
 * @brief Get current input state
 * @return Current input state structure
 */
input_state_t input_get_state(void) {
    if (!input_ready) {
        input_state_t empty_state = {0};
        return empty_state;
    }
    
    input_read_raw(&current_state);
    return current_state;
}

/**
 * @brief Get next input event (blocking with timeout)
 * @param timeout_ms Timeout in milliseconds (0 = no timeout)
 * @return Input event type
 */
input_event_t input_get_event(uint32_t timeout_ms) {
    if (!input_ready) return INPUT_NONE;
    
    uint32_t start_time = to_ms_since_boot(get_absolute_time());
    
    while (true) {
        // Save previous state
        previous_state = current_state;
        
        // Read current state
        input_read_raw(&current_state);
        
        // Check for state changes (button press detection)
        if (input_states_different(&current_state, &previous_state)) {
            // Simple debouncing
            sleep_ms(INPUT_DEBOUNCE_MS);
            input_read_raw(&current_state);
            
            // Detect rising edge (button press)
            if (current_state.up && !previous_state.up) return INPUT_UP;
            if (current_state.down && !previous_state.down) return INPUT_DOWN;
            if (current_state.left && !previous_state.left) return INPUT_LEFT;
            if (current_state.right && !previous_state.right) return INPUT_RIGHT;
            if (current_state.ok && !previous_state.ok) return INPUT_OK;
            if (current_state.back && !previous_state.back) return INPUT_BACK;
            if (current_state.menu && !previous_state.menu) return INPUT_MENU;
        }
        
        // Check timeout
        if (timeout_ms > 0) {
            uint32_t elapsed = to_ms_since_boot(get_absolute_time()) - start_time;
            if (elapsed >= timeout_ms) {
                return INPUT_NONE;
            }
        }
        
        sleep_ms(10); // Small delay to prevent excessive polling
    }
}

/**
 * @brief Check if any input is currently pressed
 * @return true if any input is active
 */
bool input_any_pressed(void) {
    if (!input_ready) return false;
    
    input_read_raw(&current_state);
    return current_state.up || current_state.down || current_state.left ||
           current_state.right || current_state.ok || current_state.back ||
           current_state.menu;
}

/**
 * @brief Wait for all inputs to be released
 */
void input_wait_release(void) {
    if (!input_ready) return;
    
    printf("Waiting for input release...\n");
    while (input_any_pressed()) {
        sleep_ms(10);
    }
    printf("All inputs released\n");
}

/**
 * @brief Get human-readable name for input event
 * @param event Input event type
 * @return String name of the event
 */
const char* input_event_name(input_event_t event) {
    switch (event) {
        case INPUT_UP:    return "UP";
        case INPUT_DOWN:  return "DOWN";
        case INPUT_LEFT:  return "LEFT";
        case INPUT_RIGHT: return "RIGHT";
        case INPUT_OK:    return "OK";
        case INPUT_BACK:  return "BACK";
        case INPUT_MENU:  return "MENU";
        case INPUT_NONE:  return "NONE";
        default:          return "UNKNOWN";
    }
}
