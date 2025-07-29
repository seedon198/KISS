# Hands-On Adventures with KISS Fuzzer 🎯

Ready to get your hands dirty? These tutorials will take you from "what's a JTAG?" to "check out this sick memory dump I just pulled." We'll start simple and work our way up to the advanced stuff.

## Tutorial 1: Your First JTAG Hunt

**Mission**: Find and identify a mystery chip on a development board.

**What you'll learn**: How to connect, scan, and make sense of what you find.

**Time needed**: About 15 minutes (plus time to celebrate when it works!)

### Setting the scene

You've got a development board sitting on your desk - maybe an Arduino, STM32 Nucleo, or some random eval board. It has chips on it, and you want to know what they are and what they're hiding.

### The connection game

First things first - let's get connected without frying anything:

1. **Power everything down**: Both your KISS Fuzzer and the target. Trust me on this one.

2. **Find those JTAG pins**: Look for a cluster of pins near the main processor, often labeled SWD, JTAG, or DEBUG. Sometimes they're hidden under labels like J1, J2, etc.

3. **Make the connection**: Here's the golden rule - always connect ground first, VCC last:
   ```
   Start with:  GND  (black wire)
   Then add:    TCK  (yellow wire)
               TMS  (purple wire)  
               TDI  (green wire)
               TDO  (blue wire)
   Finally:     VCC  (red wire - match your target's voltage!)
   ```

4. **Power up sequence**: KISS Fuzzer first (it's polite), then your target device.

### The moment of truth

Navigate to **Scan JTAG → Auto Scan** and hit OK. Your KISS Fuzzer will start probing like a digital bloodhound:

- **First, it'll try to talk**: Sending test patterns to see if anything responds
- **Then it'll identify**: If something talks back, it'll try to figure out what it is
- **Finally, it'll report**: Everything it found gets displayed and logged

### What you might see

**Jackpot scenario**:
```
🎉 Found 1 device!
Device: ARM Cortex-M4
IDCODE: 0x4BA00477
Manufacturer: STMicroelectronics
Part: STM32F407 series
```

**Plot twist scenario**:
```
🤔 Found 1 device
IDCODE: 0x12345678
Manufacturer: Unknown
Part: Mystery chip!
```

**Nothing scenario**:
```
😐 No devices found
Check connections and try SWD mode
```

Don't worry if you get the "nothing" scenario - it happens to everyone. Check your connections, make sure your target has power, and try again.
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
