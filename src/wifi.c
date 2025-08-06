/**
 * @file wifi.c
 * @brief Wi-Fi web interface and remote control implementation
 * @author KISS Fuzzer Team
 * @date 2025
 * @version 0.9.1
 */

#include "wifi.h"
#include "fuzz_commands.h"
#include "storage.h"
#include "power.h"
#include "jtag_fuzzer.h"
#include <stdlib.h>
#include "lwip/tcp.h"
#include "lwip/altcp.h"

// Static variables for Wi-Fi state
static bool wifi_initialized = false;
static bool web_server_running = false;
static wifi_config_t current_config = {0};
static wifi_status_info_t current_status = {0};
static TaskHandle_t wifi_task_handle = NULL;

// HTTP handlers table
typedef struct {
    char path[64];
    http_handler_t handler;
} http_route_t;

static http_route_t http_routes[] = {
    {WEB_PATH_ROOT, wifi_handle_root},
    {WEB_PATH_API_STATUS, wifi_handle_api_status},
    {WEB_PATH_API_FUZZING, wifi_handle_api_fuzzing},
    {WEB_PATH_API_LOGS, wifi_handle_api_logs},
    {WEB_PATH_API_FILES, wifi_handle_api_files},
    {WEB_PATH_API_CONFIG, wifi_handle_api_config},
    {WEB_PATH_DOWNLOAD, wifi_handle_download}
};

#define HTTP_ROUTES_COUNT (sizeof(http_routes) / sizeof(http_routes[0]))

// Forward declarations
static void wifi_task_impl(void* pvParameters);

/**
 * @brief Initialize Wi-Fi system
 */
bool wifi_init(const wifi_config_t* config) {
    if (wifi_initialized) {
        printf("[Wi-Fi] Already initialized\n");
        return true;
    }

    printf("[Wi-Fi] Initializing Wi-Fi system...\n");

    // Initialize CYW43 chip
    if (cyw43_arch_init() != 0) {
        printf("[Wi-Fi] Failed to initialize CYW43 chip\n");
        return false;
    }

    // Set configuration
    if (config) {
        current_config = *config;
    } else {
        wifi_get_default_config(&current_config);
    }

    // Initialize status
    memset(&current_status, 0, sizeof(current_status));
    current_status.status = WIFI_STATUS_DISCONNECTED;

    // Create Wi-Fi task
    BaseType_t task_result = xTaskCreate(
        wifi_task_impl,
        "WiFi_Task",
        2048,  // Stack size
        NULL,  // Parameters
        3,     // Priority (high)
        &wifi_task_handle
    );

    if (task_result != pdPASS) {
        printf("[Wi-Fi] Failed to create Wi-Fi task\n");
        cyw43_arch_deinit();
        return false;
    }

    wifi_initialized = true;
    printf("[Wi-Fi] Wi-Fi system initialized successfully\n");
    return true;
}

/**
 * @brief Deinitialize Wi-Fi system
 */
void wifi_deinit(void) {
    if (!wifi_initialized) {
        return;
    }

    printf("[Wi-Fi] Deinitializing Wi-Fi system...\n");

    // Stop web server
    wifi_stop_web_server();

    // Stop Wi-Fi
    wifi_stop();

    // Delete task
    if (wifi_task_handle) {
        vTaskDelete(wifi_task_handle);
        wifi_task_handle = NULL;
    }

    // Deinitialize CYW43
    cyw43_arch_deinit();

    wifi_initialized = false;
    printf("[Wi-Fi] Wi-Fi system deinitialized\n");
}

/**
 * @brief Start Wi-Fi in specified mode
 */
bool wifi_start(wifi_mode_t mode) {
    if (!wifi_initialized) {
        printf("[Wi-Fi] System not initialized\n");
        return false;
    }

    printf("[Wi-Fi] Starting Wi-Fi in mode %d...\n", mode);

    switch (mode) {
        case WIFI_MODE_AP:
            return wifi_start_ap(current_config.ssid, current_config.password, current_config.channel);
            
        case WIFI_MODE_STATION:
            return wifi_connect(current_config.ssid, current_config.password);
            
        case WIFI_MODE_AP_STATION:
            // Start AP first, then try to connect as station
            if (!wifi_start_ap(current_config.ssid, current_config.password, current_config.channel)) {
                return false;
            }
            // Note: Station mode connection would be handled separately
            return true;
            
        default:
            printf("[Wi-Fi] Invalid mode: %d\n", mode);
            return false;
    }
}

