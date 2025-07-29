# API Reference

Complete API documentation for KISS Fuzzer development.

## Core APIs

### Display API

#### Functions

##### `display_init()`
```c
bool display_init(void);
```
Initialize the OLED display system.

**Returns:** `true` if successful, `false` on error

##### `display_write_line()`
```c
void display_write_line(uint8_t line, const char* text, bool scroll);
```
Write text to a specific line on the banner display.

**Parameters:**
- `line`: Line number (0-7)
- `text`: Text string to display
- `scroll`: Enable scrolling for long text

### JTAG API

#### Functions

##### `jtag_init()`
```c
bool jtag_init(void);
```
Initialize the JTAG engine and PIO programs.

**Returns:** `true` if successful, `false` on error

##### `jtag_scan_chain()`
```c
bool jtag_scan_chain(jtag_scan_result_t* result);
```
Scan for devices in the JTAG chain.

**Parameters:**
- `result`: Pointer to store scan results

**Returns:** `true` if scan successful, `false` on error

##### `jtag_memory_dump()`
```c
uint32_t jtag_memory_dump(uint32_t start_addr, uint32_t length, uint8_t* data);
```
Perform memory dump via JTAG interface.

**Parameters:**
- `start_addr`: Starting memory address
- `length`: Number of bytes to dump
- `data`: Buffer to store dumped data

**Returns:** Number of bytes successfully dumped

#### Data Structures

##### `jtag_device_t`
```c
typedef struct {
    uint32_t idcode;           // Device IDCODE
    uint8_t ir_length;         // IR length in bits
    uint32_t manufacturer_id;  // Manufacturer ID
    uint32_t part_number;      // Part number
    uint32_t version;          // Device version
    char description[64];      // Human-readable description
} jtag_device_t;
```

##### `jtag_scan_result_t`
```c
typedef struct {
    uint8_t device_count;           // Number of devices found
    jtag_device_t devices[16];      // Device information
    uint8_t total_ir_length;        // Total IR chain length
    bool chain_valid;               // Chain integrity check
} jtag_scan_result_t;
```

### Power API

#### Functions

##### `power_init()`
```c
bool power_init(void);
```
Initialize power monitoring system.

**Returns:** `true` if successful, `false` on error

##### `power_get_battery_percent()`
```c
uint8_t power_get_battery_percent(void);
```
Get current battery charge percentage.

**Returns:** Battery percentage (0-100)

##### `power_is_charging()`
```c
bool power_is_charging(void);
```
Check if device is currently charging.

**Returns:** `true` if charging, `false` otherwise

### Storage API

#### Functions

##### `storage_init()`
```c
bool storage_init(void);
```
Initialize SD card storage system.

**Returns:** `true` if successful, `false` on error

##### `storage_write_log()`
```c
bool storage_write_log(uint32_t timestamp, const char* level, const char* message);
```
Write entry to log file.

**Parameters:**
- `timestamp`: Log entry timestamp
- `level`: Log level string
- `message`: Log message

**Returns:** `true` if written successfully, `false` on error

### Wi-Fi API

#### Functions

##### `wifi_init()`
```c
bool wifi_init(void);
```
Initialize Wi-Fi module and start access point.

**Returns:** `true` if successful, `false` on error

##### `wifi_start_ap()`
```c
bool wifi_start_ap(void);
```
Start Wi-Fi access point mode.

**Returns:** `true` if AP started successfully, `false` on error

## REST API Endpoints

The web interface provides a REST API for remote control:

### Status Endpoint
```
GET /api/status
```
Returns current system status in JSON format.

**Response:**
```json
{
    "battery_percent": 85,
    "charging": false,
    "wifi_connected": true,
    "jtag_scanning": false,
    "uptime_seconds": 3600
}
```

### Scan Control
```
POST /api/scan/start
Content-Type: application/json

{
    "frequency": 1000000,
    "voltage": 3.3
}
```
Start JTAG scanning with specified parameters.

```
POST /api/scan/stop
```
Stop current JTAG scanning operation.

### Log Access
```
GET /api/logs?lines=100&level=INFO
```
Retrieve recent log entries.

**Parameters:**
- `lines`: Number of lines to return (default: 50)
- `level`: Minimum log level (DEBUG, INFO, WARN, ERROR)

### File Operations
```
GET /api/files
```
List files on SD card.

```
GET /api/download/{filename}
```
Download a specific file from SD card.

## Error Codes

### Common Error Codes
```c
typedef enum {
    KISS_OK = 0,                    // Success
    KISS_ERROR_INVALID_PARAM = -1,  // Invalid parameter
    KISS_ERROR_HARDWARE_FAULT = -2, // Hardware failure
    KISS_ERROR_TIMEOUT = -3,        // Operation timeout
    KISS_ERROR_NO_MEMORY = -4,      // Memory allocation failed
    KISS_ERROR_NOT_INITIALIZED = -5, // Module not initialized
    KISS_ERROR_BUSY = -6,           // Resource busy
    KISS_ERROR_NOT_SUPPORTED = -7   // Feature not supported
} kiss_error_t;
```

### JTAG Specific Errors
```c
typedef enum {
    JTAG_ERROR_NO_DEVICES = -100,   // No devices detected
    JTAG_ERROR_CHAIN_BROKEN = -101, // JTAG chain integrity failure
    JTAG_ERROR_IR_MISMATCH = -102,  // IR length mismatch
    JTAG_ERROR_UNKNOWN_DEVICE = -103 // Unrecognized device IDCODE
} jtag_error_t;
```

## Configuration Constants

### System Configuration
```c
// Task priorities
#define SYSTEM_TASK_PRIORITY    (tskIDLE_PRIORITY + 4)
#define WIFI_TASK_PRIORITY      (tskIDLE_PRIORITY + 3)
#define JTAG_TASK_PRIORITY      (tskIDLE_PRIORITY + 2)
#define UI_TASK_PRIORITY        (tskIDLE_PRIORITY + 2)
#define POWER_TASK_PRIORITY     (tskIDLE_PRIORITY + 1)

// Hardware pins
#define JTAG_TCK_PIN    16
#define JTAG_TMS_PIN    17
#define JTAG_TDI_PIN    18
#define JTAG_TDO_PIN    19

// Display configuration
#define DISPLAY_WIDTH   240
#define DISPLAY_HEIGHT  64
#define FONT_WIDTH      6
#define FONT_HEIGHT     8
```

### Protocol Limits
```c
// JTAG configuration
#define JTAG_MAX_FREQUENCY     10000000  // 10 MHz
#define JTAG_MIN_FREQUENCY     1000      // 1 kHz
#define JTAG_MAX_CHAIN_LENGTH  16        // Maximum devices
#define JTAG_MAX_IR_LENGTH     32        // Maximum IR bits

// Memory operations
#define MAX_DUMP_SIZE          (1024*1024)  // 1MB maximum dump
#define DUMP_BLOCK_SIZE        1024         // Read block size
```
