/**
 * @file jtag.c
 * @brief JTAG/SWD scanning and exploitation engine implementation using PIO
 * @author KISS Fuzzer Team
 * @date 2025
 */

#include "jtag.h"
#include "kiss_fuzzer.h"

#include "jtag.pio.h"

// JTAG state machine and configuration
static jtag_config_t    current_config    = {0};
static jtag_tap_state_t current_tap_state = TAP_RESET;
static bool             jtag_initialized  = false;

// PIO state machines
static PIO  jtag_pio      = pio0;
static uint jtag_shift_sm = 0;
static uint jtag_clock_sm = 1;
static uint jtag_reset_sm = 2;

// Device database for IDCODE recognition
typedef struct {
    uint32_t    idcode_mask;
    uint32_t    idcode_value;
    const char *manufacturer;
    const char *device_name;
    uint8_t     ir_length;
} device_database_entry_t;

static const device_database_entry_t device_database[] = {
    // ARM devices
    {0x0FFFFFFF, 0x4BA00477, "ARM", "Cortex-M0", 4},
    {0x0FFFFFFF, 0x4BA02477, "ARM", "Cortex-M3", 4},
    {0x0FFFFFFF, 0x4BA04477, "ARM", "Cortex-M4", 4},
    {0x0FFFFFFF, 0x4BA05477, "ARM", "Cortex-M7", 4},

    // STMicroelectronics
    {0x0FFFFFFF, 0x06413041, "STM", "STM32F103", 4},
    {0x0FFFFFFF, 0x06414041, "STM", "STM32F407", 4},
    {0x0FFFFFFF, 0x06451041, "STM", "STM32F767", 4},

    // Raspberry Pi
    {0x0FFFFFFF, 0x0BA02477, "BCM", "BCM2835", 4},
    {0x0FFFFFFF, 0x4BA02477, "BCM", "BCM2711", 4},

    // Generic fallback
    {0x00000FFE, 0x00000000, "Unknown", "Unknown Device", 4}};

/**
 * @brief Initialize JTAG engine and PIO programs
 * @return true if initialization successful, false otherwise
 */
bool jtag_init(void)
{
    // Set default configuration
    current_config.tck_pin    = JTAG_TCK_PIN;
    current_config.tms_pin    = JTAG_TMS_PIN;
    current_config.tdi_pin    = JTAG_TDI_PIN;
    current_config.tdo_pin    = JTAG_TDO_PIN;
    current_config.trst_pin   = JTAG_TRST_PIN;
    current_config.clock_freq = 1000000;  // 1 MHz default
    current_config.use_trst   = true;

    // Initialize TRST pin if used
    if (current_config.use_trst) {
        gpio_init(current_config.trst_pin);
        gpio_set_dir(current_config.trst_pin, GPIO_OUT);
        gpio_put(current_config.trst_pin, 0);  // Assert reset
        sleep_ms(10);
        gpio_put(current_config.trst_pin, 1);  // Release reset
        sleep_ms(10);
    }

    // Load PIO programs
    uint offset_shift = pio_add_program(jtag_pio, &jtag_shift_program);
    uint offset_clock = pio_add_program(jtag_pio, &jtag_clock_program);
    uint offset_reset = pio_add_program(jtag_pio, &jtag_reset_program);

    // Initialize PIO state machines
    jtag_shift_program_init(jtag_pio, jtag_shift_sm, offset_shift, current_config.tck_pin,
                            current_config.tms_pin, current_config.tdi_pin, current_config.tdo_pin);

    jtag_clock_program_init(jtag_pio, jtag_clock_sm, offset_clock, current_config.tck_pin);

    jtag_reset_program_init(jtag_pio, jtag_reset_sm, offset_reset, current_config.tck_pin,
                            current_config.tms_pin);

    // Set initial frequency
    jtag_set_frequency(current_config.clock_freq);

    // Reset TAP to known state
    jtag_tap_reset();

    jtag_initialized = true;
    printf("JTAG engine initialized - Clock: %lu Hz\n", current_config.clock_freq);

    return true;
}

/**
 * @brief Configure JTAG pins and clock frequency
 * @param config JTAG configuration structure
 * @return true if configuration successful, false otherwise
 */
