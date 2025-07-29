/**
 * @file storage.c
 * @brief MicroSD card storage and file management implementation
 * @author KISS Fuzzer Team
 * @date 2025
 */

#include "storage.h"
#include "ff.h"

// FatFS objects
static FATFS fs;
static bool storage_mounted = false;
static char current_working_dir[MAX_PATH_LEN] = "/";

// File operation buffers
static char temp_path_buffer[MAX_PATH_LEN];
static char line_buffer[256];

/**
 * @brief Initialize storage system and mount SD card
 * @return true if initialization successful, false otherwise
 */
bool storage_init(void)
{
    FRESULT result;
    
    // Initialize SPI for SD card
    spi_init(spi1, 1000000); // 1 MHz for initialization
    gpio_set_function(SD_MISO_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SD_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SD_MOSI_PIN, GPIO_FUNC_SPI);
    
    // Initialize CS pin
    gpio_init(SD_CS_PIN);
    gpio_set_dir(SD_CS_PIN, GPIO_OUT);
    gpio_put(SD_CS_PIN, 1); // CS high (inactive)
    
    sleep_ms(100); // Allow SD card to stabilize
    
    // Mount the filesystem
    result = f_mount(&fs, "", 1); // Mount immediately
    
    if (result != FR_OK) {
        printf("ERROR: Failed to mount SD card (error %d)\n", result);
        return false;
    }
    
    storage_mounted = true;
    
    // Increase SPI speed after successful mount
    spi_set_baudrate(spi1, 10000000); // 10 MHz for data transfer
    
    // Create necessary directories
    storage_create_directory(DUMP_DIR_PATH);
    storage_create_directory("/logs");
    
    // Check free space
    uint64_t free_space = storage_get_free_space();
    uint64_t total_space = storage_get_total_space();
    
    printf("Storage initialized - Free: %llu MB, Total: %llu MB\n", 
           free_space / (1024 * 1024), total_space / (1024 * 1024));
    
    // Write initialization log
    storage_write_log(xTaskGetTickCount(), "INFO", "Storage system initialized");
    
    return true;
}

/**
 * @brief Check if SD card is mounted and accessible
 * @return true if mounted, false otherwise
 */
bool storage_is_mounted(void)
{
    return storage_mounted;
}

/**
 * @brief Create directory if it doesn't exist
 * @param path Directory path to create
 * @return true if created or already exists, false on error
 */
bool storage_create_directory(const char* path)
{
    if (!storage_mounted || !path) return false;
    
    FRESULT result = f_mkdir(path);
    
    // Success or already exists
    if (result == FR_OK || result == FR_EXIST) {
        return true;
    }
    
    printf("ERROR: Failed to create directory %s (error %d)\n", path, result);
    return false;
}

/**
 * @brief Write log entry to log file
 * @param timestamp Log timestamp
 * @param level Log level (INFO, WARN, ERROR, etc.)
 * @param message Log message
 * @return true if written successfully, false otherwise
 */
bool storage_write_log(uint32_t timestamp, const char* level, const char* message)
{
    if (!storage_mounted || !level || !message) return false;
    
    FIL log_file;
    FRESULT result;
    UINT bytes_written;
    
    // Open log file for append
    result = f_open(&log_file, LOG_FILE_PATH, FA_WRITE | FA_OPEN_APPEND | FA_CREATE_ALWAYS);
    if (result != FR_OK) {
        printf("ERROR: Failed to open log file (error %d)\n", result);
        return false;
    }
    
    // Format log entry with timestamp
    uint32_t hours = timestamp / (1000 * 3600);
    uint32_t minutes = (timestamp / (1000 * 60)) % 60;
    uint32_t seconds = (timestamp / 1000) % 60;
    uint32_t milliseconds = timestamp % 1000;
    
    snprintf(line_buffer, sizeof(line_buffer), 
             "[%02lu:%02lu:%02lu.%03lu] [%s] %s\n", 
             hours, minutes, seconds, milliseconds, level, message);
    
    // Write to file
    result = f_write(&log_file, line_buffer, strlen(line_buffer), &bytes_written);
    
    // Close file
    f_close(&log_file);
    
    if (result != FR_OK || bytes_written != strlen(line_buffer)) {
        printf("ERROR: Failed to write log entry (error %d)\n", result);
        return false;
    }
    
    return true;
}

