/**
 * @file fuzz_commands.h
 * @brief JTAG Fuzzing Command Interface for KISS Fuzzer
 * @author KISS Fuzzer Team
 * @date 2025
 * @version 0.9.0
 */

#ifndef FUZZ_COMMANDS_H
#define FUZZ_COMMANDS_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "jtag_fuzzer.h"

// Command result codes
typedef enum {
    CMD_RESULT_SUCCESS = 0,
    CMD_RESULT_ERROR,
    CMD_RESULT_INVALID_ARGS,
    CMD_RESULT_NOT_READY,
    CMD_RESULT_BUSY
} cmd_result_t;

// Command structure
typedef struct {
    const char* name;
    const char* description;
    cmd_result_t (*handler)(int argc, char* argv[], char* response, size_t response_size);
} fuzz_command_t;

/**
 * @brief Initialize the fuzzing command processor
 * @return true if successful
 */
bool fuzz_commands_init(void);

/**
 * @brief Process a fuzzing command
 * @param command_line Command line string
 * @param response Buffer for response
 * @param response_size Size of response buffer
 * @return Command result code
 */
cmd_result_t fuzz_commands_process(const char* command_line, char* response, size_t response_size);

/**
 * @brief Get list of available commands
 * @param buffer Buffer to store command list
 * @param buffer_size Size of buffer
 */
void fuzz_commands_get_help(char* buffer, size_t buffer_size);

// Individual command handlers

/**
 * @brief Discover JTAG pins automatically
 * Usage: discover
 */
cmd_result_t cmd_discover(int argc, char* argv[], char* response, size_t response_size);

/**
 * @brief Start a fuzzing session
 * Usage: fuzz <operation> <strategy> [iterations] [frequency]
 */
cmd_result_t cmd_fuzz(int argc, char* argv[], char* response, size_t response_size);

/**
 * @brief Stop current fuzzing session
 * Usage: stop
 */
cmd_result_t cmd_stop(int argc, char* argv[], char* response, size_t response_size);

/**
 * @brief Get fuzzing session status
 * Usage: status
 */
cmd_result_t cmd_status(int argc, char* argv[], char* response, size_t response_size);

/**
 * @brief Scan JTAG chain for devices
 * Usage: scan
 */
cmd_result_t cmd_scan(int argc, char* argv[], char* response, size_t response_size);

/**
 * @brief Set JTAG configuration
 * Usage: config <param> <value>
 */
cmd_result_t cmd_config(int argc, char* argv[], char* response, size_t response_size);

/**
 * @brief Get system information
 * Usage: info
 */
cmd_result_t cmd_info(int argc, char* argv[], char* response, size_t response_size);

#endif // FUZZ_COMMANDS_H
