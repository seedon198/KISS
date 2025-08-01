# KISS Fuzzer STL Files

This folder will contain the 3D printable STL files for the KISS Fuzzer enclosure and mechanical components.

## Files (Coming Soon)

The following STL files will be provided for 3D printing:

### Main Enclosure
- **KISS-Fuzzer-Case-Top.stl** - Top case half with display cutout
- **KISS-Fuzzer-Case-Bottom.stl** - Bottom case half with battery compartment
- **KISS-Fuzzer-Case-Assembly.stl** - Combined model for visualization

### Internal Components  
- **KISS-Fuzzer-PCB-Standoffs.stl** - PCB mounting standoffs
- **KISS-Fuzzer-Button-Caps.stl** - Custom button caps
- **KISS-Fuzzer-Battery-Holder.stl** - Battery retention clips
- **KISS-Fuzzer-SD-Card-Cover.stl** - MicroSD card access cover

### Accessories
- **KISS-Fuzzer-Lanyard-Mount.stl** - Lanyard attachment point
- **KISS-Fuzzer-Tripod-Adapter.stl** - 1/4"-20 tripod mount
- **KISS-Fuzzer-Desktop-Stand.stl** - Desktop display stand

## Print Settings

### Recommended Settings
- **Layer Height**: 0.2mm (0.15mm for fine details)
- **Infill**: 20% (15% for non-structural parts)
- **Print Speed**: 50mm/s
- **Travel Speed**: 120mm/s
- **Nozzle Temperature**: 210°C (PLA) / 240°C (PETG)
- **Bed Temperature**: 60°C (PLA) / 80°C (PETG)

### Support Requirements
- **Main Case**: Supports required for overhangs
- **Button Caps**: No supports needed
- **Standoffs**: Supports recommended
- **Internal Parts**: Minimal supports

### Material Recommendations
- **Primary Choice**: PETG (durability + flexibility)
- **Alternative**: PLA+ (easier printing)
- **Professional**: ABS (chemical resistance)
- **Premium**: Nylon (maximum durability)

## Color Scheme

### Recommended Colors
- **Main Case**: Translucent Purple (signature color)
- **Button Caps**: Opaque White or Black
- **Internal Parts**: Any color (not visible)
- **Accessories**: Match case or contrast

### Color Matching
- **Pantone**: 2665 C (Purple)
- **RAL**: 4008 (Signal Violet)
- **Hex**: #663399 (Web Purple)

## Printer Compatibility

### Tested Printers
- **Prusa i3 MK3S+** - Excellent results
- **Ender 3 V2** - Good with upgrades
- **Bambu Lab X1 Carbon** - Excellent quality
- **Ultimaker S3** - Professional quality

### Minimum Requirements
- **Build Volume**: 85mm × 55mm × 25mm minimum
- **Layer Resolution**: 0.3mm maximum
- **Extruder**: Single color sufficient
- **Heated Bed**: Recommended but not required

## Post-Processing

### Required Steps
1. **Support Removal** - Carefully remove support material
2. **Surface Smoothing** - Light sanding with 400-grit
3. **Hole Cleaning** - Clear screw holes with drill bit
4. **Test Fit** - Verify PCB and component fitment

### Optional Steps
- **Acetone Vapor** (ABS only) - Smooth surface finish
- **Primer + Paint** - Custom color schemes
- **Clear Coat** - Additional protection
- **Threading** - Tap screw holes for better threads

## Quality Control

### Dimensional Tolerances
- **Critical Dimensions**: ±0.1mm
- **Screw Holes**: +0.1/-0.0mm
- **Press Fits**: ±0.05mm
- **Clearances**: +0.2mm minimum

### Fit and Function Tests
- [ ] **PCB Fit** - Board slides in without force
- [ ] **Connector Access** - All ports accessible
- [ ] **Button Operation** - Smooth actuation
- [ ] **Display Visibility** - Clear viewing angle
- [ ] **Battery Access** - Easy installation/removal
- [ ] **Case Closure** - Halves mate properly

## Assembly Notes

### Hardware Required
- **M2.5×6mm screws** - PCB standoffs (4x)
- **M2.5×8mm screws** - Case assembly (4x)
- **Heat-set inserts** - M2.5 threaded inserts (optional)

### Special Instructions
- **Standoff Installation** - Press-fit or heat-set inserts
- **Button Alignment** - Verify button cap alignment
- **Display Protection** - Avoid scratching display area
- **Cable Management** - Route cables to avoid pinching

## Status

**Current Status**: ⏳ **Mechanical Design in Progress**

The 3D models are currently being designed in CAD software. STL files will be exported once the mechanical design is finalized.

**Expected Completion**: Q4 2025

## Printing Services

### Online Services
- **Shapeways** - Professional SLA printing
- **Craftcloud** - Multiple material options
- **Protolabs** - Industrial quality
- **local3d.com** - Local printer network

### Local Options
- **Public Libraries** - Many offer 3D printing
- **Makerspaces** - Community workshops
- **University Labs** - Student access
- **Print Shops** - Commercial printing services

## Design Philosophy

### Aesthetic Goals
- **Flipper Zero Inspired** - Similar form factor and feel
- **Professional Appearance** - Clean, minimal design
- **Signature Color** - Translucent purple identity
- **Functional Beauty** - Form follows function

### Functional Requirements
- **Compact Size** - Pocket-friendly dimensions
- **Durable Construction** - Drop and impact resistant
- **Easy Assembly** - Minimal tools required
- **User Accessible** - Simple disassembly for repairs

## Related Files

- **CAD Source**: `../mechanical/` (STEP files)
- **Assembly Guide**: `../docs/assembly-guide.md`  
- **PCB Design**: `../pcb/` (KiCad project)
- **Hardware Specs**: `../docs/components-summary.md`
