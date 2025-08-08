/**
 * @file crash_detection.h
 * @brief Crash detection module for feedback mechanism in KISS Fuzzer
 * @author KISS Fuzzer Team
 * @date 2025
 * @version 0.9.0
 */

#ifndef CRASH_DETECTION_H
#define CRASH_DETECTION_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Crash type enumeration
 *
 * Enumerates different types of target crashes detected during fuzzing.
 */
typedef enum {
    CRASH_NONE,         ///< No crash detected
    CRASH_TIMEOUT,      ///< Target did not respond within expected time
    CRASH_HARDFAULT,    ///< Target entered a HardFault or reset handler
    CRASH_NO_HEARTBEAT, ///< No heartbeat or polling response detected
    CRASH_TAP_STUCK     ///< JTAG TAP state stuck or unresponsive
} crash_type_t;

/**
 * @brief Initialize the crash detection system
 */
void crash_detection_init(void);

/**
 * @brief Check for crash conditions on the target device
 * @return Detected crash type (see crash_type_t)
 */
crash_type_t crash_detection_check(void);

/**
 * @brief Handle a detected crash condition
 * @param type The crash type to handle (see crash_type_t)
 */
void crash_detection_handle(crash_type_t type);

#endif // CRASH_DETECTION_H
