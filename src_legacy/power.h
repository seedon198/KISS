/**
 * @file power.h
 * @brief Battery monitoring and power management
 * @author KISS Fuzzer Team
 * @date 2025
 */

#ifndef POWER_H
#define POWER_H

#include "kiss_fuzzer.h"

// Power result codes
typedef enum {
    POWER_OK = 0,
    POWER_ERROR_INIT,
    POWER_ERROR_ADC,
    POWER_ERROR_CONFIG
} power_result_t;

// Battery configuration
#define BATTERY_MIN_VOLTAGE    3.0f                // Minimum safe voltage (V)
#define BATTERY_MAX_VOLTAGE    4.2f                // Maximum voltage when fully charged (V)
#define BATTERY_ADC_CHANNEL    0                   // ADC channel for battery monitoring
#define ADC_CONVERSION_FACTOR  (3.3f / (1 << 12))  // 12-bit ADC, 3.3V reference
#define VOLTAGE_DIVIDER_FACTOR 2.0f                // Voltage divider ratio

// Power states
typedef enum {
    POWER_STATE_NORMAL,
    POWER_STATE_LOW_BATTERY,
    POWER_STATE_CRITICAL,
    POWER_STATE_CHARGING,
    POWER_STATE_CHARGED
} power_state_t;

// Power status structure
typedef struct {
    float         battery_voltage;
    uint8_t       battery_percent;
    bool          is_charging;
    bool          is_usb_connected;
    power_state_t state;
    uint32_t      last_update;
} power_status_t;

/**
 * @brief Initialize power monitoring system
 * @return Power result code
 */
power_result_t power_init(void);

/**
 * @brief Power monitoring task function
 * @param pvParameters FreeRTOS task parameters
 * @return void
 */
void power_task(void *pvParameters);

/**
 * @brief Read current battery voltage
 * @return Battery voltage in volts
 */
float power_read_battery_voltage(void);

/**
 * @brief Convert battery voltage to percentage
 * @param voltage Battery voltage in volts
 * @return Battery percentage (0-100)
 */
uint8_t power_voltage_to_percent(float voltage);

/**
 * @brief Check if device is charging
 * @return true if charging, false otherwise
 */
bool power_is_charging(void);

/**
 * @brief Check if USB is connected
 * @return true if USB connected, false otherwise
 */
bool power_is_usb_connected(void);

/**
 * @brief Get current power status
 * @param status Pointer to store power status
 * @return void
 */
void power_get_status(power_status_t *status);

/**
 * @brief Get current battery voltage
 * @return Battery voltage in volts
 */
float power_get_voltage(void);

/**
 * @brief Get battery percentage
 * @return Battery percentage (0-100)
 */
uint8_t power_get_battery_percent(void);

/**
 * @brief Check if battery is low
 * @return true if battery is low, false otherwise
 */
bool power_is_battery_low(void);

/**
 * @brief Check if battery is critical
 * @return true if battery is critical, false otherwise
 */
bool power_is_battery_critical(void);

/**
 * @brief Enable low power mode
 * @return void
 */
void power_enable_low_power_mode(void);

/**
 * @brief Disable low power mode
 * @return void
 */
void power_disable_low_power_mode(void);

/**
 * @brief Handle power state changes
 * @param new_state New power state
 * @return void
 */
void power_handle_state_change(power_state_t new_state);

/**
 * @brief Power down non-essential systems
 * @return void
 */
void power_shutdown_non_essential(void);

/**
 * @brief Emergency shutdown sequence
 * @return void
 */
void power_emergency_shutdown(void);

// Internal helper functions
static void        power_calibrate_adc(void);
static void        power_update_status(void);
static const char *power_state_to_string(power_state_t state);

#endif  // POWER_H