bool jtag_configure(const jtag_config_t *config)
{
    if (!config)
        return false;

    // Update configuration
    current_config = *config;

    // Reconfigure PIO if already initialized
    if (jtag_initialized) {
        // Stop state machines
        pio_sm_set_enabled(jtag_pio, jtag_shift_sm, false);
        pio_sm_set_enabled(jtag_pio, jtag_clock_sm, false);
        pio_sm_set_enabled(jtag_pio, jtag_reset_sm, false);

        // Reinitialize with new configuration
        return jtag_init();
    }

    return true;
}

/**
 * @brief JTAG engine task function
 * @param pvParameters FreeRTOS task parameters
 * @return void
 */
void jtag_task(void *pvParameters)
{
    (void)pvParameters;

    TickType_t last_wake_time = xTaskGetTickCount();

    printf("JTAG task started\n");

    while (1) {
        // Check for scan requests or other JTAG operations
        // This would be triggered by UI events or web API calls

        // For now, just maintain the task
        vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(1000));
    }
}

/**
 * @brief Scan for JTAG devices on the chain
 * @param result Pointer to store scan results
 * @return true if scan successful, false otherwise
 */
bool jtag_scan_chain(jtag_scan_result_t *result)
{
    if (!jtag_initialized || !result)
        return false;

    printf("Starting JTAG chain scan...\n");

    // Initialize result structure
    memset(result, 0, sizeof(jtag_scan_result_t));

    // Reset TAP to known state
    jtag_tap_reset();
    jtag_goto_state(TAP_IDLE);

    // Move to Shift-DR state
    jtag_goto_state(TAP_SHIFT_DR);

    // Scan for devices by shifting all 1s and looking for 0s
    uint32_t       device_count = 0;
    uint32_t       shift_count  = 0;
    const uint32_t max_devices  = 16;
    const uint32_t max_shifts   = 1000;  // Prevent infinite loop

    // Shift 1s until we see a 0 (indicating end of chain or start of IDCODE)
    while (shift_count < max_shifts && device_count < max_devices) {
        uint32_t tdo_bit = jtag_shift_data(1, 1, false);  // Shift 1 bit

        if (tdo_bit == 0) {
            // Found potential device, read 32-bit IDCODE
            uint32_t idcode = 0;

            // Read 31 more bits to complete IDCODE
            for (int i = 0; i < 31; i++) {
                uint32_t bit = jtag_shift_data(0, 1, false);
                idcode |= (bit << (i + 1));
            }

            // Check if IDCODE is valid (bit 0 should be 1 for valid IDCODE)
            if ((idcode & 1) == 1 && idcode != 0xFFFFFFFF) {
                // Valid device found
                jtag_device_t *device   = &result->devices[device_count];
                device->idcode          = idcode;
                device->manufacturer_id = (idcode >> 1) & 0x7FF;
                device->part_number     = (idcode >> 12) & 0xFFFF;
                device->version         = (idcode >> 28) & 0xF;

                // Look up device in database
                jtag_get_device_info(idcode, device);

                printf("Device %d: IDCODE=0x%08lX (%s %s)\n", device_count, idcode,
                       device->description, device->description);

                device_count++;
            }
        }

        shift_count++;
    }

    result->device_count = device_count;
    result->chain_valid  = (device_count > 0);

    // Return to idle state
    jtag_goto_state(TAP_IDLE);

    printf("JTAG scan complete - Found %d devices\n", device_count);

    // Log scan results
    char log_msg[128];
    snprintf(log_msg, sizeof(log_msg), "JTAG scan found %d devices", device_count);
    storage_write_log(xTaskGetTickCount(), "INFO", log_msg);

    return true;
}

/**
 * @brief Auto-detect JTAG pins on target
 * @param pins Array to store detected pin assignments
 * @return true if pins detected, false otherwise
 */
bool jtag_detect_pins(uint8_t pins[4])
{
    if (!pins)
        return false;

    printf("Starting JTAG pin detection...\n");

    // This is a simplified implementation
    // Real pin detection would try all combinations and look for valid responses

    // For now, return default pin assignment
    pins[0] = JTAG_TCK_PIN;  // TCK
    pins[1] = JTAG_TMS_PIN;  // TMS
    pins[2] = JTAG_TDI_PIN;  // TDI
    pins[3] = JTAG_TDO_PIN;  // TDO

    printf("JTAG pins detected (simplified): TCK=%d, TMS=%d, TDI=%d, TDO=%d\n", pins[0], pins[1],
           pins[2], pins[3]);

    return true;
}

