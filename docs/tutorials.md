# Tutorial Guide

```{note}
**Navigation:** [Home](index.md) | [Getting Started](getting-started.md) | [Hardware](hardware.md) | [Software](software.md) | [API Reference](api-reference.md) | [Development](development.md) | [Troubleshooting](troubleshooting.md)
```

This section provides step-by-step procedures for common KISS Fuzzer operations and advanced analysis techniques.

## Tutorial 1: Basic Device Identification

**Objective**: Identify and catalog unknown devices using JTAG/SWD scanning

**Prerequisites**: KISS Fuzzer device, target hardware with accessible debug interface, connection cables

**Estimated Time**: 15-30 minutes

### Hardware Setup Procedure

Establish proper connections between KISS Fuzzer and target device:

1. **Power Down**: Ensure both devices are powered off before making connections
2. **Ground Connection**: Connect ground reference first to establish common potential
3. **Signal Connections**: Wire JTAG signals according to target pinout:
   ```
   KISS Fuzzer → Target Device
   VCC         → Target VCC (verify voltage compatibility)
   TCK         → Test Clock
   TMS         → Test Mode Select  
   TDI         → Test Data In
   TDO         → Test Data Out
   ```
4. **Power Sequence**: Power on KISS Fuzzer first, then target device

### Scan Execution

Perform systematic device discovery:

1. **Menu Navigation**: Access Main Menu → JTAG Scan → Auto Detect
2. **Voltage Configuration**: Set appropriate target voltage (1.8V, 3.3V, or 5V)
3. **Scan Initiation**: Execute scan and monitor progress on display
4. **Result Analysis**: Review detected devices and identification codes

### Result Interpretation

Scan results provide multiple identification parameters:

**Device Information**:
- Device count in JTAG chain
- IDCODE values (32-bit identification)
- Manufacturer identification (decoded from IDCODE)
- Part number and revision information

**Example Output**:
```
Scan Complete: 1 Device Found
IDCODE: 0x4BA00477
Manufacturer: ARM Ltd
Architecture: Cortex-M4
IR Length: 4 bits
```
  IR Length: 4 bits
```

## Tutorial 2: Memory Dumping

Extract firmware or data from target device memory.

### Step 1: Identify Target

1. **Perform** initial JTAG scan
2. **Note** device type and capabilities
3. **Check** if debug access is enabled

### Step 2: Configure Dump Parameters

1. **Navigate** to Manual Mode → Memory Operations
2. **Set** start address (e.g., 0x08000000 for STM32 flash)
3. **Set** dump size (start small, e.g., 1KB)
4. **Choose** output format (binary, hex, or both)

### Step 3: Execute Dump

1. **Start** dump operation
2. **Monitor** progress on display
3. **Wait** for completion confirmation
4. **Check** SD card for output file

### Step 4: Analyze Results

1. **Download** file via web interface
2. **Use** hex editor or disassembler
3. **Look** for:
   - Reset vectors
   - String constants
   - Firmware version info
   - Encryption signatures

## Tutorial 3: Power Glitch Attacks

Use voltage glitching to bypass security mechanisms.

### ⚠️ Safety Warning
Power glitching can permanently damage target devices. Only use on devices you own or have permission to test.

### Step 1: Setup

1. **Connect** power glitch output to target VCC
2. **Ensure** proper grounding
3. **Set** target voltage to normal operating level
4. **Configure** glitch parameters conservatively

### Step 2: Basic Glitch Test

1. **Navigate** to Glitch Tool → Voltage Glitch
2. **Set** initial parameters:
   - Delay: 1000μs
   - Width: 10μs
   - Voltage drop: 10%
3. **Enable** continuous monitoring
4. **Start** glitch sequence

### Step 3: Parameter Tuning

1. **Monitor** target behavior
2. **Adjust** timing parameters:
   - Increase/decrease delay
   - Vary glitch width
   - Change voltage levels
3. **Document** interesting responses
4. **Save** successful parameters

### Step 4: Automated Sweeping

1. **Configure** parameter ranges
2. **Enable** automatic sweeping
3. **Monitor** for anomalous behavior
4. **Log** all attempts for analysis

## Tutorial 4: Web Interface Usage

Access advanced features through the web interface.

### Step 1: Connection

1. **Connect** to KISS Fuzzer Wi-Fi network
2. **Open** browser to http://192.168.4.1
3. **Verify** connection status

### Step 2: Remote Scanning

1. **Navigate** to Scan Control tab
2. **Configure** scan parameters
3. **Start** remote scan
4. **Monitor** real-time progress

### Step 3: Log Analysis

1. **Open** Log Viewer tab
2. **Filter** by log level or time range
3. **Search** for specific events
4. **Export** logs for offline analysis

### Step 4: File Management

1. **Browse** SD card contents
2. **Download** scan results
3. **Delete** old files to free space
4. **Upload** configuration files

## Tutorial 5: Custom Protocol Implementation

Extend KISS Fuzzer for proprietary interfaces.

### Step 1: Protocol Analysis

1. **Document** signal timing requirements
2. **Identify** command structure
3. **Map** pin functions
4. **Note** voltage levels

### Step 2: PIO Program Development

1. **Create** new .pio file
2. **Implement** protocol timing
3. **Add** to CMakeLists.txt
4. **Test** basic communication

### Step 3: C Interface

1. **Add** functions to jtag.c
2. **Implement** protocol logic
3. **Add** error handling
4. **Document** new APIs

### Step 4: UI Integration

1. **Add** menu options
2. **Implement** user controls
3. **Add** status display
4. **Test** complete workflow

## Advanced Tips

### Debugging Techniques

1. **Use** oscilloscope to verify signals
2. **Check** voltage levels with multimeter
3. **Monitor** current consumption for clues
4. **Compare** with known-good reference

### Performance Optimization

1. **Adjust** PIO clock dividers for speed
2. **Use** DMA for large transfers
3. **Optimize** memory allocation
4. **Profile** code execution time

### Troubleshooting Common Issues

| Problem | Likely Cause | Solution |
|---------|--------------|----------|
| No devices detected | Wrong pinout | Try auto-detect mode |
| Scan timeout | Target not powered | Check power connections |
| Corrupted data | Signal integrity | Reduce scan frequency |
| Web interface slow | Wi-Fi interference | Change Wi-Fi channel |
