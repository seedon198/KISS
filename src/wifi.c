/**
 * @fil#include "wifi.h"

#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "display.h"
#include "jtag.h"
#include "kiss_fuzzer.h"
#include "lwip/dhcp.h"
#include "lwip/dns.h"
#include "lwip/init.h"
#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "power.h"
#include "storage.h"i-Fi web server implementation for KISS Fuzzer
 *
 * Provides Wi-Fi connectivity, web server, and HTTP endpoints for remote control.
 * Supports both AP mode and station mode with captive portal functionality.
 *
 * @author KISS Fuzzer Team
 * @date 2025
 */

#include "wifi.h"

#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "display.h"
#include "jtag.h"
#include "kiss_fuzzer.h"
#include "lwip/dhcp.h"
#include "lwip/dns.h"
#include "lwip/init.h"
#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "power.h"
#include "queue.h"
#include "semphr.h"
#include "storage.h"
#include "task.h"

// Wi-Fi configuration - Use configuration system for actual values
#define WIFI_SSID     "KISS-Fuzzer"
#define WIFI_PASSWORD "CHANGE_ME_NOW"  // TODO: Load from secure config
#define WIFI_COUNTRY  CYW43_COUNTRY_USA

// HTTP server configuration
#define HTTP_PORT        80
#define HTTP_POLL_TIME_S 5
#define HTTP_MAX_CONN    4

// Web interface HTML pages
static const char html_header[] =
    "<!DOCTYPE html><html><head>"
    "<title>KISS Fuzzer</title>"
    "<meta name='viewport' content='width=device-width, initial-scale=1'>"
    "<style>body{font-family:Arial,sans-serif;margin:20px;background:#1a1a1a;color:#fff;}"
    "h1{color:#9c27b0;}button{background:#9c27b0;color:#fff;border:none;padding:10px "
    "20px;margin:5px;border-radius:5px;cursor:pointer;}"
    "button:hover{background:#7b1fa2;}input,select{padding:8px;margin:5px;border-radius:3px;border:"
    "1px solid #555;background:#333;color:#fff;}"
    ".status{background:#333;padding:10px;border-radius:5px;margin:10px "
    "0;}.log{background:#222;padding:10px;border-radius:5px;margin:10px "
    "0;font-family:monospace;white-space:pre-wrap;max-height:300px;overflow-y:auto;}"
    "</style></head><body>";

static const char html_footer[] = "</body></html>";

// HTTP connection structure
typedef struct {
    struct tcp_pcb *pcb;
    int             sent_len;
    char            headers[512];
    char            result[2048];
    bool            header_sent;
} http_connection_t;

// Global variables
static TaskHandle_t    wifi_task_handle    = NULL;
static TaskHandle_t    server_task_handle  = NULL;
static QueueHandle_t   wifi_event_queue    = NULL;
static struct tcp_pcb *server_pcb          = NULL;
static bool            wifi_connected      = false;
static bool            ap_mode             = true;
static wifi_status_t   wifi_status         = WIFI_STATUS_DISCONNECTED;
static char            status_message[128] = "Initializing...";

// Function prototypes
static void  wifi_task(void *pvParameters);
static void  server_task(void *pvParameters);
static err_t tcp_server_accept(void *arg, struct tcp_pcb *client_pcb, err_t err);
static err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static void  tcp_server_err(void *arg, err_t err);
static void  tcp_server_close(http_connection_t *con);
static int   generate_html_page(const char *path, char *buffer, size_t buffer_size);
static void  handle_api_request(const char *path, const char *query, char *response,
                                size_t response_size);
static void  wifi_link_status_callback(struct netif *netif);

/**
 * @brief Initialize Wi-Fi subsystem
 * @return WIFI_OK on success, error code otherwise
 */
