/**
 * @file jtag_fuzzer.h
 * @brief Advanced JTAG Fuzzing Engine for KISS Fuzzer
 * @author KISS Fuzzer Team
 * @date 2025
 * @version 0.9.0
 */

#ifndef JTAG_FUZZER_H
#define JTAG_FUZZER_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "jtag.h"

// Fuzzing configuration constants
#define FUZZ_MAX_PAYLOAD_SIZE       1024    // Maximum fuzzing payload size
#define FUZZ_MAX_ITERATIONS         10000   // Maximum fuzzing iterations per session
#define FUZZ_DEFAULT_TIMEOUT_MS     1000    // Default timeout for operations
#define FUZZ_MAX_PIN_COMBINATIONS   64      // Maximum pin combinations to try

// Fuzzing operation types
typedef enum {
    FUZZ_OP_IDCODE_SCAN = 0,        // Scan for IDCODE patterns
    FUZZ_OP_INSTRUCTION_SCAN,       // Scan instruction register space
    FUZZ_OP_DATA_REGISTER_SCAN,     // Scan data register space
    FUZZ_OP_BOUNDARY_SCAN,          // IEEE 1149.1 boundary scan
    FUZZ_OP_MEMORY_PROBE,           // Memory access probing
    FUZZ_OP_DEBUG_ACCESS,           // Debug interface access
    FUZZ_OP_CUSTOM_SEQUENCE,        // Custom TAP state sequences
    FUZZ_OP_PIN_DISCOVERY,          // Automatic pin discovery
    FUZZ_OP_VOLTAGE_GLITCH,         // Voltage glitching during operations
    FUZZ_OP_TIMING_ATTACK,          // Timing-based attacks
    FUZZ_OP_COUNT
} fuzz_operation_t;

// Fuzzing strategy types
typedef enum {
    FUZZ_STRATEGY_SEQUENTIAL = 0,   // Sequential pattern testing
    FUZZ_STRATEGY_RANDOM,           // Random pattern generation
    FUZZ_STRATEGY_GENETIC,          // Genetic algorithm-based
    FUZZ_STRATEGY_SMART,            // AI-guided smart fuzzing
    FUZZ_STRATEGY_DICTIONARY,       // Dictionary-based patterns
    FUZZ_STRATEGY_COUNT
} fuzz_strategy_t;

// Fuzzing result status
typedef enum {
    FUZZ_RESULT_SUCCESS = 0,        // Operation completed successfully
    FUZZ_RESULT_NO_RESPONSE,        // No response from target
    FUZZ_RESULT_TIMEOUT,            // Operation timed out
    FUZZ_RESULT_ANOMALY,            // Anomalous response detected
    FUZZ_RESULT_POTENTIAL_VULN,     // Potential vulnerability found
    FUZZ_RESULT_ERROR,              // Error occurred
    FUZZ_RESULT_INTERRUPTED         // Operation was interrupted
} fuzz_result_t;

// Pin discovery result
typedef struct {
    uint8_t tck_pin;                // Test Clock pin
    uint8_t tms_pin;                // Test Mode Select pin
    uint8_t tdi_pin;                // Test Data Input pin
    uint8_t tdo_pin;                // Test Data Output pin
    uint8_t confidence;             // Confidence level (0-100)
    bool verified;                  // Pin assignment verified
    uint32_t idcode_found;          // IDCODE discovered during verification
} pin_discovery_result_t;

// Fuzzing payload structure
typedef struct {
    uint8_t data[FUZZ_MAX_PAYLOAD_SIZE];  // Payload data
    uint16_t length;                      // Payload length in bits
    uint32_t checksum;                    // Payload checksum for validation
    bool is_instruction;                  // True if instruction register payload
} fuzz_payload_t;

// Fuzzing session configuration
typedef struct {
    fuzz_operation_t operation;           // Type of fuzzing operation
    fuzz_strategy_t strategy;             // Fuzzing strategy to use
    uint32_t max_iterations;              // Maximum iterations
    uint32_t timeout_ms;                  // Timeout per operation (ms)
    uint32_t clock_freq_hz;               // JTAG clock frequency
    bool enable_glitching;                // Enable voltage glitching
    bool enable_logging;                  // Enable detailed logging
    bool auto_adapt;                      // Auto-adapt based on responses
    uint8_t target_voltage;               // Target voltage (18=1.8V, 33=3.3V, 50=5.0V)
} fuzz_config_t;

// Fuzzing session statistics
typedef struct {
    uint32_t total_iterations;            // Total iterations performed
    uint32_t successful_ops;              // Successful operations
    uint32_t timeouts;                    // Number of timeouts
    uint32_t anomalies;                   // Anomalous responses
    uint32_t potential_vulns;             // Potential vulnerabilities
    uint32_t start_time;                  // Session start time (ticks)
    uint32_t end_time;                    // Session end time (ticks)
    uint32_t bytes_sent;                  // Total bytes transmitted
    uint32_t bytes_received;              // Total bytes received
} fuzz_stats_t;

