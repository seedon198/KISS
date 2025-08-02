#ifndef LWIP_ARCH_SYS_ARCH_H
#define LWIP_ARCH_SYS_ARCH_H

/**
 * @file sys_arch.h
 * @brief lwIP system architecture definitions for NO_SYS mode
 * 
 * Minimal stubs for NO_SYS=1 mode where lwIP runs without RTOS
 */

#include <stdint.h>

// Types for NO_SYS mode (stubs)
typedef uint8_t sys_sem_t;
typedef uint8_t sys_mutex_t;
typedef uint8_t sys_mbox_t;
typedef uint8_t sys_thread_t;

// Error codes
#define SYS_SEM_NULL    0
#define SYS_MBOX_NULL   0

// Timeout value
#define SYS_ARCH_TIMEOUT 0xffffffffUL

// Protection level (not used in NO_SYS mode)
typedef uint32_t sys_prot_t;

#endif /* LWIP_ARCH_SYS_ARCH_H */
