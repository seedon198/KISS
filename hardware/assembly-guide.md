# KISS Fuzzer Hardware Assembly Guide

**Project**: KISS Fuzzer - Handheld JTAG/SWD Fuzzing Tool  
**Revision**: v1.0  
**Date**: 2025-01-27  

---

## Overview

This guide provides step-by-step instructions for assembling the KISS Fuzzer hardware. Before beginning, ensure you have reviewed the [Bill of Materials (BOM)](BOM.md) and have all required components and tools available.

**Prerequisites:**
- Complete [BOM.md](BOM.md) components
- SMD soldering experience (intermediate level)
- Appropriate tools and workspace
- ESD-safe work environment

**Estimated Assembly Time**: 3-4 hours  
**Difficulty**: Intermediate to Advanced

---

## Pre-Assembly Checklist

### Required Tools
- [ ] Temperature-controlled soldering station (300-350°C)
- [ ] Hot air rework station (optional but recommended)
- [ ] Microscope or magnifying glass (10-20x)
- [ ] ESD-safe tweezers (fine tip)
- [ ] Flux pen or paste
- [ ] Solder wire (0.6mm, SAC305 lead-free)
- [ ] Desoldering braid
- [ ] Isopropyl alcohol (99%)
- [ ] Lint-free wipes
- [ ] Multimeter
- [ ] Logic analyzer or oscilloscope (for testing)

### Components Verification
- [ ] Verify all components against BOM
- [ ] Check component orientation markings
- [ ] Organize components by assembly sequence
- [ ] Prepare ESD-safe workspace

---

## Assembly Sequence

### Phase 1: PCB Preparation

#### Step 1.1: PCB Inspection
1. **Visual Inspection**: Check PCB for defects, shorts, or damage
2. **Continuity Testing**: Verify critical connections with multimeter
3. **Surface Preparation**: Clean PCB with IPA if needed

#### Step 1.2: Solder Paste Application (Optional)
1. **Stencil Alignment**: Position custom stencil over PCB
2. **Paste Application**: Apply SAC305 solder paste evenly
3. **Stencil Removal**: Lift stencil carefully to avoid smearing
4. **Inspection**: Verify paste coverage on all pads

---

### Phase 2: Power Management Components

#### Step 2.1: Voltage Regulators
1. **AMS1117-3.3 (U2)**:
   - Position: Main 3.3V regulator location
   - Orientation: Pin 1 indicator matches PCB marking
   - Soldering: Tack one corner, verify alignment, complete remaining pins
   
2. **LP2985-18DBVR (U3)**:
   - Position: 1.8V LDO for low-voltage targets
   - Package: SOT-23-5
   - Critical: Verify pin 1 orientation

3. **TPS61023DRLR (U4)**:
   - Position: 5V boost converter
   - Package: SON-6
   - Note: Thermal pad connection important

#### Step 2.2: Power Input Components
1. **USB-C Connector (J1)**:
   - Mechanical fit first
   - Solder shield connections
   - Check for shorts to adjacent components
   
2. **MCP73871-2CCI/ML (U1)**:
   - Battery charging IC
   - Careful with thermal pad soldering
   - Verify power path connections

#### Step 2.3: Power Distribution
1. **Power Switch (SW1)**:
   - Main power control
   - Verify mechanical alignment with case
   
2. **Decoupling Capacitors**:
   - 100nF ceramics near each IC
   - 10µF bulk capacitors at power inputs
   - Orientation critical for tantalum caps

---

### Phase 3: Core Processing

#### Step 3.1: Raspberry Pi Pico W Socket
1. **Pin Header Installation**:
   - Use 2.54mm female headers
   - Ensure perpendicular alignment
   - Double-check pin spacing

2. **Socket Testing**:
   - Test-fit Pico W module
   - Verify all pins make contact
   - Check for mechanical interference

#### Step 3.2: Crystal and Timing
1. **12MHz Crystal (Y1)**:
   - Position carefully (SMD package)
   - Avoid mechanical stress
   - Add loading capacitors (22pF each)

---

### Phase 4: JTAG/SWD Interface

