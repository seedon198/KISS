cmake_minimum_required(VERSION 3.13)

# Include build functions from Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(kiss_fuzzer C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK
pico_sdk_init()

# Add FreeRTOS
include(FreeRTOS_Kernel_import.cmake)

add_executable(kiss_fuzzer
    src/main.c
    src/display.c
    src/ui.c
    src/jtag.c
    src/wifi.c
    src/power.c
    src/storage.c
)

# Pull in our pico_stdlib which aggregates commonly used features
target_link_libraries(kiss_fuzzer 
    pico_stdlib 
    pico_cyw43_arch_lwip_threadsafe_background
    hardware_spi
    hardware_i2c
    hardware_adc
    hardware_pio
    pico_multicore
    FreeRTOS-Kernel-Heap4
)

# Enable usb output, disable uart output
pico_enable_stdio_usb(kiss_fuzzer 1)
pico_enable_stdio_uart(kiss_fuzzer 0)

# Create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(kiss_fuzzer)

# Add any PIO programs
pico_generate_pio_header(kiss_fuzzer ${CMAKE_CURRENT_LIST_DIR}/src/jtag.pio)

# Optional: Build tests if requested
option(BUILD_TESTS "Build unit tests" OFF)
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()

# Code quality and linting options
option(ENABLE_STATIC_ANALYSIS "Enable static analysis tools" ON)
option(ENABLE_FORMAT_CHECK "Enable code formatting checks" ON)
option(ENABLE_CPPCHECK "Enable cppcheck static analysis" ON)
option(ENABLE_CLANG_TIDY "Enable clang-tidy static analysis" ON)
option(AUTO_FORMAT "Automatically format code during build" OFF)

# Include code quality tools
if(ENABLE_STATIC_ANALYSIS OR ENABLE_FORMAT_CHECK)
    include(cmake/CodeQuality.cmake)
endif()
