# Software Architecture

```{note}
**Navigation:** [Home](index.md) | [Getting Started](getting-started.md) | [Hardware](hardware.md) | [API Reference](api-reference.md) | [Tutorials](tutorials.md) | [Development](development.md) | [Troubleshooting](troubleshooting.md)
```

This section provides detailed information about the KISS Fuzzer software architecture, implementation, and development practices.

## System Overview

The KISS Fuzzer software architecture implements a multi-layered design that ensures reliable operation and maintainable code structure:

```mermaid
graph TD
    A[Interface Layer] --> B[Hardware Abstraction]
    B --> C[FreeRTOS Kernel]
    C --> D[Pico SDK]
    D --> E[Raw Hardware]
    
    subgraph "Core Modules"
        F[Display & UI]
        G[JTAG Engine]
        H[Wi-Fi Server]
        I[Power Management]
        J[File Storage]
    end
    
    A --> F
    A --> G
    A --> H
    A --> I
    A --> J
```

**Design Philosophy**: Each layer handles specific responsibilities. The JTAG engine operates independently of Wi-Fi, the display system doesn't manage battery levels, and the web server doesn't need JTAG protocol knowledge. This separation ensures reliability and simplifies debugging.

## Core Components

### Display Driver

The ultra-wide OLED display system provides real-time status information with optimized performance:

- **Smooth Operation**: Background updates prevent flicker and lag
- **Power Efficiency**: Display only updates when content changes
- **Clear Rendering**: Custom font rendering ensures readability on the narrow display

**Technical Implementation**: Double buffering renders content to memory before display transfer, eliminating visual artifacts and ensuring smooth updates.

### UI System

The user interface manages complexity through intuitive design:

- **Intuitive Navigation**: Joystick controls follow standard conventions - up/down for scrolling, left for back navigation, right/OK for selection
- **Context Awareness**: The interface displays relevant information based on current operation
- **Responsive Feedback**: Immediate visual confirmation for all user interactions

### JTAG Engine (`jtag.c/h`)

Core protocol implementation using PIO:

- **Protocols**: JTAG (IEEE 1149.1) and SWD
- **Speed**: Up to 10 MHz using dedicated PIO state machines
- **Features**: Device scanning, boundary scan, memory operations

### Advanced Fuzzing Engine (`jtag_fuzzer.c/h`, `fuzz_commands.c/h`)

**New in v0.9.0**: Sophisticated fuzzing capabilities with interactive UI integration:

- **Strategies**: Sequential, Random, Dictionary, Genetic, Smart fuzzing
- **Operations**: IDCODE scan, IR/DR fuzzing, boundary scan, memory probe
- **Automation**: Pin discovery with confidence ratings
- **Real-time**: Progress monitoring, anomaly detection, statistics
- **Interface**: Command-line integration with predefined operations
- **Logging**: Comprehensive session tracking and result storage

**UI Integration Features**:
- Dedicated fuzzing menu accessible from main interface
- Interactive command selection with joystick navigation
- Real-time status display and progress feedback
- Seamless integration with FreeRTOS task architecture

See [Fuzzing UI Integration](fuzzing-ui-integration.md) for complete details.

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

