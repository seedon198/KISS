/**
 * @file fuzz_commands.c
 * @brief JTAG Fuzzing Command Interface Implementation
 * @author KISS Fuzzer Team
 * @date 2025
 * @version 0.9.0
 */

#include "fuzz_commands.h"
#include <stdlib.h>

// Global fuzzing session state
static fuzz_session_result_t current_session_result = {0};
static bool commands_ready = false;

// Command table
static const fuzz_command_t command_table[] = {
    {"discover", "Automatically discover JTAG pins", cmd_discover},
    {"fuzz", "Start fuzzing session: fuzz <op> <strategy> [iter] [freq]", cmd_fuzz},
    {"stop", "Stop current fuzzing session", cmd_stop},
    {"status", "Get current fuzzing status", cmd_status},
    {"scan", "Scan JTAG chain for devices", cmd_scan},
    {"config", "Set configuration: config <param> <value>", cmd_config},
    {"info", "Get system information", cmd_info},
    {"help", "Show available commands", NULL}  // Special case
};

#define COMMAND_COUNT (sizeof(command_table) / sizeof(command_table[0]))

// Helper function to parse command line into argc/argv
static int parse_command_line(const char* command_line, char* argv[], int max_args) {
    static char cmd_buffer[256];
    strncpy(cmd_buffer, command_line, sizeof(cmd_buffer) - 1);
    cmd_buffer[sizeof(cmd_buffer) - 1] = '\0';
    
    int argc = 0;
    char* token = strtok(cmd_buffer, " \t\n");
    while (token && argc < max_args) {
        argv[argc++] = token;
        token = strtok(NULL, " \t\n");
    }
    
    return argc;
}

/**
 * @brief Initialize the fuzzing command processor
 * @return true if successful
 */
bool fuzz_commands_init(void) {
    printf("[Fuzz Commands] Initializing command processor...\n");
    commands_ready = true;
    return true;
}

/**
 * @brief Process a fuzzing command
 * @param command_line Command line string
 * @param response Buffer for response
 * @param response_size Size of response buffer
 * @return Command result code
 */
cmd_result_t fuzz_commands_process(const char* command_line, char* response, size_t response_size) {
    if (!commands_ready) {
        snprintf(response, response_size, "ERROR: Command processor not ready");
        return CMD_RESULT_NOT_READY;
    }
    
    if (!command_line || !response) {
        return CMD_RESULT_INVALID_ARGS;
    }
    
    // Parse command line
    char* argv[10];
    int argc = parse_command_line(command_line, argv, 10);
    
    if (argc == 0) {
        snprintf(response, response_size, "ERROR: No command specified");
        return CMD_RESULT_INVALID_ARGS;
    }
    
    // Special case for help
    if (strcmp(argv[0], "help") == 0) {
        fuzz_commands_get_help(response, response_size);
        return CMD_RESULT_SUCCESS;
    }
    
    // Find command in table
    for (size_t i = 0; i < COMMAND_COUNT; i++) {
        if (strcmp(argv[0], command_table[i].name) == 0) {
            if (command_table[i].handler) {
                return command_table[i].handler(argc, argv, response, response_size);
            }
        }
    }
    
    snprintf(response, response_size, "ERROR: Unknown command '%s'. Type 'help' for available commands.", argv[0]);
    return CMD_RESULT_ERROR;
}

/**
 * @brief Get list of available commands
 * @param buffer Buffer to store command list
 * @param buffer_size Size of buffer
 */
void fuzz_commands_get_help(char* buffer, size_t buffer_size) {
    int offset = 0;
    offset += snprintf(buffer + offset, buffer_size - offset, "KISS Fuzzer Commands:\n\n");
    
    for (size_t i = 0; i < COMMAND_COUNT - 1; i++) {  // -1 to skip help entry
        if (offset < buffer_size - 1) {
            offset += snprintf(buffer + offset, buffer_size - offset,
                             "  %-10s - %s\n", 
                             command_table[i].name, 
                             command_table[i].description);
        }
    }
    
    if (offset < buffer_size - 1) {
        snprintf(buffer + offset, buffer_size - offset, "\nExample: fuzz idcode dictionary 500 1000000\n");
    }
}

/**
 * @brief Discover JTAG pins automatically
 * Usage: discover
 */
cmd_result_t cmd_discover(int argc, char* argv[], char* response, size_t response_size) {
    (void)argc;
    (void)argv;
    
    pin_discovery_result_t result;
    if (jtag_fuzzer_discover_pins(&result)) {
        snprintf(response, response_size,
                "SUCCESS: JTAG pins discovered\n"
                "TCK: GPIO %d\n"
                "TMS: GPIO %d\n"
                "TDI: GPIO %d\n"
                "TDO: GPIO %d\n"
                "Confidence: %d%%\n"
                "IDCODE: 0x%08lX",
                result.tck_pin, result.tms_pin, result.tdi_pin, result.tdo_pin,
                result.confidence, result.idcode_found);
        return CMD_RESULT_SUCCESS;
    } else {
        snprintf(response, response_size, "ERROR: No JTAG pins discovered");
        return CMD_RESULT_ERROR;
    }
}