/**
 * @brief Save memory dump to file
 * @param filename Dump filename
 * @param data Memory dump data
 * @param size Data size in bytes
 * @return true if saved successfully, false otherwise
 */
bool storage_save_dump(const char* filename, const uint8_t* data, uint32_t size)
{
    if (!storage_mounted || !filename || !data || size == 0) return false;
    
    FIL dump_file;
    FRESULT result;
    UINT bytes_written;
    
    // Create full path
    snprintf(temp_path_buffer, sizeof(temp_path_buffer), "%s/%s", DUMP_DIR_PATH, filename);
    
    // Open file for writing
    result = f_open(&dump_file, temp_path_buffer, FA_WRITE | FA_CREATE_ALWAYS);
    if (result != FR_OK) {
        printf("ERROR: Failed to create dump file %s (error %d)\n", temp_path_buffer, result);
        return false;
    }
    
    // Write data in chunks to avoid large memory usage
    const uint32_t chunk_size = 1024;
    uint32_t bytes_remaining = size;
    uint32_t offset = 0;
    
    while (bytes_remaining > 0) {
        uint32_t write_size = (bytes_remaining > chunk_size) ? chunk_size : bytes_remaining;
        
        result = f_write(&dump_file, data + offset, write_size, &bytes_written);
        if (result != FR_OK || bytes_written != write_size) {
            printf("ERROR: Failed to write dump data (error %d)\n", result);
            f_close(&dump_file);
            return false;
        }
        
        offset += write_size;
        bytes_remaining -= write_size;
    }
    
    // Close file
    f_close(&dump_file);
    
    printf("Memory dump saved: %s (%lu bytes)\n", filename, size);
    
    // Log the operation
    snprintf(line_buffer, sizeof(line_buffer), "Memory dump saved: %s (%lu bytes)", filename, size);
    storage_write_log(xTaskGetTickCount(), "INFO", line_buffer);
    
    return true;
}

/**
 * @brief Read configuration from file
 * @param key Configuration key
 * @param value Buffer to store value
 * @param max_len Maximum value length
 * @return true if read successfully, false otherwise
 */
bool storage_read_config(const char* key, char* value, size_t max_len)
{
    if (!storage_mounted || !key || !value || max_len == 0) return false;
    
    FIL config_file;
    FRESULT result;
    
    // Open config file
    result = f_open(&config_file, CONFIG_FILE_PATH, FA_READ);
    if (result != FR_OK) {
        return false; // Config file doesn't exist
    }
    
    // Read line by line to find key
    while (f_gets(line_buffer, sizeof(line_buffer), &config_file)) {
        // Remove newline
        size_t len = strlen(line_buffer);
        if (len > 0 && line_buffer[len - 1] == '\n') {
            line_buffer[len - 1] = '\0';
        }
        
        // Parse key=value format
        char* equals_pos = strchr(line_buffer, '=');
        if (equals_pos) {
            *equals_pos = '\0'; // Split string
            char* config_key = line_buffer;
            char* config_value = equals_pos + 1;
            
            if (strcmp(config_key, key) == 0) {
                strncpy(value, config_value, max_len - 1);
                value[max_len - 1] = '\0';
                f_close(&config_file);
                return true;
            }
        }
    }
    
    f_close(&config_file);
    return false;
}

/**
 * @brief Write configuration to file
 * @param key Configuration key
 * @param value Configuration value
 * @return true if written successfully, false otherwise
 */
