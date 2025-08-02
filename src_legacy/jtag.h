/**
 * @file jtag.h
 * @brief JTAG/SWD scanning and exploitation engine using PIO
 * @author KISS Fuzzer Team
 * @date 2025
 */

#ifndef JTAG_H
#define JTAG_H

#include <stdint.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "FreeRTOS.h"
#include "queue.h"

// JTAG device information
struct jtag_device_s {
    uint32_t idcode;
    uint8_t  ir_length;
    uint32_t manufacturer_id;
    uint32_t part_number;
    uint32_t version;
    char     manufacturer[32];      // Manufacturer name string
    char     device_name[32];       // Device name string
    char     description[64];       // Full description
    uint32_t boundary_length;       // Boundary scan chain length
};

// JTAG result codes
typedef enum {
    JTAG_OK = 0,
    JTAG_ERROR,
    JTAG_ERROR_INVALID_PARAM,
    JTAG_ERROR_NOT_ACTIVE,
    JTAG_ERROR_QUEUE_FULL,
    JTAG_ERROR_TIMEOUT,
    JTAG_ERROR_HARDWARE,
    JTAG_ERROR_NO_DEVICE
} jtag_result_t;

// JTAG fuzzing pattern types
typedef enum {
    JTAG_FUZZ_RANDOM,
    JTAG_FUZZ_SEQUENTIAL,
    JTAG_FUZZ_PATTERN
} jtag_fuzz_pattern_t;

// JTAG fuzzing configuration
typedef struct {
    uint32_t start_address;
    uint32_t length;
    uint32_t iterations;
    jtag_fuzz_pattern_t pattern_type;
    uint32_t pattern_value;
    uint32_t delay_ms;
} jtag_fuzz_config_t;

// JTAG fuzzing state
typedef struct {
    bool active;
    uint32_t current_iteration;
    uint32_t current_address;
    uint32_t errors_found;
    uint32_t start_time;
} jtag_fuzz_state_t;

// JTAG glitch configuration
typedef struct {
    uint32_t duration_us;
    uint32_t delay_us;
    float voltage;
    uint32_t repetitions;
} jtag_glitch_config_t;

// JTAG security analysis structure
typedef struct {
    bool debug_enabled;
    bool jtag_locked;
    bool secure_boot;
    uint32_t protection_level;
    uint32_t accessible_devices;
    uint32_t security_level;
    uint32_t security_features[16];    // Security features per device
    bool debug_enabled_array[16];      // Debug enabled per device
    bool bypass_accessible[16];        // Bypass accessible per device
    uint32_t manufacturer_ids[16];     // Manufacturer IDs per device
    uint32_t timestamp;               // Analysis timestamp
    char analysis_report[512];
} jtag_security_analysis_t;

// JTAG boundary scan structure
typedef struct {
    uint32_t pin_count;
    uint8_t *pin_states;
    uint32_t scan_length;
    bool scan_valid;
    uint32_t device_index;
    uint32_t cell_count;
    uint32_t timestamp;
    uint32_t input_pins;
    uint32_t output_pins;
    uint32_t control_pins;
    uint32_t cell_data[128];  // Boundary scan cell data
} jtag_boundary_scan_t;

// Forward declaration for jtag_device_t (defined later)
typedef struct jtag_device_s jtag_device_t;

// Scan results structure (moved here to be available for jtag_event_t)
typedef struct {
    uint8_t       device_count;
    jtag_device_t devices[16];
    uint8_t       total_ir_length;
    bool          chain_valid;
} jtag_scan_result_t;

// JTAG event types
typedef enum {
    JTAG_EVENT_START_FUZZ,
    JTAG_EVENT_STOP_FUZZ,
    JTAG_EVENT_SCAN_COMPLETE,
    JTAG_EVENT_ERROR
} jtag_event_type_t;

// JTAG event structure
typedef struct {
    jtag_event_type_t type;
    union {
        jtag_fuzz_config_t fuzz_config;
        jtag_scan_result_t scan_result;
        jtag_result_t error_code;
    } data;
} jtag_event_t;

// JTAG state constants
#define JTAG_STATE_SHIFT_DR             TAP_SHIFT_DR
#define JTAG_STATE_SHIFT_IR             TAP_SHIFT_IR

// JTAG error codes
#define JTAG_ERROR_DEVICE_NOT_FOUND     JTAG_ERROR_NO_DEVICE

