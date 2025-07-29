# Hardware Reference

This section provides detailed technical specifications and hardware information for the KISS Fuzzer device.

## System Architecture

KISS Fuzzer is built around the Raspberry Pi Pico W microcontroller, providing a balance of processing capability, I/O flexibility, and integrated wireless connectivity.

**Core Processing**: The RP2040 dual-core ARM Cortex-M0+ processor operates at 133MHz, with one core dedicated to user interface and system management, while the second core handles JTAG/SWD operations.

**Memory Configuration**: 264KB of SRAM provides sufficient buffering for JTAG operations and system state, while 2MB of flash memory contains the firmware and device configuration.

**Programmable I/O**: The RP2040's PIO (Programmable I/O) blocks enable precise timing control for JTAG protocols, supporting operation up to 10 MHz with sub-microsecond timing accuracy.

## JTAG/SWD Interface Specifications

The JTAG interface provides comprehensive support for embedded device debugging and analysis.

**Protocol Support**: Full IEEE 1149.1 JTAG compliance with additional SWD (Serial Wire Debug) protocol support

**Operating Speed**: Variable clock rate from 1 kHz to 10 MHz with automatic timing optimization

**Voltage Compatibility**: Automatic level shifting supports target voltages from 1.8V to 5V

**Signal Protection**: Over-voltage protection, reverse polarity protection, and current limiting on all interface pins

### Connector Pinout

Standard 8-pin connector provides all necessary JTAG/SWD signals:

```
Pin │ Signal  │ Function
────┼─────────┼─────────────────────────
 1  │ VCC     │ Target power reference
 2  │ TCK     │ Test Clock / SWCLK
 3  │ TDI     │ Test Data In
 4  │ TDO     │ Test Data Out / SWO
 5  │ TMS     │ Test Mode Select / SWDIO
 6  │ TRST    │ Test Reset (optional)
 7  │ RESET   │ Target Reset (optional)
 8  │ GND     │ Ground reference
```
| **Display SPI** | 0-3 | OLED communication |
| **Joystick** | 4-8 | 5-way navigation |
| **JTAG Interface** | 10-15 | Target communication |
| **SD Card SPI** | 16-19 | Storage interface |
| **Power Control** | 20-22 | Battery management |
| **Status LEDs** | 25-27 | Visual indicators |

## Detailed Pin Assignments

### JTAG/SWD Interface
- **GPIO 0-4**: JTAG signals (TCK, TMS, TDI, TDO, TRST)
- **GPIO 5-6**: SWD signals (SWCLK, SWDIO)
- **GPIO 7**: Target power control

### Display & UI
- **GPIO 8-9**: I2C for OLED display (SDA, SCL)
- **GPIO 16-21**: 5-way joystick and buttons

### Power Management
- **GPIO 22-24**: USB detect, charge status, power enable
- **GPIO 26-27**: Battery and target voltage monitoring (ADC)

### Storage
- **GPIO 10-14**: SPI interface for MicroSD card

See [hardware/pinout.md](../hardware/pinout.md) for complete pin assignments and electrical specifications.

## Power Specifications

- **Input**: USB-C (5V, up to 3A)
- **Battery**: 18650 Li-ion (3.7V, 3000mAh)
- **System**: 3.3V regulated
- **Target Supply**: 1.8V - 5.0V adjustable
- **Power Budget**: ~90mA typical operation
- **Battery Life**: 33+ hours continuous use

## Power System

### Battery Specifications

- **Type**: Li-ion 18650 or equivalent
- **Voltage**: 3.7V nominal (3.0V - 4.2V range)
- **Capacity**: 2000-3000mAh recommended
- **Charging**: USB-C power delivery
- **Protection**: Over-charge, over-discharge, short circuit

### Power Consumption

| Mode | Current Draw | Battery Life |
|------|-------------|--------------|
| **Active Scanning** | 150-200mA | 10-15 hours |
| **Wi-Fi Active** | 100-150mA | 15-20 hours |
| **Menu Navigation** | 50-80mA | 25-40 hours |
| **Sleep Mode** | 5-10mA | 200-400 hours |

## Physical Specifications

### Enclosure

- **Material**: Translucent purple SLA resin
- **Dimensions**: 85mm × 55mm × 20mm (approx.)
- **Weight**: 120g with battery
- **Mounting**: Standard 1/4"-20 tripod thread
- **Protection**: IP54 dust/splash resistant

### Thermal Management

- **Operating Range**: 0°C to 50°C
- **Storage Range**: -20°C to 70°C
- **Cooling**: Passive convection
- **Thermal Protection**: Automatic throttling at 65°C

## Electrical Characteristics

### Signal Integrity

- **Rise/Fall Time**: < 5ns at 10 MHz
- **Jitter**: < 100ps RMS
- **Crosstalk**: < -40dB between channels
- **Input Impedance**: 100kΩ typical
- **Output Impedance**: 50Ω ±10%

### Protection Features

- **ESD Protection**: ±15kV on all I/O pins
- **Over-voltage**: 5.5V maximum on any pin
- **Short Circuit**: 100mA current limiting
- **Reverse Polarity**: Protected on power inputs

## Expansion Options

### External Interfaces

- **USB-C**: Programming, power, data transfer
- **Debug Header**: SWD access to main MCU
- **I2C**: External sensor interface
- **GPIO**: 4 general-purpose pins available

### Accessories

- **Probe Clips**: Spring-loaded test clips
- **Cable Adapters**: Common connector types
- **Carrying Case**: Protective storage
- **Charging Dock**: Desktop charging station
