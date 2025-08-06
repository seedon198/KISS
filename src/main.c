/**
 * @file main.c
 * @brief KISS Fuzzer v0.8.0 - FreeRTOS Integration
 * @author KISS Fuzzer Team
 * @date 2025
 */

#include <stdio.h>
#include <stdarg.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "display.h"
#include "input.h"
#include "ui.h"
#include "power.h"
#include "jtag.h"
#include "jtag_fuzzer.h"
#include "storage.h"
#include "wifi.h"

#define KISS_FUZZER_VERSION "0.9.1-alpha.2"

/* Task priorities as per coding guidelines */
#define WIFI_TASK_PRIORITY      3  // High
#define JTAG_TASK_PRIORITY      2  // Medium
#define UI_TASK_PRIORITY        1  // Medium/Low
#define POWER_TASK_PRIORITY     0  // Low

/* Task stack sizes */
#define WIFI_TASK_STACK_SIZE    4096
#define JTAG_TASK_STACK_SIZE    2048
#define UI_TASK_STACK_SIZE      1024
#define POWER_TASK_STACK_SIZE   512

/* Inter-task communication queues */
static QueueHandle_t log_queue;
static QueueHandle_t status_queue;
static SemaphoreHandle_t display_mutex;

/* Task handles */
static TaskHandle_t wifi_task_handle;
static TaskHandle_t jtag_task_handle;
static TaskHandle_t ui_task_handle;
static TaskHandle_t power_task_handle;

/**
 * @brief Log message structure for inter-task communication
 */
typedef struct {
    char message[64];
    uint8_t priority;
} log_message_t;

/**
 * @brief System status structure
 */
typedef struct {
    uint8_t battery_percent;
    bool charging;
    bool jtag_active;
    bool storage_ready;
} system_status_t;

/**
 * @brief JTAG task - Medium priority
 * @param pvParameters Task parameters (unused)
 */
static void jtag_task(void *pvParameters) {
    (void)pvParameters;
    
    printf("[JTAG Task] Starting advanced JTAG fuzzing engine\n");
    
    // Initialize advanced JTAG fuzzing engine
    if (!jtag_fuzzer_init()) {
        printf("[JTAG Task] Failed to initialize JTAG fuzzer\n");
        vTaskDelete(NULL);
        return;
    }
    
    // Perform initial pin discovery
    pin_discovery_result_t pin_result;
    if (jtag_fuzzer_discover_pins(&pin_result)) {
        log_message_t log_msg;
        snprintf(log_msg.message, sizeof(log_msg.message), 
                "JTAG: Pins found (TCK:%d,TMS:%d,TDI:%d,TDO:%d)", 
                pin_result.tck_pin, pin_result.tms_pin, 
                pin_result.tdi_pin, pin_result.tdo_pin);
        log_msg.priority = 2;
        xQueueSend(log_queue, &log_msg, 0);
        
        // Log to storage if available
        if (storage_is_ready()) {
            char log_entry[128];
            snprintf(log_entry, sizeof(log_entry),
                    "Pin Discovery: TCK=%d TMS=%d TDI=%d TDO=%d IDCODE=0x%08lX Confidence=%d%%",
                    pin_result.tck_pin, pin_result.tms_pin, pin_result.tdi_pin, pin_result.tdo_pin,
                    pin_result.idcode_found, pin_result.confidence);
            storage_log_system_event(log_entry, 3);
        }
    }
    
    // Example fuzzing session - this would normally be triggered by UI
    fuzz_config_t fuzz_config;
    jtag_fuzzer_get_default_config(&fuzz_config);
    fuzz_config.operation = FUZZ_OP_IDCODE_SCAN;
    fuzz_config.strategy = FUZZ_STRATEGY_DICTIONARY;
    fuzz_config.max_iterations = 100;  // Limited for demo
    
    fuzz_session_result_t fuzz_result;
    
    while (1) {
        // Check if we should start a fuzzing session (normally triggered by UI)
        static uint32_t last_fuzz_time = 0;
        uint32_t current_time = to_ms_since_boot(get_absolute_time());
        
        // Run a demo fuzzing session every 30 seconds
        if (current_time - last_fuzz_time > 30000 && !jtag_fuzzer_is_active()) {
            printf("[JTAG Task] Starting demo fuzzing session\n");
            
            if (jtag_fuzzer_start_session(&fuzz_config, &fuzz_result)) {
                last_fuzz_time = current_time;
                
                log_message_t log_msg;
                snprintf(log_msg.message, sizeof(log_msg.message), "JTAG: Fuzzing started");
                log_msg.priority = 2;
                xQueueSend(log_queue, &log_msg, 0);
            }
        }
        
        // Monitor active fuzzing session
        if (jtag_fuzzer_is_active()) {
            uint8_t progress = jtag_fuzzer_get_progress();
            
            // Update progress every 10%
            static uint8_t last_progress = 0;
            if (progress >= last_progress + 10) {
                log_message_t log_msg;
                snprintf(log_msg.message, sizeof(log_msg.message), "JTAG: Fuzzing %d%%", progress);
                log_msg.priority = 1;
                xQueueSend(log_queue, &log_msg, 0);
                last_progress = progress;
            }
        } else if (last_fuzz_time > 0 && current_time - last_fuzz_time < 1000) {
            // Session just completed, report results
            log_message_t log_msg;
            snprintf(log_msg.message, sizeof(log_msg.message), 
                    "JTAG: Complete (%lu iter, %lu anomalies)", 
                    fuzz_result.stats.total_iterations, fuzz_result.stats.anomalies);
            log_msg.priority = 2;
            xQueueSend(log_queue, &log_msg, 0);
            
            // Log detailed results to storage
            if (storage_is_ready()) {
                char log_entry[256];
                snprintf(log_entry, sizeof(log_entry),
                        "Fuzzing Session: %s with %s - %lu iterations, %lu successful, %lu anomalies, %lu findings",
                        jtag_fuzzer_operation_name(fuzz_config.operation),
                        jtag_fuzzer_strategy_name(fuzz_config.strategy),
                        fuzz_result.stats.total_iterations,
                        fuzz_result.stats.successful_ops,
                        fuzz_result.stats.anomalies,
                        (uint32_t)fuzz_result.findings_count);
                storage_log_system_event(log_entry, 4);
            }
        }
        
        // Regular status updates
        log_message_t log_msg;
        if (jtag_fuzzer_is_active()) {
            snprintf(log_msg.message, sizeof(log_msg.message), "JTAG: Fuzzing active");
        } else {
            snprintf(log_msg.message, sizeof(log_msg.message), "JTAG: Ready for fuzzing");
        }
        log_msg.priority = 1;
        xQueueSend(log_queue, &log_msg, 0);
        
        // Update system status
        system_status_t status = {0};
        status.jtag_active = true;
        status.storage_ready = storage_is_ready();
        xQueueSend(status_queue, &status, 0);
        
        vTaskDelay(pdMS_TO_TICKS(2000)); // Update every 2 seconds
    }
}