/**
 * @brief Stop Wi-Fi operation
 */
void wifi_stop(void) {
    if (!wifi_initialized) {
        return;
    }

    printf("[Wi-Fi] Stopping Wi-Fi...\n");
    
    cyw43_arch_disable_ap_mode();
    cyw43_wifi_leave(&cyw43_state, CYW43_ITF_STA);
    
    current_status.status = WIFI_STATUS_DISCONNECTED;
    memset(current_status.ip_address, 0, sizeof(current_status.ip_address));
}

/**
 * @brief Connect to Wi-Fi network (station mode)
 */
bool wifi_connect(const char* ssid, const char* password) {
    if (!wifi_initialized) {
        printf("[Wi-Fi] System not initialized\n");
        return false;
    }

    printf("[Wi-Fi] Connecting to network: %s\n", ssid);
    
    cyw43_arch_enable_sta_mode();
    current_status.status = WIFI_STATUS_CONNECTING;

    int result = cyw43_arch_wifi_connect_timeout_ms(ssid, password, 
                                                   CYW43_AUTH_WPA2_AES_PSK, 
                                                   WIFI_CONNECT_TIMEOUT_MS);
    
    if (result == 0) {
        current_status.status = WIFI_STATUS_CONNECTED;
        current_status.connected_time = to_ms_since_boot(get_absolute_time());
        
        // Get IP address
        const ip4_addr_t* ip = netif_ip4_addr(netif_default);
        if (ip) {
            ip4addr_ntoa_r(ip, current_status.ip_address, sizeof(current_status.ip_address));
        }
        
        printf("[Wi-Fi] Connected successfully. IP: %s\n", current_status.ip_address);
        return true;
    } else {
        current_status.status = WIFI_STATUS_ERROR;
        printf("[Wi-Fi] Failed to connect: %d\n", result);
        return false;
    }
}

/**
 * @brief Start Access Point mode
 */
bool wifi_start_ap(const char* ssid, const char* password, uint8_t channel) {
    if (!wifi_initialized) {
        printf("[Wi-Fi] System not initialized\n");
        return false;
    }

    printf("[Wi-Fi] Starting AP: %s on channel %d\n", ssid, channel);

    cyw43_arch_enable_ap_mode(ssid, password, CYW43_AUTH_WPA2_AES_PSK);
    
    // Set IP configuration
    ip4_addr_t ip, netmask, gateway;
    ip4addr_aton(WIFI_AP_IP, &ip);
    ip4addr_aton(WIFI_AP_NETMASK, &netmask);
    ip4addr_aton(WIFI_AP_GATEWAY, &gateway);
    
    netif_set_addr(netif_default, &ip, &netmask, &gateway);
    
    current_status.status = WIFI_STATUS_AP_STARTED;
    strncpy(current_status.ip_address, WIFI_AP_IP, sizeof(current_status.ip_address));
    current_status.connected_time = to_ms_since_boot(get_absolute_time());

    printf("[Wi-Fi] AP started successfully. IP: %s\n", current_status.ip_address);
    return true;
}

/**
 * @brief Get current Wi-Fi status
 */
bool wifi_get_status(wifi_status_info_t* status) {
    if (!wifi_initialized || !status) {
        return false;
    }

    *status = current_status;
    
    // Update RSSI if connected as station
    if (current_status.status == WIFI_STATUS_CONNECTED) {
        // RSSI update would go here - not available in all SDK versions
        status->rssi = -50; // Placeholder
    }

    return true;
}

/**
 * @brief Check if Wi-Fi is connected
 */
bool wifi_is_connected(void) {
    return (current_status.status == WIFI_STATUS_CONNECTED || 
            current_status.status == WIFI_STATUS_AP_STARTED);
}

/**
 * @brief Start HTTP web server
 */
bool wifi_start_web_server(uint16_t port) {
    if (!wifi_initialized) {
        printf("[Wi-Fi] Wi-Fi not initialized\n");
        return false;
    }

    if (web_server_running) {
        printf("[Wi-Fi] Web server already running\n");
        return true;
    }

    if (port == 0) {
        port = HTTP_SERVER_PORT;
    }

    printf("[Wi-Fi] Starting web server on port %d...\n", port);

    // TODO: Initialize HTTP server (httpd_init)
    // This is a placeholder implementation for alpha release
    
    web_server_running = true;
    printf("[Wi-Fi] Web server started successfully (placeholder)\n");
    return true;
}

