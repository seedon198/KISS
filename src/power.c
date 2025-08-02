/**
 * @file power.c
 * @brief Power management system implementation
 * @author KISS Fuzzer Team
 * @date 2025
 * @version 0.5.0
 */

#include "power.h"

// Static variables for power management
static bool power_ready = false;
static power_status_t current_status = {0};

/**
 * @brief Initialize the power management system
 * @return true if successful, false otherwise
 */
bool power_init(void) {
    printf("Initializing power management system...\n");
    
    // Initialize ADC for battery voltage monitoring
    adc_init();
    adc_gpio_init(POWER_BATTERY_ADC_PIN);
    adc_select_input(0); // ADC0 for pin 26
    
    // Initialize GPIO pins for power status
    gpio_init(POWER_CHARGING_PIN);
    gpio_set_dir(POWER_CHARGING_PIN, GPIO_IN);
    gpio_pull_up(POWER_CHARGING_PIN); // Assuming active-low charging signal
    
    gpio_init(POWER_USB_DETECT_PIN);
    gpio_set_dir(POWER_USB_DETECT_PIN, GPIO_IN);
    gpio_pull_down(POWER_USB_DETECT_PIN); // Assuming active-high USB detection
    
    // Initialize status structure
    current_status.last_update_time = to_ms_since_boot(get_absolute_time());
    
    // Perform initial reading
    power_update();
    
    power_ready = true;
    printf("Power management initialized successfully\n");
    printf("Initial status: Battery=%dmV (%d%%), USB=%s, Charging=%s\n",
           current_status.battery_mv,
           current_status.battery_percent,
           current_status.usb_connected ? "Yes" : "No",
           current_status.is_charging ? "Yes" : "No");
    
    return true;
}

/**
 * @brief Read ADC value and convert to millivolts
 * @return Voltage in millivolts
 */
static uint16_t power_read_adc_voltage(void) {
    // Read ADC (12-bit, 0-4095)
    uint16_t adc_raw = adc_read();
    
    // Convert to millivolts
    // ADC_value / 4095 * VREF * voltage_divider_factor
    uint32_t voltage_mv = (adc_raw * POWER_ADC_VREF_MV * POWER_VOLTAGE_DIVIDER) / 4095;
    
    return (uint16_t)voltage_mv;
}

/**
 * @brief Convert battery voltage to percentage
 * @param voltage_mv Battery voltage in millivolts
 * @return Battery percentage (0-100)
 */
static uint8_t power_voltage_to_percent(uint16_t voltage_mv) {
    if (voltage_mv >= POWER_BATTERY_MAX_MV) {
        return 100;
    }
    
    if (voltage_mv <= POWER_BATTERY_MIN_MV) {
        return 0;
    }
    
    // Linear interpolation between min and max
    uint32_t range = POWER_BATTERY_MAX_MV - POWER_BATTERY_MIN_MV;
    uint32_t position = voltage_mv - POWER_BATTERY_MIN_MV;
    
    return (uint8_t)((position * 100) / range);
}

/**
 * @brief Update power status
 */
void power_update(void) {
    if (!power_ready) return;
    
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    
    // Check if it's time to update (rate limiting)
    if (current_time - current_status.last_update_time < POWER_UPDATE_INTERVAL_MS) {
        return;
    }
    
    // Read battery voltage
    current_status.battery_mv = power_read_adc_voltage();
    current_status.battery_percent = power_voltage_to_percent(current_status.battery_mv);
    
    // Read power status pins
    current_status.usb_connected = gpio_get(POWER_USB_DETECT_PIN);
    current_status.is_charging = !gpio_get(POWER_CHARGING_PIN); // Assuming active-low
    
    // Determine battery status flags
    current_status.low_battery = (current_status.battery_percent <= 20);
    current_status.critical_battery = (current_status.battery_percent <= 5);
    
    current_status.last_update_time = current_time;
    
    // Debug output (can be removed in production)
    static uint32_t last_debug_time = 0;
    if (current_time - last_debug_time > 10000) { // Every 10 seconds
        printf("Power status: %dmV (%d%%), USB=%s, Charging=%s\n",
               current_status.battery_mv,
               current_status.battery_percent,
               current_status.usb_connected ? "Yes" : "No",
               current_status.is_charging ? "Yes" : "No");
        last_debug_time = current_time;
    }
}

/**
 * @brief Get current power status
 * @return Power status structure
 */
power_status_t power_get_status(void) {
    return current_status;
}

/**
 * @brief Get current power state
 * @return Power state enumeration
 */
power_state_t power_get_state(void) {
    if (!power_ready) {
        return POWER_STATE_UNKNOWN;
    }
    
    if (current_status.usb_connected) {
        if (current_status.is_charging) {
            return POWER_STATE_CHARGING;
        } else if (current_status.battery_percent >= 95) {
            return POWER_STATE_CHARGED;
        } else {
            return POWER_STATE_USB_POWERED;
        }
    } else {
        return POWER_STATE_BATTERY_ONLY;
    }
}

/**
 * @brief Get battery voltage in millivolts
 * @return Battery voltage in mV
 */
uint16_t power_get_battery_voltage(void) {
    return current_status.battery_mv;
}

/**
 * @brief Get battery percentage (0-100)
 * @return Battery percentage
 */
uint8_t power_get_battery_percent(void) {
    return current_status.battery_percent;
}

/**
 * @brief Check if USB power is connected
 * @return true if USB connected
 */
bool power_is_usb_connected(void) {
    return current_status.usb_connected;
}

/**
 * @brief Check if battery is charging
 * @return true if charging
 */
bool power_is_charging(void) {
    return current_status.is_charging;
}

/**
 * @brief Check if battery is low
 * @return true if low battery
 */
bool power_is_low_battery(void) {
    return current_status.low_battery;
}

/**
 * @brief Check if battery is critically low
 * @return true if critical battery
 */
bool power_is_critical_battery(void) {
    return current_status.critical_battery;
}

/**
 * @brief Get human-readable power state name
 * @param state Power state
 * @return String name of the state
 */
const char* power_state_name(power_state_t state) {
    switch (state) {
        case POWER_STATE_BATTERY_ONLY: return "Battery";
        case POWER_STATE_USB_POWERED:  return "USB Power";
        case POWER_STATE_CHARGING:     return "Charging";
        case POWER_STATE_CHARGED:      return "Charged";
        case POWER_STATE_UNKNOWN:      return "Unknown";
        default:                       return "Invalid";
    }
}

/**
 * @brief Get formatted battery status string
 * @param buffer Buffer to write to
 * @param buffer_size Size of buffer
 * @return Formatted string with battery info
 */
void power_get_status_string(char* buffer, size_t buffer_size) {
    if (!buffer || buffer_size == 0) return;
    
    power_state_t state = power_get_state();
    
    if (current_status.usb_connected) {
        snprintf(buffer, buffer_size, "USB %d%% %s",
                current_status.battery_percent,
                current_status.is_charging ? "CHG" : "PWR");
    } else {
        const char* status_icon = "";
        if (current_status.critical_battery) {
            status_icon = "!";
        } else if (current_status.low_battery) {
            status_icon = "*";
        }
        
        snprintf(buffer, buffer_size, "Bat %d%%%s",
                current_status.battery_percent, status_icon);
    }
}
