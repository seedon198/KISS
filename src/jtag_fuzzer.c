/**
 * @file jtag_fuzzer.c
 * @brief Advanced JTAG Fuzzing Engine Implementation
 * @author KISS Fuzzer Team
 * @date 2025
 * @version 0.9.0
 */

#include "jtag_fuzzer.h"
#include "storage.h"
#include "power.h"
#include <stdlib.h>
#include <string.h>

// Static variables for fuzzing state
static bool fuzzer_ready = false;
static bool session_active = false;
static fuzz_config_t current_config = {0};
static fuzz_session_result_t* current_result = NULL;
static uint32_t current_iteration = 0;
static TaskHandle_t fuzzer_task_handle = NULL;

// Fuzzing task function (internal)
static void fuzzer_task(void* pvParameters);

// Dictionary of common JTAG instructions for dictionary-based fuzzing
static const uint32_t instruction_dictionary[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,  // Standard instructions
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
    0xFF, 0xFE, 0xFD, 0xFC, 0xFB, 0xFA, 0xF9, 0xF8,  // Common boundary values
    0xAA, 0x55, 0xCC, 0x33, 0x0F, 0xF0, 0x00, 0xFF   // Pattern values
};

#define INSTRUCTION_DICT_SIZE (sizeof(instruction_dictionary) / sizeof(instruction_dictionary[0]))

/**
 * @brief Initialize the JTAG fuzzing engine
 * @return true if successful
 */
bool jtag_fuzzer_init(void) {
    printf("[JTAG Fuzzer] Initializing advanced fuzzing engine...\n");
    
    // Initialize base JTAG engine first
    if (!jtag_init(NULL)) {
        printf("[JTAG Fuzzer] Failed to initialize base JTAG engine\n");
        return false;
    }
    
    // Initialize random seed
    srand(to_us_since_boot(get_absolute_time()));
    
    // Reset state
    session_active = false;
    current_iteration = 0;
    fuzzer_task_handle = NULL;
    
    fuzzer_ready = true;
    printf("[JTAG Fuzzer] Advanced fuzzing engine initialized\n");
    return true;
}

/**
 * @brief Deinitialize the JTAG fuzzing engine
 */
void jtag_fuzzer_deinit(void) {
    printf("[JTAG Fuzzer] Deinitializing fuzzing engine...\n");
    
    // Stop any active session
    jtag_fuzzer_stop_session();
    
    // Cleanup
    fuzzer_ready = false;
    printf("[JTAG Fuzzer] Fuzzing engine deinitialized\n");
}

/**
 * @brief Start a fuzzing session
 * @param config Fuzzing configuration
 * @param result Pointer to store results
 * @return true if session started successfully
 */
bool jtag_fuzzer_start_session(const fuzz_config_t* config, fuzz_session_result_t* result) {
    if (!fuzzer_ready) {
        printf("[JTAG Fuzzer] Engine not initialized\n");
        return false;
    }
    
    if (session_active) {
        printf("[JTAG Fuzzer] Session already active\n");
        return false;
    }
    
    if (!config || !result) {
        printf("[JTAG Fuzzer] Invalid parameters\n");
        return false;
    }
    
    if (!jtag_fuzzer_validate_config(config)) {
        printf("[JTAG Fuzzer] Invalid configuration\n");
        return false;
    }
    
    printf("[JTAG Fuzzer] Starting fuzzing session: %s with %s strategy\n",
           jtag_fuzzer_operation_name(config->operation),
           jtag_fuzzer_strategy_name(config->strategy));
    
    // Store configuration and result pointer
    current_config = *config;
    current_result = result;
    current_iteration = 0;
    
    // Initialize result structure
    memset(result, 0, sizeof(fuzz_session_result_t));
    result->stats.start_time = to_ms_since_boot(get_absolute_time());
    
    // Set target voltage if specified
    if (config->target_voltage > 0) {
        // This would interface with power management
        printf("[JTAG Fuzzer] Setting target voltage to %d.%dV\n", 
               config->target_voltage / 10, config->target_voltage % 10);
    }
    
    // Create fuzzing task
    BaseType_t task_result = xTaskCreate(
        fuzzer_task,
        "JTAG_Fuzzer",
        2048,  // Stack size
        NULL,  // Parameters
        3,     // Priority (medium-high)
        &fuzzer_task_handle
    );
    
    if (task_result != pdPASS) {
        printf("[JTAG Fuzzer] Failed to create fuzzing task\n");
        return false;
    }
    
    session_active = true;
    printf("[JTAG Fuzzer] Fuzzing session started successfully\n");
    return true;
}