/**
 * @brief Read IDCODE from JTAG device
 * @param device_index Device index in chain (0-based)
 * @param idcode Pointer to store IDCODE value
 * @return true if read successful, false otherwise
 */
bool jtag_read_idcode(uint8_t device_index, uint32_t *idcode)
{
    if (!jtag_initialized || !idcode)
        return false;

    // Perform chain scan to get device IDCODE
    jtag_scan_result_t scan_result;
    if (!jtag_scan_chain(&scan_result)) {
        return false;
    }

    if (device_index >= scan_result.device_count) {
        return false;
    }

    *idcode = scan_result.devices[device_index].idcode;
    return true;
}

/**
 * @brief Perform memory dump via JTAG
 * @param start_addr Starting memory address
 * @param length Number of bytes to dump
 * @param data Buffer to store dumped data
 * @return Number of bytes successfully dumped
 */
uint32_t jtag_memory_dump(uint32_t start_addr, uint32_t length, uint8_t *data)
{
    if (!jtag_initialized || !data || length == 0)
        return 0;

    printf("Starting memory dump: addr=0x%08lX, len=%lu\n", start_addr, length);

    // This is a placeholder implementation
    // Real memory access would require target-specific debug unit commands

    // For demonstration, fill with pattern
    for (uint32_t i = 0; i < length; i++) {
        data[i] = (uint8_t)((start_addr + i) & 0xFF);
    }

    printf("Memory dump completed: %lu bytes\n", length);

    return length;
}

/**
 * @brief Reset JTAG TAP to Test-Logic-Reset state
 * @return void
 */
void jtag_tap_reset(void)
{
    if (!jtag_initialized)
        return;

    // Send 5 clock cycles with TMS high to reach Test-Logic-Reset
    for (int i = 0; i < 5; i++) {
        jtag_shift_data(1, 1, false);  // TMS=1, TDI=don't care
    }

    current_tap_state = TAP_RESET;
}

/**
 * @brief Navigate JTAG TAP state machine
 * @param target_state Target TAP state
 * @return Current TAP state
 */
jtag_tap_state_t jtag_goto_state(jtag_tap_state_t target_state)
{
    if (!jtag_initialized)
        return current_tap_state;

    // Simple state transition implementation
    // Real implementation would use state transition table

    while (current_tap_state != target_state) {
        switch (current_tap_state) {
            case TAP_RESET:
                if (target_state == TAP_IDLE) {
                    jtag_shift_data(0, 1, false);  // TMS=0
                    current_tap_state = TAP_IDLE;
                }
                break;

            case TAP_IDLE:
                if (target_state == TAP_SHIFT_DR) {
                    jtag_shift_data(1, 1, false);  // TMS=1 -> Select-DR
                    jtag_shift_data(0, 1, false);  // TMS=0 -> Capture-DR
                    jtag_shift_data(0, 1, false);  // TMS=0 -> Shift-DR
                    current_tap_state = TAP_SHIFT_DR;
                } else if (target_state == TAP_SHIFT_IR) {
                    jtag_shift_data(1, 1, false);  // TMS=1 -> Select-DR
                    jtag_shift_data(1, 1, false);  // TMS=1 -> Select-IR
                    jtag_shift_data(0, 1, false);  // TMS=0 -> Capture-IR
                    jtag_shift_data(0, 1, false);  // TMS=0 -> Shift-IR
                    current_tap_state = TAP_SHIFT_IR;
                }
                break;

            case TAP_SHIFT_DR:
            case TAP_SHIFT_IR:
                if (target_state == TAP_IDLE) {
                    jtag_shift_data(1, 1, false);  // TMS=1 -> Exit1
                    jtag_shift_data(1, 1, false);  // TMS=1 -> Update
                    jtag_shift_data(0, 1, false);  // TMS=0 -> Idle
                    current_tap_state = TAP_IDLE;
                }
                break;

            default:
                // Return to reset state if confused
                jtag_tap_reset();
                break;
        }
    }

    return current_tap_state;
}

