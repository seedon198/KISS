# KISS Fuzzer Hardware Design

This directory contains the hardware design files for the KISS Fuzzer.

## Documentation Overview

This hardware directory contains comprehensive documentation for building the KISS Fuzzer device:

- **[components-summary.md](components-summary.md)**: Quick reference list of all hardware components and specifications
- **[BOM.md](BOM.md)**: Complete Bill of Materials with part numbers, suppliers, and cost estimates
- **[assembly-guide.md](assembly-guide.md)**: Step-by-step assembly instructions and testing procedures  
- **[pinout.md](pinout.md)**: Detailed GPIO pin assignments and electrical specifications
- **[README.md](README.md)**: Hardware overview and design specifications (this file)

For builders and manufacturers, start with the components summary for an overview, use the BOM to procure parts, then follow the assembly guide for construction.

---

## PCB Design

The main PCB is designed around the Raspberry Pi Pico W (RP2040) and includes:

### Key Components
- **MCU**: Raspberry Pi Pico W (RP2040 + CYW43439 Wi-Fi)
- **Display**: SSD1306 OLED 240×64 ultra-wide banner display
- **Input**: 5-way joystick + 2 buttons (OK/Back)
- **Power**: Li-ion battery management with USB-C charging
- **Storage**: MicroSD card slot
- **JTAG**: Target interface with voltage level shifting
- **Glitch**: Optional power glitch injection circuit

### Power Management
- USB-C PD input with power-path management
- 18650 Li-ion battery (3.7V, 3000mAh typical)
- 3.3V and 1.8V regulators for target interfaces
- Battery monitoring via ADC
- Charging status indicators

### I/O Interfaces
- JTAG/SWD header (10-pin ARM standard)
- SPI/I2C expansion headers
- GPIO test points
- UART debug interface

## Mechanical Design

### Enclosure
- 3D-printed SLA translucent purple resin case
- Flipper Zero-inspired form factor (compact handheld)
- Cutouts for display, joystick, USB-C, SD card
- Internal standoffs for PCB and battery mounting
- Modular design for easy assembly/disassembly

### Dimensions
- Length: 100mm
- Width: 40mm  
- Height: 20mm
- Weight: ~150g with battery

## Files Structure

```
hardware/
├── README.md                   # This file - hardware overview
├── components-summary.md       # Quick reference component list
├── BOM.md                      # Complete Bill of Materials
├── assembly-guide.md           # Detailed assembly instructions
├── pinout.md                   # Pin assignments and GPIO mapping
├── pcb/                        # PCB design files (future)
│   ├── kiss-fuzzer.kicad_pro   # KiCad project file
│   ├── kiss-fuzzer.kicad_sch   # Schematic
│   ├── kiss-fuzzer.kicad_pcb   # PCB layout
│   ├── gerbers/                # Gerber files for manufacturing
│   └── assembly.pdf            # Assembly drawings
├── mechanical/                 # Mechanical design files (future)
│   ├── case-top.step           # Case top half
│   ├── case-bottom.step        # Case bottom half
│   ├── case-assembly.step      # Full assembly
│   └── prints/                 # STL files for 3D printing
└── docs/                       # Additional documentation (future)
    ├── schematic.pdf           # Schematic PDF
    ├── test-procedures.md      # Testing and validation
    └── compliance.md           # Regulatory and safety info
```

## Manufacturing Notes

### PCB Specifications
- 4-layer PCB, 1.6mm thickness
- HASL or ENIG surface finish
- 0.1mm minimum via size
- Controlled impedance for USB signals
- Purple soldermask to match case

### Component Notes
- Use low-power components where possible
- ESD protection on all external interfaces
- High-quality crystal for JTAG timing accuracy
- Proper decoupling for clean power delivery

## Revision History

- v1.0 - Initial design (current)
- v1.1 - Planned: Add power glitch injection circuit
- v2.0 - Planned: Integrate PoE for remote power and control