/**
 * @brief Stop current fuzzing session
 */
void jtag_fuzzer_stop_session(void) {
    if (!session_active) {
        return;
    }
    
    printf("[JTAG Fuzzer] Stopping fuzzing session...\n");
    
    // Signal task to stop
    session_active = false;
    
    // Wait for task to complete
    if (fuzzer_task_handle) {
        vTaskDelete(fuzzer_task_handle);
        fuzzer_task_handle = NULL;
    }
    
    // Finalize results
    if (current_result) {
        current_result->stats.end_time = to_ms_since_boot(get_absolute_time());
        current_result->stats.total_iterations = current_iteration;
        
        if (current_result->result == FUZZ_RESULT_SUCCESS) {
            snprintf(current_result->description, sizeof(current_result->description),
                    "Session completed: %lu iterations, %lu anomalies found",
                    current_result->stats.total_iterations,
                    current_result->stats.anomalies);
        }
    }
    
    printf("[JTAG Fuzzer] Fuzzing session stopped\n");
}

/**
 * @brief Check if fuzzing session is active
 * @return true if session is active
 */
bool jtag_fuzzer_is_active(void) {
    return session_active;
}

/**
 * @brief Get current session progress (0-100)
 * @return Progress percentage
 */
uint8_t jtag_fuzzer_get_progress(void) {
    if (!session_active || current_config.max_iterations == 0) {
        return 0;
    }
    
    uint32_t progress = (current_iteration * 100) / current_config.max_iterations;
    return (progress > 100) ? 100 : (uint8_t)progress;
}

/**
 * @brief Automatic JTAG pin discovery
 * @param result Pointer to store discovery results
 * @return true if pins discovered successfully
 */
bool jtag_fuzzer_discover_pins(pin_discovery_result_t* result) {
    if (!result) {
        return false;
    }
    
    printf("[JTAG Fuzzer] Starting automatic pin discovery...\n");
    
    memset(result, 0, sizeof(pin_discovery_result_t));
    
    // Define pin ranges to test (based on hardware design)
    uint8_t test_pins[] = {10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22};
    uint8_t num_pins = sizeof(test_pins) / sizeof(test_pins[0]);
    
    uint8_t best_confidence = 0;
    
    // Try different pin combinations
    for (uint8_t tck_idx = 0; tck_idx < num_pins; tck_idx++) {
        for (uint8_t tms_idx = 0; tms_idx < num_pins; tms_idx++) {
            if (tms_idx == tck_idx) continue;
            
            for (uint8_t tdi_idx = 0; tdi_idx < num_pins; tdi_idx++) {
                if (tdi_idx == tck_idx || tdi_idx == tms_idx) continue;
                
                for (uint8_t tdo_idx = 0; tdo_idx < num_pins; tdo_idx++) {
                    if (tdo_idx == tck_idx || tdo_idx == tms_idx || tdo_idx == tdi_idx) continue;
                    
                    // Test this pin combination
                    jtag_config_t test_config = {
                        .tck_pin = test_pins[tck_idx],
                        .tms_pin = test_pins[tms_idx],
                        .tdi_pin = test_pins[tdi_idx],
                        .tdo_pin = test_pins[tdo_idx],
                        .clock_hz = 100000  // Start with low frequency
                    };
                    
                    // Try to initialize with this configuration
                    if (jtag_init(&test_config)) {
                        // Test connectivity
                        if (jtag_test_connectivity()) {                        // Try to read IDCODE
                        uint32_t idcode = jtag_read_idcode(0);  // Read first device
                        if (idcode != 0 && idcode != 0xFFFFFFFF) {
                                uint8_t confidence = 90;  // High confidence if we got valid IDCODE
                                
                                if (confidence > best_confidence) {
                                    result->tck_pin = test_pins[tck_idx];
                                    result->tms_pin = test_pins[tms_idx];
                                    result->tdi_pin = test_pins[tdi_idx];
                                    result->tdo_pin = test_pins[tdo_idx];
                                    result->confidence = confidence;
                                    result->verified = true;
                                    result->idcode_found = idcode;
                                    best_confidence = confidence;
                                    
                                    printf("[JTAG Fuzzer] Found potential JTAG pins: TCK=%d, TMS=%d, TDI=%d, TDO=%d (IDCODE: 0x%08lX)\n",
                                           result->tck_pin, result->tms_pin, result->tdi_pin, result->tdo_pin, idcode);
                                }
                            }
                        }
                    }
                    
                    // Don't spend too much time on pin discovery
                    vTaskDelay(pdMS_TO_TICKS(10));
                }
            }
        }
    }
    
    if (result->confidence > 50) {
        printf("[JTAG Fuzzer] Pin discovery successful with %d%% confidence\n", result->confidence);
        return true;
    } else {
        printf("[JTAG Fuzzer] Pin discovery failed - no valid JTAG interface found\n");
        return false;
    }
}