wifi_result_t wifi_init(void)
{
    // Initialize CYW43 architecture
    if (cyw43_arch_init_with_country(WIFI_COUNTRY)) {
        LOG_ERROR("Failed to initialize CYW43");
        return WIFI_ERROR_INIT;
    }

    // Create event queue
    wifi_event_queue = xQueueCreate(10, sizeof(wifi_event_t));
    if (!wifi_event_queue) {
        LOG_ERROR("Failed to create Wi-Fi event queue");
        return WIFI_ERROR_INIT;
    }

    // Create Wi-Fi task
    BaseType_t result = xTaskCreate(wifi_task, "WiFi", WIFI_TASK_STACK_SIZE, NULL,
                                    WIFI_TASK_PRIORITY, &wifi_task_handle);

    if (result != pdPASS) {
        LOG_ERROR("Failed to create Wi-Fi task");
        return WIFI_ERROR_INIT;
    }

    // Create server task
    result = xTaskCreate(server_task, "WebServer", SERVER_TASK_STACK_SIZE, NULL,
                         SERVER_TASK_PRIORITY, &server_task_handle);

    if (result != pdPASS) {
        LOG_ERROR("Failed to create server task");
        return WIFI_ERROR_INIT;
    }

    LOG_INFO("Wi-Fi subsystem initialized");
    return WIFI_OK;
}

/**
 * @brief Start Wi-Fi in AP mode
 * @return WIFI_OK on success, error code otherwise
 */
wifi_result_t wifi_start_ap(void)
{
    cyw43_arch_enable_ap_mode(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK);

    // Configure IP address for AP mode
    ip4_addr_t gw, mask;
    IP4_ADDR(&gw, 192, 168, 4, 1);
    IP4_ADDR(&mask, 255, 255, 255, 0);

    // Set up DHCP server
    dhcp_server_t dhcp_server;
    dhcp_server_init(&dhcp_server, &gw, &mask);

    // Set up DNS server for captive portal
    dns_server_t dns_server;
    dns_server_init(&dns_server, &gw);

    ap_mode        = true;
    wifi_connected = true;
    wifi_status    = WIFI_STATUS_AP_STARTED;
    strncpy(status_message, "AP Mode: KISS-Fuzzer", sizeof(status_message) - 1);

    LOG_INFO("Wi-Fi AP started: %s", WIFI_SSID);
    return WIFI_OK;
}

/**
 * @brief Connect to Wi-Fi network in station mode
 * @param ssid Network SSID
 * @param password Network password
 * @return WIFI_OK on success, error code otherwise
 */