bool storage_write_config(const char* key, const char* value)
{
    if (!storage_mounted || !key || !value) return false;
    
    // TODO: Implement proper config file update
    // For now, just append new entries
    FIL config_file;
    FRESULT result;
    UINT bytes_written;
    
    result = f_open(&config_file, CONFIG_FILE_PATH, FA_WRITE | FA_OPEN_APPEND | FA_CREATE_ALWAYS);
    if (result != FR_OK) {
        return false;
    }
    
    snprintf(line_buffer, sizeof(line_buffer), "%s=%s\n", key, value);
    result = f_write(&config_file, line_buffer, strlen(line_buffer), &bytes_written);
    
    f_close(&config_file);
    
    return (result == FR_OK && bytes_written == strlen(line_buffer));
}

/**
 * @brief Get directory listing
 * @param path Directory path
 * @param listing Pointer to store directory listing
 * @return true if successful, false otherwise
 */
bool storage_list_directory(const char* path, dir_listing_t* listing)
{
    if (!storage_mounted || !path || !listing) return false;
    
    DIR dir;
    FILINFO file_info;
    FRESULT result;
    
    // Open directory
    result = f_opendir(&dir, path);
    if (result != FR_OK) {
        printf("ERROR: Failed to open directory %s (error %d)\n", path, result);
        return false;
    }
    
    // Initialize listing
    listing->count = 0;
    listing->capacity = 32; // Initial capacity
    listing->files = (file_info_t*)malloc(listing->capacity * sizeof(file_info_t));
    
    if (!listing->files) {
        f_closedir(&dir);
        return false;
    }
    
    // Read directory entries
    while ((result = f_readdir(&dir, &file_info)) == FR_OK && file_info.fname[0] != 0) {
        // Expand array if needed
        if (listing->count >= listing->capacity) {
            listing->capacity *= 2;
            file_info_t* new_files = (file_info_t*)realloc(listing->files, 
                                                           listing->capacity * sizeof(file_info_t));
            if (!new_files) {
                break; // Out of memory
            }
            listing->files = new_files;
        }
        
        // Fill file info
        file_info_t* file = &listing->files[listing->count];
        strncpy(file->name, file_info.fname, MAX_FILENAME_LEN - 1);
        file->name[MAX_FILENAME_LEN - 1] = '\0';
        
        snprintf(file->path, MAX_PATH_LEN, "%s/%s", path, file_info.fname);
        file->size = file_info.fsize;
        file->date = file_info.fdate;
        file->time = file_info.ftime;
        
        // Determine file type
        if (file_info.fattrib & AM_DIR) {
            file->type = FILE_TYPE_UNKNOWN; // Directory
        } else {
            const char* ext = strrchr(file->name, '.');
            if (ext) {
                if (strcmp(ext, ".log") == 0) {
                    file->type = FILE_TYPE_LOG;
                } else if (strcmp(ext, ".bin") == 0 || strcmp(ext, ".dump") == 0) {
                    file->type = FILE_TYPE_DUMP;
                } else if (strcmp(ext, ".ini") == 0 || strcmp(ext, ".cfg") == 0) {
                    file->type = FILE_TYPE_CONFIG;
                } else {
                    file->type = FILE_TYPE_UNKNOWN;
                }
            } else {
                file->type = FILE_TYPE_UNKNOWN;
            }
        }
        
        listing->count++;
    }
    
    f_closedir(&dir);
    return true;
}

/**
 * @brief Free directory listing memory
 * @param listing Directory listing to free
 * @return void
 */
void storage_free_listing(dir_listing_t* listing)
{
    if (listing && listing->files) {
        free(listing->files);
        listing->files = NULL;
        listing->count = 0;
        listing->capacity = 0;
    }
}

/**
 * @brief Delete file
 * @param path File path to delete
 * @return true if deleted successfully, false otherwise
 */
bool storage_delete_file(const char* path)
{
    if (!storage_mounted || !path) return false;
    
    FRESULT result = f_unlink(path);
    
    if (result == FR_OK) {
        printf("File deleted: %s\n", path);
        
        // Log the operation
        snprintf(line_buffer, sizeof(line_buffer), "File deleted: %s", path);
        storage_write_log(xTaskGetTickCount(), "INFO", line_buffer);
        
        return true;
    } else {
        printf("ERROR: Failed to delete file %s (error %d)\n", path, result);
        return false;
    }
}

