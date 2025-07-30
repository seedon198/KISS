# Getting Started

```{note}
**Navigation:** [Home](index.md) | [Hardware](hardware.md) | [Software](software.md) | [API Reference](api-reference.md) | [Tutorials](tutorials.md) | [Development](development.md) | [Troubleshooting](troubleshooting.md)
```

This guide provides step-by-step instructions for setting up and operating your KISS Fuzzer device for the first time.

## Prerequisites

Before beginning, ensure you have the following items:

**Essential Components:**
- KISS Fuzzer device
- USB-C cable (data-capable)
- MicroSD card (32GB or smaller, Class 10 recommended)
- Target device with accessible JTAG or SWD interface

**Optional Components:**
- Jumper wires or probe adapter
- Multimeter for voltage verification
- Target device documentation

## Initial Setup

### Power and Boot Sequence

Connect the KISS Fuzzer using the provided USB-C cable and observe the boot sequence:

1. **Power Connection**: Connect USB-C cable to KISS Fuzzer and power source
2. **Boot Process**: Device displays initialization status for each subsystem
3. **Ready State**: Main menu appears when all systems are operational

If the device fails to boot, refer to the [Troubleshooting Guide](troubleshooting.md) for diagnostic procedures.

### Wi-Fi Configuration

Configure Wi-Fi access for remote control capabilities:

Navigate to **Settings → Wi-Fi** using the joystick controls:

**Access Point Mode**: KISS Fuzzer creates network `KISS-Fuzzer` with password `fuzzing123`

**Web Interface**: Connect any device to the network and navigate to `http://192.168.4.1`

The web interface provides remote control, live monitoring, and log management capabilities.

### Storage Configuration

Insert a properly formatted MicroSD card for data logging:

**Format Requirements**: FAT32 file system recommended for optimal compatibility

**Capacity**: 32GB maximum for best performance and compatibility

**Auto-Configuration**: Directory structure is created automatically on first use

## First JTAG Operation

### Hardware Connection

Establish connections between KISS Fuzzer and target device using proper JTAG/SWD wiring:

```
KISS Fuzzer    Target Device
-----------    -------------
VCC (red)      VCC (match target voltage)
TCK (yellow)   TCK (JTAG clock)
TDI (green)    TDI (data to target)
TDO (blue)     TDO (data from target)
TMS (purple)   TMS (mode select)
GND (black)    GND (ground reference)
```

**Connection Notes:**
- Always connect ground (GND) first to establish common reference
- Verify target voltage before connecting VCC
- Ensure target device is powered before attempting communication
- Use appropriate probe adapters for small-pitch connectors

### Performing Initial Scan

Execute the following sequence for device discovery:

1. **Navigate to Scan Menu**: Main Menu → JTAG Scan → Auto Scan
2. **Initiate Scan**: Press OK button to begin automatic detection
3. **Monitor Progress**: Observe scan progress on display
4. **Review Results**: Examine detected devices and identification codes

### Interpreting Scan Results

The scan operation will display several types of information:

**Successful Detection:**
```
Found: 1 device
Device: ARM Cortex-M4
IDCODE: 0x4BA00477
Manufacturer: STMicroelectronics
Part: STM32F407 series
```

**Unknown Device:**
```
Found: 1 device
IDCODE: 0x12345678
Manufacturer: Unknown
Part: Unidentified
```

**No Detection:**
```
No devices found
Check connections and target power
Consider SWD mode if JTAG fails
```

### Troubleshooting Connection Issues

If initial scans fail to detect devices, verify the following:

**Power and Connections:**
- Confirm target device has power applied
- Verify all signal connections are secure
- Check ground connection integrity
- Ensure correct voltage level selection

**Protocol and Timing:**
- Try SWD mode if JTAG detection fails
- Reduce clock speed for problematic connections
- Verify target supports the selected debug protocol
- Your target might be using SWD instead of JTAG - try the SWD scan option
- Voltage levels might not match - check if your target uses 1.8V logic

## Web Interface Usage

The web interface provides advanced control and monitoring capabilities for remote operation.

### Accessing the Interface

Connect to the KISS Fuzzer network and access the web interface:

1. **Network Connection**: Connect device to `KISS-Fuzzer` Wi-Fi network
2. **Browser Access**: Navigate to `http://192.168.4.1`
3. **Interface Navigation**: Use responsive web controls for device operation

### Available Features

**Real-time Monitoring**: Live status display shows battery level, scan progress, and system state

**Remote Control**: Start and stop scan operations without physical device access

**Log Management**: Stream live logs with filtering and search capabilities

**File Operations**: Download scan results, memory dumps, and configuration files

## Next Steps

After completing initial setup and testing, proceed to advanced topics:

**Hardware Details**: Review the [Hardware Guide](hardware.md) for technical specifications and pinout information

**Advanced Operations**: Explore the [Tutorials](tutorials.md) section for specific use cases and techniques

**Development**: Consult the [API Reference](api-reference.md) for integration and customization options

**Support**: Reference the [Troubleshooting Guide](troubleshooting.md) for common issues and solutions