wifi_result_t wifi_connect_sta(const char *ssid, const char *password)
{
    cyw43_arch_enable_sta_mode();

    wifi_status = WIFI_STATUS_CONNECTING;
    strncpy(status_message, "Connecting...", sizeof(status_message) - 1);

    if (cyw43_arch_wifi_connect_timeout_ms(ssid, password, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        LOG_ERROR("Failed to connect to Wi-Fi: %s", ssid);
        wifi_status = WIFI_STATUS_CONNECT_FAILED;
        strncpy(status_message, "Connection failed", sizeof(status_message) - 1);
        return WIFI_ERROR_CONNECT;
    }

    ap_mode        = false;
    wifi_connected = true;
    wifi_status    = WIFI_STATUS_CONNECTED;
    snprintf(status_message, sizeof(status_message), "Connected: %s", ssid);

    LOG_INFO("Connected to Wi-Fi: %s", ssid);
    return WIFI_OK;
}

/**
 * @brief Disconnect from Wi-Fi
 * @return WIFI_OK on success, error code otherwise
 */
wifi_result_t wifi_disconnect(void)
{
    if (server_pcb) {
        tcp_close(server_pcb);
        server_pcb = NULL;
    }

    cyw43_arch_deinit();
    wifi_connected = false;
    wifi_status    = WIFI_STATUS_DISCONNECTED;
    strncpy(status_message, "Disconnected", sizeof(status_message) - 1);

    LOG_INFO("Wi-Fi disconnected");
    return WIFI_OK;
}

/**
 * @brief Get current Wi-Fi status
 * @return Current Wi-Fi status
 */
wifi_status_t wifi_get_status(void)
{
    return wifi_status;
}

/**
 * @brief Get Wi-Fi status message
 * @return Status message string
 */
const char *wifi_get_status_message(void)
{
    return status_message;
}

/**
 * @brief Send event to Wi-Fi task
 * @param event Event to send
 * @return WIFI_OK on success, error code otherwise
 */
wifi_result_t wifi_send_event(wifi_event_t event)
{
    if (!wifi_event_queue) {
        return WIFI_ERROR_NOT_INIT;
    }

    BaseType_t result = xQueueSend(wifi_event_queue, &event, pdMS_TO_TICKS(100));
    return (result == pdPASS) ? WIFI_OK : WIFI_ERROR_QUEUE_FULL;
}

/**
 * @brief Start HTTP server
 * @return WIFI_OK on success, error code otherwise
 */
wifi_result_t wifi_start_server(void)
{
    if (!wifi_connected) {
        return WIFI_ERROR_NOT_CONNECTED;
    }

    server_pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
    if (!server_pcb) {
        LOG_ERROR("Failed to create server PCB");
        return WIFI_ERROR_SERVER;
    }

    err_t err = tcp_bind(server_pcb, NULL, HTTP_PORT);
    if (err != ERR_OK) {
        LOG_ERROR("Failed to bind server: %d", err);
        return WIFI_ERROR_SERVER;
    }

    server_pcb = tcp_listen_with_backlog(server_pcb, HTTP_MAX_CONN);
    if (!server_pcb) {
        LOG_ERROR("Failed to listen on server");
        return WIFI_ERROR_SERVER;
    }

    tcp_accept(server_pcb, tcp_server_accept);

    LOG_INFO("HTTP server started on port %d", HTTP_PORT);
    return WIFI_OK;
}

/**
 * @brief Wi-Fi task implementation
 * @param pvParameters Task parameters (unused)
 */
static void wifi_task(void *pvParameters)
{
    wifi_event_t event;

    // Start in AP mode by default
    wifi_start_ap();
    wifi_start_server();

    while (1) {
        if (xQueueReceive(wifi_event_queue, &event, pdMS_TO_TICKS(1000)) == pdPASS) {
            switch (event.type) {
                case WIFI_EVENT_START_AP:
                    wifi_start_ap();
                    wifi_start_server();
                    break;

                case WIFI_EVENT_CONNECT_STA:
                    wifi_connect_sta(event.data.connect.ssid, event.data.connect.password);
                    wifi_start_server();
                    break;

                case WIFI_EVENT_DISCONNECT:
                    wifi_disconnect();
                    break;

                case WIFI_EVENT_START_SERVER:
                    wifi_start_server();
                    break;

                default:
                    break;
            }
        }

        // Poll CYW43 driver
        if (wifi_connected) {
            cyw43_arch_poll();
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/**
 * @brief Web server task implementation
 * @param pvParameters Task parameters (unused)
 */
static void server_task(void *pvParameters)
{
    while (1) {
        if (wifi_connected && server_pcb) {
            // Server is handled by LWIP callbacks
            vTaskDelay(pdMS_TO_TICKS(1000));
        } else {
            vTaskDelay(pdMS_TO_TICKS(5000));
        }
    }
}

/**
 * @brief TCP server accept callback
 */
static err_t tcp_server_accept(void *arg, struct tcp_pcb *client_pcb, err_t err)
{
    if (err != ERR_OK || client_pcb == NULL) {
        return ERR_VAL;
    }

    http_connection_t *con = malloc(sizeof(http_connection_t));
    if (!con) {
        tcp_close(client_pcb);
        return ERR_MEM;
    }

    con->pcb         = client_pcb;
    con->sent_len    = 0;
    con->header_sent = false;
    memset(con->headers, 0, sizeof(con->headers));
    memset(con->result, 0, sizeof(con->result));

    tcp_setprio(client_pcb, TCP_PRIO_MIN);
    tcp_arg(client_pcb, con);
    tcp_recv(client_pcb, tcp_server_recv);
    tcp_err(client_pcb, tcp_server_err);
    tcp_poll(client_pcb, NULL, HTTP_POLL_TIME_S * 2);

    return ERR_OK;
}

/**
 * @brief TCP server receive callback
 */
static err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    http_connection_t *con = (http_connection_t *)arg;

    if (!p) {
        tcp_server_close(con);
        return ERR_OK;
    }

    tcp_recved(tpcb, p->tot_len);

    if (p->tot_len > 0) {
        // Parse HTTP request
        char *request = malloc(p->tot_len + 1);
        if (request) {
            pbuf_copy_partial(p, request, p->tot_len, 0);
            request[p->tot_len] = '\0';

            // Extract path from request
            char *path  = NULL;
            char *query = NULL;
            if (strstr(request, "GET ") == request) {
                path        = request + 4;
                char *space = strchr(path, ' ');
                if (space) {
                    *space  = '\0';
                    char *q = strchr(path, '?');
                    if (q) {
                        *q    = '\0';
                        query = q + 1;
                    }
                }
            }

            if (path) {
                // Generate response
                if (strncmp(path, "/api/", 5) == 0) {
                    // API endpoint
                    char response[1024];
                    handle_api_request(path + 5, query, response, sizeof(response));

                    snprintf(con->result, sizeof(con->result),
                             "HTTP/1.1 200 OK\r\n"
                             "Content-Type: application/json\r\n"
                             "Access-Control-Allow-Origin: *\r\n"
                             "Content-Length: %d\r\n"
                             "Connection: close\r\n\r\n%s",
                             (int)strlen(response), response);
                } else {
                    // HTML page
                    char html_content[4096];
                    int  content_len = generate_html_page(path, html_content, sizeof(html_content));

                    snprintf(con->result, sizeof(con->result),
                             "HTTP/1.1 200 OK\r\n"
                             "Content-Type: text/html\r\n"
                             "Content-Length: %d\r\n"
                             "Connection: close\r\n\r\n%s",
                             content_len, html_content);
                }
            } else {
                // Bad request
                snprintf(con->result, sizeof(con->result),
                         "HTTP/1.1 400 Bad Request\r\n"
                         "Content-Length: 0\r\n"
                         "Connection: close\r\n\r\n");
            }

            free(request);
        }

        // Send response
        err_t wr_err = tcp_write(tpcb, con->result, strlen(con->result), TCP_WRITE_FLAG_COPY);
        if (wr_err == ERR_OK) {
            tcp_output(tpcb);
        }
    }

    pbuf_free(p);
    tcp_server_close(con);
    return ERR_OK;
}

/**
 * @brief TCP server error callback
 */
static void tcp_server_err(void *arg, err_t err)
{
    http_connection_t *con = (http_connection_t *)arg;
    if (con) {
        free(con);
    }
}

/**
 * @brief Close TCP connection
 */
static void tcp_server_close(http_connection_t *con)
{
    if (con && con->pcb) {
        tcp_arg(con->pcb, NULL);
        tcp_recv(con->pcb, NULL);
        tcp_err(con->pcb, NULL);
        tcp_poll(con->pcb, NULL, 0);
        tcp_close(con->pcb);
        free(con);
    }
}

/**
 * @brief Generate HTML page content
 */
static int generate_html_page(const char *path, char *buffer, size_t buffer_size)
{
    int len = 0;

    // Add header
    len += snprintf(buffer + len, buffer_size - len, "%s", html_header);

    if (strcmp(path, "/") == 0 || strcmp(path, "/index.html") == 0) {
        // Main page
        len += snprintf(buffer + len, buffer_size - len,
                        "<h1>🔍 KISS Fuzzer</h1>"
                        "<div class='status'>"
                        "<strong>Status:</strong> %s<br>"
                        "<strong>Power:</strong> %.1fV (%d%%)<br>"
                        "<strong>Mode:</strong> %s"
                        "</div>"
                        "<h2>JTAG Operations</h2>"
                        "<button onclick='startScan()'>🔎 Scan JTAG Chain</button>"
                        "<button onclick='detectPins()'>📍 Detect Pins</button>"
                        "<button onclick='readIdcode()'>🆔 Read IDCODE</button>"
                        "<button onclick='dumpMemory()'>💾 Dump Memory</button>"
                        "<h2>Tools</h2>"
                        "<button onclick='location.href=\"/logs\"'>📋 View Logs</button>"
                        "<button onclick='location.href=\"/files\"'>📁 File Manager</button>"
                        "<button onclick='location.href=\"/settings\"'>⚙️ Settings</button>"
                        "<div id='output' class='log'></div>"
                        "<script>"
                        "function apiCall(endpoint) {"
                        "  fetch('/api/' + endpoint)"
                        "    .then(r => r.json())"
                        "    .then(d => document.getElementById('output').textContent = "
                        "JSON.stringify(d, null, 2));"
                        "}"
                        "function startScan() { apiCall('jtag/scan'); }"
                        "function detectPins() { apiCall('jtag/detect'); }"
                        "function readIdcode() { apiCall('jtag/idcode'); }"
                        "function dumpMemory() { apiCall('jtag/dump'); }"
                        "</script>",
                        wifi_get_status_message(), power_get_voltage(), power_get_battery_percent(),
                        ap_mode ? "Access Point" : "Station");
    } else if (strcmp(path, "/logs") == 0) {
        // Logs page
        len += snprintf(buffer + len, buffer_size - len,
                        "<h1>📋 System Logs</h1>"
                        "<button onclick='location.href=\"/\"'>🏠 Home</button>"
                        "<button onclick='refreshLogs()'>🔄 Refresh</button>"
                        "<div id='logs' class='log'>Loading logs...</div>"
                        "<script>"
                        "function refreshLogs() {"
                        "  fetch('/api/logs')"
                        "    .then(r => r.text())"
                        "    .then(d => document.getElementById('logs').textContent = d);"
                        "}"
                        "refreshLogs();"
                        "</script>");
    } else if (strcmp(path, "/settings") == 0) {
        // Settings page
        len += snprintf(buffer + len, buffer_size - len,
                        "<h1>⚙️ Settings</h1>"
                        "<button onclick='location.href=\"/\"'>🏠 Home</button>"
                        "<h2>Wi-Fi Configuration</h2>"
                        "<input type='text' id='ssid' placeholder='SSID'>"
                        "<input type='password' id='password' placeholder='Password'>"
                        "<button onclick='connectWiFi()'>📶 Connect</button>"
                        "<button onclick='startAP()'>📡 Start AP Mode</button>"
                        "<h2>JTAG Settings</h2>"
                        "<label>Clock Speed: <select id='clock'>"
                        "<option value='1000'>1 MHz</option>"
                        "<option value='5000'>5 MHz</option>"
                        "<option value='10000'>10 MHz</option>"
                        "</select></label>"
                        "<button onclick='updateSettings()'>💾 Save</button>"
                        "<script>"
                        "function connectWiFi() {"
                        "  const ssid = document.getElementById('ssid').value;"
                        "  const pass = document.getElementById('password').value;"
                        "  fetch(`/api/wifi/connect?ssid=${ssid}&password=${pass}`);"
                        "}"
                        "function startAP() { fetch('/api/wifi/ap'); }"
                        "function updateSettings() {"
                        "  const clock = document.getElementById('clock').value;"
                        "  fetch(`/api/settings/jtag?clock=${clock}`);"
                        "}"
                        "</script>");
    } else {
        // 404 page
        len += snprintf(buffer + len, buffer_size - len,
                        "<h1>404 - Page Not Found</h1>"
                        "<button onclick='location.href=\"/\"'>🏠 Home</button>");
    }

    // Add footer
    len += snprintf(buffer + len, buffer_size - len, "%s", html_footer);

    return len;
}

/**
 * @brief Handle API requests
 */
static void handle_api_request(const char *path, const char *query, char *response,
                               size_t response_size)
{
    if (strncmp(path, "jtag/", 5) == 0) {
        const char *action = path + 5;

        if (strcmp(action, "scan") == 0) {
            jtag_chain_info_t chain;
            jtag_result_t     result = jtag_scan_chain(&chain);
            snprintf(response, response_size, "{\"status\":\"%s\",\"devices\":%d,\"length\":%d}",
                     (result == JTAG_OK) ? "success" : "error", chain.device_count,
                     chain.total_length);
        } else if (strcmp(action, "detect") == 0) {
            jtag_pins_t   pins;
            jtag_result_t result = jtag_detect_pins(&pins);
            snprintf(response, response_size,
                     "{\"status\":\"%s\",\"tck\":%d,\"tms\":%d,\"tdi\":%d,\"tdo\":%d}",
                     (result == JTAG_OK) ? "success" : "error", pins.tck, pins.tms, pins.tdi,
                     pins.tdo);
        } else if (strcmp(action, "idcode") == 0) {
            uint32_t      idcode;
            jtag_result_t result = jtag_read_idcode(&idcode);
            snprintf(response, response_size, "{\"status\":\"%s\",\"idcode\":\"0x%08X\"}",
                     (result == JTAG_OK) ? "success" : "error", idcode);
        } else {
            snprintf(response, response_size, "{\"error\":\"Unknown JTAG action\"}");
        }
    } else if (strncmp(path, "wifi/", 5) == 0) {
        const char *action = path + 5;

        if (strcmp(action, "status") == 0) {
            snprintf(response, response_size,
                     "{\"status\":\"%s\",\"message\":\"%s\",\"mode\":\"%s\"}",
                     (wifi_status == WIFI_STATUS_CONNECTED) ? "connected" : "disconnected",
                     wifi_get_status_message(), ap_mode ? "ap" : "sta");
        } else if (strcmp(action, "ap") == 0) {
            wifi_event_t event = {.type = WIFI_EVENT_START_AP};
            wifi_send_event(event);
            snprintf(response, response_size, "{\"status\":\"starting_ap\"}");
        } else if (strcmp(action, "connect") == 0 && query) {
            // Parse SSID and password from query
            char ssid[64]     = {0};
            char password[64] = {0};
            sscanf(query, "ssid=%63[^&]&password=%63s", ssid, password);

            wifi_event_t event = {.type = WIFI_EVENT_CONNECT_STA};
            strncpy(event.data.connect.ssid, ssid, sizeof(event.data.connect.ssid) - 1);
            strncpy(event.data.connect.password, password, sizeof(event.data.connect.password) - 1);
            wifi_send_event(event);

            snprintf(response, response_size, "{\"status\":\"connecting\"}");
        } else {
            snprintf(response, response_size, "{\"error\":\"Unknown WiFi action\"}");
        }
    } else if (strcmp(path, "status") == 0) {
        snprintf(response, response_size,
                 "{\"wifi\":\"%s\",\"power\":%.1f,\"battery\":%d,\"uptime\":%lu}",
                 wifi_get_status_message(), power_get_voltage(), power_get_battery_percent(),
                 xTaskGetTickCount() / configTICK_RATE_HZ);
    } else if (strcmp(path, "logs") == 0) {
        // Return recent log entries
        strncpy(response, "System logs would be here...", response_size - 1);
    } else {
        snprintf(response, response_size, "{\"error\":\"Unknown API endpoint\"}");
    }
}
