# KISS Fuzzer Hardware Component Quick Reference

**Last Updated**: 2025-08-02  
**Revision**: v1.0  

**📋 Component Datasheets**: All referenced datasheets are available in [`../datasheets/`](../datasheets/) directory.  
**📖 Datasheet Index**: See [`../datasheets/README.md`](../datasheets/README.md) for complete datasheet listing.

---

## Core Components Summary

### Processing & Communication
- **1× Raspberry Pi Pico W** - RP2040 + Wi-Fi module (main controller)
- **1× 12MHz Crystal** - High precision timing for JTAG operations
- **2× Pin Header Sets** - 40-pin headers for Pico W connection

### Display & User Interface  
- **1× 1.0" TFT Display** - DT010ATFT with resistive touch, SPI interface
- **1× 5-Way Joystick** - Alps SKQUCAA010 tactile joystick
- **1× Tactile Button** - 6mm back button
- **2× Button Caps** - 3D printed colored caps

### Power Management
- **1× 18650 Li-ion Battery** - 3.7V 2000-3000mAh protected cell
- **1× Battery Holder** - Keystone 1043 with solder tabs
- **1× USB-C Connector** - Power delivery capable
- **1× MCP73871 Charging IC** - Li-ion battery charger with power path
- **1× Power Switch** - SPDT slide switch

### Voltage Regulation
- **1× AMS1117-3.3** - 3.3V 1A LDO regulator  
- **1× LP2985-18** - 1.8V 150mA LDO regulator
- **1× TPS61023** - 5V boost converter

### JTAG/SWD Interface
- **1× 8-Pin Header** - Standard JTAG/SWD connector
- **4× SN74LVC2T45** - Dual voltage level translators (1.8V-5V)
- **4× TPD2E007** - Dual ESD protection diode arrays

### Storage & Connectivity
- **1× MicroSD Socket** - Push-push style with card detect
- **1× 32GB MicroSD Card** - Class 10 (user-supplied option)

### Protection & Filtering
- **10× Ferrite Beads** - 470Ω @ 100MHz EMI suppression
- **6× TVS Diodes** - 7V bidirectional surge protection

### Status Indication
- **1× Green LED** - Power indicator (0603 SMD)
- **1× Red LED** - Activity indicator (0603 SMD)  
- **1× Blue LED** - Status indicator (0603 SMD)

---

## Passive Components Count

### Resistors (0603 SMD, 1% tolerance)
- **2× 10Ω** - Current sensing
- **8× 100Ω** - Series termination  
- **12× 1kΩ** - Pull-up/down
- **15× 10kΩ** - Pull-up/down, I2C
- **4× 100kΩ** - High impedance, ADC

### Capacitors (0603 SMD)
- **20× 100nF Ceramic** - Decoupling (X7R, 50V)
- **8× 1µF Ceramic** - Power filtering (X7R, 25V)
- **6× 10µF Ceramic** - Bulk decoupling (X7R, 16V)
- **2× 47µF Tantalum** - Input filtering (16V)
- **4× 22pF Ceramic** - Crystal loading (C0G, 50V)

### Inductors (0603 SMD)
- **2× 2.2µH** - Boost converter (1A rating)
- **2× 10µH** - Power filtering (500mA rating)

---

## Mechanical Components

### Enclosure
- **1× Case Top Half** - SLA 3D printed translucent purple resin
- **1× Case Bottom Half** - SLA 3D printed with battery compartment
- **4× M2.5×6mm Standoffs** - PCB mounting (nylon or brass)
- **4× M2.5×8mm Screws** - Case assembly (Phillips head)

### Internal Hardware
- **1× Anti-Static Foam** - Battery cushioning and protection
- **1m Heat Shrink Tubing** - 2mm diameter wire protection

---

## Optional Future Components (Glitch Injection)

- **1× SI7021-A-GE3** - P-channel power MOSFET for glitching
- **1× TC4427CPA** - Dual MOSFET driver for fast switching