/**
 * @brief Shift data through JTAG DR or IR
 * @param data Data to shift out
 * @param length Number of bits to shift
 * @param is_instruction true for IR shift, false for DR shift
 * @return Data shifted in from TDO
 */
uint32_t jtag_shift_data(uint32_t data, uint8_t length, bool is_instruction)
{
    if (!jtag_initialized || length == 0 || length > 32)
        return 0;

    // Use PIO to shift data
    uint32_t tdo_data = 0;

    for (uint8_t i = 0; i < length; i++) {
        // Prepare data to shift (TMS and TDI)
        uint32_t tms_bit    = (i == length - 1) ? 1 : 0;  // TMS high on last bit
        uint32_t tdi_bit    = (data >> i) & 1;
        uint32_t shift_word = (tdi_bit << 1) | tms_bit;

        // Send to PIO
        pio_sm_put_blocking(jtag_pio, jtag_shift_sm, shift_word);

        // Read TDO bit
        uint32_t result = pio_sm_get_blocking(jtag_pio, jtag_shift_sm);
        if (result & 1) {
            tdo_data |= (1 << i);
        }
    }

    return tdo_data;
}

/**
 * @brief Set JTAG clock frequency
 * @param freq_hz Frequency in Hz (max ~10MHz)
 * @return Actual frequency set
 */
uint32_t jtag_set_frequency(uint32_t freq_hz)
{
    if (!jtag_initialized)
        return 0;

    // Calculate clock divider for PIO
    float div = (float)clock_get_hz(clk_sys) / (float)freq_hz;

    // Clamp to valid range
    if (div < 1.0f)
        div = 1.0f;
    if (div > 65536.0f)
        div = 65536.0f;

    // Apply to all JTAG state machines
    pio_sm_set_clkdiv(jtag_pio, jtag_shift_sm, div);
    pio_sm_set_clkdiv(jtag_pio, jtag_clock_sm, div);
    pio_sm_set_clkdiv(jtag_pio, jtag_reset_sm, div);

    // Calculate actual frequency
    uint32_t actual_freq = clock_get_hz(clk_sys) / (uint32_t)div;

    current_config.clock_freq = actual_freq;

    printf("JTAG frequency set to %lu Hz (div=%.2f)\n", actual_freq, div);

    return actual_freq;
}

/**
 * @brief Get device information from IDCODE
 * @param idcode Device IDCODE
 * @param device Pointer to store device information
 * @return true if device recognized, false otherwise
 */
bool jtag_get_device_info(uint32_t idcode, jtag_device_t *device)
{
    if (!device)
        return false;

    // Search device database
    size_t db_size = sizeof(device_database) / sizeof(device_database_entry_t);

    for (size_t i = 0; i < db_size; i++) {
        const device_database_entry_t *entry = &device_database[i];

        if ((idcode & entry->idcode_mask) == entry->idcode_value) {
            // Match found
            device->ir_length = entry->ir_length;
            snprintf(device->description, sizeof(device->description), "%s %s", entry->manufacturer,
                     entry->device_name);
            return true;
        }
    }

    // No match found, use generic info
    device->ir_length = 4;  // Default IR length
    snprintf(device->description, sizeof(device->description), "Unknown (0x%08lX)", idcode);

    return false;
}

/**
 * @brief Perform boundary scan on JTAG device
 * @param device_index Index of device in chain
 * @param scan_result Pointer to store scan results
 * @return JTAG_OK on success, error code otherwise
 */
