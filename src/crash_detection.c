/**
 * @file crash_detection.c
 * @brief Crash detection module implementation for KISS Fuzzer, now with deduplication!
 * @author KISS Fuzzer Team
 * @date 2025
 * @version 0.9.1
 */

#include "crash_detection.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

// Maximum number of unique crash signatures to track (adjust as needed)
#define MAX_CRASH_SIGNATURES   32

static crash_type_t last_crash_type = CRASH_NONE;
static unsigned int crash_counter[5] = {0};

// Crash deduplication data
static uint32_t crash_signatures[MAX_CRASH_SIGNATURES] = {0}; // Basic PC-as-signature
static unsigned int signature_count = 0;

// Helper: check if signature already known
static bool crash_signature_known(uint32_t pc) {
    for (unsigned int i = 0; i < signature_count; i++) {
        if (crash_signatures[i] == pc)
            return true;
    }
    return false;
}

// Helper: add new signature, ring-buffer style overwrite if full
static void crash_signature_add(uint32_t pc) {
    if (signature_count < MAX_CRASH_SIGNATURES) {
        crash_signatures[signature_count++] = pc;
    } else {
        crash_signatures[signature_count % MAX_CRASH_SIGNATURES] = pc;
        signature_count++;
    }
}

void crash_detection_init(void) {
    last_crash_type = CRASH_NONE;
    memset(crash_counter, 0, sizeof(crash_counter));
    memset(crash_signatures, 0, sizeof(crash_signatures));
    signature_count = 0;
    // TODO: Any platform-specific init
}

// Dummy function for demo: replace with a real JTAG/SWD PC fetch!
static uint32_t get_crash_pc(void) {
    // In real use: extract PC value from MCU via JTAG/SWD
    // For demo, just use an incrementing value
    static uint32_t fake_pc = 0x08000000;
    fake_pc += 4;
    return fake_pc;
}

// Checks for duplicate/unique crash and logs accordingly.
void crash_dedup_process(uint32_t pc) {
    if (!crash_signature_known(pc)) {
        printf("[CrashDedup] Unique crash detected at PC=0x%08lX\n", pc);
        crash_signature_add(pc);
        // Optionally: Save more crash context, input, etc. here!
    } else {
        printf("[CrashDedup] Duplicate crash at PC=0x%08lX\n", pc);
    }
}

crash_type_t crash_detection_check(void) {
    // TODO: Replace with real crash check logic!
    static int check_count = 0;
    check_count++;
    if (check_count % 5 == 0) {
        last_crash_type = CRASH_TIMEOUT;
    } else {
        last_crash_type = CRASH_NONE;
    }
    crash_counter[last_crash_type]++;
    return last_crash_type;
}

void crash_detection_handle(crash_type_t type) {
    if (type == CRASH_NONE) {
        return;
    }
    // Simulate fetching the PC of the crash, via JTAG/SWD:
    uint32_t crash_pc = get_crash_pc();
    crash_dedup_process(crash_pc);

    // Example handler logic (expand for your actual hardware/target)
    printf("[Crash Detection] Detected crash type %d, attempting recovery...\n", type);
    // TODO: Insert JTAG tap reset, MCU reset, recover or log event here
    last_crash_type = CRASH_NONE;
}

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
        "CRASH_TAP_STUCK: %u\n"
        "Unique crash signatures: %u\n",
        last_crash_type,
        crash_counter[CRASH_NONE],
        crash_counter[CRASH_TIMEOUT],
        crash_counter[CRASH_HARDFAULT],
        crash_counter[CRASH_NO_HEARTBEAT],
        crash_counter[CRASH_TAP_STUCK],
        signature_count
    );
    return CMD_RESULT_SUCCESS;
}
