/**
 * @file power.h
 * @brief Power management system for KISS Fuzzer
 * @author KISS Fuzzer Team
 * @date 2025
 * @version 0.5.0
 */

#ifndef POWER_H
#define POWER_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"

// Power monitoring pin assignments
#define POWER_BATTERY_ADC_PIN    26   // ADC0 - Battery voltage divider
#define POWER_CHARGING_PIN       17   // Charging status from power-path IC
#define POWER_USB_DETECT_PIN     18   // USB power detection

// Battery parameters (adjust based on actual battery)
#define POWER_BATTERY_MAX_MV     4200  // 4.2V fully charged Li-ion
#define POWER_BATTERY_MIN_MV     3300  // 3.3V empty (with safety margin)
#define POWER_USB_MAX_MV         5250  // 5.25V max USB voltage
#define POWER_USB_MIN_MV         4750  // 4.75V min USB voltage

// Voltage divider constants (adjust based on hardware design)
#define POWER_ADC_VREF_MV        3300  // 3.3V ADC reference
#define POWER_VOLTAGE_DIVIDER    2     // 1:1 voltage divider (adjust if needed)

// Update intervals
#define POWER_UPDATE_INTERVAL_MS 1000  // Update every second

/**
 * @brief Power status structure
 */
typedef struct {
    uint16_t battery_mv;        // Battery voltage in millivolts
    uint8_t battery_percent;    // Battery percentage (0-100)
    bool is_charging;           // Charging status
    bool usb_connected;         // USB power connected
    bool low_battery;           // Low battery warning
    bool critical_battery;      // Critical battery shutdown needed
    uint32_t last_update_time;  // Last update timestamp
} power_status_t;

/**
 * @brief Power state enumeration
 */
typedef enum {
    POWER_STATE_UNKNOWN,
    POWER_STATE_BATTERY_ONLY,
    POWER_STATE_USB_POWERED,
    POWER_STATE_CHARGING,
    POWER_STATE_CHARGED
} power_state_t;

/**
 * @brief Initialize the power management system
 * @return true if successful, false otherwise
 */
bool power_init(void);

/**
 * @brief Update power status (should be called regularly)
 */
void power_update(void);

/**
 * @brief Get current power status
 * @return Power status structure
 */
power_status_t power_get_status(void);

/**
 * @brief Get current power state
 * @return Power state enumeration
 */
power_state_t power_get_state(void);

/**
 * @brief Get battery voltage in millivolts
 * @return Battery voltage in mV
 */
uint16_t power_get_battery_voltage(void);

/**
 * @brief Get battery percentage (0-100)
 * @return Battery percentage
 */
uint8_t power_get_battery_percent(void);

/**
 * @brief Check if USB power is connected
 * @return true if USB connected
 */
bool power_is_usb_connected(void);

/**
 * @brief Check if battery is charging
 * @return true if charging
 */
bool power_is_charging(void);

/**
 * @brief Check if battery is low
 * @return true if low battery
 */
bool power_is_low_battery(void);

/**
 * @brief Check if battery is critically low
 * @return true if critical battery
 */
bool power_is_critical_battery(void);

/**
 * @brief Get human-readable power state name
 * @param state Power state
 * @return String name of the state
 */
const char* power_state_name(power_state_t state);

/**
 * @brief Get formatted battery status string
 * @param buffer Buffer to write to
 * @param buffer_size Size of buffer
 * @return Formatted string with battery info
 */
void power_get_status_string(char* buffer, size_t buffer_size);

#endif // POWER_H