```mermaid
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

## Detailed System Architecture

### Power Management & Glitch Injection Architecture

```mermaid
flowchart LR
    subgraph INPUT [" Input Power "]
        USB["USB-C Input<br/>5V 3A Max<br/>PD Compatible"]
        BATT["Li-ion Battery<br/>3.7V 2000mAh<br/>JST-PH"]
    end
    
    subgraph MGMT [" Power Management "]
        CHARGER["Charging IC<br/>MCP73871<br/>Path Control"]
        PMGMT["Power Unit<br/>Source Select<br/>Monitor"]
        BOOST["Boost 5V<br/>TPS61200<br/>High Efficiency"]
    end
    
    subgraph RAILS [" Power Rails "]
        RAIL33["3.3V Rail<br/>System Logic<br/>RP2040"]
        RAIL18["1.8V Rail<br/>Low Power<br/>Shifters"]
        RAIL5V["5V Rail<br/>Target Power<br/>High Current"]
    end
    
    subgraph CONTROL [" Target Control "]
        LEVELSEL["Level Select<br/>GPIO Control<br/>1.8V-5V"]
        CURRENTLIM["Current Limit<br/>MAX4173<br/>500mA Max"]
        POWERSWITCH["Power Switch<br/>Load Control<br/>Protection"]
    end
    
    subgraph GLITCH [" Glitch System "]
        GLITCHCTRL["Glitch Control<br/>PIO Timing<br/>Patterns"]
        MOSFETDRV["MOSFET Driver<br/>TC4427<br/>Fast Switch"]
        POWERMOS["Power MOSFET<br/>Si7021<br/>Low RDS"]
    end
    
    subgraph TARGET [" Target Output "]
        TARGETPWR["Target VCC<br/>JTAG Pin 1<br/>Protected"]
        TARGETGND["Target GND<br/>JTAG Pin 8<br/>Reference"]
    end
    
    USB --> CHARGER
    BATT --> CHARGER
    CHARGER --> PMGMT
    PMGMT --> BOOST
    PMGMT --> RAIL33
    PMGMT --> RAIL18
    BOOST --> RAIL5V
    
    RAIL33 --> LEVELSEL
    RAIL18 --> LEVELSEL
    RAIL5V --> LEVELSEL
    LEVELSEL --> CURRENTLIM
    CURRENTLIM --> POWERSWITCH
    POWERSWITCH --> TARGETPWR
    
    RAIL33 --> GLITCHCTRL
    GLITCHCTRL --> MOSFETDRV
    MOSFETDRV --> POWERMOS
    POWERMOS -.-> TARGETPWR
    
    classDef input fill:#e3f2fd,stroke:#90caf9,stroke-width:2px,color:#000
    classDef mgmt fill:#e1f5fe,stroke:#81d4fa,stroke-width:2px,color:#000
    classDef rail fill:#e0f7fa,stroke:#80deea,stroke-width:2px,color:#000
    classDef control fill:#e0f2f1,stroke:#80cbc4,stroke-width:2px,color:#000
    classDef glitch fill:#fce4ec,stroke:#f8bbd9,stroke-width:2px,color:#000
    classDef target fill:#f3e5f5,stroke:#ce93d8,stroke-width:2px,color:#000
    
    class USB,BATT input
    class CHARGER,PMGMT,BOOST mgmt
    class RAIL33,RAIL18,RAIL5V rail
    class LEVELSEL,CURRENTLIM,POWERSWITCH control
    class GLITCHCTRL,MOSFETDRV,POWERMOS glitch
    class TARGETPWR,TARGETGND target
```

### Hardware Block Diagram

```mermaid
flowchart TB
    subgraph "User Interface Layer"
        direction LR
        J1[5-Way Joystick<br/>Navigation] 
        USB[USB-C Port<br/>Power/Data]
        WIFI[Wi-Fi Module<br/>📶 802.11n]
    end
    
    subgraph "Processing Core"
        direction TB
        MCU[RP2040 MCU<br/>🧠 Dual ARM Cortex-M0+<br/>133MHz • 264KB SRAM]
        PIO[PIO Engine<br/>8 State Machines<br/>High-Speed Protocol I/O]
        GPIO[GPIO Controller<br/>26 Configurable Pins<br/>3.3V Logic Level]
    end
    
    subgraph "Output & Debug"
        direction LR
        OLED[240×64 OLED<br/>Banner Display<br/>Real-time Status]
        JTAG[JTAG/SWD Port<br/>Debug Interface<br/>10MHz Max Speed]
        SD[MicroSD Card<br/>Data Logging<br/>Scan Results]
    end
    
    subgraph "Power Management"
        direction TB
        BATT[Li-ion Battery<br/>🔋 3.7V 2000mAh<br/>USB-C Charging]
        PMGMT[Power Controller<br/>LDO Regulators<br/>Voltage Monitoring]
        TPWR[Target Power<br/>1.8V - 5V Output<br/>Selectable Levels]
    end
    
    subgraph "Protection & Control"
        direction LR
        PROTECT[Protection Circuit<br/>Over-voltage<br/>Reverse Polarity]
        GLITCH[Glitch Generator<br/>MOSFET Switch<br/>Fault Injection]
        LEDS[Status LEDs<br/>Power/Activity<br/>Error Indication]
    end
    
    %% Connections
    J1 --> MCU
    USB --> BATT
    WIFI --> MCU
    
    MCU --> PIO
    MCU --> GPIO
    
    MCU --> OLED
    PIO --> JTAG
    GPIO --> SD
    
    BATT --> PMGMT
    PMGMT --> MCU
    PMGMT --> TPWR
    
    GPIO --> PROTECT
    GPIO --> GLITCH
    GPIO --> LEDS
    
    PROTECT --> JTAG
    GLITCH --> TPWR
    
    %% Styling
    classDef userInterface fill:#e3f2fd,stroke:#1976d2,stroke-width:2px,color:#000
    classDef processing fill:#e8f5e8,stroke:#388e3c,stroke-width:2px,color:#000
    classDef output fill:#fff3e0,stroke:#f57c00,stroke-width:2px,color:#000
    classDef power fill:#fce4ec,stroke:#c2185b,stroke-width:2px,color:#000
    classDef protection fill:#f3e5f5,stroke:#7b1fa2,stroke-width:2px,color:#000
    
    class J1,USB,WIFI userInterface
    class MCU,PIO,GPIO processing
    class OLED,JTAG,SD output
    class BATT,PMGMT,TPWR power
    class PROTECT,GLITCH,LEDS protection
