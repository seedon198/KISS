/**
 * @file storage.c
 * @brief SD card storage system implementation
 * @author KISS Fuzzer Team
 * @date 2025
 * @version 0.7.0
 */

#include "storage.h"

// Static variables for storage state
static bool storage_ready = false;
static storage_status_t current_status = {0};

/**
 * @brief Initialize the storage system
 * @return true if successful, false otherwise
 */
bool storage_init(void) {
    printf("Initializing storage system...\n");
    
    // Initialize SPI for SD card
    spi_init(STORAGE_SPI_PORT, STORAGE_SPI_FREQ);
    
    // Configure SPI pins
    gpio_set_function(STORAGE_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(STORAGE_MOSI_PIN, GPIO_FUNC_SPI);
    gpio_set_function(STORAGE_MISO_PIN, GPIO_FUNC_SPI);
    
    // Configure CS pin as GPIO output
    gpio_init(STORAGE_CS_PIN);
    gpio_set_dir(STORAGE_CS_PIN, GPIO_OUT);
    gpio_put(STORAGE_CS_PIN, 1); // CS high (inactive)
    
    // Initialize FatFS
    FRESULT result = f_mount(&current_status.filesystem, STORAGE_ROOT_PATH, 1);
    
    if (result == FR_OK) {
        current_status.mounted = true;
        current_status.card_present = true;
        
        // Get volume information
        FATFS* fs;
        DWORD free_clusters;
        result = f_getfree(STORAGE_ROOT_PATH, &free_clusters, &fs);
        
        if (result == FR_OK) {
            // Calculate sizes (cluster size * sectors per cluster * 512 bytes per sector)
            uint32_t total_sectors = (fs->n_fatent - 2) * fs->csize;
            uint32_t free_sectors = free_clusters * fs->csize;
            
            current_status.total_size_mb = (total_sectors * 512) / (1024 * 1024);
            current_status.free_size_mb = (free_sectors * 512) / (1024 * 1024);
        }
        
        // Get volume label
        f_getlabel(STORAGE_ROOT_PATH, current_status.volume_label, NULL);
        if (strlen(current_status.volume_label) == 0) {
            strcpy(current_status.volume_label, "KISS-FUZZER");
        }
        
        // Create directory structure
        storage_create_directory(STORAGE_LOGS_DIR);
        storage_create_directory(STORAGE_SCANS_DIR);
        storage_create_directory(STORAGE_DUMPS_DIR);
        storage_create_directory(STORAGE_CONFIG_DIR);
        
        storage_ready = true;
        printf("Storage system initialized successfully\n");
        printf("SD Card: %s (%lu MB total, %lu MB free)\n",
               current_status.volume_label,
               current_status.total_size_mb,
               current_status.free_size_mb);
        
        // Log system startup
        storage_log_system_event("Storage system initialized", 6);
        
        return true;
    } else {
        printf("Failed to mount SD card (error %d)\n", result);
        current_status.mounted = false;
        current_status.card_present = false;
        storage_ready = false;
        return false;
    }
}

/**
 * @brief Deinitialize the storage system
 */
void storage_deinit(void) {
    if (!storage_ready) return;
    
    storage_log_system_event("Storage system shutting down", 6);
    f_unmount(STORAGE_ROOT_PATH);
    
    storage_ready = false;
    current_status.mounted = false;
    
    printf("Storage system deinitialized\n");
}

/**
 * @brief Check if SD card is present and mounted
 * @return true if ready for operations
 */
bool storage_is_ready(void) {
    return storage_ready && current_status.mounted;
}

/**
 * @brief Get storage system status
 * @return Storage status structure
 */
storage_status_t storage_get_status(void) {
    return current_status;
}

/**
 * @brief Create directory if it doesn't exist
 * @param path Directory path
 * @return true if successful or already exists
 */
bool storage_create_directory(const char* path) {
    if (!storage_is_ready() || !path) return false;
    
    FRESULT result = f_mkdir(path);
    return (result == FR_OK || result == FR_EXIST);
}

/**
 * @brief Get current timestamp string
 * @param buffer Buffer to store timestamp
 * @param buffer_size Size of buffer
 */
static void storage_get_timestamp(char* buffer, size_t buffer_size) {
    // For now, use system uptime. In production, use RTC
    uint32_t uptime_ms = to_ms_since_boot(get_absolute_time());
    uint32_t seconds = uptime_ms / 1000;
    uint32_t minutes = seconds / 60;
    uint32_t hours = minutes / 60;
    
    snprintf(buffer, buffer_size, "%02lu%02lu%02lu",
             hours % 24, minutes % 60, seconds % 60);
}

/**
 * @brief Generate filename with timestamp
 * @param file_type Type of file for appropriate naming
 * @param buffer Buffer to store generated filename
 * @param buffer_size Size of buffer
 * @return true if successful
 */
bool storage_generate_filename(storage_file_type_t file_type, char* buffer, size_t buffer_size) {
    if (!buffer || buffer_size == 0) return false;
    
    char timestamp[16];
    storage_get_timestamp(timestamp, sizeof(timestamp));
    
    switch (file_type) {
        case STORAGE_FILE_JTAG_SCAN:
            snprintf(buffer, buffer_size, "%s/scan_%s.txt", STORAGE_SCANS_DIR, timestamp);
            break;
        case STORAGE_FILE_MEMORY_DUMP:
            snprintf(buffer, buffer_size, "%s/dump_%s.bin", STORAGE_DUMPS_DIR, timestamp);
            break;
        case STORAGE_FILE_SYSTEM_LOG:
            snprintf(buffer, buffer_size, "%s/system_%s.log", STORAGE_LOGS_DIR, timestamp);
            break;
        case STORAGE_FILE_CONFIG:
            snprintf(buffer, buffer_size, "%s/config_%s.cfg", STORAGE_CONFIG_DIR, timestamp);
            break;
        case STORAGE_FILE_GENERIC:
        default:
            snprintf(buffer, buffer_size, "/file_%s.dat", timestamp);
            break;
    }
    
    return true;
}

/**
 * @brief Write data to file
 * @param filename Full path to file
 * @param data Data to write
 * @param size Size of data in bytes
 * @param append True to append, false to overwrite
 * @return Number of bytes written, 0 on error
 */
size_t storage_write_file(const char* filename, const void* data, size_t size, bool append) {
    if (!storage_is_ready() || !filename || !data || size == 0) return 0;
    
    FIL file;
    FRESULT result = f_open(&file, filename, FA_WRITE | (append ? FA_OPEN_APPEND : FA_CREATE_ALWAYS));
    
    if (result == FR_OK) {
        UINT bytes_written;
        result = f_write(&file, data, size, &bytes_written);
        f_close(&file);
        
        if (result == FR_OK) {
            printf("Wrote %u bytes to %s\n", bytes_written, filename);
            return bytes_written;
        }
    }
    
    printf("Failed to write to %s (error %d)\n", filename, result);
    return 0;
}

/**
 * @brief Read data from file
 * @param filename Full path to file
 * @param buffer Buffer to read into
 * @param buffer_size Size of buffer
 * @return Number of bytes read, 0 on error
 */
size_t storage_read_file(const char* filename, void* buffer, size_t buffer_size) {
    if (!storage_is_ready() || !filename || !buffer || buffer_size == 0) return 0;
    
    FIL file;
    FRESULT result = f_open(&file, filename, FA_READ);
    
    if (result == FR_OK) {
        UINT bytes_read;
        result = f_read(&file, buffer, buffer_size, &bytes_read);
        f_close(&file);
        
        if (result == FR_OK) {
            return bytes_read;
        }
    }
    
    return 0;
}

/**
 * @brief Write text line to file
 * @param filename Full path to file
 * @param text Text to write (newline added automatically)
 * @param append True to append, false to overwrite
 * @return true if successful
 */
bool storage_write_line(const char* filename, const char* text, bool append) {
    if (!text) return false;
    
    char line_buffer[512];
    snprintf(line_buffer, sizeof(line_buffer), "%s\n", text);
    
    size_t written = storage_write_file(filename, line_buffer, strlen(line_buffer), append);
    return (written > 0);
}

/**
 * @brief Log JTAG scan results
 * @param scan_data Scan result data
 * @param data_size Size of scan data
 * @return true if logged successfully
 */
bool storage_log_jtag_scan(const void* scan_data, size_t data_size) {
    if (!storage_is_ready() || !scan_data || data_size == 0) return false;
    
    char filename[STORAGE_MAX_PATH];
    if (!storage_generate_filename(STORAGE_FILE_JTAG_SCAN, filename, sizeof(filename))) {
        return false;
    }
    
    // Write header
    char header[128];
    char timestamp[16];
    storage_get_timestamp(timestamp, sizeof(timestamp));
    snprintf(header, sizeof(header), "JTAG Scan Results - Timestamp: %s\n", timestamp);
    
    bool success = storage_write_line(filename, header, false);
    if (success) {
        size_t written = storage_write_file(filename, scan_data, data_size, true);
        success = (written == data_size);
    }
    
    if (success) {
        printf("JTAG scan logged to %s\n", filename);
    }
    
    return success;
}

/**
 * @brief Log system event
 * @param event_text Event description
 * @param priority Priority level (0-7)
 * @return true if logged successfully
 */
bool storage_log_system_event(const char* event_text, uint8_t priority) {
    if (!storage_is_ready() || !event_text) return false;
    
    // Use a daily log file
    char filename[] = "/logs/system.log";
    
    char timestamp[16];
    storage_get_timestamp(timestamp, sizeof(timestamp));
    
    char log_entry[256];
    snprintf(log_entry, sizeof(log_entry), "[%s] [%d] %s",
             timestamp, priority, event_text);
    
    return storage_write_line(filename, log_entry, true);
}

/**
 * @brief Save memory dump to file
 * @param address Start address of dump
 * @param data Memory data
 * @param size Size of data
 * @return true if saved successfully
 */
bool storage_save_memory_dump(uint32_t address, const void* data, size_t size) {
    if (!storage_is_ready() || !data || size == 0) return false;
    
    char filename[STORAGE_MAX_PATH];
    if (!storage_generate_filename(STORAGE_FILE_MEMORY_DUMP, filename, sizeof(filename))) {
        return false;
    }
    
    // Write header with address information
    char header[64];
    snprintf(header, sizeof(header), "Memory dump from 0x%08lX (%zu bytes)\n", address, size);
    
    bool success = storage_write_line(filename, header, false);
    if (success) {
        size_t written = storage_write_file(filename, data, size, true);
        success = (written == size);
    }
    
    if (success) {
        printf("Memory dump saved to %s\n", filename);
    }
    
    return success;
}

/**
 * @brief List files in directory
 * @param path Directory path
 * @param files Array to store file information
 * @param max_files Maximum number of files to list
 * @return Number of files found
 */
uint16_t storage_list_files(const char* path, storage_file_info_t* files, uint16_t max_files) {
    if (!storage_is_ready() || !path || !files || max_files == 0) return 0;
    
    DIR dir;
    FILINFO file_info;
    FRESULT result = f_opendir(&dir, path);
    
    if (result != FR_OK) return 0;
    
    uint16_t file_count = 0;
    
    while (file_count < max_files) {
        result = f_readdir(&dir, &file_info);
        if (result != FR_OK || file_info.fname[0] == 0) break;
        
        strncpy(files[file_count].filename, file_info.fname, STORAGE_MAX_FILENAME - 1);
        files[file_count].filename[STORAGE_MAX_FILENAME - 1] = '\0';
        files[file_count].size_bytes = file_info.fsize;
        files[file_count].date = file_info.fdate;
        files[file_count].time = file_info.ftime;
        files[file_count].is_directory = (file_info.fattrib & AM_DIR) != 0;
        
        file_count++;
    }
    
    f_closedir(&dir);
    return file_count;
}

/**
 * @brief Delete file
 * @param filename Full path to file
 * @return true if successful
 */
bool storage_delete_file(const char* filename) {
    if (!storage_is_ready() || !filename) return false;
    
    FRESULT result = f_unlink(filename);
    return (result == FR_OK);
}

/**
 * @brief Get file size
 * @param filename Full path to file
 * @return File size in bytes, 0 if error
 */
uint32_t storage_get_file_size(const char* filename) {
    if (!storage_is_ready() || !filename) return 0;
    
    FILINFO file_info;
    FRESULT result = f_stat(filename, &file_info);
    
    return (result == FR_OK) ? file_info.fsize : 0;
}

/**
 * @brief Check if file exists
 * @param filename Full path to file
 * @return true if file exists
 */
bool storage_file_exists(const char* filename) {
    if (!storage_is_ready() || !filename) return false;
    
    FILINFO file_info;
    FRESULT result = f_stat(filename, &file_info);
    
    return (result == FR_OK);
}

/**
 * @brief Format storage (WARNING: Deletes all data)
 * @return true if successful
 */
bool storage_format(void) {
    if (!storage_ready) return false;
    
    printf("WARNING: Formatting storage - all data will be lost!\n");
    
    // Unmount first
    f_unmount(STORAGE_ROOT_PATH);
    
    // Format with default settings
    FRESULT result = f_mkfs(STORAGE_ROOT_PATH, 0, 0, NULL, 0);
    
    if (result == FR_OK) {
        // Remount and reinitialize
        storage_init();
        storage_log_system_event("Storage formatted", 5);
        return true;
    }
    
    return false;
}

/**
 * @brief Get human-readable file type name
 * @param file_type File type
 * @return String name of the file type
 */
const char* storage_file_type_name(storage_file_type_t file_type) {
    switch (file_type) {
        case STORAGE_FILE_JTAG_SCAN:   return "JTAG Scan";
        case STORAGE_FILE_MEMORY_DUMP: return "Memory Dump";
        case STORAGE_FILE_SYSTEM_LOG:  return "System Log";
        case STORAGE_FILE_CONFIG:      return "Configuration";
        case STORAGE_FILE_GENERIC:     return "Generic";
        default:                       return "Unknown";
    }
}

/**
 * @brief Get formatted storage status string
 * @param buffer Buffer to write to
 * @param buffer_size Size of buffer
 */
void storage_get_status_string(char* buffer, size_t buffer_size) {
    if (!buffer || buffer_size == 0) return;
    
    if (storage_is_ready()) {
        snprintf(buffer, buffer_size, "SD %luMB free",
                current_status.free_size_mb);
    } else {
        snprintf(buffer, buffer_size, "SD Not Ready");
    }
}