// Fuzzing session result
typedef struct {
    fuzz_result_t result;                 // Overall result
    fuzz_stats_t stats;                   // Session statistics
    char description[128];                // Human-readable description
    uint8_t findings_count;               // Number of findings
    uint32_t interesting_responses[16];   // Interesting response patterns
    pin_discovery_result_t pins;          // Pin discovery results (if applicable)
} fuzz_session_result_t;

// Advanced JTAG fuzzing engine functions

/**
 * @brief Initialize the JTAG fuzzing engine
 * @return true if successful
 */
bool jtag_fuzzer_init(void);

/**
 * @brief Deinitialize the JTAG fuzzing engine
 */
void jtag_fuzzer_deinit(void);

/**
 * @brief Start a fuzzing session
 * @param config Fuzzing configuration
 * @param result Pointer to store results
 * @return true if session started successfully
 */
bool jtag_fuzzer_start_session(const fuzz_config_t* config, fuzz_session_result_t* result);

/**
 * @brief Stop current fuzzing session
 */
void jtag_fuzzer_stop_session(void);

/**
 * @brief Check if fuzzing session is active
 * @return true if session is active
 */
bool jtag_fuzzer_is_active(void);

/**
 * @brief Get current session progress (0-100)
 * @return Progress percentage
 */
uint8_t jtag_fuzzer_get_progress(void);

/**
 * @brief Automatic JTAG pin discovery
 * @param result Pointer to store discovery results
 * @return true if pins discovered successfully
 */
bool jtag_fuzzer_discover_pins(pin_discovery_result_t* result);

/**
 * @brief Scan for JTAG devices on the chain
 * @param devices Array to store found devices
 * @param max_devices Maximum number of devices to find
 * @param found_count Pointer to store actual count found
 * @return true if scan successful
 */
bool jtag_fuzzer_scan_chain(jtag_device_t* devices, uint8_t max_devices, uint8_t* found_count);

/**
 * @brief Perform instruction register fuzzing
 * @param config Fuzzing configuration
 * @param result Pointer to store results
 * @return true if successful
 */
bool jtag_fuzzer_fuzz_ir(const fuzz_config_t* config, fuzz_session_result_t* result);

/**
 * @brief Perform data register fuzzing
 * @param config Fuzzing configuration
 * @param result Pointer to store results
 * @return true if successful
 */
bool jtag_fuzzer_fuzz_dr(const fuzz_config_t* config, fuzz_session_result_t* result);

/**
 * @brief Perform boundary scan fuzzing
 * @param config Fuzzing configuration
 * @param result Pointer to store results
 * @return true if successful
 */
bool jtag_fuzzer_boundary_scan(const fuzz_config_t* config, fuzz_session_result_t* result);

/**
 * @brief Perform memory access probing
 * @param config Fuzzing configuration
 * @param start_addr Starting memory address
 * @param end_addr Ending memory address
 * @param result Pointer to store results
 * @return true if successful
 */
bool jtag_fuzzer_memory_probe(const fuzz_config_t* config, uint32_t start_addr, 
                              uint32_t end_addr, fuzz_session_result_t* result);

/**
 * @brief Generate fuzzing payload based on strategy
 * @param strategy Fuzzing strategy
 * @param iteration Current iteration number
 * @param payload Pointer to store generated payload
 * @return true if payload generated successfully
 */
bool jtag_fuzzer_generate_payload(fuzz_strategy_t strategy, uint32_t iteration, fuzz_payload_t* payload);

/**
 * @brief Analyze response for anomalies
 * @param expected Expected response pattern
 * @param actual Actual response received
 * @param length Response length in bytes
 * @return true if anomaly detected
 */
bool jtag_fuzzer_detect_anomaly(const uint8_t* expected, const uint8_t* actual, uint16_t length);

/**
 * @brief Get human-readable operation name
 * @param operation Fuzzing operation
 * @return String name of the operation
 */
const char* jtag_fuzzer_operation_name(fuzz_operation_t operation);

/**
 * @brief Get human-readable strategy name
 * @param strategy Fuzzing strategy
 * @return String name of the strategy
 */
const char* jtag_fuzzer_strategy_name(fuzz_strategy_t strategy);

/**
 * @brief Get default fuzzing configuration
 * @param config Pointer to store default configuration
 */
void jtag_fuzzer_get_default_config(fuzz_config_t* config);

/**
 * @brief Validate fuzzing configuration
 * @param config Configuration to validate
 * @return true if configuration is valid
 */
bool jtag_fuzzer_validate_config(const fuzz_config_t* config);

#endif // JTAG_FUZZER_H
