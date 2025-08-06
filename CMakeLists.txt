cmake_minimum_required(VERSION 3.13)

# Include build functions from Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(kiss_fuzzer C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK
pico_sdk_init()

# Set FreeRTOS port for RP2040
set(FREERTOS_PORT GCC_RP2040 CACHE STRING "FreeRTOS port")

# Set FreeRTOS configuration file directory
set(FREERTOS_CONFIG_FILE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src" CACHE STRING "")

# Add FreeRTOS
include(FreeRTOS_Kernel_import.cmake)

add_executable(kiss_fuzzer
    src/main.c
    src/display.c
    src/input.c
    src/ui.c
    src/power.c
    src/jtag.c
    src/jtag_fuzzer.c
    src/fuzz_commands.c
    src/storage.c
    src/ff.c
)

# Pull in our pico_stdlib which aggregates commonly used features
target_link_libraries(kiss_fuzzer 
    pico_stdlib
    hardware_i2c
    hardware_adc
    hardware_pio
    hardware_spi
    FreeRTOS-Kernel
    FreeRTOS-Kernel-Heap4
)

# Add include directory for our custom lwIP options
target_include_directories(kiss_fuzzer PRIVATE src)

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
option(ENABLE_STATIC_ANALYSIS "Enable static analysis tools" OFF)
option(ENABLE_FORMAT_CHECK "Enable code formatting checks" OFF)
option(ENABLE_CPPCHECK "Enable cppcheck static analysis" OFF)
option(ENABLE_CLANG_TIDY "Enable clang-tidy static analysis" OFF)
option(AUTO_FORMAT "Automatically format code during build" OFF)

# Include code quality tools
#if(ENABLE_STATIC_ANALYSIS OR ENABLE_FORMAT_CHECK)
#    include(cmake/CodeQuality.cmake)
#endif()
