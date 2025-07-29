/**
 * @file jtag.h
 * @brief JTAG/SWD scanning and exploitation engine using PIO
 * @author KISS Fuzzer Team
 * @date 2025
 */

#ifndef JTAG_H
#define JTAG_H

#include "kiss_fuzzer.h"

// JTAG TAP states
typedef enum {
    TAP_RESET,
    TAP_IDLE,
    TAP_SELECT_DR,
    TAP_CAPTURE_DR,
    TAP_SHIFT_DR,
    TAP_EXIT1_DR,
    TAP_PAUSE_DR,
    TAP_EXIT2_DR,
    TAP_UPDATE_DR,
    TAP_SELECT_IR,
    TAP_CAPTURE_IR,
    TAP_SHIFT_IR,
    TAP_EXIT1_IR,
    TAP_PAUSE_IR,
    TAP_EXIT2_IR,
    TAP_UPDATE_IR
} jtag_tap_state_t;

// JTAG configuration
typedef struct {
    uint8_t tck_pin;
    uint8_t tms_pin;
    uint8_t tdi_pin;
    uint8_t tdo_pin;
    uint8_t trst_pin;
    uint32_t clock_freq;
    bool use_trst;
} jtag_config_t;

// Device information
typedef struct {
    uint32_t idcode;
    uint8_t ir_length;
    uint32_t manufacturer_id;
    uint32_t part_number;
    uint32_t version;
    char description[64];
} jtag_device_t;

// Scan results
typedef struct {
    uint8_t device_count;
    jtag_device_t devices[16];
    uint8_t total_ir_length;
    bool chain_valid;
} jtag_scan_result_t;

/**
 * @brief Initialize JTAG engine and PIO programs
 * @return true if initialization successful, false otherwise
 */
bool jtag_init(void);

/**
 * @brief Configure JTAG pins and clock frequency
 * @param config JTAG configuration structure
 * @return true if configuration successful, false otherwise
 */
bool jtag_configure(const jtag_config_t* config);

/**
 * @brief JTAG engine task function
 * @param pvParameters FreeRTOS task parameters
 * @return void
 */
void jtag_task(void* pvParameters);

/**
 * @brief Scan for JTAG devices on the chain
 * @param result Pointer to store scan results
 * @return true if scan successful, false otherwise
 */
bool jtag_scan_chain(jtag_scan_result_t* result);

/**
 * @brief Auto-detect JTAG pins on target
 * @param pins Array to store detected pin assignments
 * @return true if pins detected, false otherwise
 */
bool jtag_detect_pins(uint8_t pins[4]);

/**
 * @brief Read IDCODE from JTAG device
 * @param device_index Device index in chain (0-based)
 * @param idcode Pointer to store IDCODE value
 * @return true if read successful, false otherwise
 */
bool jtag_read_idcode(uint8_t device_index, uint32_t* idcode);

/**
 * @brief Perform memory dump via JTAG
 * @param start_addr Starting memory address
 * @param length Number of bytes to dump
 * @param data Buffer to store dumped data
 * @return Number of bytes successfully dumped
 */
uint32_t jtag_memory_dump(uint32_t start_addr, uint32_t length, uint8_t* data);

/**
 * @brief Perform boundary scan test
 * @param device_index Device index in chain
 * @return true if boundary scan successful, false otherwise
 */
bool jtag_boundary_scan(uint8_t device_index);

/**
 * @brief Reset JTAG TAP to Test-Logic-Reset state
 * @return void
 */
void jtag_tap_reset(void);

/**
 * @brief Navigate JTAG TAP state machine
 * @param target_state Target TAP state
 * @return Current TAP state
 */
jtag_tap_state_t jtag_goto_state(jtag_tap_state_t target_state);

/**
 * @brief Shift data through JTAG DR or IR
 * @param data Data to shift out
 * @param length Number of bits to shift
 * @param is_instruction true for IR shift, false for DR shift
 * @return Data shifted in from TDO
 */
uint32_t jtag_shift_data(uint32_t data, uint8_t length, bool is_instruction);

/**
 * @brief Set JTAG clock frequency
 * @param freq_hz Frequency in Hz (max ~10MHz)
 * @return Actual frequency set
 */
uint32_t jtag_set_frequency(uint32_t freq_hz);

/**
 * @brief Get device information from IDCODE
 * @param idcode Device IDCODE
 * @param device Pointer to store device information
 * @return true if device recognized, false otherwise
 */
bool jtag_get_device_info(uint32_t idcode, jtag_device_t* device);

#endif // JTAG_H