/**
 * @brief Generate fuzzing payload based on strategy
 * @param strategy Fuzzing strategy
 * @param iteration Current iteration number
 * @param payload Pointer to store generated payload
 * @return true if payload generated successfully
 */
bool jtag_fuzzer_generate_payload(fuzz_strategy_t strategy, uint32_t iteration, fuzz_payload_t* payload) {
    if (!payload) {
        return false;
    }
    
    memset(payload, 0, sizeof(fuzz_payload_t));
    
    switch (strategy) {
        case FUZZ_STRATEGY_SEQUENTIAL:
            // Sequential pattern: increment through values
            payload->length = 8;  // 8-bit instruction
            payload->data[0] = iteration & 0xFF;
            break;
            
        case FUZZ_STRATEGY_RANDOM:
            // Random payload generation
            payload->length = 8 + (rand() % 24);  // 8-32 bits
            for (int i = 0; i < (payload->length + 7) / 8; i++) {
                payload->data[i] = rand() & 0xFF;
            }
            break;
            
        case FUZZ_STRATEGY_DICTIONARY:
            // Use predefined dictionary of known patterns
            if (iteration < INSTRUCTION_DICT_SIZE) {
                payload->length = 8;
                payload->data[0] = instruction_dictionary[iteration] & 0xFF;
            } else {
                // Fall back to random after dictionary exhausted
                payload->length = 8;
                payload->data[0] = rand() & 0xFF;
            }
            break;
            
        case FUZZ_STRATEGY_GENETIC:
            // Simple genetic algorithm - mutate previous successful patterns
            payload->length = 8;
            payload->data[0] = (iteration * 17) ^ 0xAA;  // Simple mutation
            break;
            
        case FUZZ_STRATEGY_SMART:
            // AI-guided smart fuzzing (simplified heuristics)
            payload->length = 8;
            // Focus on boundary values and common patterns
            if (iteration % 4 == 0) payload->data[0] = 0x00;
            else if (iteration % 4 == 1) payload->data[0] = 0xFF;
            else if (iteration % 4 == 2) payload->data[0] = 0xAA;
            else payload->data[0] = 0x55;
            break;
            
        default:
            return false;
    }
    
    // Calculate checksum
    payload->checksum = 0;
    for (int i = 0; i < (payload->length + 7) / 8; i++) {
        payload->checksum ^= payload->data[i];
    }
    
    return true;
}

/**
 * @brief Analyze response for anomalies
 * @param expected Expected response pattern
 * @param actual Actual response received
 * @param length Response length in bytes
 * @return true if anomaly detected
 */
bool jtag_fuzzer_detect_anomaly(const uint8_t* expected, const uint8_t* actual, uint16_t length) {
    if (!expected || !actual || length == 0) {
        return false;
    }
    
    // Check for exact mismatch
    if (memcmp(expected, actual, length) != 0) {
        return true;
    }
    
    // Check for suspicious patterns
    bool all_same = true;
    for (int i = 1; i < length; i++) {
        if (actual[i] != actual[0]) {
            all_same = false;
            break;
        }
    }
    
    // All same bytes might indicate error condition
    if (all_same && (actual[0] == 0x00 || actual[0] == 0xFF)) {
        return true;
    }
    
    return false;
}

