# KISS Fuzzer - Bill of Materials (BOM)

**Project**: KISS Fuzzer - Handheld JTAG/SWD Fuzzing Tool  
**Revision**: v1.0  
**Date**: 2025-08-02 (Updated)  
**Currency**: USD  

**📋 Component Datasheets**: All referenced datasheets are available in [`datasheets/`](datasheets/) directory.  
**📖 Datasheet Index**: See [`datasheets/README.md`](datasheets/README.md) for complete datasheet listing.  

---

## Executive Summary

This document provides a complete Bill of Materials for building the KISS Fuzzer handheld JTAG/SWD fuzzing and exploitation tool. The device features a Raspberry Pi Pico W microcontroller, 240×64 OLED display, Wi-Fi connectivity, battery power management, and advanced JTAG/SWD capabilities.

**Estimated Total Cost**: $85-120 per unit (excluding assembly labor)  
**Assembly Time**: 3-4 hours for experienced builders  
**Difficulty Level**: Intermediate (SMD soldering required)

---

## Core Processing & Communication

| **Component** | **Part Number** | **Description** | **Qty** | **Unit Price** | **Total** | **Supplier** | **Datasheet** | **Notes** |
|---------------|-----------------|-----------------|---------|----------------|-----------|--------------|---------------|-----------|
| **MCU** | Raspberry Pi Pico W | RP2040 + CYW43439 Wi-Fi module | 1 | $6.00 | $6.00 | Official distributors | [📄](datasheets/RPi_Pico_W_Datasheet.pdf) | Required for Wi-Fi functionality |
| **Headers** | 2.54mm Pin Headers | 40-pin male headers for Pico W | 2 | $0.50 | $1.00 | Generic suppliers | - | Standard 0.1" spacing |

---

## Display System

| **Component** | **Part Number** | **Description** | **Qty** | **Unit Price** | **Total** | **Supplier** | **Datasheet** | **Notes** |
|---------------|-----------------|-----------------|---------|----------------|-----------|--------------|---------------|-----------|
| **Display** | DT010ATFT | 1.0" TFT with resistive touch | 1 | $15.00 | $15.00 | Displaytech | [📄](datasheets/DT010ATFT_Display_Datasheet.pdf) | SPI interface, compact size |
| **Display Connector** | JST SH 1.0mm 6-pin | Micro connector for display+touch | 1 | $0.30 | $0.30 | JST, generic | - | VCC, GND, MOSI, SCK, CS, DC |

---

## User Interface

| **Component** | **Part Number** | **Description** | **Qty** | **Unit Price** | **Total** | **Supplier** | **Datasheet** | **Notes** |
|---------------|-----------------|-----------------|---------|----------------|-----------|--------------|---------------|-----------|
| **Joystick** | Alps SKQUCAA010 | 5-way tactile joystick | 1 | $3.50 | $3.50 | Alps, Digi-Key | [📄](datasheets/SKQUCAA010_Joystick_Datasheet.pdf) | Up/Down/Left/Right/Press |
| **Back Button** | B3F-1000 | 6mm tactile button | 1 | $0.15 | $0.15 | Omron, generic | [📄](datasheets/B3F_Button_Datasheet.pdf) | Panel mount style |
| **Button Caps** | Custom 3D printed | Colored caps for buttons | 2 | $0.10 | $0.20 | DIY 3D printing | - | Match case color scheme |

---

## Power Management

