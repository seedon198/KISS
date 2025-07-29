# Under the Hood: KISS Fuzzer Hardware 🔧

Ever wondered what makes your KISS Fuzzer tick? Let's pop the hood and take a look at the engineering that went into this little powerhouse.

## The heart of the machine

At its core, KISS Fuzzer is built around the Raspberry Pi Pico W - but don't let the "Pi" fool you. This isn't your typical single-board computer. It's a dual-core ARM Cortex-M0+ running at 133MHz with seriously impressive I/O capabilities.

**Why the Pico W?** Three reasons: it's fast enough for real-time JTAG operations, has built-in Wi-Fi for remote control, and the PIO (Programmable I/O) blocks let us create custom JTAG timing that would make expensive commercial tools jealous.

### The specs that matter

**Processing power**: Dual-core ARM Cortex-M0+ at 133MHz sounds modest, but it's perfect for embedded work. One core handles the UI and Wi-Fi, while the other focuses entirely on JTAG operations.

**Memory**: 264KB of SRAM might seem small, but remember - we're not running Chrome here. It's plenty for buffering JTAG data and keeping the system responsive.

**Storage**: 2MB of flash for the firmware, plus that MicroSD slot for all your discoveries. We've seen people fill 32GB cards with interesting findings!

## The JTAG interface that doesn't suck

Most JTAG adapters are either expensive, slow, or both. We built something different.

**Speed**: Up to 10 MHz clock rate. That's fast enough for production work but slow enough to be stable with breadboard connections.

**Voltage handling**: Works with anything from 1.8V to 5V targets. The level shifters automatically adapt, so you don't have to worry about frying your expensive dev board.

**Protection**: Over-voltage protection, reverse polarity protection, and current limiting. Because everyone connects things backward sometimes (we've all been there).

### The connector you'll actually want to use

Forget those tiny 0.05" pitch connectors that require a magnifying glass. Our JTAG connector uses standard 0.1" pitch pins that work with normal jumper wires.

```
Looking at the connector (top view):

┌─── Pin 1 (VCC) - Red wire
│┌── Pin 2 (TCK) - Yellow wire  
││┌─ Pin 3 (TDI) - Green wire
│││
●●●●  ←── These connect to your target
●●●●
│││
││└─ Pin 6 (TMS) - Purple wire
│└── Pin 7 (TDO) - Blue wire
└─── Pin 8 (GND) - Black wire
```

**Pro tip**: The pins are color-coded and labeled. Red always goes to target power, black always goes to ground. Simple.
| **Display SPI** | 0-3 | OLED communication |
| **Joystick** | 4-8 | 5-way navigation |
| **JTAG Interface** | 10-15 | Target communication |
| **SD Card SPI** | 16-19 | Storage interface |
| **Power Control** | 20-22 | Battery management |
| **Status LEDs** | 25-27 | Visual indicators |

## Detailed Pin Assignments

### JTAG/SWD Interface
- **GPIO 0-4**: JTAG signals (TCK, TMS, TDI, TDO, TRST)
- **GPIO 5-6**: SWD signals (SWCLK, SWDIO)
- **GPIO 7**: Target power control

### Display & UI
- **GPIO 8-9**: I2C for OLED display (SDA, SCL)
- **GPIO 16-21**: 5-way joystick and buttons

### Power Management
- **GPIO 22-24**: USB detect, charge status, power enable
- **GPIO 26-27**: Battery and target voltage monitoring (ADC)

### Storage
- **GPIO 10-14**: SPI interface for MicroSD card

See [hardware/pinout.md](../hardware/pinout.md) for complete pin assignments and electrical specifications.

## Power Specifications

- **Input**: USB-C (5V, up to 3A)
- **Battery**: 18650 Li-ion (3.7V, 3000mAh)
- **System**: 3.3V regulated
- **Target Supply**: 1.8V - 5.0V adjustable
- **Power Budget**: ~90mA typical operation
- **Battery Life**: 33+ hours continuous use

## Power System

### Battery Specifications

- **Type**: Li-ion 18650 or equivalent
- **Voltage**: 3.7V nominal (3.0V - 4.2V range)
- **Capacity**: 2000-3000mAh recommended
- **Charging**: USB-C power delivery
- **Protection**: Over-charge, over-discharge, short circuit

### Power Consumption

| Mode | Current Draw | Battery Life |
|------|-------------|--------------|
| **Active Scanning** | 150-200mA | 10-15 hours |
| **Wi-Fi Active** | 100-150mA | 15-20 hours |
| **Menu Navigation** | 50-80mA | 25-40 hours |
| **Sleep Mode** | 5-10mA | 200-400 hours |

## Physical Specifications

### Enclosure

- **Material**: Translucent purple SLA resin
- **Dimensions**: 85mm × 55mm × 20mm (approx.)
- **Weight**: 120g with battery
- **Mounting**: Standard 1/4"-20 tripod thread
- **Protection**: IP54 dust/splash resistant

### Thermal Management

- **Operating Range**: 0°C to 50°C
- **Storage Range**: -20°C to 70°C
- **Cooling**: Passive convection
- **Thermal Protection**: Automatic throttling at 65°C

## Electrical Characteristics

### Signal Integrity

- **Rise/Fall Time**: < 5ns at 10 MHz
- **Jitter**: < 100ps RMS
- **Crosstalk**: < -40dB between channels
- **Input Impedance**: 100kΩ typical
- **Output Impedance**: 50Ω ±10%

### Protection Features

- **ESD Protection**: ±15kV on all I/O pins
- **Over-voltage**: 5.5V maximum on any pin
- **Short Circuit**: 100mA current limiting
- **Reverse Polarity**: Protected on power inputs

## Expansion Options

### External Interfaces

- **USB-C**: Programming, power, data transfer
- **Debug Header**: SWD access to main MCU
- **I2C**: External sensor interface
- **GPIO**: 4 general-purpose pins available

### Accessories

- **Probe Clips**: Spring-loaded test clips
- **Cable Adapters**: Common connector types
- **Carrying Case**: Protective storage
- **Charging Dock**: Desktop charging station