#### Step 4.1: Level Shifters
1. **SN74LVC2T45DCUR (U5-U8)**:
   - Four dual-channel translators
   - Orientation critical for proper voltage translation
   - Verify VCC connections for each voltage domain

#### Step 4.2: Protection Components
1. **ESD Protection Diodes**:
   - TPD2E007DRLR on all JTAG lines
   - Orientation matters (cathode markings)
   - Close placement to connector

2. **JTAG Connector (J2)**:
   - 8-pin 2.54mm header
   - Standard ARM JTAG pinout
   - Verify pin 1 orientation

---

### Phase 5: Display and User Interface

#### Step 5.1: Display Connection
1. **I2C Connector (J3)**:
   - 4-pin JST SH connector
   - SDA, SCL, VCC, GND connections
   - Keep traces short for signal integrity

#### Step 5.2: User Input Components
1. **5-Way Joystick (SW2)**:
   - Alps SKQUCAA010 or equivalent
   - Verify mechanical alignment with case cutout
   - Test all 5 directions and center press

2. **Back Button (SW3)**:
   - 6mm tactile button
   - Panel mount through case
   - Proper actuation force

---

### Phase 6: Storage Interface

#### Step 6.1: MicroSD Socket
1. **DM3AT-SF-PEJM5 (J4)**:
   - Push-push style socket
   - Mechanical alignment critical
   - Card detect pin connection
   - Shield connection to ground

---

### Phase 7: Status Indication

#### Step 7.1: Status LEDs
1. **Power LED (D1)** - Green:
   - 0603 package
   - Current limiting resistor (1kΩ)
   - Connect to 3.3V rail through MCU control

2. **Activity LED (D2)** - Red:
   - JTAG activity indication
   - MCU GPIO control

3. **Status LED (D3)** - Blue:
   - General status indication
   - Error conditions, Wi-Fi status

---

### Phase 8: Passive Components

#### Step 8.1: Resistors (0603 SMD)
Install in order of criticality:
1. **Pull-up resistors** (1kΩ, 10kΩ) for I2C, JTAG
2. **Current sensing** (10Ω) for power monitoring
3. **Series termination** (100Ω) for signal integrity
4. **High impedance** (100kΩ) for ADC inputs

#### Step 8.2: Capacitors (0603 SMD)
1. **Decoupling capacitors** (100nF) - near all ICs
2. **Power filtering** (1µF, 10µF) - at power inputs
3. **Bulk filtering** (47µF tantalum) - main power rails
4. **Crystal loading** (22pF) - near crystal

#### Step 8.3: Inductors
1. **Power inductors** (2.2µH, 10µH) for switching converters
2. **Ferrite beads** for EMI suppression

---

## Assembly Testing

### Step 1: Power-On Test
1. **Visual Inspection**: Check for obvious shorts or misaligned components
2. **Power Rails**: Verify 3.3V output with no load
3. **Current Draw**: Measure quiescent current (<10mA)
4. **USB Detection**: Verify USB-C connection recognition

### Step 2: MCU Programming
1. **Pico W Installation**: Insert programmed Pico W module
2. **Boot Test**: Connect USB-C and verify enumeration
3. **LED Test**: Verify on-board LED functionality
4. **Debug Output**: Check UART debug messages

### Step 3: Subsystem Testing
1. **Display Test**: 
   - I2C communication
   - Display initialization
   - Text rendering

2. **User Interface Test**:
   - Joystick 5-way operation
   - Back button functionality
   - Button debouncing

3. **JTAG Interface Test**:
   - Level shifter operation
   - Signal integrity on all pins
   - Voltage level verification

4. **Storage Test**:
   - MicroSD card detection
   - SPI communication
   - File system operations

5. **Wi-Fi Test**:
   - Access point mode
   - Web interface accessibility
   - Network communication

---

## Mechanical Assembly

### Step 1: Case Preparation
1. **3D Print Quality**: Inspect for defects, remove supports
2. **Test Fit**: Verify PCB fits properly
3. **Standoff Installation**: Install M2.5×6mm standoffs

### Step 2: Display Integration
1. **Display Mounting**: Secure OLED to case front
2. **Cable Routing**: Route display cable properly
3. **Strain Relief**: Ensure cable not stressed