// JTAG boundary scan constants
#define JTAG_MAX_BOUNDARY_CELLS         4096

// Queue handles
extern QueueHandle_t jtag_event_queue;

// Global JTAG state variables
extern jtag_scan_result_t current_chain;
extern jtag_fuzz_config_t current_fuzz_config;
extern jtag_fuzz_state_t fuzz_state;

// Hardware pin definitions
#define GLITCH_MOSFET_PIN 21

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
    uint8_t  tck_pin;
    uint8_t  tms_pin;
    uint8_t  tdi_pin;
    uint8_t  tdo_pin;
    uint8_t  trst_pin;
    uint32_t clock_freq;
    bool     use_trst;
} jtag_config_t;

// JTAG security feature flags
#define JTAG_SEC_ENCRYPTED_BITSTREAM    0x01
#define JTAG_SEC_SECURE_DEVICE_MANAGER  0x02
#define JTAG_SEC_TRUSTZONE              0x04
#define JTAG_SEC_SECURE_BOOT            0x08
#define JTAG_SEC_DEBUG_PROTECTION       0x10

// JTAG security levels
#define JTAG_SEC_LEVEL_LOW              0
#define JTAG_SEC_LEVEL_MEDIUM           1
#define JTAG_SEC_LEVEL_HIGH             2

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
bool jtag_configure(const jtag_config_t *config);

/**
 * @brief JTAG engine task function
 * @param pvParameters FreeRTOS task parameters
 * @return void
 */
void jtag_task(void *pvParameters);

/**
 * @brief Scan for JTAG devices on the chain
 * @param result Pointer to store scan results
 * @return true if scan successful, false otherwise
 */
bool jtag_scan_chain(jtag_scan_result_t *result);

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
bool jtag_read_idcode(uint8_t device_index, uint32_t *idcode);

/**
 * @brief Perform memory dump via JTAG
 * @param start_addr Starting memory address
 * @param length Number of bytes to dump
 * @param data Buffer to store dumped data
 * @return Number of bytes successfully dumped
 */
uint32_t jtag_memory_dump(uint32_t start_addr, uint32_t length, uint8_t *data);

/**
 * @brief Perform boundary scan test
 * @param device_index Device index in chain
 * @param scan_result Pointer to store scan results
 * @return JTAG result code
 */
jtag_result_t jtag_boundary_scan(uint8_t device_index, jtag_boundary_scan_t *scan_result);

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
 * @brief Shift data through JTAG instruction register (IR)
 * @param data Pointer to instruction data
 * @param length Number of bits to shift
 * @param device_index Target device index in chain
 * @return JTAG operation result
 */
jtag_result_t jtag_shift_ir(uint32_t *data, uint8_t length, uint8_t device_index);

/**
 * @brief Shift data through JTAG data register (DR)
 * @param data Pointer to data buffer
 * @param length Number of bits to shift
 * @param device_index Target device index in chain
 * @return JTAG operation result
 */
jtag_result_t jtag_shift_dr(uint8_t *data, uint32_t length, uint8_t device_index);

/**
 * @brief Read device IDCODE
 * @param device_index Device index in chain
 * @param idcode Pointer to store IDCODE
 * @return JTAG operation result
 */
jtag_result_t jtag_read_device_idcode(uint8_t device_index, uint32_t *idcode);

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
bool jtag_get_device_info(uint32_t idcode, jtag_device_t *device);

/**
 * @brief Start memory fuzzing operation
 * @param fuzz_config Fuzzing configuration
 * @return JTAG result code
 */
jtag_result_t jtag_start_memory_fuzz(const jtag_fuzz_config_t *fuzz_config);

/**
 * @brief Stop memory fuzzing operation
 * @return JTAG result code
 */
jtag_result_t jtag_stop_memory_fuzz(void);

/**
 * @brief Get current fuzzing state
 * @return Pointer to fuzzing state structure
 */
const jtag_fuzz_state_t *jtag_get_fuzz_state(void);

/**
 * @brief Perform power glitch attack
 * @param glitch_config Glitch configuration
 * @return JTAG result code
 */
jtag_result_t jtag_power_glitch(const jtag_glitch_config_t *glitch_config);

/**
 * @brief Perform security analysis
 * @param analysis_result Pointer to store analysis results
 * @return JTAG result code
 */
jtag_result_t jtag_security_analysis(jtag_security_analysis_t *analysis_result);

#endif  // JTAG_H
