# KISS Fuzzer Component Datasheets

This directory contains official datasheets for all major components used in the KISS Fuzzer project. All datasheets are organized with standardized naming conventions for easy reference.

**Last Updated**: 2025-08-02  
**Total Datasheets**: 12 files  

---

## Datasheet Index

### Core Processing & Communication

| **Component** | **Part Number** | **Datasheet File** | **Size** | **Status** | **Description** |
|---------------|-----------------|-------------------|----------|------------|-----------------|
| **Microcontroller** | Raspberry Pi Pico W | `RPi_Pico_W_Datasheet.pdf` | 19.8 MB | ✅ Complete | RP2040 + CYW43439 Wi-Fi module specifications |

### Display System

| **Component** | **Part Number** | **Datasheet File** | **Size** | **Status** | **Description** |
|---------------|-----------------|-------------------|----------|------------|-----------------|
| **Display** | DT010ATFT | `DT010ATFT_Display_Datasheet.pdf` | 919 B | ⚠️ Incomplete | 1.0" TFT with resistive touch (needs re-download) |

### User Interface

| **Component** | **Part Number** | **Datasheet File** | **Size** | **Status** | **Description** |
|---------------|-----------------|-------------------|----------|------------|-----------------|
| **Joystick** | SKQUCAA010 | `SKQUCAA010_Joystick_Datasheet.pdf` | 181 KB | ✅ Complete | Alps 5-way tactile joystick |
| **Button** | B3F-1000 | `B3F_Button_Datasheet.pdf` | 695 KB | ✅ Complete | Omron 6mm tactile button |

### Power Management

| **Component** | **Part Number** | **Datasheet File** | **Size** | **Status** | **Description** |
|---------------|-----------------|-------------------|----------|------------|-----------------|
| **Charger IC** | MCP73871 | `MCP73871_Charger_Datasheet.pdf` | 3.3 MB | ✅ Complete | Li-ion battery charger with power path |
| **3.3V Regulator** | AMS1117-3.3 | `AMS1117_Regulator_Datasheet.pdf` | 448 B | ⚠️ Incomplete | 3.3V LDO regulator (needs re-download) |
| **1.8V Regulator** | LP2985-18 | `LP2985_Regulator_Datasheet.pdf` | 10.1 MB | ✅ Complete | 1.8V precision LDO regulator |
| **5V Boost** | TPS61023 | `TPS61023_Boost_Datasheet.pdf` | 2.3 MB | ✅ Complete | 5V boost converter |

### Protection & Filtering

| **Component** | **Part Number** | **Datasheet File** | **Size** | **Status** | **Description** |
|---------------|-----------------|-------------------|----------|------------|-----------------|
| **ESD Protection** | TPD2E007 | `TPD2E007_ESD_Protection_Datasheet.pdf` | 492 KB | ✅ Complete | Dual ESD protection diode |
| **TVS Diode** | CDSOT23-SM712 | `Bourns_CDSOT23-SM712_TVS-Diode.pdf` | 625 KB | ✅ Complete | 7V bidirectional TVS diode |
| **Ferrite Bead** | BLM18KG471TN1D | `Murata_BLM18KG471TN1D_Ferrite-Bead.pdf` | 625 KB | ✅ Complete | 470Ω @ 100MHz EMI suppression |

### Storage Interface

| **Component** | **Part Number** | **Datasheet File** | **Size** | **Status** | **Description** |
|---------------|-----------------|-------------------|----------|------------|-----------------|
| **MicroSD Socket** | DM3AT-SF-PEJM5 | `Hirose_DM3AT-SF-PEJM5_MicroSD-Socket.pdf` | - | ❌ Missing | Push-push microSD socket (download failed) |

### Connectors

| **Component** | **Part Number** | **Datasheet File** | **Size** | **Status** | **Description** |
|---------------|-----------------|-------------------|----------|------------|-----------------|
| **USB-C Connector** | USB4085-GF-A | - | - | ❌ Missing | USB-C receptacle (download failed) |

### Additional Components

| **Component** | **Part Number** | **Datasheet File** | **Size** | **Status** | **Description** |
|---------------|-----------------|-------------------|----------|------------|-----------------|
| **Microcontroller** | PIC10F200 | `Microchip_PIC10F200_Microcontroller.pdf` | 836 KB | ✅ Complete | 8-bit microcontroller (reference/alternative) |

---

## Naming Convention

All datasheets follow the standardized naming format:
```
[Manufacturer]_[PartNumber]_[ComponentType]_Datasheet.pdf
```

