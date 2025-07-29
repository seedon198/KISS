/**
 * @file power.c
 * @brief Battery monitoring and power management implementation
 * @author KISS Fuzzer Team
 * @date 2025
 */

#include "power.h"

// Global power status
static power_status_t current_power_status = {0};
static bool power_initialized = false;
static bool low_power_mode_enabled = false;

// Power thresholds (in volts)
static const float BATTERY_CRITICAL_VOLTAGE = 3.2f;
static const float BATTERY_LOW_VOLTAGE = 3.4f;
static const float BATTERY_FULL_VOLTAGE = 4.1f;

// ADC calibration values
static uint16_t adc_raw_offset = 0;
static float adc_voltage_scale = 1.0f;

/**
 * @brief Initialize power monitoring system
 * @return true if initialization successful, false otherwise
 */
bool power_init(void)
{
    // Initialize ADC for battery monitoring
    adc_init();
    adc_gpio_init(BATTERY_ADC_PIN);
    adc_select_input(BATTERY_ADC_CHANNEL);
    
    // Initialize charging status GPIO
    gpio_init(CHARGE_STAT_PIN);
    gpio_set_dir(CHARGE_STAT_PIN, GPIO_IN);
    gpio_pull_up(CHARGE_STAT_PIN);
    
    // Perform initial calibration
    power_calibrate_adc();
    
    // Initialize power status structure
    memset(&current_power_status, 0, sizeof(power_status_t));
    current_power_status.state = POWER_STATE_NORMAL;
    current_power_status.last_update = xTaskGetTickCount();
    
    // Perform initial reading
    power_update_status();
    
    power_initialized = true;
    printf("Power monitoring initialized - Battery: %.2fV (%d%%)\n", 
           current_power_status.battery_voltage, 
           current_power_status.battery_percent);
    
    return true;
}

/**
 * @brief Calibrate ADC for accurate voltage readings
 * @return void
 */
static void power_calibrate_adc(void)
{
    // Take multiple ADC readings for calibration
    uint32_t adc_sum = 0;
    const uint16_t calibration_samples = 100;
    
    for (uint16_t i = 0; i < calibration_samples; i++) {
        adc_sum += adc_read();
        sleep_us(100);
    }
    
    adc_raw_offset = adc_sum / calibration_samples;
    
    // Calculate voltage scale factor
    // This should be calibrated with known voltage reference
    adc_voltage_scale = (3.3f * VOLTAGE_DIVIDER_FACTOR) / 4095.0f;
    
    printf("ADC calibrated - Offset: %d, Scale: %.6f\n", adc_raw_offset, adc_voltage_scale);
}

/**
 * @brief Update power status readings
 * @return void
 */
static void power_update_status(void)
{
    if (!power_initialized) return;
    
    // Read battery voltage
    current_power_status.battery_voltage = power_read_battery_voltage();
    
    // Convert to percentage
    current_power_status.battery_percent = power_voltage_to_percent(current_power_status.battery_voltage);
    
    // Check charging status
    current_power_status.is_charging = power_is_charging();
    current_power_status.is_usb_connected = power_is_usb_connected();
    
    // Determine power state
    power_state_t new_state = current_power_status.state;
    
    if (current_power_status.is_charging) {
        if (current_power_status.battery_percent >= 95) {
            new_state = POWER_STATE_CHARGED;
        } else {
            new_state = POWER_STATE_CHARGING;
        }
    } else {
        if (current_power_status.battery_voltage <= BATTERY_CRITICAL_VOLTAGE) {
            new_state = POWER_STATE_CRITICAL;
        } else if (current_power_status.battery_voltage <= BATTERY_LOW_VOLTAGE) {
            new_state = POWER_STATE_LOW_BATTERY;
        } else {
            new_state = POWER_STATE_NORMAL;
        }
    }
    
    // Handle state changes
    if (new_state != current_power_status.state) {
        power_handle_state_change(new_state);
        current_power_status.state = new_state;
    }
    
    current_power_status.last_update = xTaskGetTickCount();
}

/**
 * @brief Power monitoring task function
 * @param pvParameters FreeRTOS task parameters
 * @return void
 */
