COPILOT INSTRUCTIONS FOR KISS FUZZER

Project Summary:
----------------
KISS Fuzzer (Keep It Simple, Silly) is a handheld JTAG/SWD fuzzing and exploitation tool built on the Raspberry Pi Pico W (RP2040). It features:
- Ultra-wide banner-style OLED display (240×64) for single-line logs/status
- 5-way joystick + buttons for local UI
- Li-ion battery with USB-C power-path charging
- JTAG/SWD scanning, pinout detection, boundary scan, memory dump
- Optional power glitch injection via MOSFET
- Wi-Fi web UI for remote control and log retrieval
- 3D-printed SLA translucent purple case (Flipper Zero-like form factor)

Coding Guidelines for Copilot:
------------------------------
Language: C (Pico SDK) with FreeRTOS

Code must be modular and layered:
- display.c/h → OLED driver
- ui.c/h → Menu system & input handling
- jtag.c/h → JTAG/SWD routines using PIO
- wifi.c/h → Web server, HTTP endpoints
- power.c/h → Battery monitor & charger status
- storage.c/h → SD card logging
- main.c → System init & FreeRTOS task creation

Document every function with Doxygen-style comments:
Example:
/**
 * @brief Initialize OLED display
 * @param none
 * @return void
 */
void display_init(void);

Use non-blocking design (no long delay() calls; use RTOS tasks or timers).

FreeRTOS Usage:
---------------
Create separate tasks for:
- UI/Menu updates
- JTAG engine
- Wi-Fi server
- Power/battery monitoring

Assign priorities:
- Wi-Fi (High)
- JTAG (Medium)
- UI (Medium/Low)
- Power monitor (Low)

Use queues or semaphores for inter-task communication (e.g., log updates from JTAG task to UI task).

Display & UI:
-------------
- Banner-style OLED: scrolling status line or single-line menus
- Minimal text rendering (either LVGL or custom font table)
- Example menu flow:
  [Main Menu]
   > Scan JTAG
     Glitch Tool
     Logs
     Settings

- Joystick input:
  Up/Down → Menu scroll
  OK → Select
  Back → Exit to previous menu

JTAG/PIO Engine:
----------------
- Implement JTAG via RP2040 PIO for up to ~10 MHz
- Provide functions:
  jtag_scan()
  jtag_detect_pins()
  jtag_read_idcode()
  jtag_memory_dump()
- Support configurable clock rate and target voltage levels

Wi-Fi Web UI:
-------------
- Simple HTTP server (AP mode + captive portal)
- Endpoints: start/stop scan, logs, file download
- Mobile-friendly minimal HTML (no heavy frameworks)

Power & Battery:
----------------
- Use ADC to monitor battery voltage
- Show battery % on OLED
- Detect charging status from power-path IC
- Support operation while charging

Storage:
--------
- MicroSD card logging
- Store JTAG scan results and dumps
- Basic file management (list/delete/download)

Mechanical:
-----------
- Case: SLA 3D printed, translucent purple resin
- Compact (Flipper Zero-like)
- Cutouts for USB-C, display, joystick
- Internal standoffs for PCB and battery

Goal for Copilot:
-----------------
Generate production-quality C code following Pico SDK + FreeRTOS patterns
Suggest modular drivers and clean separation between hardware abstraction and application logic
Provide examples for tasks, queue handling, and peripheral initialization
