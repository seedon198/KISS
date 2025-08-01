# KISS Fuzzer Pick and Place Files

This folder will contain the pick and place files for automated SMD assembly of the KISS Fuzzer PCB.

## Files (Coming Soon)

The following files will be generated from the PCB design:

### Pick and Place Files
- **KISS-Fuzzer-top-pos.csv** - Component positions for top side
- **KISS-Fuzzer-bottom-pos.csv** - Component positions for bottom side  
- **KISS-Fuzzer-all-pos.csv** - All component positions combined
- **KISS-Fuzzer-centroid.csv** - Centroid data for pick and place machines

### Assembly Drawings
- **KISS-Fuzzer-assembly-top.pdf** - Top side assembly drawing
- **KISS-Fuzzer-assembly-bottom.pdf** - Bottom side assembly drawing
- **KISS-Fuzzer-assembly-notes.txt** - Special assembly instructions

## CSV Format

The pick and place files will follow industry standard format:

```csv
Designator,Val,Package,Mid X,Mid Y,Rotation,Layer
U1,Raspberry Pi Pico W,RP2040,42.5,27.5,0,top
U2,AMS1117-3.3,SOT-223,15.0,10.0,90,top
C1,100nF,0603,5.0,5.0,0,top
R1,10K,0603,10.0,8.0,180,top
```

### Column Descriptions
- **Designator** - Component reference (U1, R1, C1, etc.)
- **Val** - Component value or part number
- **Package** - Component package type
- **Mid X** - X coordinate of component center (mm)
- **Mid Y** - Y coordinate of component center (mm) 
- **Rotation** - Component rotation angle (degrees)
- **Layer** - PCB layer (top/bottom)

## Assembly Information

### Component Count
- **Total Components**: ~110 components
- **SMD Components**: ~95 components  
- **Through-hole**: ~15 components
- **Top Side**: ~85 components
- **Bottom Side**: ~10 components

### Package Types
- **0603** - Resistors, capacitors, LEDs (majority)
- **SOT-23** - Small transistors, regulators
- **SOT-223** - Voltage regulators
- **QFN/DFN** - Power management ICs
- **SSOP/TSSOP** - Level shifters, logic ICs
- **Custom** - Raspberry Pi Pico W, connectors

### Assembly Sequence
1. **Solder paste application** using stencil
2. **Pick and place** of SMD components
3. **Reflow soldering** in convection oven
4. **Inspection** and touch-up
5. **Through-hole assembly** (manual or wave solder)
6. **Final testing** and quality control

## Machine Compatibility

These files are compatible with:
- **JUKI** pick and place machines
- **Yamaha** SMT placement systems  
- **Panasonic** CM series machines
- **Samsung** SMT lines
- **Generic** pick and place systems

## Assembly Requirements

### Stencil Specifications
- **Thickness**: 0.12mm (5 mil) for fine pitch
- **Aperture ratio**: 0.8-1.0 for most components
- **Step stencil**: May be required for mixed component heights
- **Alignment**: ±0.05mm positioning accuracy

### Placement Accuracy
- **Fine pitch** (≤0.5mm): ±0.02mm
- **Standard pitch** (>0.5mm): ±0.05mm  
- **Large components**: ±0.1mm
- **Rotation**: ±1 degree

### Special Handling
- **Moisture sensitive** components require baking
- **ESD sensitive** components need proper handling
- **Orientation critical** for polarized components
- **Height sensitive** areas marked in assembly notes

## Status

**Current Status**: ⏳ **PCB Design in Progress**

Pick and place files will be generated automatically from the KiCad PCB layout once the design is finalized.

**Expected Completion**: Q4 2025

## Assembly Services

### Recommended Providers
- **JLCPCB** - Integrated PCB + SMT assembly
- **PCBWay** - Full turnkey assembly service
- **MacroFab** - US-based SMT assembly
- **Seeed Studio** - Fusion PCB assembly service

### Service Requirements
- **Component sourcing** included or customer supplied
- **SMT + through-hole** assembly capability
- **Quality testing** (AOI, ICT, functional)
- **Programming** of any pre-programmed components

## Quality Standards

- **IPC-A-610** - Assembly acceptability standards
- **J-STD-001** - Soldering requirements
- **AOI (Automated Optical Inspection)** for placement verification
- **X-ray inspection** for hidden solder joints (BGA/QFN)
- **In-circuit testing** for electrical verification

## Related Files

- **PCB Design**: `../pcb/` (KiCad project files)
- **Gerber Files**: `../gerber/` (PCB manufacturing)
- **BOM**: `../bom/KISS-Fuzzer-BOM-v1.0.csv`
- **Assembly Guide**: `../docs/assembly-guide.md`