void power_task(void* pvParameters)
{
    (void)pvParameters;
    
    TickType_t last_wake_time = xTaskGetTickCount();
    
    printf("Power monitoring task started\n");
    
    while (1) {
        // Update power status
        power_update_status();
        
        // Log status periodically (every minute)
        static uint32_t last_log_time = 0;
        uint32_t now = xTaskGetTickCount();
        if (now - last_log_time > pdMS_TO_TICKS(60000)) {
            printf("Power Status - V:%.2f P:%d%% S:%s C:%s\n",
                   current_power_status.battery_voltage,
                   current_power_status.battery_percent,
                   power_state_to_string(current_power_status.state),
                   current_power_status.is_charging ? "YES" : "NO");
            last_log_time = now;
        }
        
        // Sleep interval based on power state
        uint32_t sleep_interval_ms = 5000; // Default 5 seconds
        
        switch (current_power_status.state) {
            case POWER_STATE_CRITICAL:
                sleep_interval_ms = 1000; // Check every second
                break;
            case POWER_STATE_LOW_BATTERY:
                sleep_interval_ms = 2000; // Check every 2 seconds
                break;
            case POWER_STATE_CHARGING:
                sleep_interval_ms = 10000; // Check every 10 seconds
                break;
            default:
                sleep_interval_ms = 5000; // Normal interval
                break;
        }
        
        vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(sleep_interval_ms));
    }
}

/**
 * @brief Read current battery voltage
 * @return Battery voltage in volts
 */
float power_read_battery_voltage(void)
{
    // Take multiple readings for stability
    uint32_t adc_sum = 0;
    const uint8_t num_samples = 16;
    
    for (uint8_t i = 0; i < num_samples; i++) {
        adc_sum += adc_read();
        sleep_us(50);
    }
    
    uint16_t adc_average = adc_sum / num_samples;
    
    // Convert ADC reading to voltage
    float voltage = (float)adc_average * adc_voltage_scale;
    
    return voltage;
}

/**
 * @brief Convert battery voltage to percentage
 * @param voltage Battery voltage in volts
 * @return Battery percentage (0-100)
 */
uint8_t power_voltage_to_percent(float voltage)
{
    // Li-ion discharge curve approximation
    if (voltage >= BATTERY_FULL_VOLTAGE) {
        return 100;
    } else if (voltage <= BATTERY_MIN_VOLTAGE) {
        return 0;
    }
    
    // Linear approximation for simplicity
    // Real implementation would use lookup table or polynomial
    float voltage_range = BATTERY_FULL_VOLTAGE - BATTERY_MIN_VOLTAGE;
    float voltage_offset = voltage - BATTERY_MIN_VOLTAGE;
    
    uint8_t percentage = (uint8_t)((voltage_offset / voltage_range) * 100.0f);
    
    // Clamp to valid range
    if (percentage > 100) percentage = 100;
    
    return percentage;
}

/**
 * @brief Check if device is charging
 * @return true if charging, false otherwise
 */
bool power_is_charging(void)
{
    // Charging status pin is active low
    return !gpio_get(CHARGE_STAT_PIN);
}

/**
 * @brief Check if USB is connected
 * @return true if USB connected, false otherwise
 */
bool power_is_usb_connected(void)
{
    // Check VBUS voltage (simplified - assumes charging means USB connected)
    return power_is_charging() || (current_power_status.battery_voltage > BATTERY_FULL_VOLTAGE);
}

/**
 * @brief Get current power status
 * @param status Pointer to store power status
 * @return void
 */
void power_get_status(power_status_t* status)
{
    if (!status || !power_initialized) return;
    
    // Copy current status (atomic operation for simple struct)
    *status = current_power_status;
}

/**
 * @brief Get battery percentage
 * @return Battery percentage (0-100)
 */
uint8_t power_get_battery_percent(void)
{
    return current_power_status.battery_percent;
}

/**
 * @brief Check if battery is low
 * @return true if battery is low, false otherwise
 */
bool power_is_battery_low(void)
{
    return current_power_status.state == POWER_STATE_LOW_BATTERY;
}