**Examples:**
- `RPi_Pico_W_Datasheet.pdf`
- `MCP73871_Charger_Datasheet.pdf`
- `SKQUCAA010_Joystick_Datasheet.pdf`

---

## Download Status Summary

| **Status** | **Count** | **Percentage** | **Action Required** |
|------------|-----------|----------------|-------------------|
| ✅ **Complete** | 9 files | 75% | None |
| ⚠️ **Incomplete** | 2 files | 17% | Re-download required |
| ❌ **Missing** | 2 files | 17% | Manual download needed |

### Files Requiring Action

#### Incomplete Downloads (Small File Sizes)
1. **DT010ATFT_Display_Datasheet.pdf** (919 B) - Likely error page or redirect
2. **AMS1117_Regulator_Datasheet.pdf** (448 B) - Likely error page or redirect

#### Failed Downloads
1. **Hirose_DM3AT-SF-PEJM5_MicroSD-Socket.pdf** - HTTP/2 stream error
2. **USB4085-GF-A USB-C connector** - Access denied

---

## Usage in Documentation

These datasheets are referenced in the following project documents:

### Bill of Materials (BOM.md)
- Component specifications and part numbers
- Electrical characteristics and ratings
- Package information for PCB footprints

### Assembly Guide (assembly-guide.md)
- Pin configurations and orientations
- Soldering specifications and temperatures
- Package handling and ESD precautions

### Pinout Reference (pinout.md)
- GPIO assignments and electrical specifications
- Voltage levels and current ratings
- Signal timing and interface requirements

### Hardware README (README.md)
- System architecture and component selection
- Performance specifications and capabilities
- Interface standards and compatibility

---

## File Verification

To verify datasheet integrity, check file sizes against expected ranges:

| **Component Type** | **Expected Size Range** | **Notes** |
|-------------------|------------------------|-----------|
| **MCU Datasheets** | 5-50 MB | Comprehensive specifications |
| **IC Datasheets** | 500 KB - 10 MB | Detailed electrical characteristics |
| **Passive Components** | 100 KB - 1 MB | Basic specifications |
| **Mechanical Components** | 50 KB - 500 KB | Dimensional drawings |

**Files under 10 KB are likely error pages or incomplete downloads.**

---

## Manual Download Instructions

For files that failed automatic download:

### DT010ATFT Display
1. Visit: https://www.seacomp.com/sites/default/files/datasheets/DT010ATFT-Rev1_2-Displaytech-Spec.pdf
2. Save as: `DT010ATFT_Display_Datasheet.pdf`
3. Verify file size > 100 KB

### AMS1117 Regulator  
1. Search for official AMS1117 datasheet from Advanced Monolithic Systems
2. Alternative sources: Digi-Key, Mouser technical documents
3. Save as: `AMS1117_Regulator_Datasheet.pdf`

### Hirose MicroSD Socket
1. Visit Hirose official website or Mouser/Digi-Key product pages
2. Search for DM3AT-SF-PEJM5 specifications
3. Save as: `Hirose_DM3AT-SF-PEJM5_MicroSD-Socket.pdf`

### GCT USB-C Connector
1. Visit: https://gct.co/ or distributor websites
2. Search for USB4085-GF-A specifications
3. Save as: `GCT_USB4085-GF-A_USB-C-Connector.pdf`

---

## Maintenance

### Regular Updates
- Check for datasheet revisions quarterly
- Verify all links remain active
- Update file sizes and status
- Add new component datasheets as needed

### Version Control
- All datasheets are tracked in git (with LFS for large files)
- Document any datasheet version changes
- Maintain backward compatibility references

### Community Contributions
- Community members can submit missing or updated datasheets
- Follow naming convention and update this index
- Verify authenticity of manufacturer datasheets

---

## Legal and Compliance

### Copyright Notice
All datasheets are property of their respective manufacturers and are included under fair use for educational and non-commercial purposes. Users should:

- Respect manufacturer copyright and terms of use
- Use datasheets only for legitimate engineering purposes  
- Refer to original manufacturer sources for commercial applications
- Credit manufacturers appropriately in derivative works

### Redistribution
This collection is provided for educational use with the KISS Fuzzer open source project. Commercial users should obtain datasheets directly from manufacturers or authorized distributors.

---

## Contact and Support

- **Missing Datasheets**: [Open GitHub Issue](https://github.com/seedon198/KISS/issues)
- **Broken Links**: Report via project issue tracker
- **Alternative Sources**: Community contributions welcome
- **Technical Questions**: Reference original manufacturer documentation

**Document Control:**
- **Document ID**: KISS-DS-INDEX-v1.0
- **Next Review**: Q4 2025
- **Maintainer**: KISS Fuzzer development team