```

### Software Architecture Flow

```mermaid
flowchart TD
    subgraph "Application Layer"
        direction TB
        APP1[UI System<br/>Menu Navigation<br/>User Interaction]
        APP2[Wi-Fi Server<br/>Web Interface<br/>HTTP API]
        APP3[JTAG Engine<br/>Protocol Handler<br/>Command Processing]
        APP4[Logging System<br/>Event Recording<br/>File Management]
    end
    
    subgraph "FreeRTOS Kernel"
        direction LR
        RTOS1[Task Scheduler<br/>Preemptive<br/>Priority-based]
        RTOS2[Memory Manager<br/>Heap Management<br/>Stack Protection]
        RTOS3[IPC System<br/>Queues & Semaphores<br/>Inter-task Comm.]
    end
    
    subgraph "Hardware Abstraction Layer"
        direction TB
        HAL1[Display Driver<br/>SPI Interface<br/>Graphics Rendering]
        HAL2[Storage Driver<br/>💾 SD Card SPI<br/>File System]
        HAL3[Power Monitor<br/>ADC Reading<br/>Battery Status]
        HAL4[JTAG Driver<br/>PIO State Machines<br/>Protocol Timing]
        HAL5[Network Driver<br/>📶 Wi-Fi Stack<br/>TCP/IP Layer]
    end
    
    subgraph "Pico SDK Foundation"
        direction LR
        SDK1[Hardware APIs<br/>GPIO/SPI/PIO<br/>Low-level Control]
        SDK2[System Services<br/>Clocks/Timers<br/>Interrupt Handling]
        SDK3[Boot & Flash<br/>System Init<br/>Flash Management]
    end
    
    %% Vertical connections (layered architecture)
    APP1 --> RTOS1
    APP2 --> RTOS1
    APP3 --> RTOS1
    APP4 --> RTOS1
    
    RTOS1 --> HAL1
    RTOS1 --> HAL2
    RTOS1 --> HAL3
    RTOS1 --> HAL4
    RTOS1 --> HAL5
    
    HAL1 --> SDK1
    HAL2 --> SDK1
    HAL3 --> SDK1
    HAL4 --> SDK1
    HAL5 --> SDK1
    
    %% Horizontal connections (inter-component)
    APP1 -.-> RTOS3
    APP2 -.-> RTOS3
    APP3 -.-> RTOS3
    APP4 -.-> RTOS3
    
    RTOS1 -.-> RTOS2
    RTOS2 -.-> RTOS3
    
    SDK1 -.-> SDK2
    SDK2 -.-> SDK3
    
    %% Styling
    classDef application fill:#e3f2fd,stroke:#1976d2,stroke-width:2px,color:#000
    classDef rtos fill:#e8f5e8,stroke:#388e3c,stroke-width:2px,color:#000
    classDef hal fill:#fff3e0,stroke:#f57c00,stroke-width:2px,color:#000
    classDef sdk fill:#fce4ec,stroke:#c2185b,stroke-width:2px,color:#000
    
    class APP1,APP2,APP3,APP4 application
    class RTOS1,RTOS2,RTOS3 rtos
    class HAL1,HAL2,HAL3,HAL4,HAL5 hal
    class SDK1,SDK2,SDK3 sdk
