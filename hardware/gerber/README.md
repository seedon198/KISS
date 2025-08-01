# KISS Fuzzer Gerber Files

This folder will contain the PCB manufacturing files (Gerbers) for the KISS Fuzzer project.

## Files (Coming Soon)

The following files will be generated from the PCB design:

### Standard Gerber Files
- **KISS-Fuzzer-F_Cu.gbr** - Front copper layer
- **KISS-Fuzzer-B_Cu.gbr** - Back copper layer  
- **KISS-Fuzzer-In1_Cu.gbr** - Inner layer 1 (power)
- **KISS-Fuzzer-In2_Cu.gbr** - Inner layer 2 (ground)
- **KISS-Fuzzer-F_Mask.gbr** - Front solder mask
- **KISS-Fuzzer-B_Mask.gbr** - Back solder mask
- **KISS-Fuzzer-F_Silkscreen.gbr** - Front silkscreen
- **KISS-Fuzzer-B_Silkscreen.gbr** - Back silkscreen
- **KISS-Fuzzer-F_Paste.gbr** - Front solder paste
- **KISS-Fuzzer-B_Paste.gbr** - Back solder paste

### Drill Files
- **KISS-Fuzzer-PTH.drl** - Plated through holes
- **KISS-Fuzzer-NPTH.drl** - Non-plated through holes
- **KISS-Fuzzer-drill_map.pdf** - Drill map for reference

### Additional Files
- **KISS-Fuzzer-job.gbrjob** - Gerber job file (RS-274X format)
- **KISS-Fuzzer-fabrication-notes.txt** - Special fabrication instructions

## PCB Specifications

- **Board Size**: 85mm × 55mm (credit card format)
- **Layers**: 4-layer PCB
- **Thickness**: 1.6mm
- **Copper Weight**: 1oz (35µm)
- **Surface Finish**: HASL or ENIG
- **Solder Mask**: Purple
- **Silkscreen**: White
- **Minimum Via**: 0.1mm (4 mils)
- **Minimum Trace**: 0.1mm (4 mils)
- **Minimum Drill**: 0.2mm (8 mils)

## Manufacturing Notes

### Controlled Impedance
- **Single-ended**: 50Ω ±10%
- **Differential**: 90Ω ±10%
- **Critical nets**: USB data lines, crystal oscillator

### Special Requirements
- **Purple solder mask** to match case color
- **ENIG finish preferred** for fine-pitch components
- **Via-in-pad** may be required for some QFN packages
- **Impedance control** required for high-speed signals

## Status

**Current Status**: ⏳ **PCB Design in Progress**

The PCB layout is currently being designed in KiCad. Gerber files will be generated once the layout is complete and verified.

**Expected Completion**: Q4 2025

## Manufacturing Recommendations

### Tested Manufacturers
- **JLCPCB** - Cost-effective, good quality
- **PCBWay** - Premium options, excellent finish  
- **OSH Park** - Domestic (US), purple PCBs available
- **Advanced Circuits** - High-end prototyping

### Quality Requirements
- **IPC Class 2** minimum (IPC Class 3 preferred)
- **Electrical testing** of all nets
- **AOI (Automated Optical Inspection)** recommended
- **Flying probe testing** for prototype quantities

## Verification Checklist

Before releasing gerber files, the following checks will be performed:

- [ ] **DRC (Design Rule Check)** passed
- [ ] **ERC (Electrical Rule Check)** passed  
- [ ] **3D visualization** review completed
- [ ] **Stackup verification** with manufacturer
- [ ] **Impedance calculation** verified
- [ ] **Assembly compatibility** confirmed
- [ ] **Panelization** optimized (if applicable)
- [ ] **Gerber viewer** verification completed

## Related Files

- **PCB Source**: `../pcb/` (KiCad project files)
- **Assembly Guide**: `../docs/assembly-guide.md`
- **Component Placement**: `../pickandplace/`
- **BOM**: `../bom/KISS-Fuzzer-BOM-v1.0.csv`