| **Component** | **Part Number** | **Description** | **Qty** | **Unit Price** | **Total** | **Supplier** | **Datasheet** | **Notes** |
|---------------|-----------------|-----------------|---------|----------------|-----------|--------------|---------------|-----------|
| **Battery** | 18650 Li-ion | 3.7V 2000-3000mAh protected | 1 | $8.00 | $8.00 | Samsung, LG, Panasonic | - | Choose protected cells |
| **Battery Holder** | Keystone 1043 | 18650 battery holder with leads | 1 | $2.00 | $2.00 | Keystone Electronics | - | SMD or through-hole |
| **Charging IC** | MCP73871-2CCI/ML | Li-ion battery charger | 1 | $1.50 | $1.50 | Microchip | [📄](datasheets/MCP73871_Charger_Datasheet.pdf) | Power path management |
| **USB-C Connector** | USB4085-GF-A | USB-C receptacle | 1 | $1.20 | $1.20 | GCT, Amphenol | [📄](datasheets/) | Power delivery capable |
| **Power Switch** | OS102011MA1QN1 | SPDT slide switch | 1 | $0.75 | $0.75 | C&K Components | - | Main power control |

---

## Voltage Regulation

| **Component** | **Part Number** | **Description** | **Qty** | **Unit Price** | **Total** | **Supplier** | **Datasheet** | **Notes** |
|---------------|-----------------|-----------------|---------|----------------|-----------|--------------|---------------|-----------|
| **3.3V LDO** | AMS1117-3.3 | 1A low dropout regulator | 1 | $0.30 | $0.30 | AMS, generic | [📄](datasheets/AMS1117_Regulator_Datasheet.pdf) | SOT-223 package |
| **1.8V LDO** | LP2985-18DBVR | 150mA low noise LDO | 1 | $0.45 | $0.45 | Texas Instruments | [📄](datasheets/LP2985_Regulator_Datasheet.pdf) | For 1.8V target support |
| **5V Boost** | TPS61023DRLR | 5V boost converter | 1 | $1.80 | $1.80 | Texas Instruments | [📄](datasheets/TPS61023_Boost_Datasheet.pdf) | For 5V target support |

---

## JTAG/SWD Interface

| **Component** | **Part Number** | **Description** | **Qty** | **Unit Price** | **Total** | **Supplier** | **Datasheet** | **Notes** |
|---------------|-----------------|-----------------|---------|----------------|-----------|--------------|---------------|-----------|
| **JTAG Connector** | PinHeader 2×4 | 8-pin 2.54mm header | 1 | $0.25 | $0.25 | Generic suppliers | - | Standard JTAG pinout |
| **Level Shifters** | SN74LVC2T45DCUR | Dual voltage translator | 4 | $0.60 | $2.40 | Texas Instruments | - | Bidirectional 1.8V-5V |
| **ESD Protection** | TPD2E007DRLR | Dual ESD diode array | 4 | $0.35 | $1.40 | Texas Instruments | [📄](datasheets/TPD2E007_ESD_Protection_Datasheet.pdf) | Protect all JTAG lines |

---

## Storage

| **Component** | **Part Number** | **Description** | **Qty** | **Unit Price** | **Total** | **Supplier** | **Datasheet** | **Notes** |
|---------------|-----------------|-----------------|---------|----------------|-----------|--------------|---------------|-----------|
| **MicroSD Slot** | DM3AT-SF-PEJM5 | Push-push microSD socket | 1 | $1.20 | $1.20 | Hirose | [📄](datasheets/) | Card detect pin |
| **MicroSD Card** | SanDisk Ultra 32GB | Class 10 microSD card | 1 | $8.00 | $8.00 | SanDisk | - | User-supplied option |

---

## Signal Protection & Filtering

| **Component** | **Part Number** | **Description** | **Qty** | **Unit Price** | **Total** | **Supplier** | **Datasheet** | **Notes** |
|---------------|-----------------|-----------------|---------|----------------|-----------|--------------|---------------|-----------|
| **Ferrite Beads** | BLM18KG471TN1D | 470Ω @ 100MHz | 10 | $0.10 | $1.00 | Murata | [📄](datasheets/Murata_BLM18KG471TN1D_Ferrite-Bead.pdf) | Power line filtering |
| **TVS Diodes** | CDSOT23-SM712 | 7V bidirectional TVS | 6 | $0.25 | $1.50 | Bourns | [📄](datasheets/Bourns_CDSOT23-SM712_TVS-Diode.pdf) | USB and signal protection |

