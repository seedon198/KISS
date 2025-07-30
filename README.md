
<h1 align="center">
  KISS Fuzzer 💋
</h1>

<p align="center">
  <strong>Keep It Simple, Silly - Handheld JTAG/SWD Fuzzing & Exploitation Tool</strong>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/Platform-Raspberry%20Pi%20Pico%20W-black" alt="Platform"/>
  <img src="https://img.shields.io/badge/Language-C-blue" alt="Language"/>
  <img src="https://img.shields.io/badge/RTOS-FreeRTOS-black" alt="RTOS"/>
  <img src="https://img.shields.io/badge/License-MIT-darkblue" alt="License"/>s
  <img src="https://readthedocs.org/projects/kiss-fuzzer/badge/?version=latest" alt="Documentation Status"/>
</p>

---

## Project Overview

KISS Fuzzer is a compact, handheld JTAG/SWD fuzzing and exploitation tool built on the Raspberry Pi Pico W (RP2040). Designed with simplicity and portability in mind, it provides essential hardware debugging and security assessment capabilities in a sleek, Flipper Zero-inspired form factor.

### Key Features

- **Ultra-wide Banner OLED Display** (240×64) for single-line logs and status
- **5-way Joystick Navigation** with tactile button controls
- **Li-ion Battery** with USB-C power-path charging
- **High-Speed JTAG/SWD Interface** up to 10 MHz using PIO
- **Wi-Fi Web Interface** for remote control and log retrieval
- **Optional Power Glitch Injection** via MOSFET
- **MicroSD Card Logging** for scan results and memory dumps
- **Translucent Purple Case** with premium SLA 3D printing

## Documentation

