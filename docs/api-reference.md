# API Reference

```{note}
**Navigation:** [Home](index.md) | [Getting Started](getting-started.md) | [Hardware](hardware.md) | [Software](software.md) | [Tutorials](tutorials.md) | [Development](development.md) | [Troubleshooting](troubleshooting.md)
```

This section provides comprehensive documentation for KISS Fuzzer software interfaces and development APIs.

## Design Philosophy

The KISS Fuzzer APIs are designed with the following principles:

**Consistency**: Function names clearly indicate their purpose and expected behavior
**Error Handling**: All functions provide clear return values indicating success or failure conditions
**Non-blocking Operations**: Long-running functions do not halt system operation
**Comprehensive Documentation**: Every function, parameter, and return value is fully documented

## Display System API

The display API provides control over the 240×64 OLED banner display for status information and user feedback.

### Core Functions

**Display Initialization**
```c
bool display_init(void);
```
Initializes the display subsystem and establishes I2C communication.

*Returns*: `true` if initialization successful, `false` if display hardware error

**Text Output**
```c
void display_write_line(uint8_t line, const char* text, bool scroll);
```
Outputs text to specified display line with optional scrolling for long content.

*Parameters*:
- `line`: Display line number (0-7, where 0 is top line)
- `text`: Null-terminated string to display
- `scroll`: Enable horizontal scrolling for text longer than display width

**Display Management**
```c
void display_clear(void);
void display_update(void);
```
Clear display buffer and force immediate display update respectively.

### Usage Examples

```c
// Initialize display system
if (!display_init()) {
    printf("Display initialization failed\n");
    return ERROR_DISPLAY;
}

// Display static status message
display_write_line(0, "KISS Fuzzer Ready", false);

// Display scrolling message for long text
display_write_line(1, "Scanning target device for JTAG interfaces", true);
```

## JTAG Engine API

The JTAG API provides direct control over JTAG/SWD operations for device scanning and communication.

### System Control

**JTAG Initialization**
```c
bool jtag_init(void);
```
Initializes JTAG engine and configures PIO programs for timing-critical operations.

*Returns*: `true` if successful, `false` if PIO configuration failed

**Device Scanning**
```c
bool jtag_scan_chain(jtag_scan_result_t* result);
```
Performs comprehensive scan of JTAG chain to identify connected devices.

*Parameters*:
- `result`: Pointer to structure for storing scan results

*Returns*: `true` if scan completed, `false` if communication error

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

---

## Crash Detection API

The crash detection subsystem monitors for target faults (timeouts, TAP stuck, hardfault, heartbeat loss) and now supports **intelligent crash deduplication**, which ensures reporting and triage focus only on unique bug-triggering events.

### Core Functions

**Initialization**

```c
void crash_detection_init(void);
```

Initializes the crash detection system, resetting statistics and deduplication database.

**Crash State Check**

```c
crash_type_t crash_detection_check(void);
```
Checks the target for crash conditions. Returns a `crash_type_t` value.

**Crash Handling**
```c
void crash_detection_handle(crash_type_t type);
```

Handles a detected crash event, records deduplication, and attempts recovery or reset.

### Intelligent Crash Deduplication

KISS implements **intelligent crash deduplication** as of v0.9.x. Each crash is fingerprinted using the program counter (PC) via JTAG/SWD. Only new, unseen PC values are treated as unique.

**Deduplication API:**
```c
bool crash_signature_known(uint32_t pc);
void crash_signature_add(uint32_t pc);
```

**Flow Example:**
```c
if (detected_crash) {
uint32_t pc = get_crash_pc(); // platform-specific PC retrieval
if (!crash_signature_known(pc)) {
crash_signature_add(pc);
// Log/report as a unique crash
} else {
// Handle as duplicate
}
}
```

*Default deduplication buffer*: 32 unique crash PCs (configurable ring buffer).

---

### Crash Types

```c
typedef enum {
CRASH_NONE, // No crash detected
CRASH_TIMEOUT, // Target not responsive
CRASH_HARDFAULT, // CPU in fault/reset handler
CRASH_NO_HEARTBEAT, // Heartbeat or polling failure
CRASH_TAP_STUCK // JTAG TAP stuck or unresponsive
} crash_type_t;
```

---

### Example Command Handlers
```c
cmd_result_t cmd_crash_check(int argc, char* argv[], char* response, size_t response_size);
cmd_result_t cmd_crash_handle(int argc, char* argv[], char* response, size_t response_size);
cmd_result_t cmd_crash_info (int argc, char* argv[], char* response, size_t response_size);
```

---

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

## Development & Testing

### Building the Project

```bash
# Standard build
mkdir build && cd build
cmake ..
make

# Build with tests
mkdir build_test && cd build_test
cmake -DBUILD_TESTS=ON ..
make
ctest --output-on-failure
```

### Running Tests

The project includes comprehensive unit and integration tests:

```bash
# Run all tests
make run_tests

# Run specific test suites
./test_display
./test_jtag
./test_power
./test_ui
./test_storage
```

### Test Coverage

- **Unit Tests**: Each module has dedicated unit tests
- **Integration Tests**: End-to-end functionality testing
- **Mock Framework**: Hardware abstraction for testing without physical hardware
- **Continuous Integration**: Automated testing on code changes

### Hardware Testing

Physical testing requires the KISS Fuzzer hardware:

```bash
# Flash to device
make flash

# Monitor debug output
minicom -D /dev/ttyACM0 -b 115200

# Access web interface
# Connect to "KISS-Fuzzer" Wi-Fi network
# Browse to http://192.168.4.1
```