---

## Passive Components

### Resistors (0603 SMD, 1% tolerance)

| **Value** | **Description** | **Qty** | **Unit Price** | **Total** | **Applications** |
|-----------|-----------------|---------|----------------|-----------|------------------|
| **10Ω** | Current sensing | 2 | $0.02 | $0.04 | Power monitoring |
| **100Ω** | Series termination | 8 | $0.02 | $0.16 | Signal integrity |
| **1kΩ** | Pull-up/down | 12 | $0.02 | $0.24 | Digital signals |
| **10kΩ** | Pull-up/down | 15 | $0.02 | $0.30 | GPIO, I2C |
| **100kΩ** | High impedance | 4 | $0.02 | $0.08 | ADC inputs |

### Capacitors (0603 SMD)

| **Value** | **Type** | **Voltage** | **Qty** | **Unit Price** | **Total** | **Applications** |
|-----------|----------|-------------|---------|----------------|-----------|------------------|
| **100nF** | Ceramic X7R | 50V | 20 | $0.03 | $0.60 | Decoupling |
| **1µF** | Ceramic X7R | 25V | 8 | $0.05 | $0.40 | Power filtering |
| **10µF** | Ceramic X7R | 16V | 6 | $0.08 | $0.48 | Bulk decoupling |
| **47µF** | Tantalum | 16V | 2 | $0.25 | $0.50 | Power input filtering |
| **22pF** | Ceramic C0G | 50V | 4 | $0.03 | $0.12 | Crystal loading |

### Inductors (0603 SMD)

| **Value** | **Current** | **Qty** | **Unit Price** | **Total** | **Applications** |
|-----------|-------------|---------|----------------|-----------|------------------|
| **2.2µH** | 1A | 2 | $0.15 | $0.30 | Boost converter |
| **10µH** | 500mA | 2 | $0.12 | $0.24 | Power filtering |

---

## Crystal & Timing

| **Component** | **Part Number** | **Description** | **Qty** | **Unit Price** | **Total** | **Supplier** | **Notes** |
|---------------|-----------------|-----------------|---------|----------------|-----------|--------------|-----------|
| **Crystal** | ECS-120-20-5PX-TR | 12MHz crystal | 1 | $0.40 | $0.40 | ECS Inc | High precision JTAG timing |

---

## Status Indication

| **Component** | **Part Number** | **Description** | **Qty** | **Unit Price** | **Total** | **Supplier** | **Notes** |
|---------------|-----------------|-----------------|---------|----------------|-----------|--------------|-----------|
| **Power LED** | LTST-C170GKT | Green 0603 LED | 1 | $0.15 | $0.15 | Lite-On | Power indicator |
| **Activity LED** | LTST-C170KRKT | Red 0603 LED | 1 | $0.15 | $0.15 | Lite-On | Activity indicator |
| **Status LED** | LTST-C170TBKT | Blue 0603 LED | 1 | $0.15 | $0.15 | Lite-On | Status indicator |

---

## Optional Glitch Injection (Future)

| **Component** | **Part Number** | **Description** | **Qty** | **Unit Price** | **Total** | **Supplier** | **Notes** |
|---------------|-----------------|-----------------|---------|----------------|-----------|--------------|-----------|
| **MOSFET** | SI7021-A-GE3 | P-channel power MOSFET | 1 | $0.85 | $0.85 | Infineon | For power glitching |
| **Gate Driver** | TC4427CPA | Dual MOSFET driver | 1 | $1.20 | $1.20 | Microchip | Fast switching |

---

## Mechanical Components

### Enclosure