/**
 * @brief Get file size
 * @param path File path
 * @return File size in bytes, 0 if file doesn't exist
 */
uint32_t storage_get_file_size(const char* path)
{
    if (!storage_mounted || !path) return 0;
    
    FILINFO file_info;
    FRESULT result = f_stat(path, &file_info);
    
    return (result == FR_OK) ? file_info.fsize : 0;
}

/**
 * @brief Check if file exists
 * @param path File path to check
 * @return true if file exists, false otherwise
 */
bool storage_file_exists(const char* path)
{
    if (!storage_mounted || !path) return false;
    
    FILINFO file_info;
    return (f_stat(path, &file_info) == FR_OK);
}

/**
 * @brief Get free space on SD card
 * @return Free space in bytes
 */
uint64_t storage_get_free_space(void)
{
    if (!storage_mounted) return 0;
    
    FATFS* fs_ptr;
    DWORD free_clusters;
    FRESULT result;
    
    result = f_getfree("", &free_clusters, &fs_ptr);
    if (result != FR_OK) {
        return 0;
    }
    
    // Calculate free space in bytes
    uint64_t free_space = (uint64_t)free_clusters * fs_ptr->csize * 512;
    
    return free_space;
}

/**
 * @brief Get total space on SD card
 * @return Total space in bytes
 */
uint64_t storage_get_total_space(void)
{
    if (!storage_mounted) return 0;
    
    FATFS* fs_ptr;
    DWORD free_clusters;
    FRESULT result;
    
    result = f_getfree("", &free_clusters, &fs_ptr);
    if (result != FR_OK) {
        return 0;
    }
    
    // Calculate total space in bytes
    uint64_t total_space = (uint64_t)(fs_ptr->n_fatent - 2) * fs_ptr->csize * 512;
    
    return total_space;
}

/**
 * @brief Format SD card (emergency function)
 * @return true if formatted successfully, false otherwise
 */
bool storage_format_card(void)
{
    if (!storage_mounted) return false;
    
    printf("WARNING: Formatting SD card - all data will be lost!\n");
    
    // Unmount first
    f_mount(NULL, "", 0);
    storage_mounted = false;
    
    // Format with FAT32
    BYTE work[FF_MAX_SS];
    FRESULT result = f_mkfs("", FM_FAT32, 0, work, sizeof(work));
    
    if (result == FR_OK) {
        printf("SD card formatted successfully\n");
        
        // Remount
        result = f_mount(&fs, "", 1);
        if (result == FR_OK) {
            storage_mounted = true;
            storage_write_log(xTaskGetTickCount(), "WARNING", "SD card formatted");
        }
        
        return true;
    } else {
        printf("ERROR: Failed to format SD card (error %d)\n", result);
        return false;
    }
}

/**
 * @brief Read file data
 * @param path File path
 * @param buffer Buffer to store data
 * @param max_size Maximum buffer size
 * @return Number of bytes read, 0 on error
 */
uint32_t storage_read_file(const char* path, uint8_t* buffer, uint32_t max_size)
{
    if (!storage_mounted || !path || !buffer || max_size == 0) return 0;
    
    FIL file;
    FRESULT result;
    UINT bytes_read;
    
    result = f_open(&file, path, FA_READ);
    if (result != FR_OK) {
        return 0;
    }
    
    result = f_read(&file, buffer, max_size, &bytes_read);
    f_close(&file);
    
    return (result == FR_OK) ? bytes_read : 0;
}

/**
 * @brief Write file data
 * @param path File path
 * @param data Data to write
 * @param size Data size
 * @return true if written successfully, false otherwise
 */
bool storage_write_file(const char* path, const uint8_t* data, uint32_t size)
{
    if (!storage_mounted || !path || !data || size == 0) return false;
    
    FIL file;
    FRESULT result;
    UINT bytes_written;
    
    result = f_open(&file, path, FA_WRITE | FA_CREATE_ALWAYS);
    if (result != FR_OK) {
        return false;
    }
    
    result = f_write(&file, data, size, &bytes_written);
    f_close(&file);
    
    return (result == FR_OK && bytes_written == size);
}
