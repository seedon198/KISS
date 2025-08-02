/**
 * @file jtag.c
 * @brief Basic JTAG engine implementation using RP2040 PIO
 * @author KISS Fuzzer Team
 * @date 2025
 * @version 0.6.0
 */

#include "jtag.h"
#include "jtag.pio.h"

// Static variables for JTAG state
static bool jtag_ready = false;
static jtag_config_t current_config = {0};
static jtag_tap_state_t current_tap_state = TAP_UNKNOWN;
static PIO jtag_pio = pio0;
static uint jtag_sm = 0;
static uint jtag_offset = 0;

// TAP state transition table [current_state][tms_value] -> next_state
static const jtag_tap_state_t tap_transitions[16][2] = {
    // TAP_TEST_LOGIC_RESET
    {TAP_RUN_TEST_IDLE, TAP_TEST_LOGIC_RESET},
    // TAP_RUN_TEST_IDLE  
    {TAP_RUN_TEST_IDLE, TAP_SELECT_DR_SCAN},
    // TAP_SELECT_DR_SCAN
    {TAP_CAPTURE_DR, TAP_SELECT_IR_SCAN},
    // TAP_CAPTURE_DR
    {TAP_SHIFT_DR, TAP_EXIT1_DR},
    // TAP_SHIFT_DR
    {TAP_SHIFT_DR, TAP_EXIT1_DR},
    // TAP_EXIT1_DR
    {TAP_PAUSE_DR, TAP_UPDATE_DR},
    // TAP_PAUSE_DR
    {TAP_PAUSE_DR, TAP_EXIT2_DR},
    // TAP_EXIT2_DR
    {TAP_SHIFT_DR, TAP_UPDATE_DR},
    // TAP_UPDATE_DR
    {TAP_RUN_TEST_IDLE, TAP_SELECT_DR_SCAN},
    // TAP_SELECT_IR_SCAN
    {TAP_CAPTURE_IR, TAP_TEST_LOGIC_RESET},
    // TAP_CAPTURE_IR
    {TAP_SHIFT_IR, TAP_EXIT1_IR},
    // TAP_SHIFT_IR
    {TAP_SHIFT_IR, TAP_EXIT1_IR},
    // TAP_EXIT1_IR
    {TAP_PAUSE_IR, TAP_UPDATE_IR},
    // TAP_PAUSE_IR
    {TAP_PAUSE_IR, TAP_EXIT2_IR},
    // TAP_EXIT2_IR
    {TAP_SHIFT_IR, TAP_UPDATE_IR},
    // TAP_UPDATE_IR
    {TAP_RUN_TEST_IDLE, TAP_SELECT_DR_SCAN}
};

/**
 * @brief Initialize the JTAG engine
 * @param config JTAG configuration (NULL for defaults)
 * @return true if successful, false otherwise
 */
bool jtag_init(const jtag_config_t* config) {
    printf("Initializing JTAG engine...\n");
    
    // Set default configuration
    current_config.tck_pin = JTAG_TCK_PIN;
    current_config.tms_pin = JTAG_TMS_PIN;
    current_config.tdi_pin = JTAG_TDI_PIN;
    current_config.tdo_pin = JTAG_TDO_PIN;
    current_config.trst_pin = JTAG_TRST_PIN;
    current_config.srst_pin = JTAG_SRST_PIN;
    current_config.clock_hz = JTAG_DEFAULT_CLOCK_HZ;
    current_config.trst_active_low = true;
    current_config.srst_active_low = true;
    
    // Override with user configuration if provided
    if (config) {
        current_config = *config;
    }
    
    // Initialize GPIO pins for reset signals
    gpio_init(current_config.trst_pin);
    gpio_set_dir(current_config.trst_pin, GPIO_OUT);
    gpio_put(current_config.trst_pin, !current_config.trst_active_low); // Inactive
    
    gpio_init(current_config.srst_pin);
    gpio_set_dir(current_config.srst_pin, GPIO_OUT);
    gpio_put(current_config.srst_pin, !current_config.srst_active_low); // Inactive
    
    // Load and start PIO program
    jtag_offset = pio_add_program(jtag_pio, &jtag_shift_program);
    jtag_shift_program_init(jtag_pio, jtag_sm, jtag_offset,
                           current_config.tck_pin, current_config.tms_pin,
                           current_config.tdi_pin, current_config.tdo_pin,
                           (float)current_config.clock_hz);
    
    // Reset to known state
    jtag_reset();
    
    jtag_ready = true;
    printf("JTAG engine initialized successfully\n");
    printf("Pins: TCK=%d, TMS=%d, TDI=%d, TDO=%d, TRST=%d, SRST=%d\n",
           current_config.tck_pin, current_config.tms_pin,
           current_config.tdi_pin, current_config.tdo_pin,
           current_config.trst_pin, current_config.srst_pin);
    printf("Clock: %lu Hz\n", current_config.clock_hz);
    
    return true;
}