| **Component** | **Material** | **Description** | **Qty** | **Unit Price** | **Total** | **Supplier** | **Notes** |
|---------------|--------------|-----------------|---------|----------------|-----------|--------------|-----------|
| **Case Top** | SLA Resin | 3D printed top half | 1 | $5.00 | $5.00 | 3D printing service | Translucent purple |
| **Case Bottom** | SLA Resin | 3D printed bottom half | 1 | $5.00 | $5.00 | 3D printing service | Includes battery compartment |
| **Standoffs** | M2.5×6mm | PCB mounting standoffs | 4 | $0.15 | $0.60 | Generic suppliers | Nylon or brass |
| **Screws** | M2.5×8mm | Case assembly screws | 4 | $0.05 | $0.20 | Generic suppliers | Phillips head |

### Internal Hardware

| **Component** | **Description** | **Qty** | **Unit Price** | **Total** | **Notes** |
|---------------|-----------------|---------|----------------|-----------|-----------|
| **Foam Padding** | Anti-static foam | 1 | $1.00 | $1.00 | Battery cushioning |
| **Heat Shrink** | 2mm diameter | 1m | $0.50 | $0.50 | Wire protection |

---

## PCB Manufacturing

| **Specification** | **Requirement** | **Cost Impact** | **Notes** |
|------------------|-----------------|-----------------|-----------|
| **Layers** | 4-layer | +$15-25 | Required for signal integrity |
| **Size** | 85mm × 55mm | Base cost | Credit card size |
| **Thickness** | 1.6mm | Standard | Industry standard |
| **Surface Finish** | HASL or ENIG | +$3-8 | ENIG preferred for fine pitch |
| **Soldermask** | Purple | +$2-5 | Aesthetic choice |
| **Silkscreen** | White | Standard | Component labels |
| **Via Size** | 0.1mm minimum | Standard | High density routing |

**PCB Cost Estimate**: $20-35 per board (qty 10-25)

---

## Assembly Tools & Consumables

### Required Tools

| **Tool** | **Purpose** | **Estimated Cost** | **Notes** |
|----------|-------------|-------------------|-----------|
| **Soldering Station** | SMD component assembly | $50-200 | Temperature controlled |
| **Hot Air Rework** | SMD rework and repair | $30-150 | For difficult components |
| **Microscope/Magnifier** | SMD inspection | $20-100 | 10-20x magnification |
| **Tweezers** | Component placement | $10-30 | ESD-safe preferred |
| **Flux** | SMD soldering aid | $5-15 | No-clean flux paste |
| **Solder Wire** | 0.6mm diameter | $10-20 | SAC305 lead-free |
| **Desoldering Braid** | Rework and repairs | $3-8 | Flux-core type |

### Assembly Consumables

| **Item** | **Purpose** | **Estimated Cost** | **Notes** |
|----------|-------------|-------------------|-----------|
| **Solder Paste** | SMD assembly | $15-30 | SAC305 lead-free |
| **Stencil** | Paste application | $15-25 | Custom laser-cut |
| **IPA Cleaning** | Flux removal | $5-10 | 99% isopropyl alcohol |
| **Lint-free Wipes** | PCB cleaning | $5-10 | Anti-static preferred |

---

## Cost Summary

### Component Costs

| **Category** | **Subtotal** | **Percentage** |
|--------------|--------------|----------------|
| **Core Processing** | $7.00 | 8% |
| **Display & UI** | $16.10 | 18% |
| **Power Management** | $14.20 | 16% |
| **Interface & Protection** | $5.55 | 6% |
| **Passive Components** | $3.52 | 4% |
| **Mechanical** | $11.30 | 13% |
| **PCB Manufacturing** | $30.00 | 34% |
| **Assembly (estimated)** | $10.00 | 11% |

**Total Estimated Cost**: $97.67 per unit

### Cost Scaling

| **Quantity** | **Cost Per Unit** | **Total Cost** | **Savings** |
|--------------|-------------------|----------------|-------------|
| **1 unit** | $110-120 | $110-120 | Base price |
| **5 units** | $95-105 | $475-525 | 10-15% savings |
| **10 units** | $85-95 | $850-950 | 20-25% savings |
| **25 units** | $75-85 | $1875-2125 | 30-35% savings |

---

## Procurement Strategy

### Recommended Suppliers

