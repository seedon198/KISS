/**
 * @file main.c
 * @brief Main entry point for KISS Fuzzer
 *
 * Initializes all subsystems and starts FreeRTOS scheduler.
 * Handles system-level configuration and task coordination.
 *
 * @author KISS Fuzzer Team
 * @date 2025
 */

#include "FreeRTOS.h"
#include "display.h"
#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "hardware/watchdog.h"
#include "jtag.h"
#include "kiss_fuzzer.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "power.h"
#include "storage.h"
#include "task.h"
#include "timers.h"
#include "ui.h"
#include "wifi.h"

// System task handles
static TaskHandle_t  system_task_handle = NULL;
static TimerHandle_t watchdog_timer     = NULL;

// System state
static bool     system_initialized = false;
static uint32_t boot_timestamp     = 0;

// Function prototypes
static void system_task(void *pvParameters);
static void system_init_hardware(void);
static void system_init_peripherals(void);
static void watchdog_callback(TimerHandle_t timer);
static void panic_handler(void);

/**
 * @brief Main entry point
 * @return Should never return
 */
int main(void)
{
    // Record boot time
    boot_timestamp = time_us_32();

    // Initialize hardware
    system_init_hardware();

    // Initialize peripherals
    system_init_peripherals();

    // Create watchdog timer (30 second timeout)
    watchdog_timer = xTimerCreate("Watchdog", pdMS_TO_TICKS(30000),
                                  pdTRUE,  // Auto-reload
                                  NULL, watchdog_callback);

    if (watchdog_timer) {
        xTimerStart(watchdog_timer, 0);
    }

    // Create system monitoring task
    BaseType_t result = xTaskCreate(system_task, "System", SYSTEM_TASK_STACK_SIZE, NULL,
                                    SYSTEM_TASK_PRIORITY, &system_task_handle);

    if (result != pdPASS) {
        LOG_ERROR("Failed to create system task");
        panic_handler();
    }

    // Mark system as initialized
    system_initialized = true;

    LOG_INFO("KISS Fuzzer v%s starting...", FIRMWARE_VERSION);
    LOG_INFO("Build: %s %s", __DATE__, __TIME__);

    // Start FreeRTOS scheduler
    vTaskStartScheduler();

    // Should never reach here
    LOG_ERROR("Scheduler returned - system panic!");
    panic_handler();

    return 0;
}

/**
 * @brief Initialize basic hardware
 */
static void system_init_hardware(void)
{
    // Initialize stdio for debugging
    stdio_init_all();

    // Wait for USB serial to stabilize
    sleep_ms(1000);

    printf("\n\n=== KISS Fuzzer Boot ===\n");
    printf("Hardware initialization...\n");

    // Initialize CYW43 for LED and Wi-Fi
    if (cyw43_arch_init()) {
        printf("FATAL: CYW43 initialization failed\n");
        panic_handler();
    }

    // Turn on LED to indicate boot process
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

    // Initialize hardware watchdog (8 seconds)
    if (watchdog_caused_reboot()) {
        printf("WARNING: System recovered from watchdog reset\n");
    }
    watchdog_enable(8000, 1);

    printf("Hardware initialization complete\n");
}

/**
 * @brief Initialize all peripheral subsystems
 */
static void system_init_peripherals(void)
{
    printf("Peripheral initialization...\n");

    // Initialize display first (for status messages)
    printf("- Display...");
    if (display_init() != DISPLAY_OK) {
        printf(" FAILED\n");
        LOG_ERROR("Failed to initialize display");
    } else {
        printf(" OK\n");
    }

    // Show boot message on display
    display_clear();
    display_set_line("KISS Fuzzer v" FIRMWARE_VERSION);
    display_update();
    sleep_ms(1000);

    // Initialize power management
    printf("- Power management...");
    if (power_init() != POWER_OK) {
        printf(" FAILED\n");
        LOG_ERROR("Failed to initialize power subsystem");
    } else {
        printf(" OK\n");
    }

    // Show power status
    display_set_line("Power: OK");
    display_update();
    sleep_ms(500);

    // Initialize storage
    printf("- Storage...");
    if (storage_init() != STORAGE_OK) {
        printf(" FAILED\n");
        LOG_ERROR("Failed to initialize storage");
        display_set_line("Storage: FAILED");
    } else {
        printf(" OK\n");
        display_set_line("Storage: OK");
    }
    display_update();
    sleep_ms(500);

    // Initialize JTAG engine
    printf("- JTAG engine...");
    if (jtag_init() != JTAG_OK) {
        printf(" FAILED\n");
        LOG_ERROR("Failed to initialize JTAG subsystem");
        display_set_line("JTAG: FAILED");
    } else {
        printf(" OK\n");
        display_set_line("JTAG: OK");
    }
    display_update();
    sleep_ms(500);

    // Initialize UI system
    printf("- User interface...");
    if (ui_init() != UI_OK) {
        printf(" FAILED\n");
        LOG_ERROR("Failed to initialize UI");
        display_set_line("UI: FAILED");
    } else {
        printf(" OK\n");
        display_set_line("UI: OK");
    }
    display_update();
    sleep_ms(500);

    // Initialize Wi-Fi (last due to complexity)
    printf("- Wi-Fi subsystem...");
    display_set_line("Wi-Fi: Starting...");
    display_update();

    if (wifi_init() != WIFI_OK) {
        printf(" FAILED\n");
        LOG_ERROR("Failed to initialize Wi-Fi");
        display_set_line("Wi-Fi: FAILED");
    } else {
        printf(" OK\n");
        display_set_line("Wi-Fi: OK");
    }
    display_update();
    sleep_ms(500);

    // Final boot message
    display_set_line("System Ready!");
    display_update();

    printf("All subsystems initialized\n");
    printf("Boot complete - system ready\n");
    printf("=======================\n\n");

    LOG_INFO("All subsystems initialized successfully");
}