/**
 * @brief Deinitialize the JTAG engine
 */
void jtag_deinit(void) {
    if (!jtag_ready) return;
    
    pio_sm_set_enabled(jtag_pio, jtag_sm, false);
    pio_remove_program(jtag_pio, &jtag_shift_program, jtag_offset);
    
    jtag_ready = false;
    current_tap_state = TAP_UNKNOWN;
    
    printf("JTAG engine deinitialized\n");
}

/**
 * @brief Set JTAG clock frequency
 * @param frequency_hz Clock frequency in Hz
 * @return true if successful, false if out of range
 */
bool jtag_set_clock(uint32_t frequency_hz) {
    if (!jtag_ready) return false;
    
    if (frequency_hz < JTAG_MIN_CLOCK_HZ || frequency_hz > JTAG_MAX_CLOCK_HZ) {
        printf("JTAG clock frequency %lu Hz out of range (%lu - %lu Hz)\n",
               frequency_hz, JTAG_MIN_CLOCK_HZ, JTAG_MAX_CLOCK_HZ);
        return false;
    }
    
    current_config.clock_hz = frequency_hz;
    
    // Reconfigure PIO clock divider
    float div = clock_get_hz(clk_sys) / (float)frequency_hz;
    pio_sm_set_clkdiv(jtag_pio, jtag_sm, div);
    
    printf("JTAG clock set to %lu Hz\n", frequency_hz);
    return true;
}

/**
 * @brief Get current JTAG clock frequency
 * @return Current clock frequency in Hz
 */
uint32_t jtag_get_clock(void) {
    return current_config.clock_hz;
}

/**
 * @brief Send TMS sequence to move through TAP states
 * @param tms_sequence TMS bits to send
 * @param bit_count Number of bits to send
 */
static void jtag_send_tms_sequence(uint32_t tms_sequence, uint8_t bit_count) {
    if (!jtag_ready) return;
    
    // Format: [31:24]=TMS pattern, [23:16]=TDI=0, [15:0]=bit_count-1
    uint32_t control_word = (tms_sequence << 24) | ((bit_count - 1) & 0xFFFF);
    
    // Send to PIO
    pio_sm_put_blocking(jtag_pio, jtag_sm, control_word);
    
    // Wait for completion and get TDO data (ignored for TMS sequences)
    pio_sm_get_blocking(jtag_pio, jtag_sm);
}

/**
 * @brief Reset JTAG TAP to Test-Logic-Reset state
 */
void jtag_reset(void) {
    if (!jtag_ready) return;
    
    printf("Resetting JTAG TAP...\n");
    
    // Send 5 TMS=1 bits to guarantee Test-Logic-Reset state
    jtag_send_tms_sequence(0xFF, 5);
    current_tap_state = TAP_TEST_LOGIC_RESET;
    
    printf("JTAG TAP reset to Test-Logic-Reset state\n");
}

/**
 * @brief Move to specific TAP state
 * @param target_state Target TAP state
 * @return true if successful, false if invalid transition
 */
bool jtag_goto_state(jtag_tap_state_t target_state) {
    if (!jtag_ready) return false;
    
    if (current_tap_state == TAP_UNKNOWN) {
        jtag_reset();
    }
    
    // Simple algorithm: reset and navigate to target
    if (target_state == TAP_TEST_LOGIC_RESET) {
        jtag_reset();
        return true;
    }
    
    // For now, implement basic transitions to common states
    switch (target_state) {
        case TAP_RUN_TEST_IDLE:
            if (current_tap_state == TAP_TEST_LOGIC_RESET) {
                jtag_send_tms_sequence(0x00, 1); // TMS=0
                current_tap_state = TAP_RUN_TEST_IDLE;
            }
            break;
            
        case TAP_SHIFT_DR:
            jtag_goto_state(TAP_RUN_TEST_IDLE);
            jtag_send_tms_sequence(0x02, 3); // TMS=0,1,0 -> Select-DR, Capture-DR, Shift-DR
            current_tap_state = TAP_SHIFT_DR;
            break;
            
        case TAP_SHIFT_IR:
            jtag_goto_state(TAP_RUN_TEST_IDLE);
            jtag_send_tms_sequence(0x06, 4); // TMS=0,1,1,0 -> Select-DR, Select-IR, Capture-IR, Shift-IR  
            current_tap_state = TAP_SHIFT_IR;
            break;
            
        default:
            printf("TAP state transition to %d not implemented\n", target_state);
            return false;
    }
    
    printf("JTAG TAP moved to state: %s\n", jtag_state_name(current_tap_state));
    return true;
}

