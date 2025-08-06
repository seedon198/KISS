# KISS Fuzzer

**Keep It Simple, Silly** - Handheld JTAG/SWD Fuzzing & Exploitation Tool

![Platform](https://img.shields.io/badge/Platform-Raspberry%20Pi%20Pico%20W-black) ![Language](https://img.shields.io/badge/Language-C-blue) ![RTOS](https://img.shields.io/badge/RTOS-FreeRTOS-black) ![License](https://img.shields.io/badge/License-MIT-darkblue) [![Documentation](https://readthedocs.org/projects/kiss-fuzz/badge/?version=latest)](https://kiss-fuzz.readthedocs.io/)

---

## What is KISS Fuzzer?

KISS Fuzzer is a compact, handheld JTAG/SWD fuzzing and exploitation tool built on the Raspberry Pi Pico W. It provides hardware debugging and security assessment capabilities in a portable form factor.

**Key Features:**
- JTAG/SWD protocol engine (10MHz via PIO)
- Hardware fault injection and voltage glitching
- Automatic target detection and pinout discovery
- Wi-Fi web interface for remote operation
- 8+ hour battery life with USB-C charging
- Multi-voltage support (1.8V/3.3V/5V)

## Quick Start

### 1. Prerequisites
```bash
# Install ARM toolchain and dependencies
curl -sSL https://raw.githubusercontent.com/seedon198/KISS/main/scripts/setup-macos-arm64.sh | bash
```

### 2. Build & Flash
```bash
git clone https://github.com/seedon198/KISS.git
cd KISS
export PICO_SDK_PATH=/path/to/pico-sdk
./build.sh
```

### 3. Flash Firmware
1. Hold BOOTSEL button on Pico W
2. Connect USB-C cable
3. Copy `build/kiss_fuzzer.uf2` to RPI-RP2 drive

### 4. First Boot
1. Connect via serial console (115200 baud)
2. Device will create "KISS-Fuzzer" Wi-Fi AP
3. Connect and navigate to http://192.168.4.1
4. Start your first JTAG scan!

## Documentation

📖 **[Complete Documentation](https://kiss-fuzz.readthedocs.io/)** - Everything you need to know

Quick Links:
- 🚀 [Getting Started Guide](docs/getting-started.md)
- 🔧 [Hardware Assembly](docs/hardware.md) 
- 💻 [Software Architecture](docs/software.md)
- 🎯 [Usage Tutorials](docs/tutorials.md)
- 🛠️ [Development Guide](docs/development.md)
- 🐛 [Troubleshooting](docs/troubleshooting.md)

## Support & Community

- 📖 [Wiki](https://github.com/seedon198/KISS/wiki) - Community knowledge base
- 🐛 [Issues](https://github.com/seedon198/KISS/issues) - Bug reports and feature requests
- 💬 [Discussions](https://github.com/seedon198/KISS/discussions) - Community Q&A
- 🔒 [Security](SECURITY.md) - Security policy and bug bounty

## Contributing

We welcome contributions! See [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

Quick contribution areas:
- JTAG protocol extensions
- Target device support
- Web UI improvements  
- Documentation and tutorials
- Hardware design enhancements

## License

MIT License - see [LICENSE](LICENSE) file for details.

---

**⚠️ Legal Notice:** This tool is for authorized security testing only. Users are responsible for compliance with applicable laws and regulations.
