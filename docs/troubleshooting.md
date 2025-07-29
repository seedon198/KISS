# Troubleshooting Guide

This section provides diagnostic procedures and solutions for common KISS Fuzzer operational issues.

## Device Boot Problems

### Complete System Failure

**Symptoms**: No display output, no LED activity, device appears non-functional

**Diagnostic Steps**:

**Power System Verification**:
- Verify USB-C cable is fully seated and data-capable
- Test with multiple power sources (wall adapter, computer USB port)
- Check for any visible damage to USB-C connector

**Battery Assessment**:
- Allow device to charge for minimum 30 minutes before testing
- If available, measure battery voltage (should be above 3.0V)
- Consider battery replacement if voltage is consistently low

**Firmware Recovery**:
- Enter recovery mode by holding BOOTSEL button during USB connection
- Device should appear as USB mass storage device
- Flash latest firmware UF2 file to restore operation

### Partial Boot Failure

**Symptoms**: Some systems initialize but device does not reach operational state

**Common Causes**:
- Corrupted configuration data
- Hardware subsystem failure
- Incomplete firmware installation

**Resolution Steps**:
- Perform factory reset through recovery mode
- Verify all hardware connections are secure
- Reflash firmware with verified UF2 file

## JTAG Interface Issues

### Device Detection Failure

**Symptoms**: Scan operations report "No devices found"

**Systematic Diagnosis**:

**Physical Connection Verification**:
- Confirm all signal wires are properly connected
- Verify ground connection is established first
- Check target device power status
- Ensure voltage level compatibility (1.8V, 3.3V, 5V)

**Signal Integrity Checks**:
- Test connections with multimeter for continuity
- Verify signal voltage levels match target requirements
- Check for proper impedance on clock signals

**Protocol Configuration**:
- Attempt SWD mode if JTAG detection fails
- Reduce clock frequency for marginal connections
- Verify target device supports selected debug protocol
   - Double-check target device pinout
   - Use oscilloscope to verify signal integrity
   - Check for proper voltage levels

2. **Power Issues**
   - Ensure target device is powered
   - Check VCC voltage matches target requirements
   - Verify ground connections

3. **Clock Frequency**
   - Reduce JTAG frequency to 1 kHz
   - Some devices require slower clocking
   - Try different frequency settings

#### JTAG Chain Errors
```
Symptom: "Chain integrity check failed"
```

**Solutions:**
1. **Chain Configuration**
   - Verify device count in chain
   - Check for proper daisy-chaining
   - Ensure all devices powered

2. **Signal Quality**
   - Check cable length (keep < 30cm)
   - Add pull-up resistors if needed
   - Verify signal rise/fall times

### Inconsistent Scan Results

#### Intermittent Detection
```
Symptom: Devices detected sometimes, not others
```

**Solutions:**
1. **Connection Quality**
   - Clean connector contacts
   - Check for loose connections
   - Use proper probe clips

2. **EMI Issues**
   - Keep away from interference sources
   - Use shielded cables
   - Add ferrite cores if needed

## Display Problems

### Blank/Corrupted Display

#### No Display Output
```
Symptom: OLED remains black
```

**Solutions:**
1. **I2C Communication**
   - Check SDA/SCL connections
   - Verify I2C address (0x3C)
   - Test with I2C scanner

2. **Power Supply**
   - Verify 3.3V supply to display
   - Check current consumption
   - Test display independently

#### Garbled Text
```
Symptom: Random pixels or corrupted characters
```

**Solutions:**
1. **Signal Integrity**
   - Check I2C signal quality
   - Reduce clock frequency
   - Add pull-up resistors

2. **Software Issues**
   - Update display driver
   - Check frame buffer integrity
   - Verify font data

## Wi-Fi Connectivity Issues

### Cannot Connect to Access Point

#### Network Not Visible
```
Symptom: KISS-Fuzzer network doesn't appear
```

**Solutions:**
1. **Wi-Fi Module**
   - Restart device
   - Check Wi-Fi status LEDs
   - Factory reset Wi-Fi settings

2. **Channel Conflicts**
   - Change Wi-Fi channel in settings
   - Scan for available channels
   - Avoid crowded frequencies

#### Connection Drops
```
Symptom: Frequent disconnections
```

