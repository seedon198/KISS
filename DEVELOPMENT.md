# KISS Fuzzer Development Guide

## Quick Start

### Prerequisites
- **Hardware**: Raspberry Pi Pico W
- **SDK**: Pico SDK (install to `/opt/pico-sdk` or set `PICO_SDK_PATH`)
- **Toolchain**: ARM GCC (`arm-none-eabi-gcc`)
- **Build System**: CMake 3.13+
- **Optional**: OpenOCD, GDB for debugging

### Installation

1. **Clone the repository**:
   ```bash
   git clone <repository-url>
   cd KISS-Fuzzer
   ```

2. **Install dependencies** (Ubuntu/Debian):
   ```bash
   sudo apt update
   sudo apt install cmake gcc-arm-none-eabi libnewlib-arm-none-eabi \
                    libstdc++-arm-none-eabi-newlib git python3
   ```

3. **Install Pico SDK**:
   ```bash
   git clone https://github.com/raspberrypi/pico-sdk.git /opt/pico-sdk
   cd /opt/pico-sdk
   git submodule update --init
   ```

### Building

Use the provided build script for easy compilation:

```bash
# Debug build (default)
./build.sh

# Release build
./build.sh release

# Clean and rebuild
./build.sh clean debug

# Flash to device
./build.sh flash
```

Manual build process:
```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DPICO_SDK_PATH=/opt/pico-sdk ..
make -j$(nproc)
```

### Flashing

1. **Hold BOOTSEL** button on Pico W while connecting USB
2. **Run flash command**:
   ```bash
   ./build.sh flash
   ```
3. **Device reboots** automatically and starts running

## Development Workflow

### Code Organization

```
src/
├── main.c              # System initialization and main loop
├── kiss_fuzzer.h       # Global definitions and configuration
├── display.[ch]        # OLED display driver
├── ui.[ch]             # User interface and menu system
├── jtag.[ch]           # JTAG/SWD engine with PIO
├── jtag.pio           # PIO assembly for JTAG timing
├── wifi.[ch]           # Wi-Fi web server and HTTP endpoints
├── power.[ch]          # Battery monitoring and power management
├── storage.[ch]        # SD card logging and file management
└── FreeRTOSConfig.h   # RTOS configuration
```

### Adding New Features

1. **Create module** in `src/` directory
2. **Add header** with Doxygen documentation
3. **Implement functions** following existing patterns
4. **Update CMakeLists.txt** if needed
5. **Test and validate** functionality

### Configuration

Edit `config.env` to customize:
- Hardware pin assignments
- Network settings
- JTAG parameters
- Performance tuning
- Feature enables/disables

### Debugging

#### Serial Debug Output
Connect to USB serial at 115200 baud:
```bash
# macOS
screen /dev/tty.usbmodem* 115200

# Linux
screen /dev/ttyACM0 115200
```

#### GDB Debugging
1. **Connect SWD debugger** to Pico W
2. **Start OpenOCD**:
   ```bash
   openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg
   ```
3. **Launch GDB**:
   ```bash
   arm-none-eabi-gdb build/kiss_fuzzer.elf
   (gdb) target remote localhost:3333
   (gdb) monitor reset init
   (gdb) break main
   (gdb) continue
   ```

#### Web Interface Debug
Access the device via:
- **AP Mode**: Connect to "KISS-Fuzzer" network → http://192.168.4.1
- **Station Mode**: Find IP via router → http://device-ip

## Testing

### Unit Testing
```bash
# Run unit tests (if implemented)
cd tests
mkdir build && cd build
cmake ..
make test
```

### Integration Testing
1. **Flash firmware** to device
2. **Connect JTAG target** to test pins
3. **Access web interface** and verify functionality
4. **Test JTAG operations** through UI and API

### Hardware Validation
- **Power consumption** measurement
- **JTAG signal integrity** with oscilloscope
- **Wi-Fi range and stability** testing
- **Battery life** under various loads

## Deployment

### Production Build
```bash
# Optimized release build
./build.sh release
```

### Configuration Management
1. **Copy config.env** to deployment-specific file
2. **Modify settings** for target environment
3. **Rebuild with custom config** if needed

### Over-the-Air Updates
The web interface supports firmware updates:
1. **Build new firmware** (.uf2 file)
2. **Access web interface** → Settings → Firmware Update
3. **Upload .uf2 file** and device reboots with new firmware

## API Reference

### Web API Endpoints

#### System Status
```
GET /api/status
Response: {"wifi":"Connected","power":3.7,"battery":85,"uptime":12345}
```

#### JTAG Operations
```
GET /api/jtag/scan       # Scan JTAG chain
GET /api/jtag/detect     # Detect pin configuration
GET /api/jtag/idcode     # Read device IDCODE
GET /api/jtag/dump       # Memory dump operation
```

#### Wi-Fi Management
```
GET /api/wifi/status                    # Get connection status
GET /api/wifi/ap                       # Start AP mode
GET /api/wifi/connect?ssid=X&password=Y # Connect to network
```

#### File Operations
```
GET /api/files/list      # List stored files
GET /api/files/download?name=X  # Download file
DELETE /api/files/delete?name=X # Delete file
```

### FreeRTOS Task Structure

| Task | Priority | Stack | Function |
|------|----------|-------|----------|
| System | High | 4KB | Health monitoring, watchdog |
| Wi-Fi | High | 8KB | Network operations, web server |
| UI | Medium | 2KB | User interface, input handling |
| JTAG | Medium | 4KB | JTAG operations, fuzzing |
| Power | Low | 1KB | Battery monitoring |

### Memory Usage

| Component | RAM | Flash |
|-----------|-----|-------|
| FreeRTOS | ~8KB | ~20KB |
| Wi-Fi Stack | ~40KB | ~100KB |
| Application | ~20KB | ~80KB |
| **Total** | **~68KB** | **~200KB** |

## Troubleshooting

### Common Issues

1. **Build Failures**
   - Check Pico SDK path
   - Verify ARM GCC installation
   - Update submodules

2. **Flash Failures**
   - Ensure BOOTSEL mode
   - Check USB connection
   - Verify .uf2 file exists

3. **Wi-Fi Issues**
   - Check antenna connection
   - Verify network credentials
   - Monitor signal strength

4. **JTAG Problems**
   - Verify pin connections
   - Check target power
   - Test with known good target

### Log Analysis
Logs are available via:
- Serial console (immediate)
- Web interface (/logs endpoint)
- SD card files (persistent)

### Performance Optimization

1. **Reduce stack sizes** if memory constrained
2. **Adjust task priorities** for responsiveness
3. **Enable compiler optimizations** for release builds
4. **Profile task CPU usage** with FreeRTOS stats

## Contributing

### Code Style
- Follow existing naming conventions
- Add Doxygen comments for all functions
- Use consistent indentation (4 spaces)
- Keep lines under 100 characters

### Pull Requests
1. **Create feature branch** from main
2. **Implement changes** with tests
3. **Update documentation** as needed
4. **Submit PR** with clear description

### Issue Reporting
Include:
- Hardware revision
- Firmware version
- Steps to reproduce
- Expected vs actual behavior
- Log output if applicable

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Support

- **Documentation**: This README and code comments
- **Issues**: GitHub issue tracker
- **Community**: Project discussions forum
- **Commercial**: Contact for professional support