### Step 3: Battery Installation
1. **18650 Cell**: Insert protected Li-ion battery
2. **Connections**: Verify positive/negative orientation
3. **Cushioning**: Add foam padding as needed

### Step 4: Final Assembly
1. **PCB Installation**: Secure PCB with standoffs
2. **Case Closure**: Align top and bottom halves
3. **Screw Installation**: Use M2.5×8mm screws
4. **Button Alignment**: Verify all buttons actuate properly

---

## Final Testing and Calibration

### Functional Test Suite
1. **Power Management**:
   - Battery charging
   - Power consumption measurements
   - Low battery detection

2. **JTAG Operations**:
   - Chain scanning
   - IDCODE reading
   - Memory access operations

3. **Wireless Functionality**:
   - AP mode establishment
   - Web interface operation
   - Remote command execution

4. **Data Logging**:
   - SD card write operations
   - Log file integrity
   - Storage capacity verification

### Performance Validation
1. **JTAG Timing**: Verify up to 10MHz operation
2. **Battery Life**: Extended operation testing
3. **Temperature Testing**: Operating range verification
4. **EMC Testing**: Basic emissions compliance

---

## Troubleshooting

### Common Issues

#### Power Problems
- **No Power**: Check USB-C connection, power switch, fuse continuity
- **High Current**: Look for shorts, check component orientation
- **Voltage Issues**: Verify regulator operation, load conditions

#### Communication Issues
- **I2C Problems**: Check pull-up resistors, signal levels, timing
- **SPI Issues**: Verify clock polarity, chip select timing
- **JTAG Problems**: Check level shifters, target voltage, signal integrity

#### Display Problems
- **No Display**: Verify I2C communication, power supply, initialization
- **Garbled Display**: Check signal integrity, timing parameters
- **Brightness Issues**: Verify PWM control, power supply stability

### Debug Techniques
1. **Systematic Testing**: Test one subsystem at a time
2. **Signal Probing**: Use oscilloscope for timing verification
3. **Component Substitution**: Replace suspect components
4. **Reference Comparison**: Compare with known-good unit

---

## Quality Control

### Final Inspection Checklist
- [ ] All components properly soldered
- [ ] No cold joints or bridges
- [ ] Proper component orientation
- [ ] Mechanical fit and finish
- [ ] All functions tested and verified
- [ ] Calibration completed
- [ ] Documentation updated

### Acceptance Criteria
- **Visual**: Professional appearance, no defects
- **Functional**: All features operational
- **Performance**: Meets design specifications
- **Reliability**: Passes stress testing

---

## Documentation

### Assembly Records
- Serial number assignment
- Component lot tracking
- Test results documentation
- Calibration certificates
- Warranty activation

### User Documentation
- Operating instructions
- Safety warnings
- Maintenance procedures
- Technical specifications
- Support contact information

---

## Safety Notes

### ESD Protection
- Use ESD-safe work environment
- Ground yourself and tools
- Handle components by edges
- Store in anti-static containers

### Soldering Safety
- Adequate ventilation required
- Eye protection recommended
- Avoid breathing flux fumes
- Proper iron temperature control

### Battery Safety
- Use only protected Li-ion cells
- Proper charging procedures
- No short circuit conditions
- Dispose of properly when expired

---

## Revision History

| **Version** | **Date** | **Changes** | **Author** |
|-------------|----------|-------------|-----------|
| **v1.0** | 2025-01-27 | Initial assembly guide | KISS Fuzzer Team |

---

**Document Control:**  
- **Document ID**: KISS-ASM-v1.0  
- **Security Classification**: Public  
- **Distribution**: Open Source Community  
- **Next Review**: Q2 2025  

**Related Documents:**
- [Bill of Materials (BOM.md)](BOM.md)
- [Hardware README](README.md)
- [Pinout Reference](pinout.md)
- [Schematic Documentation](docs/)

**Support:**
- **Technical Questions**: [GitHub Issues](https://github.com/seedon198/KISS/issues)
- **Community Support**: [Project Wiki](https://github.com/seedon198/KISS/wiki)
- **Assembly Videos**: Coming soon to project documentation
