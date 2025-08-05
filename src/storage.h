/**
 * @file storage.h
 * @brief SD card storage system for KISS Fuzzer
 * @author KISS Fuzzer Team
 * @date 2025
 * @version 0.7.0
 */

#ifndef STORAGE_H
#define STORAGE_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "ff.h"

// SD card SPI configuration
#define STORAGE_SPI_PORT        spi1
#define STORAGE_SCK_PIN         10
#define STORAGE_MOSI_PIN        11
#define STORAGE_MISO_PIN        12
#define STORAGE_CS_PIN          13
#define STORAGE_SPI_FREQ        12500000  // 12.5 MHz

// Storage paths and file naming
#define STORAGE_ROOT_PATH       "/"
#define STORAGE_LOGS_DIR        "/logs"
#define STORAGE_SCANS_DIR       "/scans" 
#define STORAGE_DUMPS_DIR       "/dumps"
#define STORAGE_CONFIG_DIR      "/config"

#define STORAGE_MAX_FILENAME    64
#define STORAGE_MAX_PATH        128
#define STORAGE_BUFFER_SIZE     512

// File types for organized storage
typedef enum {
    STORAGE_FILE_JTAG_SCAN,
    STORAGE_FILE_MEMORY_DUMP,
    STORAGE_FILE_SYSTEM_LOG,
    STORAGE_FILE_CONFIG,
    STORAGE_FILE_GENERIC
} storage_file_type_t;

// Storage status structure
typedef struct {
    bool card_present;
    bool mounted;
    uint32_t total_size_mb;
    uint32_t free_size_mb;
    uint32_t files_count;
    char volume_label[16];
    FATFS filesystem;
} storage_status_t;

// File information structure
typedef struct {
    char filename[STORAGE_MAX_FILENAME];
    uint32_t size_bytes;
    uint16_t date;
    uint16_t time;
    bool is_directory;
} storage_file_info_t;

/**
 * @brief Initialize the storage system
 * @return true if successful, false otherwise
 */
bool storage_init(void);

/**
 * @brief Deinitialize the storage system
 */
void storage_deinit(void);

/**
 * @brief Check if SD card is present and mounted
 * @return true if ready for operations
 */
bool storage_is_ready(void);

/**
 * @brief Get storage system status
 * @return Storage status structure
 */
storage_status_t storage_get_status(void);

/**
 * @brief Create directory if it doesn't exist
 * @param path Directory path
 * @return true if successful or already exists
 */
bool storage_create_directory(const char* path);

/**
 * @brief Generate filename with timestamp
 * @param file_type Type of file for appropriate naming
 * @param buffer Buffer to store generated filename
 * @param buffer_size Size of buffer
 * @return true if successful
 */
bool storage_generate_filename(storage_file_type_t file_type, char* buffer, size_t buffer_size);

/**
 * @brief Write data to file
 * @param filename Full path to file
 * @param data Data to write
 * @param size Size of data in bytes
 * @param append True to append, false to overwrite
 * @return Number of bytes written, 0 on error
 */
size_t storage_write_file(const char* filename, const void* data, size_t size, bool append);

/**
 * @brief Read data from file
 * @param filename Full path to file
 * @param buffer Buffer to read into
 * @param buffer_size Size of buffer
 * @return Number of bytes read, 0 on error
 */
size_t storage_read_file(const char* filename, void* buffer, size_t buffer_size);

/**
 * @brief Write text line to file
 * @param filename Full path to file
 * @param text Text to write (newline added automatically)
 * @param append True to append, false to overwrite
 * @return true if successful
 */
bool storage_write_line(const char* filename, const char* text, bool append);

/**
 * @brief Log JTAG scan results
 * @param scan_data Scan result data
 * @param data_size Size of scan data
 * @return true if logged successfully
 */
bool storage_log_jtag_scan(const void* scan_data, size_t data_size);

/**
 * @brief Log system event
 * @param event_text Event description
 * @param priority Priority level (0-7)
 * @return true if logged successfully
 */
bool storage_log_system_event(const char* event_text, uint8_t priority);

/**
 * @brief Save memory dump to file
 * @param address Start address of dump
 * @param data Memory data
 * @param size Size of data
 * @return true if saved successfully
 */
bool storage_save_memory_dump(uint32_t address, const void* data, size_t size);

/**
 * @brief List files in directory
 * @param path Directory path
 * @param files Array to store file information
 * @param max_files Maximum number of files to list
 * @return Number of files found
 */
uint16_t storage_list_files(const char* path, storage_file_info_t* files, uint16_t max_files);

/**
 * @brief Delete file
 * @param filename Full path to file
 * @return true if successful
 */
bool storage_delete_file(const char* filename);

/**
 * @brief Get file size
 * @param filename Full path to file
 * @return File size in bytes, 0 if error
 */
uint32_t storage_get_file_size(const char* filename);

/**
 * @brief Check if file exists
 * @param filename Full path to file
 * @return true if file exists
 */
bool storage_file_exists(const char* filename);

/**
 * @brief Format storage (WARNING: Deletes all data)
 * @return true if successful
 */
bool storage_format(void);

/**
 * @brief Get human-readable file type name
 * @param file_type File type
 * @return String name of the file type
 */
const char* storage_file_type_name(storage_file_type_t file_type);

/**
 * @brief Get formatted storage status string
 * @param buffer Buffer to write to
 * @param buffer_size Size of buffer
 */
void storage_get_status_string(char* buffer, size_t buffer_size);

#endif // STORAGE_H
