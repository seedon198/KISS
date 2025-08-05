/**
 * @file lwipopts.h
 * @brief lwIP configuration options for KISS Fuzzer
 * @author KISS Fuzzer Team
 * @date 2025
 */

#ifndef LWIPOPTS_H
#define LWIPOPTS_H

// Platform specific locking
#define NO_SYS                     0
#define LWIP_SOCKET                0
#define LWIP_NETCONN               0
#define LWIP_NETIF_API             0

// Memory options
#define MEM_LIBC_MALLOC            0
#define MEMP_MEM_MALLOC            0
#define MEM_ALIGNMENT              4
#define MEM_SIZE                   4000
#define MEMP_NUM_PBUF              4
#define MEMP_NUM_UDP_PCB           4
#define MEMP_NUM_TCP_PCB           4
#define MEMP_NUM_TCP_PCB_LISTEN    2
#define MEMP_NUM_TCP_SEG           8
#define MEMP_NUM_ALTCP_PCB         MEMP_NUM_TCP_PCB
#define MEMP_NUM_REASSDATA         1
#define MEMP_NUM_ARP_QUEUE         2
#define MEMP_NUM_IGMP_GROUP        2
#define MEMP_NUM_SYS_TIMEOUT       4

// Pbuf options
#define PBUF_POOL_SIZE             4
#define LWIP_PBUF_REF_T            u8_t

// TCP options
#define LWIP_TCP                   1
#define TCP_TTL                    255
#define TCP_QUEUE_OOSEQ            0
#define TCP_MSS                    536
#define TCP_SND_BUF                2048
#define TCP_SND_QUEUELEN           4
#define TCP_WND                    2048
#define LWIP_TCP_KEEPALIVE         1

// UDP options
#define LWIP_UDP                   1
#define UDP_TTL                    255

// DHCP options
#define LWIP_DHCP                  1
#define DHCP_DOES_ARP_CHECK        0

// DNS options
#define LWIP_DNS                   1
#define DNS_TABLE_SIZE             2
#define DNS_MAX_NAME_LENGTH        64

// IGMP options
#define LWIP_IGMP                  1

// Checksum options
#define LWIP_CHECKSUM_CTRL_PER_NETIF 1

// IPv4 options
#define LWIP_IPV4                  1
#define LWIP_ARP                   1
#define ARP_QUEUEING               1

// IPv6 options
#define LWIP_IPV6                  0

// Hook options
#define LWIP_NETIF_HOSTNAME        1

// Threading options
#define TCPIP_THREAD_STACKSIZE     1024
#define DEFAULT_THREAD_STACKSIZE   1024
#define DEFAULT_RAW_RECVMBOX_SIZE  2
#define TCPIP_MBOX_SIZE            4
#define DEFAULT_UDP_RECVMBOX_SIZE  2
#define DEFAULT_TCP_RECVMBOX_SIZE  2
#define DEFAULT_ACCEPTMBOX_SIZE    2

// Poll options
#define LWIP_SOCKET_POLL           0

// Statistics options
#define LWIP_STATS                 0

// PPP options
#define PPP_SUPPORT                0

// Debugging options (disable for production)
#define LWIP_DEBUG                 0

#endif /* LWIPOPTS_H */