---

## PCB Specifications

- **Size**: 85mm × 55mm (credit card format)
- **Layers**: 4-layer with controlled impedance
- **Thickness**: 1.6mm standard
- **Surface Finish**: HASL or ENIG preferred  
- **Soldermask**: Purple to match case aesthetic
- **Silkscreen**: White component labels
- **Via Size**: 0.1mm minimum for high density routing

---

## Cost Breakdown (Estimated)

| **Category** | **Component Count** | **Cost Range** |
|--------------|-------------------|----------------|
| **Major ICs** | 8 components | $15-20 |
| **Display & UI** | 4 components | $15-20 |
| **Power Components** | 7 components | $12-18 |
| **Passive Components** | 80+ components | $3-5 |
| **Mechanical** | 10+ components | $10-15 |
| **PCB Manufacturing** | 1 board | $20-35 |
| **Assembly Labor** | Estimated | $10-15 |
| **Total per Unit** | **110+ components** | **$85-120** |

---

## Assembly Skill Requirements

### Required Skills
- **SMD Soldering**: 0603 package experience required
- **Component Identification**: Ability to read component markings
- **Basic Testing**: Multimeter and logic analyzer usage
- **Mechanical Assembly**: 3D printing and case assembly

### Recommended Tools
- **Soldering Station**: Temperature controlled (300-350°C)
- **Magnification**: 10-20x microscope or magnifier
- **Precision Tools**: ESD-safe tweezers, fine flux pen
- **Test Equipment**: Multimeter, basic logic analyzer

---

## Procurement Notes

### Critical Components (Buy from authorized distributors)
- Raspberry Pi Pico W
- MCP73871 charging IC
- Level shifters and protection diodes
- 12MHz crystal (timing critical)

### Standard Components (Generic suppliers acceptable)
- Passive components (R, L, C)
- Connectors and mechanical parts
- Status LEDs and switches
- Wire and heat shrink

### Lead Time Considerations
- **Specialty ICs**: 4-12 weeks (plan ahead)
- **Pico W availability**: Check stock regularly
- **3D printing**: 1-2 weeks for SLA quality
- **PCB manufacturing**: 1-3 weeks typical

---

## Quality & Testing

### Critical Test Points
1. **Power rails**: 3.3V, 1.8V, 5V regulation
2. **JTAG signals**: Level shifting and timing
3. **Display communication**: I2C functionality  
4. **Wi-Fi operation**: Access point and web interface
5. **Battery management**: Charging and monitoring

### Performance Targets
- **JTAG Speed**: Up to 10 MHz operation
- **Battery Life**: 25+ hours typical operation
- **Signal Integrity**: <5ns rise/fall times
- **Current Draw**: <90mA active, <2mA standby

---

## Safety & Compliance

### Battery Safety
- Use only protected Li-ion cells
- Verify proper charging voltage and current
- Include over-discharge protection
- Follow proper disposal procedures

### ESD Protection
- Comprehensive protection on all external interfaces
- Use ESD-safe assembly procedures
- Test protection circuits before final assembly

### EMC Considerations  
- Ferrite beads on power lines
- Proper grounding and shielding
- Controlled impedance for high-speed signals
- EMI testing recommended for commercial use

---

## Support Resources

- **Complete BOM**: [BOM.md](BOM.md) - Detailed component specifications
- **Assembly Guide**: [assembly-guide.md](assembly-guide.md) - Step-by-step instructions  
- **Pinout Reference**: [pinout.md](pinout.md) - GPIO assignments and electrical specs
- **Community Support**: [GitHub Issues](https://github.com/seedon198/KISS/issues)
- **Project Wiki**: [Documentation Hub](https://github.com/seedon198/KISS/wiki)

---

**Document Information:**
- **Document ID**: KISS-HW-QUICK-REF-v1.0
- **Classification**: Public / Open Source
- **Maintenance**: Updated with each hardware revision
- **Contact**: KISS Fuzzer development team via GitHub
