/**
 * @file jtag.h
 * @brief Basic JTAG engine using RP2040 PIO for KISS Fuzzer
 * @author KISS Fuzzer Team
 * @date 2025
 * @version 0.6.0
 */

#ifndef JTAG_H
#define JTAG_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/gpio.h"

// JTAG pin assignments (configurable based on hardware design)
#define JTAG_TCK_PIN    2   // Test Clock
#define JTAG_TMS_PIN    3   // Test Mode Select  
#define JTAG_TDI_PIN    4   // Test Data In
#define JTAG_TDO_PIN    5   // Test Data Out
#define JTAG_TRST_PIN   6   // Test Reset (optional)
#define JTAG_SRST_PIN   7   // System Reset (optional)

// JTAG timing configuration
#define JTAG_MAX_CLOCK_HZ    10000000  // 10 MHz maximum
#define JTAG_DEFAULT_CLOCK_HZ 1000000  // 1 MHz default
#define JTAG_MIN_CLOCK_HZ    1000      // 1 kHz minimum

// JTAG TAP states
typedef enum {
    TAP_UNKNOWN = -1,
    TAP_TEST_LOGIC_RESET = 0,
    TAP_RUN_TEST_IDLE,
    TAP_SELECT_DR_SCAN,
    TAP_CAPTURE_DR,
    TAP_SHIFT_DR,
    TAP_EXIT1_DR,
    TAP_PAUSE_DR,
    TAP_EXIT2_DR,
    TAP_UPDATE_DR,
    TAP_SELECT_IR_SCAN,
    TAP_CAPTURE_IR,
    TAP_SHIFT_IR,
    TAP_EXIT1_IR,
    TAP_PAUSE_IR,
    TAP_EXIT2_IR,
    TAP_UPDATE_IR
} jtag_tap_state_t;

// JTAG configuration structure
typedef struct {
    uint8_t tck_pin;
    uint8_t tms_pin;
    uint8_t tdi_pin;
    uint8_t tdo_pin;
    uint8_t trst_pin;
    uint8_t srst_pin;
    uint32_t clock_hz;
    bool trst_active_low;
    bool srst_active_low;
} jtag_config_t;

// JTAG device information
typedef struct {
    uint32_t idcode;
    uint8_t ir_length;
    char manufacturer[32];
    char device_name[32];
    bool identified;
} jtag_device_t;

// Scan chain information
typedef struct {
    jtag_device_t devices[8];  // Support up to 8 devices in chain
    uint8_t device_count;
    uint8_t total_ir_length;
    bool valid;
} jtag_chain_t;

/**
 * @brief Initialize the JTAG engine
 * @param config JTAG configuration (NULL for defaults)
 * @return true if successful, false otherwise
 */
bool jtag_init(const jtag_config_t* config);

/**
 * @brief Deinitialize the JTAG engine
 */
void jtag_deinit(void);

/**
 * @brief Set JTAG clock frequency
 * @param frequency_hz Clock frequency in Hz
 * @return true if successful, false if out of range
 */
bool jtag_set_clock(uint32_t frequency_hz);

/**
 * @brief Get current JTAG clock frequency
 * @return Current clock frequency in Hz
 */
uint32_t jtag_get_clock(void);

/**
 * @brief Reset JTAG TAP to Test-Logic-Reset state
 */
void jtag_reset(void);

/**
 * @brief Move to specific TAP state
 * @param target_state Target TAP state
 * @return true if successful, false if invalid transition
 */
bool jtag_goto_state(jtag_tap_state_t target_state);

/**
 * @brief Get current TAP state
 * @return Current TAP state
 */
jtag_tap_state_t jtag_get_state(void);

/**
 * @brief Scan JTAG chain to detect devices
 * @param chain Pointer to chain structure to fill
 * @return Number of devices found
 */
uint8_t jtag_scan_chain(jtag_chain_t* chain);

/**
 * @brief Read IDCODE from device
 * @param device_index Device index in chain (0-based)
 * @return IDCODE value, 0 if error
 */
uint32_t jtag_read_idcode(uint8_t device_index);

/**
 * @brief Shift data through DR (Data Register)
 * @param data_out Data to shift out (TDI)
 * @param data_in Buffer for data shifted in (TDO)
 * @param bit_count Number of bits to shift
 * @param goto_state TAP state to enter after shift
 * @return true if successful
 */
bool jtag_shift_dr(const uint8_t* data_out, uint8_t* data_in, 
                   uint16_t bit_count, jtag_tap_state_t goto_state);

/**
 * @brief Shift data through IR (Instruction Register)
 * @param data_out Data to shift out (TDI)
 * @param data_in Buffer for data shifted in (TDO)
 * @param bit_count Number of bits to shift
 * @param goto_state TAP state to enter after shift
 * @return true if successful
 */
bool jtag_shift_ir(const uint8_t* data_out, uint8_t* data_in,
                   uint16_t bit_count, jtag_tap_state_t goto_state);

/**
 * @brief Perform basic connectivity test
 * @return true if JTAG signals appear to be connected
 */
bool jtag_test_connectivity(void);

/**
 * @brief Get human-readable TAP state name
 * @param state TAP state
 * @return String name of the state
 */
const char* jtag_state_name(jtag_tap_state_t state);

/**
 * @brief Identify device from IDCODE
 * @param idcode IDCODE value
 * @param device Pointer to device structure to fill
 * @return true if device identified
 */
bool jtag_identify_device(uint32_t idcode, jtag_device_t* device);

/**
 * @brief Get formatted scan results string
 * @param chain Pointer to scan chain results
 * @param buffer Buffer to write to
 * @param buffer_size Size of buffer
 */
void jtag_get_scan_string(const jtag_chain_t* chain, char* buffer, size_t buffer_size);

#endif // JTAG_H
