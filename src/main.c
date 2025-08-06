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
#include "storage.h"

#define KISS_FUZZER_VERSION "0.8.0"

/* Task priorities as per coding guidelines */
#define JTAG_TASK_PRIORITY      2  // Medium
#define UI_TASK_PRIORITY        1  // Medium/Low
#define POWER_TASK_PRIORITY     0  // Low

/* Task stack sizes */
#define JTAG_TASK_STACK_SIZE    2048
#define UI_TASK_STACK_SIZE      1024
#define POWER_TASK_STACK_SIZE   512

/* Inter-task communication queues */
static QueueHandle_t log_queue;
static QueueHandle_t status_queue;
static SemaphoreHandle_t display_mutex;

/* Task handles */
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
    
    printf("[JTAG Task] Starting JTAG engine\n");
    
    // Initialize JTAG engine with default config
    jtag_init(NULL);
    
    while (1) {
        // JTAG operations would be triggered by UI commands
        // For now, just periodic status updates
        
        log_message_t log_msg;
        snprintf(log_msg.message, sizeof(log_msg.message), "JTAG: Ready");
        log_msg.priority = 1;
        
        xQueueSend(log_queue, &log_msg, 0);
        
        // Update system status
        system_status_t status = {0};
        status.jtag_active = true;
        status.storage_ready = storage_is_ready();
        xQueueSend(status_queue, &status, 0);
        
        vTaskDelay(pdMS_TO_TICKS(5000)); // Update every 5 seconds
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
    
    // Initialize task communication
    init_task_communication();
    
    // Initial display message
    display_clear();
    display_print(0, 0, "KISS Fuzzer v0.8.0");
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
