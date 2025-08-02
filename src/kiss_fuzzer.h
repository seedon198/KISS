/**
 * @file kiss_fuzzer.h
 * @brief Main header file for KISS Fuzzer - common definitions and includes
 * @author KISS Fuzzer Team
 * @date 2025
 */

#ifndef KISS_FUZZER_H
#define KISS_FUZZER_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Pico SDK includes
#include "hardware/adc.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"
#include "hardware/spi.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"

// FreeRTOS includes
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"
#include "timers.h"

// Project includes
#include "display.h"
#include "jtag.h"
#include "power.h"
#include "storage.h"
#include "ui.h"
#include "wifi.h"

// Hardware pin definitions
#define OLED_SDA_PIN  4
#define OLED_SCL_PIN  5
#define OLED_I2C_INST i2c0

#define JOYSTICK_UP    6
#define JOYSTICK_DOWN  7
#define JOYSTICK_LEFT  8
#define JOYSTICK_RIGHT 9
#define JOYSTICK_OK    10
#define JOYSTICK_BACK  11

#define JTAG_TCK_PIN  16
#define JTAG_TMS_PIN  17
#define JTAG_TDI_PIN  18
#define JTAG_TDO_PIN  19
#define JTAG_TRST_PIN 20

#define BATTERY_ADC_PIN 26
#define CHARGE_STAT_PIN 22

#define SD_MISO_PIN 0
#define SD_MOSI_PIN 3
#define SD_SCK_PIN  2
#define SD_CS_PIN   1

// Project version
#define FIRMWARE_VERSION    "1.0.0"

// System configuration
#define SYSTEM_TASK_PRIORITY (tskIDLE_PRIORITY + 4)
#define WIFI_TASK_PRIORITY   (tskIDLE_PRIORITY + 3)
#define JTAG_TASK_PRIORITY   (tskIDLE_PRIORITY + 2)
#define UI_TASK_PRIORITY     (tskIDLE_PRIORITY + 2)
#define POWER_TASK_PRIORITY  (tskIDLE_PRIORITY + 1)

#define SYSTEM_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 4)
#define UI_TASK_STACK_SIZE    (configMINIMAL_STACK_SIZE * 4)
#define JTAG_TASK_STACK_SIZE  (configMINIMAL_STACK_SIZE * 6)
#define WIFI_TASK_STACK_SIZE  (configMINIMAL_STACK_SIZE * 8)
#define POWER_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 2)

// Inter-task communication
extern QueueHandle_t     log_queue;
extern QueueHandle_t     ui_event_queue;
extern SemaphoreHandle_t display_mutex;

// Global status structure
typedef struct {
    bool    wifi_connected;
    bool    jtag_scanning;
    uint8_t battery_percent;
    bool    charging;
    char    status_line[64];
} system_status_t;

extern system_status_t g_system_status;

// Logging macros (simple printf-based for now)
#define LOG_ERROR(fmt, ...)   printf("[ERROR] " fmt "\n", ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)    printf("[WARN]  " fmt "\n", ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)    printf("[INFO]  " fmt "\n", ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...)   printf("[DEBUG] " fmt "\n", ##__VA_ARGS__)

// Additional includes for low-level functions
#include "hardware/clocks.h"

// ARM CMSIS functions (inline implementations)
static inline void __wfi(void) {
    __asm volatile ("wfi" ::: "memory");
}

// Power management includes

#endif  // KISS_FUZZER_H