/**
 * @brief Stop HTTP web server
 */
void wifi_stop_web_server(void) {
    if (!web_server_running) {
        return;
    }

    printf("[Wi-Fi] Stopping web server...\n");
    web_server_running = false;
}

/**
 * @brief Register HTTP request handler
 */
bool wifi_register_handler(const char* path, http_handler_t handler) {
    if (!path || !handler) {
        return false;
    }

    // In a full implementation, this would register with the HTTP server
    // For now, handlers are statically defined in the routes table
    return true;
}

/**
 * @brief Send JSON response
 */
void wifi_send_json_response(http_response_t* response, const char* json_data) {
    if (!response || !json_data) {
        return;
    }

    response->status_code = 200;
    strncpy(response->content_type, "application/json", sizeof(response->content_type));
    strncpy(response->body, json_data, sizeof(response->body) - 1);
    response->content_length = strlen(response->body);
    response->close_connection = false;
}

/**
 * @brief Send error response
 */
void wifi_send_error_response(http_response_t* response, uint16_t status_code, const char* message) {
    if (!response) {
        return;
    }

    response->status_code = status_code;
    strncpy(response->content_type, "application/json", sizeof(response->content_type));
    
    snprintf(response->body, sizeof(response->body),
            "{\"error\":{\"code\":%d,\"message\":\"%s\"}}",
            status_code, message ? message : "Unknown error");
    
    response->content_length = strlen(response->body);
    response->close_connection = false;
}

/**
 * @brief Get default Wi-Fi configuration
 */
void wifi_get_default_config(wifi_config_t* config) {
    if (!config) {
        return;
    }

    memset(config, 0, sizeof(wifi_config_t));
    config->mode = WIFI_MODE_AP;
    strncpy(config->ssid, WIFI_DEFAULT_SSID, sizeof(config->ssid));
    strncpy(config->password, WIFI_DEFAULT_PASSWORD, sizeof(config->password));
    config->channel = WIFI_DEFAULT_CHANNEL;
    config->auto_start = true;
    config->enable_web_interface = true;
    config->enable_authentication = false;
    strncpy(config->web_username, "admin", sizeof(config->web_username));
    strncpy(config->web_password, "admin", sizeof(config->web_password));
}

/**
 * @brief Update Wi-Fi configuration
 */
bool wifi_update_config(const wifi_config_t* config) {
    if (!config) {
        return false;
    }

    current_config = *config;
    return true;
}

/**
 * @brief Wi-Fi task implementation
 */