/**
 * @brief Fuzzing task implementation
 * @param pvParameters Task parameters (unused)
 */
static void fuzzer_task(void* pvParameters) {
    (void)pvParameters;
    
    printf("[JTAG Fuzzer] Fuzzing task started\n");
    
    while (session_active && current_iteration < current_config.max_iterations) {
        // Generate payload based on strategy
        fuzz_payload_t payload;
        if (!jtag_fuzzer_generate_payload(current_config.strategy, current_iteration, &payload)) {
            current_result->stats.timeouts++;
            current_iteration++;
            continue;
        }
        
        // Execute fuzzing operation based on type
        bool operation_success = false;
        uint8_t response_buffer[64] = {0};
        
        switch (current_config.operation) {
            case FUZZ_OP_INSTRUCTION_SCAN: {
                // Test instruction register with generated payload
                operation_success = jtag_shift_ir(payload.data, response_buffer, 
                                                 payload.length, TAP_RUN_TEST_IDLE);
                break;
            }
            
            case FUZZ_OP_DATA_REGISTER_SCAN: {
                // Test data register with generated payload
                operation_success = jtag_shift_dr(payload.data, response_buffer, 
                                                 payload.length, TAP_RUN_TEST_IDLE);
                break;
            }
            
            case FUZZ_OP_IDCODE_SCAN: {
                // Scan for IDCODE with different approaches
                uint32_t idcode = jtag_read_idcode(0);  // Read first device
                operation_success = (idcode != 0 && idcode != 0xFFFFFFFF);
                if (operation_success) {
                    current_result->interesting_responses[current_result->findings_count % 16] = idcode;
                    current_result->findings_count++;
                }
                break;
            }
            
            default:
                // Placeholder for other operations
                operation_success = true;
                break;
        }
        
        // Update statistics
        if (operation_success) {
            current_result->stats.successful_ops++;
            
            // Check for anomalies if we have expected data
            uint8_t expected_response[64] = {0};  // Would be based on normal operation
            if (jtag_fuzzer_detect_anomaly(expected_response, response_buffer, 
                                          (payload.length + 7) / 8)) {
                current_result->stats.anomalies++;
                
                // Log interesting finding
                if (current_config.enable_logging) {
                    printf("[JTAG Fuzzer] Anomaly detected at iteration %lu\n", current_iteration);
                }
            }
        } else {
            current_result->stats.timeouts++;
        }
        
        current_result->stats.bytes_sent += (payload.length + 7) / 8;
        current_result->stats.bytes_received += (payload.length + 7) / 8;
        
        current_iteration++;
        
        // Small delay to prevent overwhelming the target
        vTaskDelay(pdMS_TO_TICKS(1));
        
        // Check for task cancellation
        if (!session_active) {
            break;
        }
    }
    
    // Session completed
    if (current_result) {
        if (current_iteration >= current_config.max_iterations) {
            current_result->result = FUZZ_RESULT_SUCCESS;
        } else {
            current_result->result = FUZZ_RESULT_INTERRUPTED;
        }
    }
    
    session_active = false;
    printf("[JTAG Fuzzer] Fuzzing task completed\n");
    
    // Task will be deleted by stop_session()
    vTaskDelete(NULL);
}

/**
 * @brief Get human-readable operation name
 * @param operation Fuzzing operation
 * @return String name of the operation
 */
const char* jtag_fuzzer_operation_name(fuzz_operation_t operation) {
    switch (operation) {
        case FUZZ_OP_IDCODE_SCAN: return "IDCODE Scan";
        case FUZZ_OP_INSTRUCTION_SCAN: return "Instruction Register Scan";
        case FUZZ_OP_DATA_REGISTER_SCAN: return "Data Register Scan";
        case FUZZ_OP_BOUNDARY_SCAN: return "Boundary Scan";
        case FUZZ_OP_MEMORY_PROBE: return "Memory Probe";
        case FUZZ_OP_DEBUG_ACCESS: return "Debug Access";
        case FUZZ_OP_CUSTOM_SEQUENCE: return "Custom Sequence";
        case FUZZ_OP_PIN_DISCOVERY: return "Pin Discovery";
        case FUZZ_OP_VOLTAGE_GLITCH: return "Voltage Glitch";
        case FUZZ_OP_TIMING_ATTACK: return "Timing Attack";
        default: return "Unknown";
    }
}