```

### JTAG/SWD Operation Flow

```mermaid
flowchart TD
    Start([Start Power On<br/>System Boot]) --> Init[Initialize Hardware<br/>GPIO • SPI • PIO • Wi-Fi]
    Init --> SelfTest[Self Test<br/>Display • SD Card • Battery]
    SelfTest --> Menu{Main Menu<br/>Select Operation}
    
    Menu -->|1| AutoScan[Auto JTAG Scan<br/>Smart Detection]
    Menu -->|2| Manual[Manual Mode<br/>Expert Operations]
    Menu -->|3| Glitch[Power Glitch<br/>Fault Injection]
    Menu -->|4| Network[Network Operations<br/>Wi-Fi & Web UI]
    Menu -->|5| Settings[Configuration<br/>System Settings]
    
    %% Auto Scan Flow
    AutoScan --> CheckTarget[Check Target<br/>Power & Connectivity]
    CheckTarget --> PinDetect[Detect JTAG Pins<br/>TCK • TMS • TDI • TDO]
    PinDetect --> ValidateChain[Validate Chain<br/>TAP Response Test]
    ValidateChain --> ScanDevices[Scan JTAG Chain<br/>Read Device IDCODEs]
    ScanDevices --> ChainAnalysis[Chain Analysis<br/>Device Count & Types]
    ChainAnalysis --> DisplayResults[Show Results<br/>OLED Display]
    
    %% Manual Mode Flow
    Manual --> SelectOp{Select Operation<br/>Choose Command Type}
    SelectOp -->|Mem| MemDump[Memory Dump<br/>Read Flash/RAM]
    SelectOp -->|Scan| BoundaryScan[Boundary Scan<br/>IEEE 1149.1 Test]
    SelectOp -->|Cmd| CustomCmd[Custom Commands<br/>Direct JTAG Access]
    SelectOp -->|Write| MemWrite[Memory Write<br/>Program Flash]
    
    %% Glitch Attack Flow
    Glitch --> GlitchSetup[Setup Parameters<br/>Timing • Voltage • Count]
    GlitchSetup --> GlitchArm[Arm Trigger<br/>Wait for Target State]
    GlitchArm --> GlitchFire[Execute Glitch<br/>MOSFET Pulse]
    GlitchFire --> GlitchAnalyze[Analyze Response<br/>Success Detection]
    GlitchAnalyze --> GlitchRepeat{Repeat Attack?<br/>Continue Campaign}
    GlitchRepeat -->|Yes| GlitchArm
    GlitchRepeat -->|No| DisplayResults
    
    %% Network Operations
    Network --> WebStart[Start Web Server<br/>HTTP on Port 80]
    WebStart --> WebInterface[Web Interface<br/>Remote Control]
    WebInterface --> WebLogs[Live Logs<br/>Real-time Status]
    
    %% Common Flow
    DisplayResults --> LogSD[Save to SD Card<br/>Timestamped Results]
    MemDump --> LogSD
    BoundaryScan --> LogSD
    CustomCmd --> LogSD
    MemWrite --> LogSD
    GlitchAnalyze --> LogSD
    WebLogs --> LogSD
    
    LogSD --> WebUpdate[Update Web Interface<br/>Refresh Dashboard]
    WebUpdate --> BattCheck[Battery Check<br/>Power Management]
    BattCheck --> BackToMenu{Return to Menu?<br/>Continue Operation}
    BackToMenu -->|Yes| Menu
    BackToMenu -->|No| Shutdown[System Shutdown<br/>Safe Power Down]
    
    Settings --> ConfigNetwork[Wi-Fi Settings<br/>SSID • Password]
    Settings --> ConfigTarget[Target Config<br/>Voltage • Speed]
    Settings --> ConfigSystem[System Config<br/>Display • Logging]
    ConfigNetwork --> Menu
    ConfigTarget --> Menu
    ConfigSystem --> Menu
    
    %% Error Handling
    PinDetect -->|Failed| ErrorHandler[Error Handler<br/>Display Issue]
    ValidateChain -->|Failed| ErrorHandler
    ScanDevices -->|Failed| ErrorHandler
    ErrorHandler --> Menu
    
    %% Styling
    classDef startEnd fill:#c8e6c9,stroke:#81c784,stroke-width:2px,color:#000
    classDef menu fill:#bbdefb,stroke:#90caf9,stroke-width:2px,color:#000
    classDef operation fill:#ffe0b2,stroke:#ffcc02,stroke-width:2px,color:#000
    classDef storage fill:#e1bee7,stroke:#ce93d8,stroke-width:2px,color:#000
    classDef network fill:#b2ebf2,stroke:#80deea,stroke-width:2px,color:#000
    classDef error fill:#ffcdd2,stroke:#f8bbd9,stroke-width:2px,color:#000
    classDef glitch fill:#f8bbd9,stroke:#f48fb1,stroke-width:2px,color:#000
    
    class Start,Shutdown startEnd
    class Menu,BackToMenu menu
    class AutoScan,Manual,PinDetect,ScanDevices,MemDump,BoundaryScan,CustomCmd,MemWrite operation
    class LogSD,WebUpdate,DisplayResults storage
    class Network,WebStart,WebInterface,WebLogs,ConfigNetwork network
    class ErrorHandler error
    class Glitch,GlitchSetup,GlitchArm,GlitchFire,GlitchAnalyze,GlitchRepeat glitch
