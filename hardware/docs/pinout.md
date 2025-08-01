# KISS Fuzzer Pinout Reference

**📋 Component Datasheets**: All referenced component datasheets are available in [`../datasheets/`](../datasheets/) directory.  
**📖 Datasheet Index**: See [`../datasheets/README.md`](../datasheets/README.md) for complete datasheet listing.

## Raspberry Pi Pico W Pin Assignments

### Primary Functions

| GPIO | Function | Description | Notes |
|------|----------|-------------|-------|
| 0    | JTAG TCK | JTAG Clock | PIO-driven, up to 10MHz |
| 1    | JTAG TMS | JTAG Mode Select | PIO-driven |
| 2    | JTAG TDI | JTAG Data In | PIO-driven |
| 3    | JTAG TDO | JTAG Data Out | Input with pull-up |
| 4    | JTAG TRST| JTAG Reset (optional) | Active low |
| 5    | SWD SWCLK| SWD Clock | Shared with TCK |
| 6    | SWD SWDIO| SWD Data I/O | Bidirectional |
| 7    | TARGET_PWR| Target Power Control | MOSFET gate driver |

### Display Interface (SPI0)

| GPIO | Function | Description |
|------|----------|-------------|
| 8    | MOSI     | SPI Data for TFT |
| 9    | SCK      | SPI Clock for TFT |
| 10   | CS       | TFT Chip Select |
| 11   | DC       | Data/Command Select |
| 12   | TOUCH_CS | Touch Controller CS |

### User Interface

| GPIO | Function | Description |
|------|----------|-------------|
| 16   | JOY_UP   | Joystick Up |
| 17   | JOY_DOWN | Joystick Down |
| 18   | JOY_LEFT | Joystick Left |
| 19   | JOY_RIGHT| Joystick Right |
| 20   | JOY_OK   | Joystick Press/OK |
| 21   | BTN_BACK | Back Button |

### Power Management

| GPIO | Function | Description |
|------|----------|-------------|
| 22   | USB_DETECT| USB Connection Detect |
| 23   | CHG_STAT | Charging Status (active low) |
| 24   | PWR_EN   | Power Path Enable |

### Storage Interface (SPI1)

| GPIO | Function | Description |
|------|----------|-------------|
| 10   | SD_CS    | SD Card Chip Select |
| 11   | SD_MOSI  | SD Card MOSI |
| 12   | SD_MISO  | SD Card MISO |
| 13   | SD_SCK   | SD Card Clock |
| 14   | SD_DETECT| SD Card Detect |

### Expansion Interface

| GPIO | Function | Description |
|------|----------|-------------|
| 25   | GLITCH_OUT| Power Glitch Output |
| 26   | VBAT_SENSE| Battery Voltage (ADC0) |
| 27   | VTARGET_SENSE| Target Voltage (ADC1) |
| 28   | EXPANSION_1| General Purpose I/O |

### Internal Connections

| Pin  | Function | Description |
|------|----------|-------------|
| CYW43| LED      | On-board LED |
| CYW43| Wi-Fi    | Built-in Wi-Fi module |

## Voltage Levels

- **System Logic**: 3.3V
- **Target Interface**: 1.8V - 5.0V (level shifted)
- **Battery**: 3.0V - 4.2V Li-ion
- **USB**: 5V input

## Power Budget

| Component | Typical Current | Notes |
|-----------|----------------|-------|
| RP2040 Core | 30mA | At 125MHz |
| CYW43 Wi-Fi | 40mA | Active mode |
| OLED Display | 20mA | Full brightness |
| Target Power | 0-500mA | Configurable limit |
| Standby Mode | 2mA | Sleep mode |

**Total Active**: ~90mA (without target)
**Battery Life**: ~33 hours continuous operation (3000mAh battery)

## ESD Protection

All external interfaces include ESD protection:
- JTAG/SWD: TVS diodes on all signals
- USB-C: Integrated protection in USB-C connector
- User inputs: RC filters + Schottky clamps
- Power inputs: Ferrite beads + bulk capacitors

## Signal Integrity

- **JTAG Clock**: Controlled impedance, series termination
- **USB Differential**: 90Ω differential impedance
- **Power Planes**: Dedicated 3.3V and GND planes
- **Decoupling**: 100nF ceramics + 10µF tantalum per IC
