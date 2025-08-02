/**
 * @file storage.h
 * @brief MicroSD card storage and file management
 * @author KISS Fuzzer Team
 * @date 2025
 */

#ifndef STORAGE_H
#define STORAGE_H

#include "ff.h"  // FatFS
#include "kiss_fuzzer.h"

// Storage result codes
typedef enum {
    STORAGE_OK = 0,
    STORAGE_ERROR_INIT,
    STORAGE_ERROR_MOUNT,
    STORAGE_ERROR_FILE_NOT_FOUND,
    STORAGE_ERROR_WRITE,
    STORAGE_ERROR_READ,
    STORAGE_ERROR_FULL
} storage_result_t;

// Storage configuration
#define STORAGE_MOUNT_POINT "/"
#define LOG_FILE_PATH       "/logs/fuzzer.log"
#define DUMP_DIR_PATH       "/dumps"
#define CONFIG_FILE_PATH    "/config.ini"
#define MAX_FILENAME_LEN    64
#define MAX_PATH_LEN        256

// File types
typedef enum { FILE_TYPE_LOG, FILE_TYPE_DUMP, FILE_TYPE_CONFIG, FILE_TYPE_UNKNOWN } file_type_t;

// File information structure
typedef struct {
    char        name[MAX_FILENAME_LEN];
    char        path[MAX_PATH_LEN];
    uint32_t    size;
    file_type_t type;
    FDATE       date;
    FTIME       time;
} file_info_t;

// Directory listing structure
typedef struct {
    file_info_t *files;
    uint16_t     count;
    uint16_t     capacity;
} dir_listing_t;

/**
 * @brief Initialize storage system and mount SD card
 * @return Storage result code
 */
storage_result_t storage_init(void);

/**
 * @brief Check if storage is available
 * @return true if available, false otherwise
 */
bool storage_is_available(void);

/**
 * @brief Create directory if it doesn't exist
 * @param path Directory path to create
 * @return true if created or already exists, false on error
 */
bool storage_create_directory(const char *path);

/**
 * @brief Write log entry to log file
 * @param timestamp Log timestamp
 * @param level Log level (INFO, WARN, ERROR, etc.)
 * @param message Log message
 * @return true if written successfully, false otherwise
 */
bool storage_write_log(uint32_t timestamp, const char *level, const char *message);

/**
 * @brief Save memory dump to file
 * @param filename Dump filename
 * @param data Memory dump data
 * @param size Data size in bytes
 * @return true if saved successfully, false otherwise
 */
bool storage_save_dump(const char *filename, const uint8_t *data, uint32_t size);

/**
 * @brief Read configuration from file
 * @param key Configuration key
 * @param value Buffer to store value
 * @param max_len Maximum value length
 * @return true if read successfully, false otherwise
 */
bool storage_read_config(const char *key, char *value, size_t max_len);

/**
 * @brief Write configuration to file
 * @param key Configuration key
 * @param value Configuration value
 * @return true if written successfully, false otherwise
 */
bool storage_write_config(const char *key, const char *value);

/**
 * @brief Get directory listing
 * @param path Directory path
 * @param listing Pointer to store directory listing
 * @return true if successful, false otherwise
 */
bool storage_list_directory(const char *path, dir_listing_t *listing);

/**
 * @brief Free directory listing memory
 * @param listing Directory listing to free
 * @return void
 */
void storage_free_listing(dir_listing_t *listing);

/**
 * @brief Delete file
 * @param path File path to delete
 * @return true if deleted successfully, false otherwise
 */
bool storage_delete_file(const char *path);

/**
 * @brief Get file size
 * @param path File path
 * @return File size in bytes, 0 if file doesn't exist
 */
uint32_t storage_get_file_size(const char *path);

/**
 * @brief Check if file exists
 * @param path File path to check
 * @return true if file exists, false otherwise
 */
bool storage_file_exists(const char *path);

/**
 * @brief Get free space on SD card
 * @return Free space in bytes
 */
uint64_t storage_get_free_space(void);

/**
 * @brief Get total space on SD card
 * @return Total space in bytes
 */
uint64_t storage_get_total_space(void);

/**
 * @brief Format SD card (emergency function)
 * @return true if formatted successfully, false otherwise
 */
bool storage_format_card(void);

/**
 * @brief Read file data
 * @param path File path
 * @param buffer Buffer to store data
 * @param max_size Maximum buffer size
 * @return Number of bytes read, 0 on error
 */
uint32_t storage_read_file(const char *path, uint8_t *buffer, uint32_t max_size);

/**
 * @brief Write file data
 * @param path File path
 * @param data Data to write
 * @param size Data size
 * @return true if written successfully, false otherwise
 */
bool storage_write_file(const char *path, const uint8_t *data, uint32_t size);

/**
 * @brief Append log entry to file
 * @param filename Log filename
 * @param entry Log entry string
 * @return Storage result code
 */
storage_result_t storage_append_log(const char* filename, const char* entry);

#endif  // STORAGE_H