**Electronic Components:**
- **Digi-Key Electronics**: Wide selection, reliable inventory
- **Mouser Electronics**: Competitive pricing, fast shipping
- **LCSC Electronics**: Cost-effective for bulk orders
- **Arrow Electronics**: Good for obsolete/hard-to-find parts

**PCB Manufacturing:**
- **JLCPCB**: Cost-effective, good quality
- **PCBWay**: Premium options, excellent finish
- **OSH Park**: Domestic (US), purple PCBs
- **Advanced Circuits**: High-end prototyping

**3D Printing:**
- **Shapeways**: Professional SLA printing
- **Protolabs**: Quick turnaround
- **Local makerspaces**: Cost-effective option
- **DIY SLA printer**: Form 3, Elegoo Mars

### Lead Times

| **Component Category** | **Typical Lead Time** | **Rush Options** |
|----------------------|---------------------|------------------|
| **Standard ICs** | 1-3 weeks | Express shipping |
| **Specialty ICs** | 4-12 weeks | Pay premium for stock |
| **Passive Components** | 1-2 weeks | Usually in stock |
| **Mechanical Parts** | 2-4 weeks | 3D print locally |
| **PCB Manufacturing** | 1-3 weeks | 24-48hr options available |
| **Assembly Services** | 1-2 weeks | Rush assembly available |

---

## Quality Considerations

### Component Selection Criteria

**Critical Components:**
- **MCU**: Use genuine Raspberry Pi Pico W from authorized distributors
- **Display**: Verify I2C compatibility and voltage levels
- **Battery**: Choose protected cells from reputable manufacturers
- **Crystal**: High precision required for JTAG timing accuracy

**Cost Optimization:**
- **Passive Components**: Generic suppliers acceptable for most values
- **Connectors**: Brand name for reliability (USB-C, JTAG header)
- **Protection Devices**: Don't compromise on ESD protection
- **Power Components**: Use quality parts for safety and reliability

### Testing Requirements

**Component Testing:**
- **Incoming inspection**: Check critical component values
- **Functional testing**: Verify all subsystems before final assembly
- **Stress testing**: Power cycling, temperature extremes
- **JTAG compliance**: Verify timing and signal integrity

---

## Environmental & Compliance

### Material Compliance

- **RoHS Compliant**: All components must be lead-free
- **REACH Compliance**: Verify restricted substances
- **Conflict Minerals**: Source responsibly where possible
- **Recycling**: Consider end-of-life disposal

### Safety Standards

- **Battery Safety**: Use protected Li-ion cells
- **ESD Protection**: Comprehensive protection on all interfaces
- **Thermal Management**: Ensure adequate heat dissipation
- **EMC Compliance**: Follow good PCB design practices

---

## Revision History

| **Version** | **Date** | **Changes** | **Author** |
|-------------|----------|-------------|-----------|
| **v1.0** | 2025-01-27 | Initial BOM creation | KISS Fuzzer Team |

---

## Notes

1. **Pricing**: Costs are estimates based on small quantity purchases (1-10 units). Bulk pricing available for larger quantities.

2. **Availability**: Component availability subject to change. Consider approved alternates for critical components.

3. **Assembly**: SMD assembly requires appropriate equipment and experience. Consider professional assembly services for production quantities.

4. **Testing**: Functional testing recommended after assembly to verify all systems operate correctly.

5. **Modifications**: This BOM supports the base functionality. Advanced features (glitch injection) require additional components.

6. **Updates**: This document will be updated as the design evolves and component selections are refined.

---

**Document Control:**  
- **Document ID**: KISS-BOM-v1.0  
- **Security Classification**: Public  
- **Distribution**: Open Source Community  
- **Next Review**: Q2 2025  

**Contacts:**  
- **Technical Questions**: [GitHub Issues](https://github.com/seedon198/KISS/issues)  
- **Commercial Inquiries**: Contact repository maintainers  
- **Community Support**: [Project Wiki](https://github.com/seedon198/KISS/wiki)