/**
 * @brief System monitoring task
 * @param pvParameters Task parameters (unused)
 */
static void system_task(void *pvParameters)
{
    uint32_t last_heap_check    = 0;
    uint32_t last_status_update = 0;
    uint32_t heartbeat_counter  = 0;

    // Wait for system to fully initialize
    vTaskDelay(pdMS_TO_TICKS(2000));

    while (1) {
        uint32_t current_time = xTaskGetTickCount();

        // Feed hardware watchdog
        watchdog_update();

        // System health check every 10 seconds
        if ((current_time - last_heap_check) >= pdMS_TO_TICKS(10000)) {
            size_t   free_heap      = xPortGetFreeHeapSize();
            size_t   min_heap       = xPortGetMinimumEverFreeHeapSize();
            uint32_t uptime_seconds = current_time / configTICK_RATE_HZ;

            LOG_INFO("System health check #%lu", heartbeat_counter++);
            LOG_INFO("- Free heap: %d bytes", free_heap);
            LOG_INFO("- Min heap: %d bytes", min_heap);
            LOG_INFO("- Uptime: %lu seconds", uptime_seconds);
            LOG_INFO("- Power: %.2fV (%d%%)", power_get_voltage(), power_get_battery_percent());
            LOG_INFO("- Wi-Fi: %s", wifi_get_status_message());

            // Check for critical conditions
            if (free_heap < 1024) {
                LOG_WARN("CRITICAL: Low memory - %d bytes free", free_heap);
                display_set_line("LOW MEMORY!");
                display_update();

                // Force garbage collection if possible
                // In a real system, you might trigger cleanup tasks
            }

            // Check battery level
            int battery_percent = power_get_battery_percent();
            if (battery_percent < 10) {
                LOG_WARN("CRITICAL: Low battery - %d%%", battery_percent);
                display_set_line("LOW BATTERY!");
                display_update();
            }

            last_heap_check = current_time;
        }

        // Update status display every 5 seconds
        if ((current_time - last_status_update) >= pdMS_TO_TICKS(5000)) {
            char status_line[64];
            snprintf(status_line, sizeof(status_line), "%.1fV %d%% %s", power_get_voltage(),
                     power_get_battery_percent(),
                     wifi_get_status() == WIFI_STATUS_CONNECTED ? "WiFi" : "AP");

            // Only update display if not showing menu
            if (ui_get_state() == UI_STATE_IDLE) {
                display_set_line(status_line);
                display_update();
            }

            last_status_update = current_time;
        }

        // Heartbeat LED (fast blink = alive, slow = error)
        bool error_condition = (xPortGetFreeHeapSize() < 1024) || (power_get_battery_percent() < 5);

        uint32_t blink_rate = error_condition ? 100 : 1000;

        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(50));
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);

        // Variable delay based on system state
        vTaskDelay(pdMS_TO_TICKS(blink_rate - 50));
    }
}

/**
 * @brief Watchdog timer callback
 * @param timer Timer handle
 */
static void watchdog_callback(TimerHandle_t timer)
{
    static uint32_t watchdog_count = 0;

    // Software watchdog - check if system is responsive
    if (system_initialized) {
        // Check if critical tasks are still running
        if (eTaskGetState(system_task_handle) == eDeleted) {
            LOG_ERROR("System task died - triggering reset");
            watchdog_reboot(0, 0, 0);
        }

        LOG_DEBUG("Software watchdog check #%lu passed", ++watchdog_count);
    }
}

/**
 * @brief Panic handler for critical errors
 */
static void panic_handler(void)
{
    // Disable interrupts
    taskDISABLE_INTERRUPTS();

    // Try to show panic on display
    if (system_initialized) {
        display_clear();
        display_set_line("SYSTEM PANIC!");
        display_update();
    }

    // Flash LED rapidly to indicate panic
    while (1) {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        sleep_ms(100);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        sleep_ms(100);

        // Print debug info if possible
        printf("PANIC: System in unrecoverable state\n");
        printf("Free heap: %d bytes\n", xPortGetFreeHeapSize());

        // Trigger watchdog reset after 5 seconds
        static int panic_count = 0;
        if (++panic_count > 25) {
            watchdog_reboot(0, 0, 0);
        }
    }
}

/**
 * @brief FreeRTOS stack overflow hook
 */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    printf("STACK OVERFLOW in task: %s\n", pcTaskName);
    LOG_ERROR("Stack overflow detected in task: %s", pcTaskName);
    panic_handler();
}

/**
 * @brief FreeRTOS malloc failed hook
 */
void vApplicationMallocFailedHook(void)
{
    printf("MALLOC FAILED - out of heap memory\n");
    LOG_ERROR("Memory allocation failed - heap exhausted");
    panic_handler();
}
