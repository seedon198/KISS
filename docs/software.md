# Software Architecture

Understanding the KISS Fuzzer software design and implementation.

## System Overview

KISS Fuzzer uses a layered architecture built on FreeRTOS with modular components for maximum maintainability and extensibility.

## Architecture Layers

```{mermaid}
graph TD
    A[Application Layer] --> B[Hardware Abstraction Layer]
    B --> C[FreeRTOS Kernel]
    C --> D[Pico SDK]
    D --> E[Hardware]
    
    subgraph "Application Components"
        F[UI System]
        G[JTAG Engine]
        H[Wi-Fi Server]
        I[Power Management]
        J[Storage Manager]
    end
    
    A --> F
    A --> G
    A --> H
    A --> I
    A --> J
```

## Core Modules

### Display Driver (`display.c/h`)

Manages the 240×64 OLED banner display:

- **Protocol**: I2C communication with SSD1306 controller
- **Features**: Text rendering, graphics primitives, brightness control
- **Performance**: Non-blocking updates with frame buffering

### UI System (`ui.c/h`)

Handles user interaction and menu navigation:

- **Input**: 5-way joystick with debouncing
- **Menus**: Hierarchical menu system with context awareness
- **Events**: Queue-based event handling for responsiveness

### JTAG Engine (`jtag.c/h`)

Core protocol implementation using PIO:

- **Protocols**: JTAG (IEEE 1149.1) and SWD
- **Speed**: Up to 10 MHz using dedicated PIO state machines
- **Features**: Device scanning, boundary scan, memory operations

### Wi-Fi Server (`wifi.c/h`)

Web interface and remote control:

- **Mode**: Access Point with captive portal
- **Protocol**: HTTP server with REST API
- **Features**: Real-time status, file transfer, remote control

### Power Management (`power.c/h`)

Battery monitoring and power optimization:

- **Monitoring**: Voltage, current, and charge status
- **Protection**: Over-discharge and thermal protection
- **Optimization**: Dynamic frequency scaling and sleep modes

### Storage Manager (`storage.c/h`)

MicroSD card operations:

- **Filesystem**: FAT32 with wear leveling
- **Logging**: Structured log files with rotation
- **Data**: Scan results, memory dumps, configuration

## Task Architecture

### Task Priorities

| Task | Priority | Stack Size | Description |
|------|----------|------------|-------------|
| **System** | Highest (4) | 4KB | Critical system monitoring |
| **Wi-Fi** | High (3) | 8KB | Network communication |
| **JTAG** | Medium (2) | 6KB | Protocol operations |
| **UI** | Medium (2) | 4KB | User interface |
| **Power** | Low (1) | 2KB | Background monitoring |

### Inter-Task Communication

```{mermaid}
graph LR
    A[UI Task] -->|Events| B[Queue]
    B --> C[JTAG Task]
    
    D[JTAG Task] -->|Logs| E[Log Queue]
    E --> F[Storage Task]
    
    G[Power Task] -->|Status| H[Status Queue]
    H --> A
    H --> I[Wi-Fi Task]
```

## Memory Management

### Heap Configuration

- **Total Heap**: 128KB FreeRTOS heap
- **Allocation**: heap_4 algorithm with coalescence
- **Protection**: Stack overflow detection enabled
- **Monitoring**: Runtime heap usage tracking

### Buffer Management

- **Display Buffer**: 2KB frame buffer for OLED
- **JTAG Buffer**: 4KB for protocol data
- **Wi-Fi Buffer**: 8KB for HTTP requests/responses
- **SD Buffer**: 2KB for filesystem operations

## Error Handling

### Exception Handling

```c
// Example error handling pattern
typedef enum {
    KISS_OK = 0,
    KISS_ERROR_INVALID_PARAM,
    KISS_ERROR_HARDWARE_FAULT,
    KISS_ERROR_TIMEOUT,
    KISS_ERROR_NO_MEMORY
} kiss_error_t;

kiss_error_t operation_with_error_handling(void) {
    if (!validate_parameters()) {
        return KISS_ERROR_INVALID_PARAM;
    }
    
    if (!hardware_ready()) {
        return KISS_ERROR_HARDWARE_FAULT;
    }
    
    // Perform operation...
    return KISS_OK;
}
```

### Logging System

- **Levels**: DEBUG, INFO, WARN, ERROR, FATAL
- **Destinations**: UART, SD card, Wi-Fi
- **Format**: Structured logging with timestamps
- **Rotation**: Automatic log file rotation

## Configuration Management

### Compile-Time Configuration

Settings defined in header files:

```c
// kiss_fuzzer.h
#define JTAG_MAX_FREQUENCY_HZ    10000000
#define WIFI_SSID                "KISS-Fuzzer"
#define BATTERY_LOW_THRESHOLD    15
#define LOG_LEVEL                LOG_INFO
```

### Runtime Configuration

User-configurable settings stored on SD card:

- **Target Voltage**: 1.8V, 3.3V, 5V options
- **JTAG Speed**: Variable from 1 kHz to 10 MHz
- **Wi-Fi Settings**: SSID, password, channel
- **Display**: Brightness, timeout, orientation

## Security Considerations

### Input Validation

- **Bounds Checking**: All user inputs validated
- **Sanitization**: Web interface input sanitization
- **Rate Limiting**: Protection against abuse

### Memory Safety

- **Stack Protection**: Canary values on critical stacks
- **Buffer Overflow**: Compile-time and runtime checks
- **Integer Overflow**: Safe arithmetic operations

## Performance Optimization

### Critical Paths

- **JTAG Timing**: PIO ensures deterministic timing
- **Display Updates**: Frame rate limiting for efficiency
- **Wi-Fi Response**: Asynchronous request handling

### Power Optimization

- **CPU Scaling**: Dynamic frequency adjustment
- **Peripheral Control**: Selective peripheral shutdown
- **Sleep Modes**: Aggressive sleep when idle
