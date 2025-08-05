/**
 * @file kiss_fuzzer.h
 * @brief Main header file for KISS Fuzzer project
 * @author KISS Fuzzer Team
 * @date 2025
 */

#ifndef KISS_FUZZER_H
#define KISS_FUZZER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

// Pico SDK includes
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "hardware/spi.h"
#include "hardware/adc.h"
#include "hardware/pio.h"

// FreeRTOS includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include "timers.h"

// Project version
#define KISS_FUZZER_VERSION_MAJOR 1
#define KISS_FUZZER_VERSION_MINOR 0
#define KISS_FUZZER_VERSION_PATCH 0
#define KISS_FUZZER_VERSION_STRING "1.0.0"

// Hardware pin definitions
#define OLED_SDA_PIN    8
#define OLED_SCL_PIN    9
#define OLED_I2C_PORT   i2c0

#define JOYSTICK_UP     10
#define JOYSTICK_DOWN   11
#define JOYSTICK_LEFT   12
#define JOYSTICK_RIGHT  13
#define JOYSTICK_OK     14
#define JOYSTICK_BACK   15

#define JTAG_TMS_PIN    16
#define JTAG_TCK_PIN    17
#define JTAG_TDI_PIN    18
#define JTAG_TDO_PIN    19
#define JTAG_TRST_PIN   20

#define BATTERY_ADC_PIN 26
#define CHARGING_PIN    21

// System configuration
#define SYSTEM_CLOCK_KHZ 125000
#define I2C_CLOCK_KHZ    400

// Queue sizes
#define UI_EVENT_QUEUE_SIZE    10
#define LOG_MESSAGE_QUEUE_SIZE 20
#define JTAG_CMD_QUEUE_SIZE    5

// Event group bits
#define EVENT_WIFI_CONNECTED    (1 << 0)
#define EVENT_JTAG_SCAN_DONE    (1 << 1)
#define EVENT_BATTERY_LOW       (1 << 2)
#define EVENT_SD_CARD_READY     (1 << 3)

// Global event group handle
extern EventGroupHandle_t g_system_events;

// Global queue handles
extern QueueHandle_t g_ui_event_queue;
extern QueueHandle_t g_log_message_queue;
extern QueueHandle_t g_jtag_cmd_queue;

// System state structure
typedef struct {
    bool wifi_enabled;
    bool jtag_active;
    uint8_t battery_percent;
    bool charging;
    uint32_t uptime_seconds;
} system_state_t;

extern system_state_t g_system_state;

// Common data types
typedef enum {
    UI_EVENT_JOYSTICK_UP,
    UI_EVENT_JOYSTICK_DOWN,
    UI_EVENT_JOYSTICK_LEFT,
    UI_EVENT_JOYSTICK_RIGHT,
    UI_EVENT_JOYSTICK_OK,
    UI_EVENT_JOYSTICK_BACK,
    UI_EVENT_TIMEOUT
} ui_event_type_t;

typedef struct {
    ui_event_type_t type;
    uint32_t timestamp;
} ui_event_t;

typedef enum {
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR
} log_level_t;

typedef struct {
    log_level_t level;
    char message[128];
    uint32_t timestamp;
} log_message_t;

// Utility macros
#define LOG_DEBUG(msg, ...) log_message(LOG_LEVEL_DEBUG, msg, ##__VA_ARGS__)
#define LOG_INFO(msg, ...)  log_message(LOG_LEVEL_INFO, msg, ##__VA_ARGS__)
#define LOG_WARN(msg, ...)  log_message(LOG_LEVEL_WARN, msg, ##__VA_ARGS__)
#define LOG_ERROR(msg, ...) log_message(LOG_LEVEL_ERROR, msg, ##__VA_ARGS__)

// Function prototypes
void log_message(log_level_t level, const char* format, ...);
void system_init(void);
void system_start_tasks(void);

#endif // KISS_FUZZER_H
