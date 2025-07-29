# Getting Started

This guide will help you get your KISS Fuzzer up and running quickly.

## Prerequisites

Before you begin, ensure you have:

- KISS Fuzzer hardware device
- USB-C cable for power/programming
- MicroSD card (32GB or smaller, Class 10 recommended)
- Target device with JTAG/SWD interface

## Initial Setup

### 1. First Boot

1. **Power Connection**: Connect your KISS Fuzzer using the USB-C cable
2. **Boot Process**: The device will display the KISS logo and initialization status
3. **Default State**: The device boots into the main menu

### 2. Wi-Fi Configuration

Navigate to **Settings → Wi-Fi** using the joystick:

- **Access Point Mode**: Default SSID is `KISS-Fuzzer`
- **Password**: `fuzzing123`
- **Web Interface**: Connect and visit `http://192.168.4.1`

### 3. SD Card Setup

Insert a formatted MicroSD card:

- **Format**: FAT32 recommended
- **Size**: 32GB or smaller for best compatibility
- **Directory Structure**: Automatically created on first use

## First JTAG Scan

### Hardware Connection

Connect your target device using the JTAG/SWD connector:

```
KISS Fuzzer    Target Device
-----------    -------------
VCC       →    VCC (3.3V/5V)
TCK       →    TCK/SWCLK
TDI       →    TDI
TDO       →    TDO/SWO
TMS       →    TMS/SWDIO
GND       →    GND
```

### Performing a Scan

1. **Navigate**: Main Menu → Scan JTAG
2. **Auto Detect**: Select "Auto Detect Pins" for unknown pinouts
3. **Manual Mode**: Use if pinout is known
4. **Results**: View detected devices and IDCODEs

### Interpreting Results

The scan will display:

- **Device Count**: Number of devices in JTAG chain
- **IDCODE**: Unique identifier for each device
- **Manufacturer**: Decoded manufacturer information
- **IR Length**: Instruction register length

## Web Interface

Access the web interface for advanced features:

### Connection

1. Connect to the KISS Fuzzer Wi-Fi network
2. Open browser to `http://192.168.4.1`
3. Navigate using the responsive web interface

### Features

- **Real-time Status**: Battery level, scan progress
- **Remote Control**: Start/stop scans remotely
- **Log Viewer**: Live log streaming and filtering
- **File Download**: Retrieve scan results and memory dumps

## Next Steps

- Read the [Hardware Guide](hardware.md) for detailed specifications
- Explore [Tutorials](tutorials.md) for advanced techniques
- Check the [API Reference](api-reference.md) for development
