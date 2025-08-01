# KISS Fuzzer BOM (Bill of Materials)

This folder contains the Bill of Materials files for the KISS Fuzzer project.

## Files

- **KISS-Fuzzer-BOM-v1.0.csv** - Complete component list in CSV format for manufacturing
- **README.md** - This file

## CSV Format

The BOM CSV file follows industry standard format with the following columns:
- **Designator** - PCB reference designator (U1, R1, C1, etc.)
- **Package** - Component package type (0603, SOT-23, etc.)
- **Quantity** - Number of components required
- **Designation** - Component function/type
- **Supplier and ref** - Recommended supplier
- **Manufacturer** - Component manufacturer
- **Manufacturer Part Number** - Exact part number for ordering
- **Description** - Component description
- **Unit Price** - Cost per component (USD)
- **Total Price** - Total cost for quantity
- **Datasheet** - Path to component datasheet

## Usage

This CSV file can be imported into:
- **PCB CAD software** (KiCad, Altium, Eagle)
- **Manufacturing systems** for automated assembly
- **Procurement systems** for component ordering
- **Spreadsheet software** for cost analysis

## Status

**Current Version**: v1.0  
**Last Updated**: 2025-08-02  
**Total Components**: ~110 components  
**Estimated Cost**: $85-120 per unit  

**Note**: This BOM will be updated as the PCB design is finalized and refined.

## Related Documentation

- Full BOM details: `../docs/BOM.md`
- Assembly instructions: `../docs/assembly-guide.md`
- Component datasheets: `../datasheets/`
