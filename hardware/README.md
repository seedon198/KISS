# KISS Fuzzer Hardware Design

This directory contains the hardware design files for the KISS Fuzzer.

## Documentation Overview

This hardware directory contains manufacturing files and comprehensive documentation for building the KISS Fuzzer device:

**📁 Manufacturing Files:**
- **[bom/](bom/)**: Bill of Materials in CSV format for procurement and assembly
- **[gerber/](gerber/)**: PCB manufacturing files (Gerber, drill, job files)
- **[pickandplace/](pickandplace/)**: SMD assembly files for automated pick and place
- **[stl/](stl/)**: 3D printable STL files for enclosure and accessories

**📚 Documentation:**
- **[docs/](docs/)**: Complete technical documentation and assembly guides
- **[datasheets/](datasheets/)**: Component datasheets and specifications
- **[README.md](README.md)**: Hardware overview and design specifications (this file)

For manufacturers: Use the manufacturing files (bom/, gerber/, pickandplace/, stl/) for production.
For builders: Start with the documentation in docs/ for assembly instructions and component details.

---

## PCB Design

The main PCB is designed around the Raspberry Pi Pico W (RP2040) and includes:

### Key Components
- **MCU**: Raspberry Pi Pico W (RP2040 + CYW43439 Wi-Fi)
- **Display**: DT010ATFT 1.0" TFT with resistive touch (128×128)
- **Input**: 5-way joystick + 2 buttons (OK/Back) + touch interface
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
├── bom/                        # Bill of Materials for manufacturing
│   ├── README.md               # BOM documentation
│   └── KISS-Fuzzer-BOM-v1.0.csv
├── gerber/                     # PCB manufacturing files
│   ├── README.md               # Gerber file documentation
│   └── KISS-Fuzzer-sample.gbrjob
├── pickandplace/              # SMD assembly files  
│   ├── README.md               # Pick and place documentation
│   └── KISS-Fuzzer-sample-pos.csv
├── stl/                       # 3D printable enclosure files
│   └── README.md               # 3D printing documentation
├── docs/                      # Technical documentation
│   ├── BOM.md                  # Detailed Bill of Materials
│   ├── assembly-guide.md       # Assembly instructions
│   ├── components-summary.md   # Component quick reference
│   └── pinout.md              # GPIO assignments
├── datasheets/                # Component datasheets
│   ├── README.md               # Datasheet index
│   ├── RPi_Pico_W_Datasheet.pdf
│   ├── DT010ATFT_Display_Datasheet.pdf
│   └── [Additional datasheets...]
├── pcb/                       # PCB design files (future)
│   ├── kiss-fuzzer.kicad_pro   # KiCad project file
│   ├── kiss-fuzzer.kicad_sch   # Schematic
│   └── kiss-fuzzer.kicad_pcb   # PCB layout
└── mechanical/                # Mechanical design files (future)
    ├── case-top.step           # Case top half (CAD)
    ├── case-bottom.step        # Case bottom half (CAD)
    └── case-assembly.step      # Full assembly (CAD)
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
