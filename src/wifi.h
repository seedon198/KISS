/**
 * @file wifi.h
 * @brief Wi-Fi web interface and remote control for KISS Fuzzer
 * @author KISS Fuzzer Team
 * @date 2025
 * @version 0.9.1
 */

#ifndef WIFI_H
#define WIFI_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/netif.h"
#include "lwip/ip4_addr.h"
#include "lwip/apps/httpd.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

// Wi-Fi configuration constants
#define WIFI_SSID_MAX_LEN           32
#define WIFI_PASSWORD_MAX_LEN       64
#define WIFI_DEFAULT_SSID           "KISS-Fuzzer"
#define WIFI_DEFAULT_PASSWORD       "kissfuzz123"
#define WIFI_DEFAULT_CHANNEL        6
#define WIFI_CONNECT_TIMEOUT_MS     10000
#define WIFI_AP_IP                  "192.168.4.1"
#define WIFI_AP_GATEWAY             "192.168.4.1"
#define WIFI_AP_NETMASK             "255.255.255.0"

// HTTP server configuration
#define HTTP_SERVER_PORT            80
#define HTTP_MAX_REQUEST_SIZE       2048
#define HTTP_MAX_RESPONSE_SIZE      4096
#define HTTP_MAX_CONNECTIONS        4

// Web interface paths
#define WEB_PATH_ROOT               "/"
#define WEB_PATH_API_STATUS         "/api/status"
#define WEB_PATH_API_FUZZING        "/api/fuzzing"
#define WEB_PATH_API_LOGS           "/api/logs"
#define WEB_PATH_API_FILES          "/api/files"
#define WEB_PATH_API_CONFIG         "/api/config"
#define WEB_PATH_DOWNLOAD           "/download"

/**
 * @brief Wi-Fi operating modes
 */
typedef enum {
    WIFI_MODE_DISABLED = 0,
    WIFI_MODE_AP,           // Access Point mode
    WIFI_MODE_STATION,      // Station mode (connect to existing network)
    WIFI_MODE_AP_STATION    // Both AP and Station mode
} wifi_mode_t;

/**
 * @brief Wi-Fi connection status
 */
typedef enum {
    WIFI_STATUS_DISCONNECTED = 0,
    WIFI_STATUS_CONNECTING,
    WIFI_STATUS_CONNECTED,
    WIFI_STATUS_AP_STARTED,
    WIFI_STATUS_ERROR
} wifi_status_t;

/**
 * @brief Wi-Fi configuration structure
 */
typedef struct {
    wifi_mode_t mode;
    char ssid[WIFI_SSID_MAX_LEN];
    char password[WIFI_PASSWORD_MAX_LEN];
    uint8_t channel;
    bool auto_start;
    bool enable_web_interface;
    bool enable_authentication;
    char web_username[32];
    char web_password[32];
} wifi_config_t;

/**
 * @brief Wi-Fi status information
 */
typedef struct {
    wifi_status_t status;
    char ip_address[16];
    char gateway[16];
    char netmask[16];
    int8_t rssi;
    uint32_t connected_time;
    uint32_t bytes_sent;
    uint32_t bytes_received;
    uint8_t connected_clients;
} wifi_status_info_t;

/**
 * @brief HTTP request structure
 */
typedef struct {
    char method[8];         // GET, POST, PUT, DELETE
    char path[128];
    char query[256];
    char body[1024];
    size_t content_length;
    char content_type[64];
} http_request_t;

/**
 * @brief HTTP response structure
 */
typedef struct {
    uint16_t status_code;
    char content_type[64];
    char body[HTTP_MAX_RESPONSE_SIZE];
    size_t content_length;
    bool close_connection;
} http_response_t;

/**
 * @brief HTTP handler function type
 */
typedef void (*http_handler_t)(const http_request_t* request, http_response_t* response);

/**
 * @brief Initialize Wi-Fi system
 * @param config Wi-Fi configuration (NULL for defaults)
 * @return true if successful
 */
bool wifi_init(const wifi_config_t* config);

/**
 * @brief Deinitialize Wi-Fi system
 */
void wifi_deinit(void);

/**
 * @brief Start Wi-Fi in specified mode
 * @param mode Operating mode
 * @return true if successful
 */
bool wifi_start(wifi_mode_t mode);

/**
 * @brief Stop Wi-Fi operation
 */
void wifi_stop(void);

/**
 * @brief Connect to Wi-Fi network (station mode)
 * @param ssid Network SSID
 * @param password Network password
 * @return true if connection successful
 */
bool wifi_connect(const char* ssid, const char* password);

/**
 * @brief Start Access Point mode
 * @param ssid AP SSID
 * @param password AP password
 * @param channel Wi-Fi channel
 * @return true if AP started successfully
 */
bool wifi_start_ap(const char* ssid, const char* password, uint8_t channel);

/**
 * @brief Get current Wi-Fi status
 * @param status Pointer to store status information
 * @return true if status retrieved successfully
 */
bool wifi_get_status(wifi_status_info_t* status);

/**
 * @brief Check if Wi-Fi is connected
 * @return true if connected
 */
bool wifi_is_connected(void);

/**
 * @brief Start HTTP web server
 * @param port Server port (0 for default)
 * @return true if server started successfully
 */
bool wifi_start_web_server(uint16_t port);

/**
 * @brief Stop HTTP web server
 */
void wifi_stop_web_server(void);

/**
 * @brief Register HTTP request handler
 * @param path URL path to handle
 * @param handler Handler function
 * @return true if registered successfully
 */
bool wifi_register_handler(const char* path, http_handler_t handler);

/**
 * @brief Send JSON response
 * @param response Response structure
 * @param json_data JSON string
 */
void wifi_send_json_response(http_response_t* response, const char* json_data);

/**
 * @brief Send error response
 * @param response Response structure
 * @param status_code HTTP status code
 * @param message Error message
 */
void wifi_send_error_response(http_response_t* response, uint16_t status_code, const char* message);

/**
 * @brief Send file response
 * @param response Response structure
 * @param file_path Path to file
 * @param content_type MIME content type
 */
void wifi_send_file_response(http_response_t* response, const char* file_path, const char* content_type);

/**
 * @brief Get default Wi-Fi configuration
 * @param config Pointer to store default configuration
 */
void wifi_get_default_config(wifi_config_t* config);

/**
 * @brief Update Wi-Fi configuration
 * @param config New configuration
 * @return true if configuration updated successfully
 */
bool wifi_update_config(const wifi_config_t* config);

/**
 * @brief Wi-Fi task function (for FreeRTOS)
 * @param pvParameters Task parameters
 */
void wifi_task(void* pvParameters);

// Web API handlers
void wifi_handle_api_status(const http_request_t* request, http_response_t* response);
void wifi_handle_api_fuzzing(const http_request_t* request, http_response_t* response);
void wifi_handle_api_logs(const http_request_t* request, http_response_t* response);
void wifi_handle_api_files(const http_request_t* request, http_response_t* response);
void wifi_handle_api_config(const http_request_t* request, http_response_t* response);
void wifi_handle_root(const http_request_t* request, http_response_t* response);
void wifi_handle_download(const http_request_t* request, http_response_t* response);

#endif // WIFI_H
