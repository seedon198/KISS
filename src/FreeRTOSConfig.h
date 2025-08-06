/**
 * @file FreeRTOSConfig.h
 * @brief FreeRTOS configuration for KISS Fuzzer
 * @author KISS Fuzzer Team
 * @date 2025
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <stdint.h>
extern uint32_t SystemCoreClock;

// Basic FreeRTOS configuration
#define configUSE_PREEMPTION     1
#define configUSE_IDLE_HOOK      0
#define configUSE_TICK_HOOK      0
#define configCPU_CLOCK_HZ       125000000
#define configTICK_RATE_HZ       ((TickType_t)1000)
#define configMAX_PRIORITIES     (8)
#define configMINIMAL_STACK_SIZE ((uint16_t)256)
#define configTOTAL_HEAP_SIZE    ((size_t)32768)  // 32KB heap
#define configMAX_TASK_NAME_LEN  (16)
#define configUSE_TRACE_FACILITY 0
#define configUSE_16_BIT_TICKS   0
#define configIDLE_SHOULD_YIELD  1

// Memory allocation
#define configUSE_MALLOC_FAILED_HOOK        1
#define configCHECK_FOR_STACK_OVERFLOW      0
#define configUSE_APPLICATION_TASK_TAG      0
#define configENABLE_BACKWARD_COMPATIBILITY 0
#define configUSE_COUNTING_SEMAPHORES       1
#define configUSE_RECURSIVE_MUTEXES         1

// Software timer configuration
#define configUSE_TIMERS             1
#define configTIMER_TASK_PRIORITY    ( configMAX_PRIORITIES - 1 )
#define configTIMER_QUEUE_LENGTH     10
#define configTIMER_TASK_STACK_DEPTH 1024

// Queue configuration
#define configUSE_MUTEXES          1
#define configQUEUE_REGISTRY_SIZE  8
#define configUSE_QUEUE_SETS       0
#define configUSE_TIME_SLICING     1
#define configUSE_NEWLIB_REENTRANT 0

// Event groups
#define configUSE_EVENT_GROUPS 1

// Application specific settings  
#define configASSERT(x) if((x) == 0) {portDISABLE_INTERRUPTS(); for(;;);}

// Hook function prototypes
void vApplicationMallocFailedHook(void);

// Set the following definitions to 1 to include the API function, or zero
// to exclude the API function.
#define INCLUDE_vTaskPrioritySet                1
#define INCLUDE_uxTaskPriorityGet               1
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_vTaskDelayUntil                 1
#define INCLUDE_vTaskDelay                      1
#define INCLUDE_xTaskGetSchedulerState          1
#define INCLUDE_xTaskGetCurrentTaskHandle       1
#define INCLUDE_uxTaskGetStackHighWaterMark     1
#define INCLUDE_xTaskGetIdleTaskHandle          1
#define INCLUDE_eTaskGetState                   1
#define INCLUDE_xTimerPendFunctionCall          1
#define INCLUDE_xTaskAbortDelay                 1
#define INCLUDE_xTaskGetHandle                  1
#define INCLUDE_xTaskResumeFromISR              1
#define INCLUDE_xQueueGetMutexHolder            1

// Interrupt priorities
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 4

// Include standard C library definitions
#include <stddef.h>
#include <stdint.h>

#endif /* FREERTOS_CONFIG_H */