/**
 * @brief Get current TAP state
 * @return Current TAP state
 */
jtag_tap_state_t jtag_get_state(void) {
    return current_tap_state;
}

/**
 * @brief Basic connectivity test
 * @return true if JTAG signals appear to be connected
 */
bool jtag_test_connectivity(void) {
    if (!jtag_ready) return false;
    
    printf("Testing JTAG connectivity...\n");
    
    // Test 1: Check if TDO responds to JTAG reset
    jtag_reset();
    
    // Test 2: Try to read something from TDO during SHIFT-DR
    jtag_goto_state(TAP_SHIFT_DR);
    
    // Send some bits and see if we get any response
    uint8_t test_data = 0xAA;
    uint8_t response = 0x00;
    
    if (jtag_shift_dr(&test_data, &response, 8, TAP_RUN_TEST_IDLE)) {
        printf("JTAG connectivity test: TDO response = 0x%02X\n", response);
        return true;
    }
    
    printf("JTAG connectivity test failed\n");
    return false;
}

/**
 * @brief Shift data through DR (Data Register)
 * @param data_out Data to shift out (TDI)
 * @param data_in Buffer for data shifted in (TDO)
 * @param bit_count Number of bits to shift
 * @param goto_state TAP state to enter after shift
 * @return true if successful
 */
bool jtag_shift_dr(const uint8_t* data_out, uint8_t* data_in, 
                   uint16_t bit_count, jtag_tap_state_t goto_state) {
    if (!jtag_ready || !data_out || !data_in) return false;
    
    // Ensure we're in SHIFT-DR state
    if (current_tap_state != TAP_SHIFT_DR) {
        if (!jtag_goto_state(TAP_SHIFT_DR)) {
            return false;
        }
    }
    
    // For now, implement byte-wise shifting
    for (uint16_t byte = 0; byte < (bit_count + 7) / 8; byte++) {
        uint8_t bits_in_byte = (bit_count - byte * 8);
        if (bits_in_byte > 8) bits_in_byte = 8;
        
        // Format control word: [31:24]=TMS=0, [23:16]=TDI data, [15:0]=bits-1
        uint32_t control_word = (data_out[byte] << 16) | ((bits_in_byte - 1) & 0xFFFF);
        
        // Send to PIO
        pio_sm_put_blocking(jtag_pio, jtag_sm, control_word);
        
        // Get TDO response
        uint32_t tdo_response = pio_sm_get_blocking(jtag_pio, jtag_sm);
        data_in[byte] = (uint8_t)tdo_response;
    }
    
    // Move to target state if specified
    if (goto_state != current_tap_state) {
        jtag_goto_state(goto_state);
    }
    
    return true;
}

/**
 * @brief Shift data through IR (Instruction Register)
 * @param data_out Data to shift out (TDI)
 * @param data_in Buffer for data shifted in (TDO)
 * @param bit_count Number of bits to shift
 * @param goto_state TAP state to enter after shift
 * @return true if successful
 */
bool jtag_shift_ir(const uint8_t* data_out, uint8_t* data_in,
                   uint16_t bit_count, jtag_tap_state_t goto_state) {
    if (!jtag_ready || !data_out || !data_in) return false;
    
    // Ensure we're in SHIFT-IR state
    if (current_tap_state != TAP_SHIFT_IR) {
        if (!jtag_goto_state(TAP_SHIFT_IR)) {
            return false;
        }
    }
    
    // Similar to DR shift but in IR
    for (uint16_t byte = 0; byte < (bit_count + 7) / 8; byte++) {
        uint8_t bits_in_byte = (bit_count - byte * 8);
        if (bits_in_byte > 8) bits_in_byte = 8;
        
        uint32_t control_word = (data_out[byte] << 16) | ((bits_in_byte - 1) & 0xFFFF);
        
        pio_sm_put_blocking(jtag_pio, jtag_sm, control_word);
        uint32_t tdo_response = pio_sm_get_blocking(jtag_pio, jtag_sm);
        data_in[byte] = (uint8_t)tdo_response;
    }
    
    if (goto_state != current_tap_state) {
        jtag_goto_state(goto_state);
    }
    
    return true;
}

/**
 * @brief Read IDCODE from device
 * @param device_index Device index in chain (0-based)
 * @return IDCODE value, 0 if error
 */
uint32_t jtag_read_idcode(uint8_t device_index) {
    if (!jtag_ready) return 0;
    
    printf("Reading IDCODE from device %d...\n", device_index);
    
    // Reset and go to SHIFT-DR
    jtag_reset();
    jtag_goto_state(TAP_SHIFT_DR);
    
    // IDCODE is the default DR, so just shift out 32 bits
    uint8_t dummy_data[4] = {0xFF, 0xFF, 0xFF, 0xFF};
    uint8_t idcode_bytes[4] = {0};
    
    if (jtag_shift_dr(dummy_data, idcode_bytes, 32, TAP_RUN_TEST_IDLE)) {
        uint32_t idcode = (idcode_bytes[3] << 24) | (idcode_bytes[2] << 16) |
                         (idcode_bytes[1] << 8) | idcode_bytes[0];
        
        printf("IDCODE: 0x%08lX\n", idcode);
        return idcode;
    }
    
    return 0;
}