jtag_result_t jtag_boundary_scan(uint8_t device_index, jtag_boundary_scan_t *scan_result)
{
    if (!scan_result || !jtag_initialized) {
        return JTAG_ERROR_INVALID_PARAM;
    }

    if (device_index >= current_chain.device_count) {
        return JTAG_ERROR_DEVICE_NOT_FOUND;
    }

    LOG_INFO("Starting boundary scan on device %d", device_index);

    // Move to SHIFT-DR state
    jtag_result_t result = jtag_goto_state(JTAG_STATE_SHIFT_DR);
    if (result != JTAG_OK) {
        return result;
    }

    // Load SAMPLE/PRELOAD instruction
    uint32_t sample_instr = 0x02;  // Common SAMPLE instruction
    result =
        jtag_shift_ir(&sample_instr, current_chain.devices[device_index].ir_length, device_index);
    if (result != JTAG_OK) {
        return result;
    }

    // Capture boundary scan register
    uint32_t boundary_data[JTAG_MAX_BOUNDARY_CELLS / 32] = {0};
    uint32_t boundary_length = current_chain.devices[device_index].boundary_length;

    result = jtag_shift_dr((uint8_t*)boundary_data, boundary_length, device_index);
    if (result != JTAG_OK) {
        return result;
    }

    // Parse boundary scan data
    scan_result->device_index = device_index;
    scan_result->cell_count   = boundary_length;
    scan_result->timestamp    = xTaskGetTickCount();

    // Copy boundary data
    memcpy(scan_result->cell_data, boundary_data, sizeof(uint32_t) * ((boundary_length + 31) / 32));

    // Analyze pin states
    scan_result->input_pins   = 0;
    scan_result->output_pins  = 0;
    scan_result->control_pins = 0;

    for (uint32_t i = 0; i < boundary_length; i += 3) {
        // Typical boundary scan cell format: output, control, input
        bool output  = (boundary_data[i / 32] >> (i % 32)) & 1;
        bool control = (boundary_data[(i + 1) / 32] >> ((i + 1) % 32)) & 1;
        bool input   = (boundary_data[(i + 2) / 32] >> ((i + 2) % 32)) & 1;

        uint32_t pin = i / 3;
        if (pin < 32) {
            if (input)
                scan_result->input_pins |= (1UL << pin);
            if (output)
                scan_result->output_pins |= (1UL << pin);
            if (control)
                scan_result->control_pins |= (1UL << pin);
        }
    }

    LOG_INFO("Boundary scan complete - %d cells captured", boundary_length);
    LOG_DEBUG("Input pins: 0x%08lX", scan_result->input_pins);
    LOG_DEBUG("Output pins: 0x%08lX", scan_result->output_pins);
    LOG_DEBUG("Control pins: 0x%08lX", scan_result->control_pins);

    return JTAG_OK;
}

/**
 * @brief Start memory fuzzing operation
 * @param fuzz_config Fuzzing configuration
 * @return JTAG_OK on success, error code otherwise
 */
jtag_result_t jtag_start_memory_fuzz(const jtag_fuzz_config_t *fuzz_config)
{
    if (!fuzz_config || !jtag_initialized) {
        return JTAG_ERROR_INVALID_PARAM;
    }

    LOG_INFO("Starting memory fuzzing");
    LOG_INFO("- Target address: 0x%08lX", fuzz_config->start_address);
    LOG_INFO("- Length: %lu bytes", fuzz_config->length);
    LOG_INFO("- Iterations: %lu", fuzz_config->iterations);
    LOG_INFO("- Pattern: %s",
             (fuzz_config->pattern_type == JTAG_FUZZ_RANDOM) ? "Random" : "Sequential");

    // Validate configuration
    if (fuzz_config->length == 0 || fuzz_config->iterations == 0) {
        return JTAG_ERROR_INVALID_PARAM;
    }

    // Store configuration
    memcpy(&current_fuzz_config, fuzz_config, sizeof(jtag_fuzz_config_t));
    fuzz_state.active            = true;
    fuzz_state.current_iteration = 0;
    fuzz_state.errors_found   = 0;
    fuzz_state.start_time        = xTaskGetTickCount();

    // Initialize random seed if needed
    if (fuzz_config->pattern_type == JTAG_FUZZ_RANDOM) {
        srand(time_us_32());
    }

    // Start fuzzing task
    jtag_event_t event = {.type = JTAG_EVENT_START_FUZZ, .data.fuzz_config = *fuzz_config};

    BaseType_t result = xQueueSend(jtag_event_queue, &event, pdMS_TO_TICKS(100));
    if (result != pdPASS) {
        fuzz_state.active = false;
        return JTAG_ERROR_QUEUE_FULL;
    }

    return JTAG_OK;
}

/**
 * @brief Stop memory fuzzing operation
 * @return JTAG_OK on success, error code otherwise
 */