/**
 * @brief Check if battery is critical
 * @return true if battery is critical, false otherwise
 */
bool power_is_battery_critical(void)
{
    return current_power_status.state == POWER_STATE_CRITICAL;
}

/**
 * @brief Enable low power mode
 * @return void
 */
void power_enable_low_power_mode(void)
{
    if (low_power_mode_enabled) return;
    
    low_power_mode_enabled = true;
    
    // Reduce system clock frequency
    set_sys_clock_khz(48000, true); // 48 MHz instead of 133 MHz
    
    // Reduce display brightness
    display_set_brightness(64); // 25% brightness
    
    printf("Low power mode enabled\n");
}

/**
 * @brief Disable low power mode
 * @return void
 */
void power_disable_low_power_mode(void)
{
    if (!low_power_mode_enabled) return;
    
    low_power_mode_enabled = false;
    
    // Restore normal clock frequency
    set_sys_clock_khz(133000, true); // 133 MHz
    
    // Restore display brightness
    display_set_brightness(127); // 50% brightness
    
    printf("Low power mode disabled\n");
}

/**
 * @brief Handle power state changes
 * @param new_state New power state
 * @return void
 */
void power_handle_state_change(power_state_t new_state)
{
    const char* state_name = power_state_to_string(new_state);
    printf("Power state changed to: %s\n", state_name);
    
    switch (new_state) {
        case POWER_STATE_LOW_BATTERY:
            // Enable low power mode
            power_enable_low_power_mode();
            
            // Show warning message
            ui_show_status("Low Battery Warning", 3000);
            break;
            
        case POWER_STATE_CRITICAL:
            // Keep low power mode enabled
            power_enable_low_power_mode();
            
            // Show critical warning
            ui_show_status("CRITICAL BATTERY!", 5000);
            
            // Start graceful shutdown timer
            printf("WARNING: Critical battery level - system will shutdown soon\n");
            break;
            
        case POWER_STATE_CHARGING:
            // Disable low power mode when charging
            power_disable_low_power_mode();
            
            // Show charging status
            ui_show_status("Charging...", 2000);
            break;
            
        case POWER_STATE_CHARGED:
            ui_show_status("Battery Full", 2000);
            break;
            
        case POWER_STATE_NORMAL:
            // Disable low power mode
            power_disable_low_power_mode();
            break;
    }
}

/**
 * @brief Power down non-essential systems
 * @return void
 */
void power_shutdown_non_essential(void)
{
    printf("Shutting down non-essential systems\n");
    
    // Disable Wi-Fi to save power
    // wifi_stop_ap();
    
    // Reduce display brightness further
    display_set_brightness(32);
    
    // Disable unused peripherals
    // TODO: Implement peripheral power down
}

/**
 * @brief Emergency shutdown sequence
 * @return void
 */
void power_emergency_shutdown(void)
{
    printf("EMERGENCY SHUTDOWN INITIATED\n");
    
    // Save critical data to SD card
    // storage_write_log(xTaskGetTickCount(), "CRITICAL", "Emergency shutdown");
    
    // Turn off display
    display_set_brightness(0);
    
    // Show final message
    display_update_status("SHUTDOWN");
    
    // Disable all peripherals
    power_shutdown_non_essential();
    
    // Enter deep sleep
    printf("Entering deep sleep mode\n");
    sleep_ms(1000); // Allow printf to complete
    
    // TODO: Implement proper deep sleep
    while (1) {
        __wfi(); // Wait for interrupt (low power)
    }
}

/**
 * @brief Convert power state to string representation
 * @param state Power state enum
 * @return String representation of power state
 */
static const char* power_state_to_string(power_state_t state)
{
    switch (state) {
        case POWER_STATE_NORMAL:
            return "NORMAL";
        case POWER_STATE_LOW_BATTERY:
            return "LOW";
        case POWER_STATE_CRITICAL:
            return "CRITICAL";
        case POWER_STATE_CHARGING:
            return "CHARGING";
        case POWER_STATE_CHARGED:
            return "CHARGED";
        default:
            return "UNKNOWN";
    }
}