/**
 * @brief Start a fuzzing session
 * Usage: fuzz <operation> <strategy> [iterations] [frequency]
 */
cmd_result_t cmd_fuzz(int argc, char* argv[], char* response, size_t response_size) {
    if (argc < 3) {
        snprintf(response, response_size, 
                "ERROR: Usage: fuzz <operation> <strategy> [iterations] [frequency]\n"
                "Operations: idcode, instruction, data, boundary, memory, debug\n"
                "Strategies: sequential, random, genetic, smart, dictionary");
        return CMD_RESULT_INVALID_ARGS;
    }
    
    if (jtag_fuzzer_is_active()) {
        snprintf(response, response_size, "ERROR: Fuzzing session already active. Use 'stop' first.");
        return CMD_RESULT_BUSY;
    }
    
    fuzz_config_t config;
    jtag_fuzzer_get_default_config(&config);
    
    // Parse operation
    if (strcmp(argv[1], "idcode") == 0) config.operation = FUZZ_OP_IDCODE_SCAN;
    else if (strcmp(argv[1], "instruction") == 0) config.operation = FUZZ_OP_INSTRUCTION_SCAN;
    else if (strcmp(argv[1], "data") == 0) config.operation = FUZZ_OP_DATA_REGISTER_SCAN;
    else if (strcmp(argv[1], "boundary") == 0) config.operation = FUZZ_OP_BOUNDARY_SCAN;
    else if (strcmp(argv[1], "memory") == 0) config.operation = FUZZ_OP_MEMORY_PROBE;
    else if (strcmp(argv[1], "debug") == 0) config.operation = FUZZ_OP_DEBUG_ACCESS;
    else {
        snprintf(response, response_size, "ERROR: Invalid operation '%s'", argv[1]);
        return CMD_RESULT_INVALID_ARGS;
    }
    
    // Parse strategy
    if (strcmp(argv[2], "sequential") == 0) config.strategy = FUZZ_STRATEGY_SEQUENTIAL;
    else if (strcmp(argv[2], "random") == 0) config.strategy = FUZZ_STRATEGY_RANDOM;
    else if (strcmp(argv[2], "genetic") == 0) config.strategy = FUZZ_STRATEGY_GENETIC;
    else if (strcmp(argv[2], "smart") == 0) config.strategy = FUZZ_STRATEGY_SMART;
    else if (strcmp(argv[2], "dictionary") == 0) config.strategy = FUZZ_STRATEGY_DICTIONARY;
    else {
        snprintf(response, response_size, "ERROR: Invalid strategy '%s'", argv[2]);
        return CMD_RESULT_INVALID_ARGS;
    }
    
    // Parse optional parameters
    if (argc > 3) {
        config.max_iterations = atoi(argv[3]);
        if (config.max_iterations == 0 || config.max_iterations > FUZZ_MAX_ITERATIONS) {
            snprintf(response, response_size, "ERROR: Invalid iteration count");
            return CMD_RESULT_INVALID_ARGS;
        }
    }
    
    if (argc > 4) {
        config.clock_freq_hz = atoi(argv[4]);
        if (config.clock_freq_hz < 1000 || config.clock_freq_hz > 10000000) {
            snprintf(response, response_size, "ERROR: Invalid frequency (1000-10000000 Hz)");
            return CMD_RESULT_INVALID_ARGS;
        }
    }
    
    // Start fuzzing session
    if (jtag_fuzzer_start_session(&config, &current_session_result)) {
        snprintf(response, response_size, 
                "SUCCESS: Fuzzing session started\n"
                "Operation: %s\n"
                "Strategy: %s\n"
                "Iterations: %lu\n"
                "Frequency: %lu Hz",
                jtag_fuzzer_operation_name(config.operation),
                jtag_fuzzer_strategy_name(config.strategy),
                config.max_iterations,
                config.clock_freq_hz);
        return CMD_RESULT_SUCCESS;
    } else {
        snprintf(response, response_size, "ERROR: Failed to start fuzzing session");
        return CMD_RESULT_ERROR;
    }
}

/**
 * @brief Stop current fuzzing session
 * Usage: stop
 */
