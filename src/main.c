/**
 * @file main.c
 * @brief Main entry point for KISS Fuzzer - system initialization and FreeRTOS task creation
 * @author KISS Fuzzer Team
 * @date 2025
 */

#include "kiss_fuzzer.h"

// Global system status
system_status_t g_system_status = {0};

// Inter-task communication
QueueHandle_t log_queue = NULL;
QueueHandle_t ui_event_queue = NULL;
SemaphoreHandle_t display_mutex = NULL;

// Task handles
TaskHandle_t ui_task_handle = NULL;
TaskHandle_t jtag_task_handle = NULL;
TaskHandle_t wifi_task_handle = NULL;
TaskHandle_t power_task_handle = NULL;

/**
 * @brief System initialization - hardware and peripherals
 * @return true if initialization successful, false otherwise
 */
static bool system_init(void)
{
    // Initialize stdio
    stdio_init_all();
    
    // Initialize system status
    memset(&g_system_status, 0, sizeof(system_status_t));
    strcpy(g_system_status.status_line, "KISS Fuzzer Starting...");
    
    // Initialize display first for early feedback
    if (!display_init()) {
        printf("ERROR: Display initialization failed\n");
        return false;
    }
    
    display_update_status("Display OK");
    sleep_ms(500);
    
    // Initialize storage
    if (!storage_init()) {
        printf("WARNING: Storage initialization failed\n");
        display_update_status("Storage FAIL");
        sleep_ms(1000);
    } else {
        display_update_status("Storage OK");
        sleep_ms(500);
    }
    
    // Initialize power monitoring
    if (!power_init()) {
        printf("ERROR: Power monitoring initialization failed\n");
        display_update_status("Power FAIL");
        return false;
    }
    
    display_update_status("Power OK");
    sleep_ms(500);
    
    // Initialize JTAG engine
    if (!jtag_init()) {
        printf("ERROR: JTAG initialization failed\n");
        display_update_status("JTAG FAIL");
        return false;
    }
    
    display_update_status("JTAG OK");
    sleep_ms(500);
    
    // Initialize UI system
    ui_init();
    display_update_status("UI OK");
    sleep_ms(500);
    
    // Initialize Wi-Fi (non-blocking)
    if (!wifi_init()) {
        printf("WARNING: Wi-Fi initialization failed\n");
        display_update_status("WiFi FAIL");
        sleep_ms(1000);
    } else {
        display_update_status("WiFi OK");
        sleep_ms(500);
    }
    
    printf("KISS Fuzzer initialized successfully\n");
    display_update_status("System Ready");
    
    return true;
}

/**
 * @brief Create FreeRTOS synchronization objects
 * @return true if creation successful, false otherwise
 */
static bool create_sync_objects(void)
{
    // Create queues
    log_queue = xQueueCreate(32, sizeof(char) * 128);
    if (log_queue == NULL) {
        printf("ERROR: Failed to create log queue\n");
        return false;
    }
    
    ui_event_queue = xQueueCreate(16, sizeof(ui_event_t));
    if (ui_event_queue == NULL) {
        printf("ERROR: Failed to create UI event queue\n");
        return false;
    }
    
    // Create mutexes
    display_mutex = xSemaphoreCreateMutex();
    if (display_mutex == NULL) {
        printf("ERROR: Failed to create display mutex\n");
        return false;
    }
    
    return true;
}

/**
 * @brief Create and start FreeRTOS tasks
 * @return true if all tasks created successfully, false otherwise
 */
static bool create_tasks(void)
{
    BaseType_t result;
    
    // Create UI task (medium priority)
    result = xTaskCreate(
        ui_task,
        "UI_Task",
        UI_TASK_STACK_SIZE,
        NULL,
        UI_TASK_PRIORITY,
        &ui_task_handle
    );
    
    if (result != pdPASS) {
        printf("ERROR: Failed to create UI task\n");
        return false;
    }
    
    // Create JTAG task (medium priority)
    result = xTaskCreate(
        jtag_task,
        "JTAG_Task",
        JTAG_TASK_STACK_SIZE,
        NULL,
        JTAG_TASK_PRIORITY,
        &jtag_task_handle
    );
    
    if (result != pdPASS) {
        printf("ERROR: Failed to create JTAG task\n");
        return false;
    }
    
    // Create Wi-Fi task (high priority)
    result = xTaskCreate(
        wifi_task,
        "WiFi_Task",
        WIFI_TASK_STACK_SIZE,
        NULL,
        WIFI_TASK_PRIORITY,
        &wifi_task_handle
    );
    
    if (result != pdPASS) {
        printf("ERROR: Failed to create Wi-Fi task\n");
        return false;
    }
    
    // Create power monitoring task (low priority)
    result = xTaskCreate(
        power_task,
        "Power_Task",
        POWER_TASK_STACK_SIZE,
        NULL,
        POWER_TASK_PRIORITY,
        &power_task_handle
    );
    
    if (result != pdPASS) {
        printf("ERROR: Failed to create Power task\n");
        return false;
    }
    
    printf("All FreeRTOS tasks created successfully\n");
    return true;
}

/**
 * @brief Main application entry point
 * @return Should never return
 */
int main(void)
{
    printf("\n");
    printf("=================================\n");
    printf("    KISS Fuzzer v1.0\n");
    printf("    Keep It Simple, Silly\n");
    printf("=================================\n");
    printf("Initializing system...\n");
    
    // Create synchronization objects first
    if (!create_sync_objects()) {
        printf("FATAL: Failed to create synchronization objects\n");
        while (1) {
            tight_loop_contents();
        }
    }
    
    // Initialize hardware and peripherals
    if (!system_init()) {
        printf("FATAL: System initialization failed\n");
        display_update_status("INIT FAILED");
        while (1) {
            tight_loop_contents();
        }
    }
    
    // Create and start FreeRTOS tasks
    if (!create_tasks()) {
        printf("FATAL: Failed to create FreeRTOS tasks\n");
        display_update_status("TASK FAILED");
        while (1) {
            tight_loop_contents();
        }
    }
    
    printf("Starting FreeRTOS scheduler...\n");
    display_update_status("Starting Tasks...");
    
    // Start the FreeRTOS scheduler
    vTaskStartScheduler();
    
    // Should never reach here
    printf("FATAL: FreeRTOS scheduler returned\n");
    display_update_status("SCHEDULER FAIL");
    
    while (1) {
        tight_loop_contents();
    }
    
    return 0;
}

/**
 * @brief FreeRTOS stack overflow hook
 * @param pxTask Task handle that overflowed
 * @param pcTaskName Name of the task that overflowed
 * @return void
 */
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
    printf("FATAL: Stack overflow in task %s\n", pcTaskName);
    display_update_status("STACK OVERFLOW");
    
    // Emergency shutdown
    power_emergency_shutdown();
    
    while (1) {
        tight_loop_contents();
    }
}

/**
 * @brief FreeRTOS malloc failed hook
 * @return void
 */
void vApplicationMallocFailedHook(void)
{
    printf("FATAL: Memory allocation failed\n");
    display_update_status("MALLOC FAILED");
    
    // Emergency shutdown
    power_emergency_shutdown();
    
    while (1) {
        tight_loop_contents();
    }
}

/**
 * @brief FreeRTOS idle hook
 * @return void
 */
void vApplicationIdleHook(void)
{
    // Put CPU in low power mode when idle
    __wfi();
}