/**
 * @brief Get human-readable strategy name
 * @param strategy Fuzzing strategy
 * @return String name of the strategy
 */
const char* jtag_fuzzer_strategy_name(fuzz_strategy_t strategy) {
    switch (strategy) {
        case FUZZ_STRATEGY_SEQUENTIAL: return "Sequential";
        case FUZZ_STRATEGY_RANDOM: return "Random";
        case FUZZ_STRATEGY_GENETIC: return "Genetic";
        case FUZZ_STRATEGY_SMART: return "Smart";
        case FUZZ_STRATEGY_DICTIONARY: return "Dictionary";
        default: return "Unknown";
    }
}

/**
 * @brief Get default fuzzing configuration
 * @param config Pointer to store default configuration
 */
void jtag_fuzzer_get_default_config(fuzz_config_t* config) {
    if (!config) return;
    
    config->operation = FUZZ_OP_IDCODE_SCAN;
    config->strategy = FUZZ_STRATEGY_DICTIONARY;
    config->max_iterations = 1000;
    config->timeout_ms = FUZZ_DEFAULT_TIMEOUT_MS;
    config->clock_freq_hz = 1000000;  // 1 MHz
    config->enable_glitching = false;
    config->enable_logging = true;
    config->auto_adapt = false;
    config->target_voltage = 33;  // 3.3V
}

/**
 * @brief Validate fuzzing configuration
 * @param config Configuration to validate
 * @return true if configuration is valid
 */
bool jtag_fuzzer_validate_config(const fuzz_config_t* config) {
    if (!config) return false;
    
    if (config->operation >= FUZZ_OP_COUNT) return false;
    if (config->strategy >= FUZZ_STRATEGY_COUNT) return false;
    if (config->max_iterations == 0 || config->max_iterations > FUZZ_MAX_ITERATIONS) return false;
    if (config->timeout_ms == 0 || config->timeout_ms > 60000) return false;
    if (config->clock_freq_hz < 1000 || config->clock_freq_hz > 10000000) return false;
    if (config->target_voltage != 18 && config->target_voltage != 33 && config->target_voltage != 50) return false;
    
    return true;
}

/**
 * @brief Scan JTAG chain for devices (wrapper for command interface)
 * @param devices Array to store found devices
 * @param max_devices Maximum number of devices to find
 * @param found_count Pointer to store number of devices found
 * @return true if scan completed successfully
 */
bool jtag_fuzzer_scan_chain(jtag_device_t* devices, uint8_t max_devices, uint8_t* found_count) {
    if (!devices || !found_count || max_devices == 0) {
        return false;
    }
    
    printf("[JTAG Fuzzer] Scanning JTAG chain...\n");
    
    // Use the existing JTAG scan functionality
    jtag_chain_t chain;
    uint8_t device_count = jtag_scan_chain(&chain);
    
    *found_count = (device_count > max_devices) ? max_devices : device_count;
    
    // Copy device information
    for (uint8_t i = 0; i < *found_count; i++) {
        devices[i].idcode = chain.devices[i].idcode;
        devices[i].ir_length = chain.devices[i].ir_length;
        devices[i].identified = chain.devices[i].identified;
        
        // Copy strings safely
        strncpy(devices[i].manufacturer, chain.devices[i].manufacturer, 
                sizeof(devices[i].manufacturer) - 1);
        devices[i].manufacturer[sizeof(devices[i].manufacturer) - 1] = '\0';
        
        strncpy(devices[i].device_name, chain.devices[i].device_name, 
                sizeof(devices[i].device_name) - 1);
        devices[i].device_name[sizeof(devices[i].device_name) - 1] = '\0';
    }
    
    printf("[JTAG Fuzzer] Chain scan completed: %d device(s) found\n", *found_count);
    return (*found_count > 0);
}
