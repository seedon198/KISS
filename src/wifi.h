/**
 * @file wifi.h
 * @brief Wi-Fi web server and remote control interface
 * @author KISS Fuzzer Team
 * @date 2025
 */

#ifndef WIFI_H
#define WIFI_H

#include "kiss_fuzzer.h"
#include "lwip/apps/httpd.h"

// Wi-Fi configuration
#define WIFI_SSID           "KISS-Fuzzer"
#define WIFI_PASSWORD       "fuzzing123"
#define WIFI_COUNTRY        CYW43_COUNTRY_USA
#define WIFI_AUTH           CYW43_AUTH_WPA2_AES_PSK

// HTTP server configuration
#define HTTP_PORT           80
#define MAX_CONNECTIONS     4
#define MAX_REQUEST_SIZE    1024

// API endpoints
#define API_STATUS          "/api/status"
#define API_SCAN_START      "/api/scan/start"
#define API_SCAN_STOP       "/api/scan/stop"
#define API_LOGS            "/api/logs"
#define API_FILES           "/api/files"
#define API_DOWNLOAD        "/api/download"
#define API_CONFIG          "/api/config"

/**
 * @brief Initialize Wi-Fi module and start access point
 * @return true if initialization successful, false otherwise
 */
bool wifi_init(void);

/**
 * @brief Wi-Fi task function for handling connections and HTTP server
 * @param pvParameters FreeRTOS task parameters
 * @return void
 */
void wifi_task(void* pvParameters);

/**
 * @brief Start Wi-Fi access point mode
 * @return true if AP started successfully, false otherwise
 */
bool wifi_start_ap(void);

/**
 * @brief Stop Wi-Fi access point
 * @return void
 */
void wifi_stop_ap(void);

/**
 * @brief Start HTTP server
 * @return true if server started successfully, false otherwise
 */
bool wifi_start_http_server(void);

/**
 * @brief Stop HTTP server
 * @return void
 */
void wifi_stop_http_server(void);

/**
 * @brief Check if Wi-Fi is connected
 * @return true if connected, false otherwise
 */
bool wifi_is_connected(void);

/**
 * @brief Get current IP address
 * @param ip_str Buffer to store IP address string
 * @param max_len Maximum buffer length
 * @return void
 */
void wifi_get_ip_address(char* ip_str, size_t max_len);

/**
 * @brief Send log message to connected web clients
 * @param message Log message to send
 * @return void
 */
void wifi_send_log(const char* message);

// HTTP handler functions
const char* api_status_handler(int iIndex, int iNumParams, char* pcParam[], char* pcValue[]);
const char* api_scan_start_handler(int iIndex, int iNumParams, char* pcParam[], char* pcValue[]);
const char* api_scan_stop_handler(int iIndex, int iNumParams, char* pcParam[], char* pcValue[]);
const char* api_logs_handler(int iIndex, int iNumParams, char* pcParam[], char* pcValue[]);
const char* api_files_handler(int iIndex, int iNumParams, char* pcParam[], char* pcValue[]);
const char* api_config_handler(int iIndex, int iNumParams, char* pcParam[], char* pcValue[]);

// Web interface HTML
extern const char* index_html;
extern const char* style_css;
extern const char* script_js;

#endif // WIFI_H