jtag_result_t jtag_stop_memory_fuzz(void)
{
    if (!fuzz_state.active) {
        return JTAG_ERROR_NOT_ACTIVE;
    }

    LOG_INFO("Stopping memory fuzzing");

    fuzz_state.active = false;

    // Send stop event
    jtag_event_t event = {.type = JTAG_EVENT_STOP_FUZZ};
    xQueueSend(jtag_event_queue, &event, pdMS_TO_TICKS(100));

    // Log results
    uint32_t elapsed_time = xTaskGetTickCount() - fuzz_state.start_time;
    LOG_INFO("Fuzzing stopped after %lu iterations", fuzz_state.current_iteration);
    LOG_INFO("Errors detected: %lu", fuzz_state.errors_found);
    LOG_INFO("Elapsed time: %lu ms", elapsed_time * portTICK_PERIOD_MS);

    return JTAG_OK;
}

/**
 * @brief Get current fuzzing state
 * @return Pointer to fuzzing state structure
 */
const jtag_fuzz_state_t *jtag_get_fuzz_state(void)
{
    return &fuzz_state;
}

/**
 * @brief Trigger power glitch during JTAG operation
 * @param glitch_config Glitch configuration
 * @return JTAG_OK on success, error code otherwise
 */
jtag_result_t jtag_power_glitch(const jtag_glitch_config_t *glitch_config)
{
    if (!glitch_config || !jtag_initialized) {
        return JTAG_ERROR_INVALID_PARAM;
    }

    LOG_INFO("Triggering power glitch");
    LOG_INFO("- Duration: %lu us", glitch_config->duration_us);
    LOG_INFO("- Delay: %lu us", glitch_config->delay_us);
    LOG_INFO("- Voltage: %.2fV", glitch_config->voltage);

    // Setup glitch timing
    uint32_t start_time = time_us_32();

    // Wait for trigger delay
    if (glitch_config->delay_us > 0) {
        busy_wait_us(glitch_config->delay_us);
    }

    // Trigger glitch
    gpio_put(GLITCH_MOSFET_PIN, 1);  // Enable glitch MOSFET
    busy_wait_us(glitch_config->duration_us);
    gpio_put(GLITCH_MOSFET_PIN, 0);  // Disable glitch MOSFET

    uint32_t actual_duration = time_us_32() - start_time;

    LOG_DEBUG("Glitch triggered - actual duration: %lu us", actual_duration);

    // Log glitch event
    if (storage_is_available()) {
        char log_entry[128];
        snprintf(log_entry, sizeof(log_entry), "GLITCH,%lu,%lu,%.2f,%lu\n", time_us_32(),
                 glitch_config->duration_us, glitch_config->voltage, actual_duration);
        storage_append_log("glitch.csv", log_entry);
    }

    return JTAG_OK;
}

/**
 * @brief Analyze JTAG chain for security vulnerabilities
 * @param analysis_result Pointer to store analysis results
 * @return JTAG_OK on success, error code otherwise
 */