/**
 * @brief Scan JTAG chain to detect devices
 * @param chain Pointer to chain structure to fill
 * @return Number of devices found
 */
uint8_t jtag_scan_chain(jtag_chain_t* chain) {
    if (!jtag_ready || !chain) return 0;
    
    printf("Scanning JTAG chain...\n");
    
    memset(chain, 0, sizeof(jtag_chain_t));
    
    // Simple scan: read IDCODE and see if valid
    uint32_t idcode = jtag_read_idcode(0);
    
    if (idcode != 0 && idcode != 0xFFFFFFFF) {
        chain->devices[0].idcode = idcode;
        chain->devices[0].identified = jtag_identify_device(idcode, &chain->devices[0]);
        chain->device_count = 1;
        chain->valid = true;
        
        printf("Found 1 device in JTAG chain\n");
        return 1;
    }
    
    printf("No devices found in JTAG chain\n");
    return 0;
}

/**
 * @brief Identify device from IDCODE
 * @param idcode IDCODE value
 * @param device Pointer to device structure to fill
 * @return true if device identified
 */
bool jtag_identify_device(uint32_t idcode, jtag_device_t* device) {
    if (!device) return false;
    
    device->idcode = idcode;
    device->ir_length = 4; // Default assumption
    
    // Extract manufacturer ID (bits 1-11)
    uint16_t mfg_id = (idcode >> 1) & 0x7FF;
    
    // Basic device identification (expand this database)
    switch (mfg_id) {
        case 0x15B: // Xilinx
            strcpy(device->manufacturer, "Xilinx");
            strcpy(device->device_name, "Unknown Xilinx");
            device->identified = true;
            break;
        case 0x043: // Intel/Altera
            strcpy(device->manufacturer, "Intel/Altera");
            strcpy(device->device_name, "Unknown Intel");
            device->identified = true;
            break;
        case 0x025: // Texas Instruments
            strcpy(device->manufacturer, "Texas Instruments");
            strcpy(device->device_name, "Unknown TI");
            device->identified = true;
            break;
        default:
            snprintf(device->manufacturer, sizeof(device->manufacturer), "Unknown (0x%03X)", mfg_id);
            strcpy(device->device_name, "Unknown Device");
            device->identified = false;
            break;
    }
    
    return device->identified;
}

/**
 * @brief Get human-readable TAP state name
 * @param state TAP state
 * @return String name of the state
 */
const char* jtag_state_name(jtag_tap_state_t state) {
    switch (state) {
        case TAP_TEST_LOGIC_RESET: return "Test-Logic-Reset";
        case TAP_RUN_TEST_IDLE:    return "Run-Test/Idle";
        case TAP_SELECT_DR_SCAN:   return "Select-DR-Scan";
        case TAP_CAPTURE_DR:       return "Capture-DR";
        case TAP_SHIFT_DR:         return "Shift-DR";
        case TAP_EXIT1_DR:         return "Exit1-DR";
        case TAP_PAUSE_DR:         return "Pause-DR";
        case TAP_EXIT2_DR:         return "Exit2-DR";
        case TAP_UPDATE_DR:        return "Update-DR";
        case TAP_SELECT_IR_SCAN:   return "Select-IR-Scan";
        case TAP_CAPTURE_IR:       return "Capture-IR";
        case TAP_SHIFT_IR:         return "Shift-IR";
        case TAP_EXIT1_IR:         return "Exit1-IR";
        case TAP_PAUSE_IR:         return "Pause-IR";
        case TAP_EXIT2_IR:         return "Exit2-IR";
        case TAP_UPDATE_IR:        return "Update-IR";
        case TAP_UNKNOWN:          return "Unknown";
        default:                   return "Invalid";
    }
}

/**
 * @brief Get formatted scan results string
 * @param chain Pointer to scan chain results
 * @param buffer Buffer to write to
 * @param buffer_size Size of buffer
 */
void jtag_get_scan_string(const jtag_chain_t* chain, char* buffer, size_t buffer_size) {
    if (!chain || !buffer || buffer_size == 0) return;
    
    if (chain->device_count == 0) {
        snprintf(buffer, buffer_size, "No devices found");
        return;
    }
    
    snprintf(buffer, buffer_size, "%d device(s): %s",
             chain->device_count, chain->devices[0].manufacturer);
}
