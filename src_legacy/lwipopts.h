#ifndef LWIP_LWIPOPTS_H
#define LWIP_LWIPOPTS_H

/**
 * @file lwipopts.h
 * @brief lwIP configuration for KISS Fuzzer
 * 
 * Based on Pico W standard configuration with FreeRTOS support
 */

// Platform specific includes
#include <stdint.h>

// System configuration - using NO_SYS with polling mode
#define NO_SYS                          1
#define LWIP_SOCKET                     0
#define LWIP_NETCONN                    0
#define LWIP_NETIF_API                  0

// Polling mode configuration
#define LWIP_FREERTOS                   0
#define LWIP_TCPIP_CORE_LOCKING         0
#define LWIP_TCPIP_CORE_LOCKING_INPUT   0

// Memory configuration
#define MEM_LIBC_MALLOC                 0
#define LWIP_MEM_ALIGN_SIZE(size)       (((size) + 3U) & ~3U)
#define MEM_ALIGNMENT                   4
#define MEM_SIZE                        (8 * 1024)
#define MEMP_NUM_PBUF                   12
#define MEMP_NUM_RAW_PCB                4
#define MEMP_NUM_UDP_PCB                4
#define MEMP_NUM_TCP_PCB                8
#define MEMP_NUM_TCP_PCB_LISTEN         8
#define MEMP_NUM_TCP_SEG                16
#define MEMP_NUM_ALTCP_PCB              MEMP_NUM_TCP_PCB
#define MEMP_NUM_REASSDATA              1
#define MEMP_NUM_FRAG_PBUF              4
#define MEMP_NUM_ARP_QUEUE              2
#define MEMP_NUM_IGMP_GROUP             2
#define MEMP_NUM_SYS_TIMEOUT            8
#define MEMP_NUM_NETBUF                 2
#define MEMP_NUM_NETCONN                8
#define MEMP_NUM_TCPIP_MSG_API          8
#define MEMP_NUM_TCPIP_MSG_INPKT        8
#define MEMP_NUM_NETDB                  1

// ARP configuration
#define LWIP_ARP                        1
#define ARP_TABLE_SIZE                  4
#define ARP_QUEUEING                    1

// IP configuration
#define LWIP_IPV4                       1
#define LWIP_IPV6                       0
#define IP_FORWARD                      0
#define IP_OPTIONS_ALLOWED              1
#define IP_REASSEMBLY                   1
#define IP_FRAG                         1
#define IP_REASS_MAXAGE                 3
#define IP_REASS_MAX_PBUFS              4
#define IP_DEFAULT_TTL                  255

// ICMP configuration
#define LWIP_ICMP                       1
#define LWIP_BROADCAST_PING             1
#define LWIP_MULTICAST_PING             1

// RAW protocol configuration
#define LWIP_RAW                        1

// DHCP configuration
#define LWIP_DHCP                       1
#define DHCP_DOES_ARP_CHECK             1

// UDP configuration
#define LWIP_UDP                        1
#define LWIP_UDPLITE                    0
#define UDP_TTL                         255
#define LWIP_NETBUF_RECVINFO            0

// TCP configuration
#define LWIP_TCP                        1
#define TCP_TTL                         255
#define TCP_WND                         (4 * TCP_MSS)
#define TCP_MAXRTX                      12
#define TCP_SYNMAXRTX                   6
#define TCP_QUEUE_OOSEQ                 1
#define TCP_MSS                         1460
#define TCP_CALCULATE_EFF_SEND_MSS      1
#define TCP_SND_BUF                     (4 * TCP_MSS)
#define TCP_SND_QUEUELEN                (2 * (TCP_SND_BUF) / (TCP_MSS))
#define TCP_SNDLOWAT                    ((TCP_SND_BUF) / 2)
#define TCP_LISTEN_BACKLOG              1
#define LWIP_TCP_KEEPALIVE              1

// PBUF configuration
#define PBUF_LINK_HLEN                  14
#define PBUF_POOL_BUFSIZE               LWIP_MEM_ALIGN_SIZE(TCP_MSS + 40 + PBUF_LINK_HLEN)

// Network interfaces
#define LWIP_NETIF_HOSTNAME             1
#define LWIP_NETIF_API                  1
#define LWIP_NETIF_STATUS_CALLBACK      1
#define LWIP_NETIF_LINK_CALLBACK        1
#define LWIP_NETIF_REMOVE_CALLBACK      1
#define LWIP_NETIF_HWADDRHINT           1

// Loopback interface
#define LWIP_HAVE_LOOPIF                0

// Threading and protection
#define LWIP_TCPIP_TIMEOUT              1
#define LWIP_NETCONN                    1
#define LWIP_SOCKET                     0 // Disable BSD sockets

// Statistics
#define LWIP_STATS                      0
#define LWIP_STATS_DISPLAY              0

// PPP support (disabled)
#define PPP_SUPPORT                     0

// Checksum configuration
#define LWIP_CHECKSUM_CTRL_PER_NETIF    1
#define CHECKSUM_GEN_IP                 1
#define CHECKSUM_GEN_UDP                1
#define CHECKSUM_GEN_TCP                1
#define CHECKSUM_GEN_ICMP               1
#define CHECKSUM_CHECK_IP               1
#define CHECKSUM_CHECK_UDP              1
#define CHECKSUM_CHECK_TCP              1
#define CHECKSUM_CHECK_ICMP             1

// Debug configuration
#define LWIP_DEBUG                      0
#define LWIP_DBG_MIN_LEVEL              LWIP_DBG_LEVEL_ALL
#define LWIP_DBG_TYPES_ON               LWIP_DBG_ON

// DNS configuration
#define LWIP_DNS                        1
#define DNS_TABLE_SIZE                  2
#define DNS_MAX_NAME_LENGTH             256

// SNMP configuration (disabled)
#define LWIP_SNMP                       0

// IGMP configuration
#define LWIP_IGMP                       0

// Autoip configuration (disabled)
#define LWIP_AUTOIP                     0

// DHCP configuration
#define LWIP_DHCP_AUTOIP_COOP           0

// Application specific options
#define LWIP_HTTPD                      0
#define LWIP_HTTPD_CGI                  0
#define LWIP_HTTPD_SSI                  0

// lwIP errno support
#define LWIP_ERRNO_STDINCLUDE           1

// Platform specific thread/mutex functions
#define LWIP_COMPAT_MUTEX               0

#endif /* LWIP_LWIPOPTS_H */