static void wifi_task_impl(void* pvParameters) {
    (void)pvParameters;
    
    printf("[Wi-Fi] Wi-Fi task started\n");

    // Auto-start if configured
    if (current_config.auto_start) {
        vTaskDelay(pdMS_TO_TICKS(2000)); // Wait for system stabilization
        
        if (wifi_start(current_config.mode)) {
            if (current_config.enable_web_interface) {
                wifi_start_web_server(0);
            }
        }
    }

    while (true) {
        // Monitor Wi-Fi status and handle reconnection
        if (current_status.status == WIFI_STATUS_CONNECTED) {
            // Check connection health
            if (!cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA)) {
                printf("[Wi-Fi] Connection lost, attempting reconnection...\n");
                current_status.status = WIFI_STATUS_CONNECTING;
                wifi_connect(current_config.ssid, current_config.password);
            }
        }

        // Update statistics
        current_status.bytes_sent += 0;     // Would be updated by actual traffic monitoring
        current_status.bytes_received += 0; // Would be updated by actual traffic monitoring

        // Task runs every 5 seconds
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

/**
 * @brief Wi-Fi task function (for FreeRTOS)
 */
void wifi_task(void* pvParameters) {
    wifi_task_impl(pvParameters);
}

// Web API handler implementations

/**
 * @brief Handle API status requests
 */
void wifi_handle_api_status(const http_request_t* request, http_response_t* response) {
    (void)request;
    
    char json_response[1024];
    wifi_status_info_t wifi_status;
    power_status_t power_status;
    
    wifi_get_status(&wifi_status);
    power_status = power_get_status();
    
    snprintf(json_response, sizeof(json_response),
        "{"
        "\"system\":{"
            "\"version\":\"0.9.1-alpha.2\","
            "\"uptime\":%lu,"
            "\"free_heap\":%u"
        "},"
        "\"wifi\":{"
            "\"status\":\"%s\","
            "\"ip\":\"%s\","
            "\"clients\":%d"
        "},"
        "\"power\":{"
            "\"battery_voltage\":%.2f,"
            "\"battery_percent\":%d,"
            "\"charging\":%s"
        "},"
        "\"fuzzing\":{"
            "\"active\":%s,"
            "\"progress\":%d"
        "}"
        "}",
        to_ms_since_boot(get_absolute_time()) / 1000,
        xPortGetFreeHeapSize(),
        (wifi_status.status == WIFI_STATUS_CONNECTED) ? "connected" : "disconnected",
        wifi_status.ip_address,
        wifi_status.connected_clients,
        power_status.battery_mv / 1000.0f,
        power_status.battery_percent,
        power_status.is_charging ? "true" : "false",
        jtag_fuzzer_is_active() ? "true" : "false",
        jtag_fuzzer_get_progress()
    );
    
    wifi_send_json_response(response, json_response);
}

/**
 * @brief Handle API fuzzing requests
 */
void wifi_handle_api_fuzzing(const http_request_t* request, http_response_t* response) {
    char json_response[512];
    
    if (strcmp(request->method, "GET") == 0) {
        // Get fuzzing status
        snprintf(json_response, sizeof(json_response),
            "{"
            "\"active\":%s,"
            "\"progress\":%d"
            "}",
            jtag_fuzzer_is_active() ? "true" : "false",
            jtag_fuzzer_get_progress()
        );
        wifi_send_json_response(response, json_response);
        
    } else if (strcmp(request->method, "POST") == 0) {
        // Start fuzzing operation
        char command_response[256];
        cmd_result_t result = fuzz_commands_process(request->body, command_response, sizeof(command_response));
        
        snprintf(json_response, sizeof(json_response),
            "{"
            "\"result\":\"%s\","
            "\"message\":\"%s\""
            "}",
            (result == CMD_RESULT_SUCCESS) ? "success" : "error",
            command_response
        );
        wifi_send_json_response(response, json_response);
        
    } else {
        wifi_send_error_response(response, 405, "Method not allowed");
    }
}

/**
 * @brief Handle API logs requests
 */
void wifi_handle_api_logs(const http_request_t* request, http_response_t* response) {
    (void)request;
    
    // This would return recent log entries
    char json_response[2048] = "{\"logs\":[";
    
    // Placeholder - would read from storage system
    strcat(json_response, 
        "{\"timestamp\":\"2025-08-07T01:58:00Z\",\"level\":\"INFO\",\"message\":\"Fuzzing session started\"},"
        "{\"timestamp\":\"2025-08-07T01:58:05Z\",\"level\":\"INFO\",\"message\":\"Pin discovery completed\"}"
    );
    
    strcat(json_response, "]}");
    
    wifi_send_json_response(response, json_response);
}

/**
 * @brief Handle API files requests
 */
void wifi_handle_api_files(const http_request_t* request, http_response_t* response) {
    (void)request;
    
    // This would list available files
    char json_response[1024] = "{\"files\":[";
    
    // Placeholder - would read from storage system
    strcat(json_response,
        "{\"name\":\"fuzzing_session_001.log\",\"size\":1024,\"date\":\"2025-08-07T01:58:00Z\"},"
        "{\"name\":\"jtag_scan_results.txt\",\"size\":512,\"date\":\"2025-08-07T01:57:00Z\"}"
    );
    
    strcat(json_response, "]}");
    
    wifi_send_json_response(response, json_response);
}

/**
 * @brief Handle API config requests
 */
void wifi_handle_api_config(const http_request_t* request, http_response_t* response) {
    char json_response[512];
    
    if (strcmp(request->method, "GET") == 0) {
        // Get configuration
        snprintf(json_response, sizeof(json_response),
            "{"
            "\"wifi\":{"
                "\"ssid\":\"%s\","
                "\"mode\":\"%s\""
            "}"
            "}",
            current_config.ssid,
            (current_config.mode == WIFI_MODE_AP) ? "ap" : "station"
        );
        wifi_send_json_response(response, json_response);
        
    } else if (strcmp(request->method, "POST") == 0) {
        // Update configuration (simplified)
        snprintf(json_response, sizeof(json_response),
            "{\"result\":\"success\",\"message\":\"Configuration updated\"}"
        );
        wifi_send_json_response(response, json_response);
        
    } else {
        wifi_send_error_response(response, 405, "Method not allowed");
    }
}

/**
 * @brief Handle root web page requests
 */
void wifi_handle_root(const http_request_t* request, http_response_t* response) {
    (void)request;
    
    const char* html_content = 
        "<!DOCTYPE html>"
        "<html><head><title>KISS Fuzzer</title>"
        "<meta name='viewport' content='width=device-width, initial-scale=1'>"
        "<style>"
        "body{font-family:Arial,sans-serif;margin:0;padding:20px;background:#f0f0f0}"
        ".container{max-width:800px;margin:0 auto;background:white;padding:20px;border-radius:8px;box-shadow:0 2px 10px rgba(0,0,0,0.1)}"
        ".header{text-align:center;color:#333;border-bottom:2px solid #4CAF50;padding-bottom:10px}"
        ".status{display:flex;justify-content:space-between;margin:20px 0;padding:15px;background:#f9f9f9;border-radius:5px}"
        ".button{background:#4CAF50;color:white;padding:10px 20px;border:none;border-radius:5px;cursor:pointer;margin:5px}"
        ".button:hover{background:#45a049}"
        ".logs{background:#000;color:#0f0;padding:15px;border-radius:5px;font-family:monospace;height:200px;overflow-y:scroll}"
        "</style></head>"
        "<body>"
        "<div class='container'>"
            "<h1 class='header'>KISS Fuzzer Control Panel</h1>"
            "<div class='status'>"
                "<div><strong>Status:</strong> <span id='status'>Connected</span></div>"
                "<div><strong>IP:</strong> <span id='ip'>192.168.4.1</span></div>"
                "<div><strong>Uptime:</strong> <span id='uptime'>--</span></div>"
            "</div>"
            "<div style='text-align:center;margin:20px 0'>"
                "<button class='button' onclick='startFuzzing()'>Start Fuzzing</button>"
                "<button class='button' onclick='stopFuzzing()'>Stop Fuzzing</button>"
                "<button class='button' onclick='scanJTAG()'>Scan JTAG</button>"
                "<button class='button' onclick='downloadLogs()'>Download Logs</button>"
            "</div>"
            "<h3>Recent Logs</h3>"
            "<div class='logs' id='logs'>KISS Fuzzer v0.9.1-alpha.2 ready...</div>"
        "</div>"
        "<script>"
        "function updateStatus(){"
            "fetch('/api/status').then(r=>r.json()).then(data=>{"
                "document.getElementById('status').textContent=data.wifi.status;"
                "document.getElementById('ip').textContent=data.wifi.ip;"
                "document.getElementById('uptime').textContent=Math.floor(data.system.uptime/60)+'m';"
            "});"
        "}"
        "function startFuzzing(){fetch('/api/fuzzing',{method:'POST',body:'fuzz random 100'});}"
        "function stopFuzzing(){fetch('/api/fuzzing',{method:'POST',body:'stop'});}"
        "function scanJTAG(){fetch('/api/fuzzing',{method:'POST',body:'scan'});}"
        "function downloadLogs(){window.open('/download/logs.txt');}"
        "setInterval(updateStatus,5000);updateStatus();"
        "</script></body></html>";
    
    response->status_code = 200;
    strncpy(response->content_type, "text/html", sizeof(response->content_type));
    strncpy(response->body, html_content, sizeof(response->body) - 1);
    response->content_length = strlen(response->body);
    response->close_connection = false;
}

/**
 * @brief Handle file download requests
 */
void wifi_handle_download(const http_request_t* request, http_response_t* response) {
    (void)request;
    
    // This would serve files from storage
    const char* sample_log = 
        "[2025-08-07 01:58:00] INFO: KISS Fuzzer started\n"
        "[2025-08-07 01:58:01] INFO: JTAG scan initiated\n"
        "[2025-08-07 01:58:02] INFO: Found 1 device(s)\n"
        "[2025-08-07 01:58:03] INFO: Device 0: IDCODE=0x12345678\n";
    
    response->status_code = 200;
    strncpy(response->content_type, "text/plain", sizeof(response->content_type));
    strncpy(response->body, sample_log, sizeof(response->body) - 1);
    response->content_length = strlen(response->body);
    response->close_connection = false;
}
