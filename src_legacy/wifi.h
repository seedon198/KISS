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

// Wi-Fi status
typedef enum {
    WIFI_STATUS_DISABLED,
    WIFI_STATUS_DISCONNECTED,
    WIFI_STATUS_CONNECTING,
    WIFI_STATUS_CONNECTED,
    WIFI_STATUS_AP_MODE,
    WIFI_STATUS_AP_STARTED,
    WIFI_STATUS_CONNECT_FAILED,
    WIFI_STATUS_ERROR
} wifi_status_t;

// WiFi result codes
typedef enum {
    WIFI_OK = 0,
    WIFI_ERROR,
    WIFI_ERROR_INIT,
    WIFI_ERROR_NOT_INIT,
    WIFI_ERROR_CONNECT,
    WIFI_ERROR_NOT_CONNECTED,
    WIFI_ERROR_SERVER,
    WIFI_ERROR_HTTP_SERVER,
    WIFI_ERROR_INVALID_PARAM,
    WIFI_ERROR_TIMEOUT,
    WIFI_ERROR_QUEUE_FULL
} wifi_result_t;

// WiFi event types
typedef enum {
    WIFI_EVENT_START_AP,
    WIFI_EVENT_CONNECT_STA,
    WIFI_EVENT_DISCONNECT,
    WIFI_EVENT_START_SERVER
} wifi_event_type_t;

// WiFi event structure
typedef struct {
    wifi_event_type_t type;
    union {
        struct {
            char ssid[32];
            char password[64];
        } connect;
    } data;
} wifi_event_t;

// Task configuration
#define SERVER_TASK_STACK_SIZE  2048
#define SERVER_TASK_PRIORITY    2

// Wi-Fi configuration
#define WIFI_SSID     "KISS-Fuzzer"
#define WIFI_PASSWORD "fuzzing123"
#define WIFI_COUNTRY  CYW43_COUNTRY_USA
#define WIFI_AUTH     CYW43_AUTH_WPA2_AES_PSK

// HTTP server configuration
#define HTTP_PORT        80
#define MAX_CONNECTIONS  4
#define MAX_REQUEST_SIZE 1024

// API endpoints
#define API_STATUS     "/api/status"
#define API_SCAN_START "/api/scan/start"
#define API_SCAN_STOP  "/api/scan/stop"
#define API_LOGS       "/api/logs"
#define API_FILES      "/api/files"
#define API_DOWNLOAD   "/api/download"
#define API_CONFIG     "/api/config"

/**
 * @brief Initialize Wi-Fi module and start access point
 * @return Wi-Fi result code
 */
wifi_result_t wifi_init(void);

/**
 * @brief Start Wi-Fi access point mode
 * @return wifi_result_t
 */
wifi_result_t wifi_start_ap(void);

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
void wifi_get_ip_address(char *ip_str, size_t max_len);

/**
 * @brief Send log message to connected web clients
 * @param message Log message to send
 * @return void
 */
void wifi_send_log(const char *message);

/**
 * @brief Get current Wi-Fi status
 * @return Current Wi-Fi status
 */
wifi_status_t wifi_get_status(void);

/**
 * @brief Get Wi-Fi status message
 * @return Status message string
 */
const char* wifi_get_status_message(void);

// HTTP handler functions
const char *api_status_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
const char *api_scan_start_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
const char *api_scan_stop_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
const char *api_logs_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
const char *api_files_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
const char *api_config_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);

// Web interface HTML
extern const char *index_html;
extern const char *style_css;
extern const char *script_js;

#endif  // WIFI_H
