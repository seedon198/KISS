/**
 * @file crash_detection.c
 * @brief Crash detection module implementation for KISS Fuzzer
 * @author KISS Fuzzer Team
 * @date 2025
 * @version 0.9.0
 */

#include "crash_detection.h"
#include <stdio.h>
#include <string.h>

// Simulated backend for crash status
static crash_type_t last_crash_type = CRASH_NONE;

// (Optional) Statistics – expand or remove if you want more detail
static unsigned int crash_counter[5] = {0};

//----------------------
// Crash Detection Logic
//----------------------

// Initialize detection system (example: setup timers/hardware)
void crash_detection_init(void) {
    last_crash_type = CRASH_NONE;
    memset(crash_counter, 0, sizeof(crash_counter));
    // TODO: Add any platform-specific initialization here
}

// Simulate/Check for a crash (replace with real logic!)
crash_type_t crash_detection_check(void) {
    // TODO: Replace with a real check:
    // - TAP stuck, device non-responsive
    // - HardFault PC, RAM/ping value, etc.
    // - Timeout waiting for response
    // Simulate: alternate between none and timeout for demonstration.
    static int check_count = 0;
    check_count++;
    if (check_count % 5 == 0) {
        last_crash_type = CRASH_TIMEOUT;
    } else {
        last_crash_type = CRASH_NONE;
    }
    // Track stats
    crash_counter[last_crash_type]++;
    return last_crash_type;
}

// Handle a crash (reset, log, notify, recover)
void crash_detection_handle(crash_type_t type) {
    // Example handler logic (expand to fit your board!)
    switch (type) {
        case CRASH_NONE:
            // Nothing to do
            break;
        case CRASH_TIMEOUT:
        case CRASH_HARDFAULT:
        case CRASH_NO_HEARTBEAT:
        case CRASH_TAP_STUCK:
            // Attempt recovery: e.g. JTAG tap reset, device reset, or logging
            printf("[Crash Detection] Detected crash type %d, attempting recovery...\n", type);
            // TODO: Insert JTAG tap reset, MCU reset, recover or log event here
            break;
    }
    // Clear status after handling
    last_crash_type = CRASH_NONE;
}

//----------------------
// Command Handlers
//----------------------

// Command return codes (reuse from fuzz_commands.h if possible)
typedef enum {
    CMD_RESULT_SUCCESS = 0,
    CMD_RESULT_ERROR,
    CMD_RESULT_INVALID_ARGS,
    CMD_RESULT_NOT_READY,
    CMD_RESULT_BUSY
} cmd_result_t;


/**
 * @brief Check for any crash condition
 * Usage: crash_check
 */
cmd_result_t cmd_crash_check(int argc, char* argv[], char* response, size_t response_size) {
    (void)argc;
    (void)argv;
    crash_type_t detected = crash_detection_check();
    const char* msg = "NONE";
    switch (detected) {
        case CRASH_NONE:         msg = "No crash detected"; break;
        case CRASH_TIMEOUT:      msg = "Timeout: Target unresponsive"; break;
        case CRASH_HARDFAULT:    msg = "HardFault: CPU in fault/reset"; break;
        case CRASH_NO_HEARTBEAT: msg = "No heartbeat detected"; break;
        case CRASH_TAP_STUCK:    msg = "TAP state stuck"; break;
    }
    snprintf(response, response_size, "CRASH CHECK: %s (%d)", msg, detected);
    return CMD_RESULT_SUCCESS;
}

/**
 * @brief Attempt crash recovery/handling
 * Usage: crash_handle
 */
cmd_result_t cmd_crash_handle(int argc, char* argv[], char* response, size_t response_size) {
    (void)argc;
    (void)argv;
    crash_detection_handle(last_crash_type);
    snprintf(response, response_size, "CRASH HANDLE: Attempted recovery for last type");
    return CMD_RESULT_SUCCESS;
}

/**
 * @brief Show last crash type or statistics
 * Usage: crash_info
 */
cmd_result_t cmd_crash_info(int argc, char* argv[], char* response, size_t response_size) {
    (void)argc;
    (void)argv;
    snprintf(response, response_size,
        "CRASH INFO:\n"
        "Last crash type: %d\n"
        "CRASH_NONE: %u\n"
        "CRASH_TIMEOUT: %u\n"
        "CRASH_HARDFAULT: %u\n"
        "CRASH_NO_HEARTBEAT: %u\n"
        "CRASH_TAP_STUCK: %u\n",
        last_crash_type,
        crash_counter[CRASH_NONE],
        crash_counter[CRASH_TIMEOUT],
        crash_counter[CRASH_HARDFAULT],
        crash_counter[CRASH_NO_HEARTBEAT],
        crash_counter[CRASH_TAP_STUCK]
    );
    return CMD_RESULT_SUCCESS;
}