/**
 * @brief UI task - Medium/Low priority
 * @param pvParameters Task parameters (unused)
 */
static void ui_task(void *pvParameters) {
    (void)pvParameters;
    
    printf("[UI Task] Starting UI system\n");
    
    // Initialize UI
    ui_init();
    
    log_message_t log_msg;
    system_status_t system_status = {0};
    
    while (1) {
        // Take display mutex before updating
        if (xSemaphoreTake(display_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            
            // Process input and update UI
            ui_update();
            
            // Check for log messages
            if (xQueueReceive(log_queue, &log_msg, 0) == pdTRUE) {
                printf("[UI] Log: %s\n", log_msg.message);
                // Could display on OLED status line
            }
            
            // Check for status updates
            if (xQueueReceive(status_queue, &system_status, 0) == pdTRUE) {
                // Update status display - could show on OLED status line
                printf("[UI] Status: Batt:%d%% %s JTAG:%s Storage:%s\n", 
                      system_status.battery_percent,
                      system_status.charging ? "CHG" : "",
                      system_status.jtag_active ? "OK" : "OFF",
                      system_status.storage_ready ? "OK" : "ERR");
            }
            
            xSemaphoreGive(display_mutex);
        }
        
        vTaskDelay(pdMS_TO_TICKS(50)); // 20 FPS UI updates
    }
}

/**
 * @brief Power monitoring task - Low priority
 * @param pvParameters Task parameters (unused)
 */
static void power_task(void *pvParameters) {
    (void)pvParameters;
    
    printf("[Power Task] Starting power monitoring\n");
    
    // Initialize power management
    power_init();
    
    while (1) {
        // Read battery status
        uint8_t battery_percent = power_get_battery_percent();
        bool charging = power_is_charging();
        
        // Update system status
        system_status_t status = {0};
        status.battery_percent = battery_percent;
        status.charging = charging;
        status.jtag_active = false;    // Will be updated by JTAG task
        status.storage_ready = storage_is_ready();
        
        xQueueSend(status_queue, &status, 0);
        
        // Log battery status periodically
        if (battery_percent < 20) {
            log_message_t log_msg;
            snprintf(log_msg.message, sizeof(log_msg.message), 
                    "Battery Low: %d%%", battery_percent);
            log_msg.priority = 2;
            xQueueSend(log_queue, &log_msg, 0);
        }
        
        vTaskDelay(pdMS_TO_TICKS(10000)); // Update every 10 seconds
    }
}

/**
 * @brief Initialize inter-task communication
 */
static void init_task_communication(void) {
    // Create queues
    log_queue = xQueueCreate(10, sizeof(log_message_t));
    status_queue = xQueueCreate(5, sizeof(system_status_t));
    
    // Create mutex for display access
    display_mutex = xSemaphoreCreateMutex();
    
    if (log_queue == NULL || status_queue == NULL || display_mutex == NULL) {
        printf("Failed to create FreeRTOS objects\n");
        while (1) {
            tight_loop_contents();
        }
    }
}

/**
 * @brief Create all FreeRTOS tasks
 */
static void create_tasks(void) {
    BaseType_t result;
    
    // Create Wi-Fi task (High priority)
    result = xTaskCreate(wifi_task, "WiFi", WIFI_TASK_STACK_SIZE,
                        NULL, WIFI_TASK_PRIORITY, &wifi_task_handle);
    if (result != pdPASS) {
        printf("Failed to create Wi-Fi task\n");
        return;
    }
    
    // Create JTAG task (Medium priority)
    result = xTaskCreate(jtag_task, "JTAG", JTAG_TASK_STACK_SIZE,
                        NULL, JTAG_TASK_PRIORITY, &jtag_task_handle);
    if (result != pdPASS) {
        printf("Failed to create JTAG task\n");
        return;
    }
    
    // Create UI task (Medium/Low priority)
    result = xTaskCreate(ui_task, "UI", UI_TASK_STACK_SIZE,
                        NULL, UI_TASK_PRIORITY, &ui_task_handle);
    if (result != pdPASS) {
        printf("Failed to create UI task\n");
        return;
    }
    
    // Create Power task (Low priority)
    result = xTaskCreate(power_task, "Power", POWER_TASK_STACK_SIZE,
                        NULL, POWER_TASK_PRIORITY, &power_task_handle);
    if (result != pdPASS) {
        printf("Failed to create Power task\n");
        return;
    }
    
    printf("All FreeRTOS tasks created successfully\n");
}

/**
 * @brief Main application entry point
 */
int main(void) {
    stdio_init_all();
    
    printf("\n=== KISS Fuzzer v%s ===\n", KISS_FUZZER_VERSION);
    printf("Initializing FreeRTOS system...\n");
    
    // Initialize LED for system status
    const uint LED_PIN = 25;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 1); // LED on during initialization
    
    // Initialize hardware
    display_init();
    input_init();
    
    // Initialize storage system
    if (storage_init()) {
        printf("Storage system initialized successfully\n");
        storage_log_system_event("KISS Fuzzer started with FreeRTOS", 6);
    } else {
        printf("Storage system initialization failed (continuing without SD)\n");
    }
    
    // Initialize Wi-Fi system
    if (wifi_init(NULL)) {
        printf("Wi-Fi system initialized successfully\n");
    } else {
        printf("Wi-Fi system initialization failed (continuing without Wi-Fi)\n");
    }
    
    // Initialize task communication
    init_task_communication();
    
    // Initial display message
    display_clear();
    display_print(0, 0, "KISS Fuzzer v0.9.1");
    display_print(0, 1, "Starting FreeRTOS...");
    display_update();
    
    // Create all tasks
    create_tasks();
    
    printf("Starting FreeRTOS scheduler...\n");
    
    // LED off - system ready
    gpio_put(LED_PIN, 0);
    
    // Start the FreeRTOS scheduler
    vTaskStartScheduler();
    
    // Should never reach here
    printf("ERROR: FreeRTOS scheduler returned!\n");
    while (1) {
        gpio_put(LED_PIN, 1);
        sleep_ms(100);
        gpio_put(LED_PIN, 0);
        sleep_ms(100);
    }
    
    return 0;
}

/**
 * @brief FreeRTOS malloc failed hook
 */
void vApplicationMallocFailedHook(void) {
    printf("FreeRTOS malloc failed!\n");
    while (1) {
        tight_loop_contents();
    }
}

/**
 * @brief FreeRTOS stack overflow hook
 */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    printf("FreeRTOS stack overflow in task: %s\n", pcTaskName);
    while (1) {
        tight_loop_contents();
    }
}