cmd_result_t cmd_stop(int argc, char* argv[], char* response, size_t response_size) {
    (void)argc;
    (void)argv;
    
    if (!jtag_fuzzer_is_active()) {
        snprintf(response, response_size, "No active fuzzing session to stop");
        return CMD_RESULT_SUCCESS;
    }
    
    jtag_fuzzer_stop_session();
    
    snprintf(response, response_size, 
            "SUCCESS: Fuzzing session stopped\n"
            "Results: %lu iterations, %lu successful, %lu anomalies",
            current_session_result.stats.total_iterations,
            current_session_result.stats.successful_ops,
            current_session_result.stats.anomalies);
    
    return CMD_RESULT_SUCCESS;
}

/**
 * @brief Get fuzzing session status
 * Usage: status
 */
cmd_result_t cmd_status(int argc, char* argv[], char* response, size_t response_size) {
    (void)argc;
    (void)argv;
    
    if (jtag_fuzzer_is_active()) {
        uint8_t progress = jtag_fuzzer_get_progress();
        snprintf(response, response_size,
                "STATUS: Fuzzing active\n"
                "Progress: %d%%\n"
                "Iterations: %lu\n"
                "Successful: %lu\n"
                "Anomalies: %lu",
                progress,
                current_session_result.stats.total_iterations,
                current_session_result.stats.successful_ops,
                current_session_result.stats.anomalies);
    } else {
        snprintf(response, response_size, "STATUS: No active fuzzing session");
    }
    
    return CMD_RESULT_SUCCESS;
}

/**
 * @brief Scan JTAG chain for devices
 * Usage: scan
 */
cmd_result_t cmd_scan(int argc, char* argv[], char* response, size_t response_size) {
    (void)argc;
    (void)argv;
    
    jtag_device_t devices[8];
    uint8_t found_count = 0;
    
    if (jtag_fuzzer_scan_chain(devices, 8, &found_count)) {
        int offset = snprintf(response, response_size, "SUCCESS: Found %d device(s)\n", found_count);
        
        for (int i = 0; i < found_count && offset < response_size - 1; i++) {
            offset += snprintf(response + offset, response_size - offset,
                             "Device %d: IDCODE=0x%08lX, IR_LEN=%d\n",
                             i + 1, devices[i].idcode, devices[i].ir_length);
        }
        return CMD_RESULT_SUCCESS;
    } else {
        snprintf(response, response_size, "ERROR: Chain scan failed or no devices found");
        return CMD_RESULT_ERROR;
    }
}

/**
 * @brief Set JTAG configuration
 * Usage: config <param> <value>
 */
cmd_result_t cmd_config(int argc, char* argv[], char* response, size_t response_size) {
    if (argc < 3) {
        snprintf(response, response_size, 
                "ERROR: Usage: config <param> <value>\n"
                "Parameters: voltage, frequency, timeout");
        return CMD_RESULT_INVALID_ARGS;
    }
    
    if (strcmp(argv[1], "voltage") == 0) {
        int voltage = atoi(argv[2]);
        if (voltage == 18 || voltage == 33 || voltage == 50) {
            snprintf(response, response_size, "SUCCESS: Voltage set to %d.%dV", voltage / 10, voltage % 10);
            return CMD_RESULT_SUCCESS;
        } else {
            snprintf(response, response_size, "ERROR: Invalid voltage. Use 18, 33, or 50 (for 1.8V, 3.3V, 5.0V)");
            return CMD_RESULT_INVALID_ARGS;
        }
    } else if (strcmp(argv[1], "frequency") == 0) {
        int freq = atoi(argv[2]);
        if (freq >= 1000 && freq <= 10000000) {
            snprintf(response, response_size, "SUCCESS: Frequency set to %d Hz", freq);
            return CMD_RESULT_SUCCESS;
        } else {
            snprintf(response, response_size, "ERROR: Frequency must be between 1000 and 10000000 Hz");
            return CMD_RESULT_INVALID_ARGS;
        }
    } else {
        snprintf(response, response_size, "ERROR: Unknown parameter '%s'", argv[1]);
        return CMD_RESULT_INVALID_ARGS;
    }
}

/**
 * @brief Get system information
 * Usage: info
 */
cmd_result_t cmd_info(int argc, char* argv[], char* response, size_t response_size) {
    (void)argc;
    (void)argv;
    
    uint32_t free_heap = xPortGetFreeHeapSize();
    uint32_t uptime = to_ms_since_boot(get_absolute_time()) / 1000;
    
    snprintf(response, response_size,
            "KISS Fuzzer v0.9.0\n"
            "Uptime: %lu seconds\n"
            "Free heap: %lu bytes\n"
            "JTAG engine: %s\n"
            "Fuzzing: %s",
            uptime,
            free_heap,
            "Ready",
            jtag_fuzzer_is_active() ? "Active" : "Idle");
    
    return CMD_RESULT_SUCCESS;
}