**[Complete Documentation](https://kiss-fuzzer.readthedocs.io/)** - Comprehensive guides, tutorials, and API reference

- **[Getting Started](https://kiss-fuzzer.readthedocs.io/en/latest/getting-started.html)** - First steps and basic usage
- **[Hardware Guide](https://kiss-fuzzer.readthedocs.io/en/latest/hardware.html)** - Technical specifications and pinouts
- **[Tutorials](https://kiss-fuzzer.readthedocs.io/en/latest/tutorials.html)** - Step-by-step examples and techniques
- **[API Reference](https://kiss-fuzzer.readthedocs.io/en/latest/api-reference.html)** - Developer documentation
- **[Troubleshooting](https://kiss-fuzzer.readthedocs.io/en/latest/troubleshooting.html)** - Common issues and solutions

---

## System Architecture

### Hardware Block Diagram

```mermaid
block-beta
    columns 3
    
    block:INPUT["User Input"]:3
        A["5-Way Joystick"]
        B["USB-C Port"]
        C["Wi-Fi Interface"]
    end
    
    space:3
    
    block:CORE["Core Processing"]:3
        D["RP2040 MCU<br/>Dual ARM Cortex-M0+<br/>133MHz, 264KB SRAM"]
        E["PIO Engine<br/>8 State Machines<br/>High-Speed I/O"]
        F["ADC & GPIO<br/>26 pins<br/>3.3V Logic"]
    end
    
    space:3
    
    block:OUTPUT["Output Interfaces"]:3
        G["240×64 OLED<br/>Banner Display"]
        H["JTAG/SWD Port<br/>10MHz Max"]
        I["MicroSD Card<br/>Data Logging"]
    end
    
    space:3
    
    block:POWER["Power System"]:3
        J["Li-ion Battery<br/>3.7V, USB-C Charging"]
        K["Power Management<br/>Voltage Regulation"]
        L["Target Power<br/>1.8V - 5V Output"]
    end
    
    A --> D
    B --> J
    C --> D
    D --> G
    D --> H
    D --> I
    J --> K
    K --> L
    K --> D
    E --> H
    F --> H
```

### Software Architecture Flow

```mermaid
graph TD
    subgraph "Application Layer"
        A[UI System] --> B[Menu Navigation]
        C[Wi-Fi Server] --> D[Web Interface]
        E[JTAG Engine] --> F[Protocol Handler]
    end
    
    subgraph "FreeRTOS Kernel"
        G[Task Scheduler] --> H[Memory Management]
        I[Queue System] --> J[Inter-task Communication]
        K[Semaphores] --> L[Resource Protection]
    end
    
    subgraph "Hardware Abstraction Layer"
        M[Display Driver] --> N[SPI Interface]
        O[Storage Driver] --> P[SD Card SPI]
        Q[Power Monitor] --> R[ADC Reading]
        S[JTAG Driver] --> T[PIO State Machines]
    end
    
    subgraph "Pico SDK"
        U[Hardware APIs] --> V[System Calls]
    end
    
    A --> G
    C --> I
    E --> K
    M --> U
    O --> U
    Q --> U
    S --> U
    
    style A fill:#e1f5fe
    style C fill:#e8f5e8
    style E fill:#fff3e0
    style G fill:#f3e5f5
    style M fill:#fce4ec
```

### JTAG/SWD Operation Flow

```mermaid
flowchart TD
    Start([Power On]) --> Init[Initialize Hardware]
    Init --> Menu{Main Menu}
    
    Menu -->|1| AutoScan[Auto JTAG Scan]
    Menu -->|2| Manual[Manual Mode]
    Menu -->|3| Glitch[Power Glitch]
    Menu -->|4| Settings[Configuration]
    
    AutoScan --> DetectPins[Detect JTAG Pins]
    DetectPins --> ScanChain[Scan JTAG Chain]
    ScanChain --> ReadID[Read Device IDCODEs]
    ReadID --> DisplayResults[Show Results on OLED]
    
    Manual --> SelectOp{Select Operation}
    SelectOp -->|Memory| MemDump[Memory Dump]
    SelectOp -->|Boundary| BoundaryScan[Boundary Scan]
    SelectOp -->|Custom| CustomCmd[Custom Commands]
    
    Glitch --> SetParams[Set Glitch Parameters]
    SetParams --> TriggerGlitch[Execute Glitch Attack]
    TriggerGlitch --> AnalyzeResult[Analyze Target Response]
    
    DisplayResults --> LogSD[Save to SD Card]
    MemDump --> LogSD
    BoundaryScan --> LogSD
    AnalyzeResult --> LogSD
    
    LogSD --> WebUpdate[Update Web Interface]
    WebUpdate --> Menu
    
    Settings --> Menu
    
    style Start fill:#4caf50
    style Menu fill:#2196f3
    style LogSD fill:#ff9800
    style WebUpdate fill:#9c27b0
```

### Task Architecture & Communication

```mermaid
graph TB
    subgraph "System Tasks (Priority 5)"
        ST[System Monitor<br/>Stack: 2KB<br/>Watchdog & Health]
    end
    
    subgraph "High Priority Tasks (Priority 4)"
        WT[Wi-Fi Task<br/>Stack: 4KB<br/>Web Server & HTTP]
    end
    
    subgraph "Medium Priority Tasks (Priority 3)"
        JT[JTAG Task<br/>Stack: 3KB<br/>Protocol Engine]
    end
    
    subgraph "Low Priority Tasks (Priority 2)"
        UT[UI Task<br/>Stack: 2KB<br/>Display & Input]
    end
    
    subgraph "Background Tasks (Priority 1)"
        PT[Power Task<br/>Stack: 1KB<br/>Battery Monitor]
        STO[Storage Task<br/>Stack: 2KB<br/>SD Card & Logging]
    end
    
    subgraph "Communication"
        Q1[JTAG Command Queue]
        Q2[UI Event Queue]
        Q3[Log Message Queue]
        S1[Wi-Fi Semaphore]
        S2[SD Card Semaphore]
    end
    
    WT --> Q1
    UT --> Q1
    JT --> Q3
    ST --> Q3
    PT --> Q2
    
    Q1 --> JT
    Q2 --> UT
    Q3 --> STO
    
    WT -.-> S1
    STO -.-> S2
    
    style ST fill:#f44336
    style WT fill:#ff9800
    style JT fill:#2196f3
    style UT fill:#4caf50
    style PT fill:#9e9e9e
    style STO fill:#9e9e9e
```
│  │             │  │              │  │                     │ │
│  └─────────────┘  └──────────────┘  └─────────────────────┘ │
├─────────────────────────────────────────────────────────────┤
│                       Pico SDK                              │
└─────────────────────────────────────────────────────────────┘
```

---

## Components & Specifications

### Core Hardware
- **MCU**: Raspberry Pi Pico W (RP2040)
  - Dual-core ARM Cortex-M0+ @ 133MHz
  - 264KB SRAM, 2MB Flash
  - 8 PIO state machines for high-speed protocols
- **Display**: 240×64 OLED (Banner style)
- **Input**: 5-way joystick with center press
- **Power**: 3.7V Li-ion battery with USB-C charging
- **Storage**: MicroSD card slot
- **Connectivity**: Wi-Fi 802.11n (2.4GHz)

### JTAG/SWD Interface
- **Protocols**: JTAG (IEEE 1149.1), SWD (Serial Wire Debug)
- **Speed**: Up to 10 MHz using PIO
- **Pins**: TCK, TDI, TDO, TMS, TRST, RESET
- **Voltage Levels**: 1.8V - 5V (configurable)
- **Protection**: Over-voltage and reverse polarity protection

### Additional Features
- **Power Glitch**: Optional MOSFET-based voltage glitching
- **Target Power**: 3.3V/5V output with current limiting
- **Status LEDs**: Power, Activity, Error indication
- **Case**: Translucent purple SLA 3D printed

---

## Capabilities

### JTAG/SWD Operations
- **Device Scanning**: Automatic JTAG chain detection
- **Pinout Detection**: Smart pin identification
- **Boundary Scan**: IEEE 1149.1 boundary scan testing
- **Memory Operations**: Read/Write/Dump target memory
- **Debug Interface**: Basic debugging capabilities
- **Custom Protocols**: Extensible for proprietary interfaces

### Security Features
- **Fuzzing Engine**: Protocol and data fuzzing
- **Power Glitching**: Fault injection attacks
- **Flash Extraction**: Firmware dumping
- **Bypass Techniques**: Debug protection circumvention
- **Side-channel**: Basic timing and power analysis

### User Interface
- **Local Control**: Joystick navigation with OLED feedback
- **Web Interface**: Mobile-friendly remote control
- **Logging**: Real-time status and comprehensive logging
- **File Management**: SD card file operations via web UI

---

## 🚀 Getting Started

### Prerequisites
- Raspberry Pi Pico SDK
- FreeRTOS Kernel
- CMake 3.13+
- GCC ARM toolchain

### Building the Firmware

```bash
# Clone the repository
git clone https://github.com/your-username/KISS-Fuzzer.git
cd KISS-Fuzzer

# Initialize submodules
git submodule update --init --recursive

# Create build directory
mkdir build && cd build

# Configure and build
cmake ..
make -j4

# Flash to Pico W
cp kiss_fuzzer.uf2 /path/to/pico/mount/
```

### First Boot Setup

1. **Power On**: Connect USB-C cable or insert charged battery
2. **Wi-Fi Setup**: Use joystick to navigate to Wi-Fi settings
3. **Web Access**: Connect to `http://kiss-fuzzer.local` or displayed IP
4. **Target Connection**: Connect JTAG/SWD cables to target device
5. **Start Scanning**: Use local UI or web interface to begin operations

---

## Usage

### Local Interface Navigation

```mermaid
flowchart TD
    Main[Main Menu] --> Scan[Scan JTAG]
    Main --> Manual[Manual Mode]
    Main --> Glitch[Glitch Tool]
    Main --> Logs[Logs]
    Main --> Settings[Settings]
    Main --> About[About]
    
    Scan --> AutoDetect[Auto Detect]
    Scan --> PinScan[Pin Detection]
    Scan --> ChainScan[Chain Analysis]
    
    Manual --> MemOps[Memory Operations]
    Manual --> Boundary[Boundary Scan]
    Manual --> Custom[Custom Commands]
    
    MemOps --> MemRead[Memory Read]
    MemOps --> MemWrite[Memory Write]
    MemOps --> MemDump[Memory Dump]
    
    Glitch --> VoltGlitch[Voltage Glitch]
    Glitch --> ClockGlitch[Clock Glitch]
    Glitch --> TempGlitch[Temperature Glitch]
    
    Settings --> WiFiSet[Wi-Fi Settings]
    Settings --> TargetSet[Target Config]
    Settings --> SystemSet[System Config]
    
    Logs --> LiveLog[Live View]
    Logs --> SavedLog[Saved Logs]
    Logs --> Export[Export Logs]
    
    style Main fill:#2196f3,color:#fff
    style Scan fill:#4caf50,color:#fff
    style Manual fill:#ff9800,color:#fff
    style Glitch fill:#f44336,color:#fff
    style Settings fill:#9c27b0,color:#fff
```

### Web Interface Features
- **Dashboard**: Real-time status and battery level
- **Scan Control**: Start/stop operations remotely
- **Log Viewer**: Live log streaming and filtering
- **File Manager**: Download scan results and dumps
- **Settings**: Wi-Fi, target voltage, scan parameters

### Command Examples

**Basic JTAG Scan:**
```
Navigate: Main Menu → Scan JTAG → Auto Detect
Result: Displays detected devices and IDCODE
```

**Memory Dump:**
```
Navigate: Main Menu → Manual Mode → Memory Ops → Dump
Config: Start Address: 0x08000000, Size: 64KB
Result: Saves firmware.bin to SD card
```

**Power Glitch Attack:**
```
Navigate: Main Menu → Glitch Tool → Voltage Glitch
Config: Delay: 1000µs, Width: 10µs, Repeat: 1000
Result: Attempts to bypass security checks
```

### Usage Workflow Sequence

```mermaid
sequenceDiagram
    participant U as User
    participant UI as UI Task
    participant J as JTAG Task
    participant S as Storage Task
    participant T as Target Device
    
    Note over U,T: JTAG Scanning Process
    
    U->>UI: Press "Scan JTAG"
    UI->>J: Send SCAN_START command
    J->>T: Test pin connectivity
    T-->>J: Pin response signals
    J->>J: Detect JTAG pins
    J->>UI: Send PIN_DETECTED status
    
    loop Device Detection
        J->>T: Read device IDCODE
        T-->>J: Return IDCODE
        J->>UI: Send DEVICE_FOUND
    end
    
    J->>S: Log scan results
    J->>UI: Send SCAN_COMPLETE
    UI->>U: Display results on OLED
```

---

## Pin Configuration & GPIO Mapping

### JTAG/SWD Connector Pinout

```mermaid
block-beta
    columns 4
    
    block:CONN["JTAG/SWD Connector"]:4
        1["1: VCC<br/>Target Power<br/>3.3V/5V"]
        2["2: TCK<br/>Test Clock<br/>SWCLK"]
        3["3: TDI<br/>Test Data In<br/>-"]
        4["4: TDO<br/>Test Data Out<br/>SWO"]
        
        5["5: TMS<br/>Test Mode Select<br/>SWDIO"]
        6["6: TRST<br/>Test Reset<br/>(Optional)"]
        7["7: RESET<br/>Target Reset<br/>NRST"]
        8["8: GND<br/>Ground<br/>Common"]
    end
    
    classDef power fill:#f44336,color:#fff
    classDef clock fill:#2196f3,color:#fff
    classDef data fill:#4caf50,color:#fff
    classDef control fill:#ff9800,color:#fff
    classDef ground fill:#424242,color:#fff
    
    class 1 power
    class 2 clock
    class 3,4,5 data
    class 6,7 control
    class 8 ground
```

### Internal GPIO Mapping

```mermaid
graph LR
    subgraph "RP2040 GPIO Pins"
        subgraph "Display Interface"
            G0[GPIO 0: SPI0 SCK]
            G1[GPIO 1: SPI0 TX]
            G2[GPIO 2: SPI0 RX]
            G3[GPIO 3: SPI0 CS]
        end
        
        subgraph "User Interface"
            G4[GPIO 4: Joy Up]
            G5[GPIO 5: Joy Down]
            G6[GPIO 6: Joy Left]
            G7[GPIO 7: Joy Right]
            G8[GPIO 8: Joy OK]
        end
        
        subgraph "JTAG Interface"
            G10[GPIO 10: TCK]
            G11[GPIO 11: TMS]
            G12[GPIO 12: TDI]
            G13[GPIO 13: TDO]
            G14[GPIO 14: TRST]
            G15[GPIO 15: RESET]
        end
        
        subgraph "Storage Interface"
            G16[GPIO 16: SPI1 SCK]
            G17[GPIO 17: SPI1 TX]
            G18[GPIO 18: SPI1 RX]
            G19[GPIO 19: SPI1 CS]
        end
        
        subgraph "Power & Status"
            G20[GPIO 20: Power Ctrl]
            G21[GPIO 21: Glitch Ctrl]
            G22[GPIO 22: Target VCC]
            G25[GPIO 25: Status LED]
            G26[GPIO 26: Battery ADC]
            G27[GPIO 27: Activity LED]
        end
    end
    
    subgraph "External Connections"
        OLED[240×64 OLED Display]
        JOY[5-Way Joystick]
        JTAG[JTAG/SWD Port]
        SD[MicroSD Card]
        PWR[Power System]
        LED[Status LEDs]
    end
    
    G0 --> OLED
    G1 --> OLED
    G2 --> OLED
    G3 --> OLED
    
    G4 --> JOY
    G5 --> JOY
    G6 --> JOY
    G7 --> JOY
    G8 --> JOY
    
    G10 --> JTAG
    G11 --> JTAG
    G12 --> JTAG
    G13 --> JTAG
    G14 --> JTAG
    G15 --> JTAG
    
    G16 --> SD
    G17 --> SD
    G18 --> SD
    G19 --> SD
    
    G20 --> PWR
    G21 --> PWR
    G22 --> PWR
    G25 --> LED
    G26 --> PWR
    G27 --> LED
```
 8 │ GND    │ Ground
```

### Internal GPIO Mapping
- **Display**: SPI0 (GPIO 0-3)
- **Joystick**: GPIO 4-8 (Up/Down/Left/Right/OK)
- **JTAG**: GPIO 10-15
- **SD Card**: SPI1 (GPIO 16-19)
- **Power Control**: GPIO 20-22
- **Status LEDs**: GPIO 25-27

---

## Development

### Project Structure
```
KISS-Fuzzer/
├── src/                    # Source code
│   ├── main.c             # Main application entry
│   ├── display.c/h        # OLED display driver
│   ├── ui.c/h            # User interface system
│   ├── jtag.c/h          # JTAG/SWD implementation
│   ├── wifi.c/h          # Wi-Fi and web server
│   ├── power.c/h         # Power management
│   ├── storage.c/h       # SD card operations
│   └── jtag.pio          # PIO assembly for JTAG
├── res/                   # Resources
│   └── img/              # Images and graphics
├── hardware/             # PCB design files
├── case/                 # 3D printable case files
├── CMakeLists.txt        # Build configuration
└── README.md            # This file
```

### Development Workflow

```mermaid
flowchart TD
    Start([Start Development]) --> Clone[Clone Repository]
    Clone --> Setup[Run Setup Script]
    Setup --> Branch[Create Feature Branch]
    
    Branch --> Code{Development Tasks}
    
    Code -->|New Feature| NewMod[Create New Module]
    Code -->|Bug Fix| Debug[Debug Existing Code]
    Code -->|Enhancement| Enhance[Modify Existing Feature]
    
    NewMod --> WriteCode[Write C Code]
    Debug --> WriteCode
    Enhance --> WriteCode
    
    WriteCode --> AddTests[Add Unit Tests]
    AddTests --> RunQuality[Run Quality Checks]
    
    RunQuality --> QualityPass{Quality OK?}
    QualityPass -->|No| FixIssues[Fix Code Issues]
    FixIssues --> RunQuality
    
    QualityPass -->|Yes| BuildTest[Build & Test]
    BuildTest --> BuildPass{Build OK?}
    BuildPass -->|No| FixBuild[Fix Build Errors]
    FixBuild --> BuildTest
    
    BuildPass -->|Yes| Document[Update Documentation]
    Document --> Commit[Commit Changes]
    Commit --> Push[Push to Repository]
    Push --> PR[Create Pull Request]
    
    PR --> Review[Code Review]
    Review --> ReviewPass{Review OK?}
    ReviewPass -->|No| AddressComments[Address Comments]
    AddressComments --> Code
    
    ReviewPass -->|Yes| Merge[Merge to Main]
    Merge --> Deploy[Deploy/Release]
    Deploy --> End([Development Complete])
    
    style Start fill:#4caf50
    style End fill:#4caf50
    style QualityPass fill:#2196f3
    style BuildPass fill:#2196f3
    style ReviewPass fill:#2196f3
```

### Quality Assurance Process

```mermaid
graph LR
    subgraph "Pre-Commit Checks"
        A1[clang-format] --> A2[cppcheck]
        A2 --> A3[clang-tidy]
        A3 --> A4[Unit Tests]
    end
    
    subgraph "CI/CD Pipeline"
        B1[Build Firmware] --> B2[Run Tests]
        B2 --> B3[Static Analysis]
        B3 --> B4[Generate Artifacts]
    end
    
    subgraph "Manual Testing"
        C1[Hardware Tests] --> C2[Integration Tests]
        C2 --> C3[Performance Tests]
        C3 --> C4[Security Tests]
    end
    
    A4 --> B1
    B4 --> C1
    C4 --> Release[Ready for Release]
    
    style A1 fill:#e3f2fd
    style A2 fill:#e3f2fd
    style A3 fill:#e3f2fd
    style A4 fill:#e3f2fd
    style Release fill:#4caf50
```

### Adding New Features

1. **Create Module**: Add new .c/.h files in `src/`
2. **Update CMake**: Add sources to `CMakeLists.txt`
3. **FreeRTOS Task**: Create dedicated task for new functionality
4. **Inter-task Communication**: Use queues/semaphores for data sharing
5. **Documentation**: Add Doxygen comments for all functions

### Coding Standards
- **Language**: C99 with GCC extensions
- **Style**: 4-space indentation, snake_case naming
- **Documentation**: Doxygen-style comments for all public functions
- **Error Handling**: Always check return values and handle errors gracefully
- **Memory Management**: Use FreeRTOS heap, avoid dynamic allocation in ISRs

---

## Hardware Assembly

### PCB Specifications
- **Layers**: 4-layer PCB with controlled impedance
- **Size**: 85mm × 55mm (credit card size)
- **Thickness**: 1.6mm with HASL finish
- **Components**: All surface-mount for compact design

### Case Design
- **Material**: Translucent purple resin (SLA 3D printing)
- **Finish**: Smooth surface with minimal layer lines
- **Features**: Snap-fit assembly, integrated button caps
- **Cutouts**: Precise tolerances for connectors and display

### Assembly Steps
1. **PCB Population**: Solder all SMD components (use stencil for efficiency)
2. **Programming**: Flash bootloader and test firmware
3. **Case Preparation**: Remove support material, test fit
4. **Final Assembly**: Install PCB, battery, and close case
5. **Quality Control**: Test all functions and calibrate display

---

## Technical Deep Dive

### PIO Implementation
The RP2040's Programmable I/O (PIO) enables high-speed, deterministic JTAG/SWD communication:

```asm
; JTAG Clock Generation - 10 MHz
.program jtag_clock
    set pins, 1    ; TCK high
    wait 1 gpio 0  ; Wait for ready signal  
    set pins, 0    ; TCK low
    wait 0 gpio 0  ; Wait for ready signal
```

### Memory Management
FreeRTOS heap configuration optimized for embedded constraints:
- **Heap Size**: 128KB for tasks and buffers
- **Stack Sizes**: UI (2KB), JTAG (4KB), Wi-Fi (8KB)
- **DMA Buffers**: Dedicated regions for high-speed transfers

### Power Optimization
- **Sleep Modes**: Automatic CPU scaling based on activity
- **Wi-Fi Management**: Connection keep-alive with power saving
- **Display**: Automatic timeout and brightness control
- **Battery Life**: 8+ hours typical usage, 48+ hours standby

---

## 🤝 Contributing

We welcome contributions from the hardware hacking and embedded development community!

### How to Contribute
1. **Fork** the repository
2. **Create** a feature branch (`git checkout -b feature/amazing-feature`)
3. **Commit** your changes (`git commit -m 'Add amazing feature'`)
4. **Push** to the branch (`git push origin feature/amazing-feature`)
5. **Open** a Pull Request

### Development Areas
- **Protocol Support**: Additional debug interfaces (SPI, I2C, UART)
- **Fuzzing Algorithms**: Advanced fuzzing strategies
- **Hardware Mods**: Power glitching improvements
- **Web Interface**: Enhanced UI/UX features
- **Documentation**: Tutorials and use cases

---

## Legal & Disclaimer

### Educational Purpose
KISS Fuzzer is designed for:
- **Security Research**: Authorized penetration testing
- **Education**: Learning hardware security concepts  
- **Development**: Debugging embedded systems
- **Compliance**: Security assessment and validation

### Responsible Use
- Only use on systems you own or have explicit permission to test
- Respect local laws and regulations regarding security tools
- Follow responsible disclosure for any vulnerabilities found
- Do not use for malicious purposes or unauthorized access

### License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

## 📞 Support & Community

### Getting Help
- **Documentation**: Check this README and inline code comments
- **Issues**: Report bugs and request features on GitHub Issues
- **Discussions**: Join the community discussion forum
- **Discord**: Real-time chat with other users and developers

### Links
- **GitHub**: [https://github.com/your-username/KISS-Fuzzer](https://github.com/your-username/KISS-Fuzzer)
- **Documentation**: [https://kiss-fuzzer.readthedocs.io](https://kiss-fuzzer.readthedocs.io)
- **Hardware Files**: Available in the `hardware/` directory
- **3D Models**: STL files in the `case/` directory

---

<p align="center">
  Made with 💜 by the hardware security community<br>
  <em>"Keep It Simple, Silly - but make it powerful"</em>
</p>
