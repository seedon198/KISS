# KISS Fuzzer Documentation

Welcome to the official documentation for KISS Fuzzer - a handheld JTAG/SWD fuzzing and exploitation tool.

```{toctree}
:maxdepth: 2
:caption: Contents:

getting-started
hardware
software
api-reference
tutorials
development
troubleshooting
```

## Quick Start

KISS Fuzzer (Keep It Simple, Silly) is a compact, handheld JTAG/SWD fuzzing and exploitation tool built on the Raspberry Pi Pico W (RP2040). 

### Key Features

- Ultra-wide banner OLED display (240×64)
- 5-way joystick navigation
- Li-ion battery with USB-C charging
- High-speed JTAG/SWD interface up to 10 MHz
- Wi-Fi web interface for remote control
- Optional power glitch injection
- MicroSD card logging

## Architecture Overview

```{mermaid}
graph TB
    A[User Interface] --> B[JTAG Engine]
    A --> C[Wi-Fi Server]
    A --> D[Power Management]
    B --> E[Target Device]
    C --> F[Web Browser]
    D --> G[Battery Monitor]
    
    subgraph "Hardware Layer"
        H[Raspberry Pi Pico W]
        I[OLED Display]
        J[5-Way Joystick]
        K[MicroSD Card]
    end
    
    A --> H
    B --> H
    C --> H
    D --> H
```

## Getting Help

- **GitHub Issues**: Report bugs and request features
- **Documentation**: Comprehensive guides and API reference
- **Community**: Join our Discord for real-time support

```{admonition} Safety Notice
:class: warning

KISS Fuzzer is designed for authorized security testing and educational purposes only. 
Always ensure you have permission to test target devices and comply with local laws.
```
