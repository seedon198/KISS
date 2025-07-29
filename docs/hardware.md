# Hardware Reference

Detailed specifications and technical information about KISS Fuzzer hardware.

## System Specifications

### Core Components

| Component | Specification |
|-----------|--------------|
| **MCU** | Raspberry Pi Pico W (RP2040) |
| **CPU** | Dual-core ARM Cortex-M0+ @ 133MHz |
| **Memory** | 264KB SRAM, 2MB Flash |
| **Wi-Fi** | 802.11n (2.4GHz) |
| **Display** | 240×64 OLED (Banner style) |
| **Input** | 5-way joystick + center button |
| **Storage** | MicroSD card slot |
| **Power** | Li-ion battery + USB-C charging |

### JTAG/SWD Interface

| Parameter | Specification |
|-----------|--------------|
| **Protocols** | JTAG (IEEE 1149.1), SWD |
| **Speed** | Up to 10 MHz |
| **Voltage Levels** | 1.8V - 5V (configurable) |
| **Pins** | TCK, TDI, TDO, TMS, TRST, RESET |
| **Protection** | Over-voltage, reverse polarity |

## Pin Configuration

### JTAG/SWD Connector

```
Pin │ Signal  │ Description
────┼─────────┼─────────────────────────
 1  │ VCC     │ Target power (3.3V/5V)
 2  │ TCK     │ Test Clock / SWCLK
 3  │ TDI     │ Test Data In
 4  │ TDO     │ Test Data Out / SWO
 5  │ TMS     │ Test Mode Select / SWDIO
 6  │ TRST    │ Test Reset (optional)
 7  │ RESET   │ Target Reset
 8  │ GND     │ Ground
```

### GPIO Mapping

| Function | GPIO Pin | Description |
|----------|----------|-------------|
| **Display SPI** | 0-3 | OLED communication |
| **Joystick** | 4-8 | 5-way navigation |
| **JTAG Interface** | 10-15 | Target communication |
| **SD Card SPI** | 16-19 | Storage interface |
| **Power Control** | 20-22 | Battery management |
| **Status LEDs** | 25-27 | Visual indicators |

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