**Solutions:**
1. **Range Issues**
   - Move closer to device
   - Check antenna connections
   - Remove interference sources

2. **Power Management**
   - Disable Wi-Fi power saving
   - Check battery level
   - Ensure stable power supply

### Web Interface Problems

#### Page Won't Load
```
Symptom: Browser shows connection error
```

**Solutions:**
1. **Network Configuration**
   - Verify IP address (192.168.4.1)
   - Check DHCP assignment
   - Try direct IP access

2. **Browser Issues**
   - Clear browser cache
   - Try different browser
   - Disable browser extensions

## Storage Issues

### SD Card Not Recognized

#### Card Detection Failure
```
Symptom: "SD card not found" error
```

**Solutions:**
1. **Physical Issues**
   - Ensure card fully inserted
   - Clean card contacts
   - Try different SD card

2. **Format Problems**
   - Format card as FAT32
   - Use 32GB or smaller card
   - Check for bad sectors

#### File System Errors
```
Symptom: Cannot read/write files
```

**Solutions:**
1. **File System Corruption**
   - Run chkdsk/fsck on card
   - Reformat if necessary
   - Use high-quality SD card

2. **Write Protection**
   - Check write-protect switch
   - Verify file permissions
   - Test with different files

## Power Management Issues

### Poor Battery Life

#### Rapid Discharge
```
Symptom: Battery drains quickly
```

**Solutions:**
1. **Power Settings**
   - Reduce display brightness
   - Enable power saving mode
   - Disable unused features

2. **Battery Health**
   - Check battery voltage curve
   - Replace aged battery
   - Calibrate battery meter

#### Charging Problems
```
Symptom: Battery won't charge or charges slowly
```

**Solutions:**
1. **Charger Issues**
   - Use proper USB-C charger
   - Check cable for damage
   - Verify charging current

2. **Thermal Issues**
   - Ensure proper ventilation
   - Check for overheating
   - Allow device to cool

## Performance Issues

### Slow Operation

#### UI Responsiveness
```
Symptom: Laggy menu navigation
```

**Solutions:**
1. **System Load**
   - Close unnecessary tasks
   - Check memory usage
   - Restart device

2. **SD Card Speed**
   - Use Class 10 or better card
   - Defragment if possible
   - Check for card errors

#### JTAG Speed Issues
```
Symptom: Slow scanning/dumping
```

**Solutions:**
1. **Frequency Settings**
   - Increase JTAG frequency
   - Check target capabilities
   - Monitor signal quality

2. **Buffer Settings**
   - Increase buffer sizes
   - Optimize memory allocation
   - Use DMA for transfers

## Error Code Reference

### System Error Codes

| Code | Description | Solution |
|------|-------------|----------|
| E001 | Hardware initialization failed | Check hardware connections |
| E002 | Memory allocation error | Restart device, check memory |
| E003 | SD card error | Check/replace SD card |
| E004 | Wi-Fi initialization failed | Reset Wi-Fi settings |
| E005 | JTAG timeout | Check target connections |

### JTAG Error Codes

| Code | Description | Solution |
|------|-------------|----------|
| J001 | No response from target | Check power and connections |
| J002 | Invalid IDCODE | Verify target device type |
| J003 | Chain length mismatch | Recount devices in chain |
| J004 | IR scan failed | Check TDI/TDO connections |
| J005 | Boundary scan error | Reduce frequency, check signals |

## Getting Help

### Self-Diagnosis

1. **Built-in Diagnostics**
   - Run system self-test
   - Check hardware status
   - Review error logs

2. **External Tools**
   - Use oscilloscope for signals
   - Multimeter for power checks
   - Logic analyzer for protocols

### Community Support

1. **Documentation**
   - Check this troubleshooting guide
   - Review API documentation
   - Search existing issues

2. **Online Resources**
   - GitHub Issues
   - Community forum
   - Discord chat

### Reporting Issues

When reporting problems, include:

1. **System Information**
   - Firmware version
   - Hardware revision
   - Battery level

2. **Problem Description**
   - Exact error messages
   - Steps to reproduce
   - Expected vs actual behavior

3. **Environment Details**
   - Target device type
   - Connection method
   - External tools used

4. **Logs and Data**
   - Relevant log files
   - Screenshots if applicable
   - Test results