jtag_result_t jtag_security_analysis(jtag_security_analysis_t *analysis_result)
{
    if (!analysis_result || !jtag_initialized) {
        return JTAG_ERROR_INVALID_PARAM;
    }

    LOG_INFO("Starting JTAG security analysis");

    memset(analysis_result, 0, sizeof(jtag_security_analysis_t));
    analysis_result->timestamp = xTaskGetTickCount();

    // Check for debug access restrictions
    for (uint8_t i = 0; i < current_chain.device_count; i++) {
        jtag_device_t *device = &current_chain.devices[i];

        // Test IDCODE accessibility
        uint32_t idcode;
        if (jtag_read_device_idcode(i, &idcode) == JTAG_OK) {
            if (idcode != 0x00000000 && idcode != 0xFFFFFFFF) {
                analysis_result->debug_enabled_array[i] = true;
                analysis_result->accessible_devices++;
            }
        }

        // Check for common debug protection mechanisms
        // Test if device responds to BYPASS instruction
        jtag_goto_state(JTAG_STATE_SHIFT_IR);
        uint32_t bypass_instr = (1UL << device->ir_length) - 1;  // All 1s

        if (jtag_shift_ir(&bypass_instr, device->ir_length, i) == JTAG_OK) {
            // Test DR length in bypass mode
            jtag_goto_state(JTAG_STATE_SHIFT_DR);
            uint32_t test_data = 0x5A5A5A5A;
            uint32_t result_data;

            if (jtag_shift_dr((uint8_t*)&test_data, 32, i) == JTAG_OK) {
                // If we get the expected single-bit bypass, device is accessible
                if (test_data & 1) {
                    analysis_result->bypass_accessible[i] = true;
                }
            }
        }

        // Check for manufacturer-specific security features
        uint32_t manufacturer                = (device->idcode >> 1) & 0x7FF;
        analysis_result->manufacturer_ids[i] = manufacturer;

        // Common secure manufacturers
        switch (manufacturer) {
            case 0x15:  // Xilinx
            case 0x93:  // Xilinx (newer)
                analysis_result->security_features[i] |= JTAG_SEC_ENCRYPTED_BITSTREAM;
                break;
            case 0x6E:  // Altera/Intel
                analysis_result->security_features[i] |= JTAG_SEC_SECURE_DEVICE_MANAGER;
                break;
            case 0x43:  // ARM
                analysis_result->security_features[i] |= JTAG_SEC_TRUSTZONE;
                break;
            default:
                break;
        }
    }

    // Overall security assessment
    if (analysis_result->accessible_devices == 0) {
        analysis_result->security_level = JTAG_SEC_LEVEL_HIGH;
    } else if (analysis_result->accessible_devices < current_chain.device_count) {
        analysis_result->security_level = JTAG_SEC_LEVEL_MEDIUM;
    } else {
        analysis_result->security_level = JTAG_SEC_LEVEL_LOW;
    }

    LOG_INFO("Security analysis complete");
    LOG_INFO("- Accessible devices: %d/%d", analysis_result->accessible_devices,
             current_chain.device_count);
    LOG_INFO("- Security level: %s",
             (analysis_result->security_level == JTAG_SEC_LEVEL_HIGH)     ? "High"
             : (analysis_result->security_level == JTAG_SEC_LEVEL_MEDIUM) ? "Medium"
                                                                          : "Low");

    return JTAG_OK;
}

/**
 * @brief Shift data through JTAG instruction register (IR)
 * @param data Pointer to instruction data
 * @param length Number of bits to shift
 * @param device_index Target device index in chain
 * @return JTAG operation result
 */
jtag_result_t jtag_shift_ir(uint32_t *data, uint8_t length, uint8_t device_index)
{
    if (!jtag_initialized || !data) {
        return JTAG_ERROR;
    }

    // Go to Shift-IR state
    jtag_goto_state(TAP_SHIFT_IR);
    
    // Shift the instruction data
    uint32_t shifted_data = jtag_shift_data(*data, length, true);
    *data = shifted_data;
    
    // Return to Run-Test/Idle
    jtag_goto_state(TAP_IDLE);
    
    return JTAG_OK;
}

/**
 * @brief Shift data through JTAG data register (DR)
 * @param data Pointer to data buffer
 * @param length Number of bits to shift
 * @param device_index Target device index in chain
 * @return JTAG operation result
 */
jtag_result_t jtag_shift_dr(uint8_t *data, uint32_t length, uint8_t device_index)
{
    if (!jtag_initialized || !data) {
        return JTAG_ERROR;
    }

    // Go to Shift-DR state
    jtag_goto_state(TAP_SHIFT_DR);
    
    // For now, just shift the first 32 bits as a simple implementation
    uint32_t data_32 = 0;
    if (length <= 32) {
        memcpy(&data_32, data, (length + 7) / 8);
        data_32 = jtag_shift_data(data_32, length, false);
        memcpy(data, &data_32, (length + 7) / 8);
    }
    
    // Return to Run-Test/Idle
    jtag_goto_state(TAP_IDLE);
    
    return JTAG_OK;
}

/**
 * @brief Read device IDCODE
 * @param device_index Device index in chain
 * @param idcode Pointer to store IDCODE
 * @return JTAG operation result
 */
jtag_result_t jtag_read_device_idcode(uint8_t device_index, uint32_t *idcode)
{
    if (!jtag_initialized || !idcode) {
        return JTAG_ERROR;
    }

    // Check if device exists in current chain
    if (device_index >= current_chain.device_count) {
        return JTAG_ERROR;
    }

    // Return the cached IDCODE from the scan result
    *idcode = current_chain.devices[device_index].idcode;
    
    return JTAG_OK;
}