```

### FreeRTOS Task Architecture & Communication

```mermaid
flowchart LR
    subgraph CRITICAL [" Critical Priority 5 "]
        SYS["System Monitor<br/>Stack 2KB<br/>Watchdog Health"]
    end
    
    subgraph HIGH [" High Priority 4 "]
        WIFI["Wi-Fi Task<br/>Stack 8KB<br/>HTTP Server"]
        JTAG["JTAG Task<br/>Stack 4KB<br/>Protocol Engine"]
    end
    
    subgraph MEDIUM [" Medium Priority 3 "]
        UI["UI Task<br/>Stack 3KB<br/>Display Input"]
    end
    
    subgraph LOW [" Low Priority 2 "]
        PWR["Power Task<br/>Stack 1KB<br/>Battery Monitor"]
        STORE["Storage Task<br/>Stack 2KB<br/>SD Card"]
    end
    
    subgraph QUEUES [" Message Queues "]
        Q1["JTAG Queue<br/>16 messages"]
        Q2["UI Queue<br/>8 messages"]
        Q3["Log Queue<br/>32 messages"]
    end
    
    subgraph SYNC [" Synchronization "]
        S1["WiFi Mutex<br/>1000ms timeout"]
        S2["SD Mutex<br/>5000ms timeout"]
        S3["Display Mutex<br/>100ms timeout"]
    end
    
    WIFI --> Q1
    UI --> Q1
    Q1 --> JTAG
    Q2 --> UI
    Q3 --> STORE
    
    JTAG --> Q3
    SYS --> Q3
    PWR --> Q2
    
    WIFI -.-> S1
    STORE -.-> S2
    UI -.-> S3
    
    classDef critical fill:#ffcdd2,stroke:#f8bbd9,stroke-width:2px,color:#000
    classDef high fill:#e1f5fe,stroke:#81d4fa,stroke-width:2px,color:#000
    classDef medium fill:#e3f2fd,stroke:#90caf9,stroke-width:2px,color:#000
    classDef low fill:#e0f7fa,stroke:#80deea,stroke-width:2px,color:#000
    classDef queue fill:#e8f5e8,stroke:#a5d6a7,stroke-width:2px,color:#000
    classDef sync fill:#fff3e0,stroke:#ffcc02,stroke-width:2px,color:#000
    
    class SYS critical
    class WIFI,JTAG high
    class UI medium
    class PWR,STORE low
    class Q1,Q2,Q3 queue
    class S1,S2,S3 sync
```
