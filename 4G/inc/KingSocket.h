#ifndef __KING_SOCKET_H__
#define __KING_SOCKET_H__

#include "KingDef.h"
#include "KingRTC.h"

#define DNS_MAX_ADDR_COUNT      4

/*
 * Structure used for manipulating linger option.
 */
struct linger {
    int l_onoff;                /* option on/off */
    int l_linger;               /* linger time , second as unit*/
};

#define SOL_SOCKET      0xfff           /* socket level for get/set socket option */

//ipproto type
#define IPPROTO_IP      0
#define IPPROTO_ICMP    1
#define IPPROTO_TCP     6
#define IPPROTO_UDP     17
#define IPPROTO_IPV6    41
#define IPPROTO_ICMPV6  58
#define IPPROTO_UDPLITE 136
#define IPPROTO_RAW     255

#define PPP_AUTH_NONE   0
#define PPP_AUTH_PAP    1
#define PPP_AUTH_CHAP   2

// Address families.
#define AF_UNSPEC       0           /* unspecified */
#define AF_NS           1           /* local to host (pipes, portals) */
#define AF_INET         2           /* IPV4 internetwork: UDP, TCP, etc. */
#define AF_INET6        10           /* IPV6 internetwork: UDP, TCP, etc. */
#define PF_INET         AF_INET
#define PF_INET6        AF_INET6
#define PF_UNSPEC       AF_UNSPEC

// Socket protocol  Types(TCP/UDP/RAW)
#define SOCK_STREAM     1           /* stream socket */
#define SOCK_DGRAM      2           /* datagram socket */
#define SOCK_RAW        3           /* raw-protocol interface */

/* Flags we can use with send and recv. */
#define MSG_PEEK       0x01    /* Peeks at an incoming message */
#define MSG_WAITALL    0x02    /* Requests that the function block until the full amount of data requested can be returned */
#define MSG_OOB        0x04    /* Requests out-of-band data. The significance and semantics of out-of-band data are protocol-specific */
#define MSG_DONTWAIT   0x08    /* Nonblocking i/o for this operation only */
#define MSG_MORE       0x10    /* Sender will send more */
#define MSG_NOSIGNAL   0x20    /* Requests not to send the SIGPIPE signal if an attempt to send is made on a stream-oriented socket that is no longer connected. */

// socket shutdown options
#define SHUT_RECV         0
#define SHUT_SEND         1
#define SHUT_BOTH         2

#define SO_RXDATA       0x1011      /* get count of bytes in sb_rcv */
#define SO_NBIO         0x1014      /* set socket into NON-blocking mode */
#define SO_BIO          0x1015      /* set socket into blocking mode */

/*
 * Options for level SOL_SOCKET
 */
#define SO_REUSEADDR   0x0004 /* Allow local address reuse */
#define SO_KEEPALIVE   0x0008 /* keep connections alive */
#define SO_BROADCAST   0x0020 /* permit to send and to receive broadcast messages (see IP_SOF_BROADCAST option) */

/*
 * Additional options, Options for level SOL_SOCKET
 */
#define SO_ACCEPTCONN   0x0002 /* socket has had listen() */
#define SO_LINGER       0x0080 /* linger on close if data present */
#define SO_RCVBUF       0x1002 /* receive buffer size */
#define SO_SNDTIMEO     0x1005 /* send timeout */
#define SO_RCVTIMEO     0x1006 /* receive timeout */
#define SO_ERROR        0x1007 /* get error status and clear */
#define SO_TYPE         0x1008 /* get socket type */
#define SO_NO_CHECK     0x100a /* don't create UDP checksum */
#define SO_BINDTODEVICE 0x100b /* bind to device */

/*
 * Options for level IPPROTO_IP
 */
#define IP_TOS             1
#define IP_TTL             2

/*
 * Options for level IPPROTO_TCP
 */
#define TCP_NODELAY    0x01    /* don't delay send to coalesce packets */
#define TCP_KEEPALIVE  0x02    /* send KEEPALIVE probes when idle for pcb->keep_idle milliseconds */
#define TCP_KEEPIDLE   0x03    /* set pcb->keep_idle  - Same as TCP_KEEPALIVE, but use seconds for get/setsockopt */
#define TCP_KEEPINTVL  0x04    /* set pcb->keep_intvl - Use seconds for get/setsockopt */
#define TCP_KEEPCNT    0x05    /* set pcb->keep_cnt   - Use number of probes sent for get/setsockopt */

/*
 * Options for level IPPROTO_IPV6
 */
#define IPV6_CHECKSUM       7  /*  calculate and insert the ICMPv6 checksum for raw sockets. */
#define IPV6_V6ONLY         27 /* boolean control to restrict AF_INET6 sockets to IPv6 communications only. */

/**
 * Socket Events
 */
#define SOCKET_READ_EVENT_IND                   0x0001
#define SOCKET_READ_BUFFER_STATUS_EVENT_IND     0x0002
#define SOCKET_WRITE_EVENT_IND                  0x0003
#define SOCKET_CONNECT_EVENT_IND                0x0004
#define SOCKET_CONNECTION_CLOSE_EVENT_IND       0x0005
#define SOCKET_ACCEPT_EVENT_IND                 0x0006
#define SOCKET_FULL_CLOSED_EVENT_IND            0x0007
#define SOCKET_ERROR_EVENT_IND                  0x0008

#define SOCKET_CONNECT_EVENT_RSP                0X0101
#define SOCKET_CLOSE_EVENT_RSP                  0X0102
#define SOCKET_ASYNC_GETHOSTBYNAME_CNF          0x0103

#define TCPIP_PING_RESULT_IND                   0x0201
#define TCPIP_DHCP_RESULT_IND                   0x0202
#define TCPIP_IPCONFLICT_RESULT_IND             0x0203

#define IPADDR_ANY          ((uint32)0x00000000UL)

#define IP_GET_TYPE(ipaddr)           ((ipaddr)->type)

/**  ip4addr */
#define IP_IS_V4_VAL(ipaddr)          (IP_GET_TYPE(&ipaddr) == IP_ADDR_TYPE_V4)
/**  ip6addr */
#define IP_IS_V6_VAL(ipaddr)          (IP_GET_TYPE(&ipaddr) == IP_ADDR_TYPE_V6)

/**  ip4addr */
#define IP_IS_V4(ipaddr)              (((ipaddr) == NULL) || IP_IS_V4_VAL(*(ipaddr)))
/**  ip6addr */
#define IP_IS_V6(ipaddr)              (((ipaddr) != NULL) && IP_IS_V6_VAL(*(ipaddr)))

#define ip_2_ip4(ipaddr)   (&((ipaddr)->u_addr.ip4))
#define ip_2_ip6(ipaddr)   (&((ipaddr)->u_addr.ip6))

#define IP_SET_TYPE_VAL(ipaddr, iptype) do { (ipaddr).type = (iptype); }while(0)

#define ip4_addr_set_u32(dest_ipaddr, src_u32) ((dest_ipaddr)->addr = (src_u32))
#define ip4_addr_get_u32(src_ipaddr) ((src_ipaddr)->addr)

#define ip6_addr_isany_val(ip6addr) (((ip6addr).addr[0] == 0) && \
                                     ((ip6addr).addr[1] == 0) && \
                                     ((ip6addr).addr[2] == 0) && \
                                     ((ip6addr).addr[3] == 0))
#define ip6_addr_isany(ip6addr) (((ip6addr) == NULL) || ip6_addr_isany_val(*(ip6addr)))

#define ip4_addr_isany_val(addr1)   ((addr1).addr == IPADDR_ANY)
#define ip4_addr_isany(addr1) ((addr1) == NULL || ip4_addr_isany_val(*(addr1)))

#define ip_addr_isany(ipaddr)        ((IP_IS_V6(ipaddr)) ? \
  ip6_addr_isany(ip_2_ip6(ipaddr)) : \
  ip4_addr_isany(ip_2_ip4(ipaddr))) 


typedef enum
{
    /** IPv4 */
    IP_ADDR_TYPE_V4 =   0U,
    /** IPv6 */
    IP_ADDR_TYPE_V6 =   6U,
    /** IPv4+IPv6 ("dual-stack") */
    IP_ADDR_TYPE_ANY = 46U
} IP_ADDR_TYPE_E;

typedef struct ipv4_addr {
    uint32 addr;
} IPV4_ADDR_S;

typedef struct ip6_addr {
    uint32 addr[4];
    uint8 zone;
} IPV6_ADDR_S;

typedef struct ipv4_dns {
    IPV4_ADDR_S primary_dns;
    IPV4_ADDR_S secondary_dns;
} IPV4_DNS;

typedef struct ipv6_dns {
    IPV6_ADDR_S primary_dns;
    IPV6_ADDR_S secondary_dns;
} IPV6_DNS;

typedef struct {
    union {
        IPV6_ADDR_S ip6;
        IPV4_ADDR_S ip4;
    } u_addr;
    uint8 type;  /* @ref IP_ADDR_TYPE_E */
} SOCK_IN_ADDR_T;

typedef enum
{
    DNS_PARSE_SUCCESS = 0,
    DNS_PARSE_TIMEOUT,
    DNS_PARSE_ERROR,
    DNS_PARSE_MAX,
}DNS_PARSE_RESULT_E;


typedef struct 
{
    DNS_PARSE_RESULT_E result; //dns 解析结果
    SOCK_IN_ADDR_T addr; //dns 解析返回的地址
    char* hostname; //主机名
} ASYN_DNS_PARSE_T;

typedef struct 
{
    DNS_PARSE_RESULT_E result;  //dns result,see DNS_PARSE_RESULT_E
    SOCK_IN_ADDR_T addr[DNS_MAX_ADDR_COUNT]; //DNS ip addr
    char* hostname; //hostname
    int count; //dns ip addr count
} ASYN_MULTI_DNS_PARSE_T;

typedef unsigned int PING_HANDLE;

typedef enum
{
    PINGRES_SUCCESS         = 0,    ///< ping OK, received ping echo reply
    PINGRES_DNS_TIMEOUT     = 1,    ///< ping fail, DNS timeout
    PINGRES_DNS_ERROR       = 2,    ///< ping fail, DNS error
    PINGRES_ICMP_TIMEOUT    = 3,    ///< ping fail, icmp timeout
    PINGRES_ICMP_ERROR      = 4     ///< ping fail, icmp error
}TCPIP_PING_RESULT_E;

typedef void (*DNS_CALLBACK)(void* pData, uint32 len);

typedef void (*TCPIP_PING_CALLBACK)(
    TCPIP_PING_RESULT_E     res,          ///< ping result, 0 - succeed; other - fail
    uint32                  time_delay,   ///< ping time delay, only valid when success, unit: ms
    PING_HANDLE             ping_handle,  ///< ping handle
    uint8                   ttl,
    char*                   ipaddr        ///< uint16 ipaddr_size*/
    );

typedef struct in_addr {
    uint32 addr;
} IN_ADDR_T;

typedef struct in6_addr {
    uint32 addr[4];
} IN6_ADDR_T;

typedef struct sockaddr_in
{
    uint8 sin_len;         ///< address family
    uint8 sin_family;         ///< address family
    uint16 sin_port;           ///< port number
    IN_ADDR_T sin_addr;        ///< ip address
    char sa_data[8];     ///< up to 14 bytes of direct address
} SOCK_ADDR_IN_T;

typedef struct sockaddr_in6 {
  uint8 sin6_len;             /* length of this structure */
  uint8 sin6_family;          /* AF_INET6                 */
  uint16 sin6_port;           /* Transport layer port #   */
  uint32 sin6_flowinfo;       /* IPv6 flow information    */
  IN6_ADDR_T sin6_addr;      /* IPv6 address             */
  uint32 sin6_scope_id;     /* Set of interfaces for scope */
} SOCK_ADDR_IN6_T;

typedef struct sockaddr {
    uint8 sa_len;
    uint8 sa_family;
    char sa_data[26];
} SOCK_ADDR_T;

typedef union sockaddr_aligned
{
    SOCK_ADDR_T sa;
    SOCK_ADDR_IN_T  sin;  /**< IPv4 address. */
    SOCK_ADDR_IN6_T sin6;  /**< IPv6 address. */
} SOCK_ADDR_ALIGNED_U;

typedef enum
{
    NIC_TYPE_NONE,
    NIC_TYPE_WWAN,
    NIC_TYPE_WLAN,
    NIC_TYPE_ETHERNET
} NIC_TYPE_E;

typedef int32 hardwareHandle; //Hardware NIC, such as WWAN, WLAN, Ethernet
typedef struct _tagHardwareNIC {
    uint8 NIC_Type; //WWAN, WLAN, Ethernet (NIC_TYPE_E)
    uint8 NIC_Protocol; // Direct, PPP, L2TP, Proxy ...
    uint32 NIC_ThroughPut; //Max Throuthput, bps ...
} NIC_INFO;

typedef int32 softwareHandle; //kinds of Qos based APNs
typedef struct _tagSoftwareContext {
    uint8 APN_Name[16];
    uint8 USER_Name[16];
    uint8 Password[16];
    uint8 AuthType; //pap, chap...
    uint8 IpType;/* @ref IP_ADDR_TYPE_E */
} NIC_CONTEXT;

// from LWIP stack.
#ifndef SHUT_RD
  #define SHUT_RD   0
  #define SHUT_WR   1
  #define SHUT_RDWR 2
#endif

#ifndef FD_SET
#define SOCKET_OFFSET           1
#define MEMP_NUM_NETCONN        8
#define FD_SETSIZE              MEMP_NUM_NETCONN
#define FDSETSAFESET(n, code) do { \
  if (((n) - SOCKET_OFFSET < MEMP_NUM_NETCONN) && (((int)(n) - SOCKET_OFFSET) >= 0)) { \
  code; }} while(0)
#define FDSETSAFEGET(n, code) (((n) - SOCKET_OFFSET < MEMP_NUM_NETCONN) && (((int)(n) - SOCKET_OFFSET) >= 0) ?\
  (code) : 0)
#define FD_SET(n, p)  FDSETSAFESET(n, (p)->fd_bits[((n)-SOCKET_OFFSET)/8] |=  (1 << (((n)-SOCKET_OFFSET) & 7)))
#define FD_CLR(n, p)  FDSETSAFESET(n, (p)->fd_bits[((n)-SOCKET_OFFSET)/8] &= ~(1 << (((n)-SOCKET_OFFSET) & 7)))
#define FD_ISSET(n,p) FDSETSAFEGET(n, (p)->fd_bits[((n)-SOCKET_OFFSET)/8] &   (1 << (((n)-SOCKET_OFFSET) & 7)))
#define FD_ZERO(p)    memset((void*)(p), 0, sizeof(*(p)))

typedef struct fd_set
{
  unsigned char fd_bits[(FD_SETSIZE+7)/8];
} fd_set;
#endif // FD_SET
// End from LWIP stack


/**
 * Create Socket
 *
 * @param[in]  domain    Same as address family. AF_INET, AF_INET6, etc.
 * @param[in]  type      Socket Type. SOCK_STREAM SOCK_DGRAM
 * @param[in]  protocol  protocol Type. IPPROTO_IP,IPPROTO_ICMP,IPPROTO_TCP,IPPROTO_UDP,etc.
 * @param[inout] sockId  New socket ID.
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SocketCreate(int domain, int type, int protocol, int* sockId);

/**
 * bind() assigns the address specified by addr to the socket referred to by
 * the file descriptor sockfd.  addrlen specifies the size, in bytes, of the
 * address structure pointed to by addr.
 * Traditionally, this operation is called “assigning a name to a socket”.
 * It is normally necessary to assign a local address using bind().
 *
 * @param[in] sockfd    socket ID.
 * @param[in] addr      local socket address.
 * @param[in] addrlen   addr structure size.
 *
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SocketBind(int sockfd, const SOCK_ADDR_ALIGNED_U *addr, uint32 addrlen);

/**
 * Socket listen
 *
 * @param[in] sockfd    socket ID.
 * @param[in] backlog   允许接收的客户端数量
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SocketListen(int sockfd, int backlog);

/**
 * Socket connect
 *
 * @param[in] sockfd    socket ID.
 * @param[in] addr      address
 * @param[in] addrlen   addr structure size.
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SocketConnect(int sockfd, const SOCK_ADDR_ALIGNED_U *addr, uint32 addrlen);

/**
 * Socket accept
 *
 * @param[in] sockfd    socket ID.
 * @param[in] addr      address
 * @param[in] addrlen   addr structure size.
 * @return the socket descriptor of the new socket on success   -1 FAIL
 */
int KING_SocketAccept(int sockfd, SOCK_ADDR_ALIGNED_U *addr, uint32 *addrlen);


/**
 * @brief Sends a specified number of bytes in a buffer over TCP transmission.
 *
 * @param[in]  sockfd    Socket FD
 * @param[in]  buf       user buffer from which to copy data.
 * @param[in]  len       number of bytes to be send to socket.
 * @param[in]  flags     Refer to socket flag. Set it to 0 normally.
 * @return the number of bytes to be send, which could be less than the 
 *    number of bytes specified -1 FAIL
 */
int KING_SocketSend(int sockfd, const void *buf, size_t len, int flags);

/**
 * @brief receive specified number of bytes into buffer from the TCP transport.
 *
 * @param[in]  sockfd    Socket FD
 * @param[in]  buf       user buffer to which to copy data.
 * @param[in]  len       number of bytes to be received from socket.
 * @param[in]  flags     Refer to socket flag. Set it to 0 normally.
 * @return the number of bytes read, which could be less than the number of
 *    bytes specified. -1 FAIL
 */
int KING_SocketRecv(int sockfd, void *buf, size_t len, int flags);


/**
 * @brief sendto() is send a specified number of bytes in a buffer over TCP transmission.
 *
 * @note If sendto() is used on a connection-mode (SOCK_STREAM) socket,
 * the arguments dest_addr and addrlen  are  ignored  (and  the  error
 * EISCONN  may be returned when they are not NULL and 0), and the error
 * ENOTCONN is returned when the socket was not actually connected.
 * Otherwise, the address of the target is given by dest_addr with addrlen
 * specifying its size.
 *
 * @param[in]  sockfd    Socket FD
 * @param[in]  buf       Pointer to Send Buffer.
 * @param[in]  len       Length to send of data in send buffer.
 * @param[in]  flags     Refer to socket flag. Set it to 0 normally.
 * @param[in]  dest_addr target address of the message.
 * @param[in]  addrlen   the size of the buffer associated with src_addr.
 *
 * @return the number of bytes to be send, which could be less than the number
 *    of bytes specified   -1 FAIL
 */
int KING_SocketSendto(int sockfd, const void *buf, size_t len, int flags,
                      const SOCK_ADDR_ALIGNED_U *dest_addr, uint32 addrlen);

/**
 * @brief RecvFrom() receive messages from a socket.
 * it may be used to receive data on both connectionless and connection-oriented sockets.
 * recvfrom() places the received message into the buffer buf. The caller must specify
 * the size of the buffer in len.
 *
 * @brief If src_addr is not NULL, and the underlying protocol provides the source address
 * of the message, that source address is placed in the buffer pointed  to  by  src_addr.
 * In this case, addrlen is a value-result argument.  Before the call, it should be
 * initialized to the size of the buffer associated with src_addr.
 *
 * @note If the caller is not interested in the source address, src_addr and addrlen should
 * be specified as NULL. Also, the following call:
 *
 *     recv(sockfd, buf, len, flags);
 *
 *     is equivalent to
 *
 *     recvfrom(sockfd, buf, len, flags, NULL, NULL);
 *
 * @param[in]  sockfd    Socket FD
 * @param[in]  buf       Pointer to receive buffer.
 * @param[in]  len       the size of receive buffer (could be less than the real buffer size.)
 * @param[in]  flags     Refer to socket flag. Set it to 0 normally.
 * @param[inout] src_addr source address of the message.
 * @param[inout] addrlen  the size of the buffer associated with src_addr.
 *
 * @return the number of bytes to be written, which could be less than the number
 *    of bytes specified   -1 FAIL
 */
int KING_SocketRecvfrom(int sockfd, void *buf, size_t len, int flags,
                        SOCK_ADDR_ALIGNED_U *src_addr, uint32 *addrlen);

/**
 * socket select
 *
 * @param[in]  maxfdp1   max Socket FD add 1
 * @param[in]  readset   socket read set
 * @param[in]  writeset  socket write set
 * @param[in]  exceptset  socket except set
 * @param[in]  timeout  socket wait timeout
 * @return >= 0  -1 FAIL
 */
int KING_SocketSelect(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset, struct timeval *timeout);

/**
 * socket close
 *
 * @param[in]  sockfd  Socket FD
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SocketClose(int sockfd);


/**
 * shut down part of a full-duplex connection
 *
 * @param[in]  sockfd    Socket FD
 * @param[in]  how       Ways of shutdown socket connection. SHUT_SEND, SHUT_RECV, etc.
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SocketShutdown(int sockfd, int how);


/**
 * get options on sockets
 *
 * @param[in]  sockfd    Socket FD
 * @param[in]  level     Protocl level. For now only support SOL_SOCKET.
 * @param[in]  optname   Option Name. SO_BIO, SO_NBIO
 * @param[in]  optval    Option Value. Can be null if no values for option name.
 * @param[in]  optlen    Length of option value. Can be null if no option value.
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SocketOptGet(int sockfd, int level, int optname, void* optval, uint32* optlen);


/**
 * set options on sockets
 *
 * @param[in]  sockfd    Socket FD
 * @param[in]  level     Protocl level. For now only support SOL_SOCKET.
 * @param[in]  optname   Option Name. SO_BIO, SO_NBIO
 * @param[in]  optval    Option Value. Can be null if no values for option name.
 * @param[in]  optlen    Length of option value. Can be 0 if no option value.
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SocketOptSet(int sockfd, int level, int optname, void* optval, uint32 optlen);

/**
 * get a Handle of NIC
 *
 * @param[in]  info    NIC info
 * @param[out] hNIC  Handle of NIC
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_HardwareNICRequest(NIC_INFO info, hardwareHandle *hNIC);

/**
 * Acquire a Handle of context by Handle of NIC
 *
 * @param[in]  hNIC    Handle of NIC
 * @param[in] ApnInfo  apn info
 * @param[out] hContext Handle of context
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_ConnectionAcquire(hardwareHandle hNIC, NIC_CONTEXT ApnInfo, softwareHandle *hContext);

/**
 * Release a Handle of NIC
 *
 * @param[in] hContext Handle of context
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_ConnectionRelease(softwareHandle hContext);

/**
 * 查询指定pdp的ip(ipv4类型)等信息
 *
 * @param[in] hContext       Handle of context
 * @param[out] ip       ipv4地址
 * @param[out] dns      使用的dns地址
 * @param[out] gateway  使用的网关地址
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_NetIpv4AddrInfoGet(softwareHandle hContext, IPV4_ADDR_S* ip, IPV4_DNS* dns, IPV4_ADDR_S* gateway);

/**
 * 查询指定pdp的ip(ipv6类型)等信息
 *
 * @param[in] hContext       Handle of context
 * @param[out] ip       ipv6地址
 * @param[out] dns      使用的dns地址
 * @param[out] gateway  使用的网关地址
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_NetIpv6AddrInfoGet(softwareHandle hContext, IPV6_ADDR_S* ip, IPV6_DNS* dns, IPV6_ADDR_S* gateway);

/**
 * ipv4网络下做ping
 *
 * @param[in]  hContext        Handle of context
 * @param[in]  pAddr      ping的地址
 * @param[in]  data_size  每次ping数据块大小
 * @param[in]  time_out   ping超时时间，单位ms
 * @param[in]  fun        ping回调函数，启动ping后，底层每ping一次调用fun一次告知app相应信息
 * @param[out] pHandle    每次启动ping给的对应句柄，用于停止ping动作时使用。
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_NetPingV4Request(softwareHandle hContext, char* pAddr, uint32 data_size, uint32 time_out, TCPIP_PING_CALLBACK fun, PING_HANDLE* pHandle);

/**
 * ipv6网络下做ping
 *
 * @param[in]  hContext       Handle of context
 * @param[in]  pAddr      ping的地址
 * @param[in]  data_size  每次ping数据块大小
 * @param[in]  time_out   ping超时时间，单位ms
 * @param[in]  fun        ping回调函数，启动ping后，底层每ping一次调用fun一次告知app相应信息
 * @param[out] pHandle    每次启动ping给的对应句柄，用于停止ping动作时使用。
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_NetPingV6Request(softwareHandle hContext, char* pAddr,uint32 data_size,uint32 time_out,TCPIP_PING_CALLBACK fun,PING_HANDLE* pHandle);

/**
 * 停止ping动作
 *
 * @param[in]  pHandle        启动ping时获取到的handle
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_NetPingCancel(PING_HANDLE pHandle);

/**
 * 根据指定类型做域名解析。
 * APP通过指定SOCK_IN_ADDR_T中的type类型来明确
 * 期望解析得到的IP地址类型。解析得到的IP地址
 * 存放到SOCK_IN_ADDR_T中的u_addr返回给APP。
 *
 * 异步返回时，API返回值为0，但hostIP直接被设置成NULL。
 * APP使用hostIP时应先检查hostIP是否为NULL。
 * 异步callback返回的数据类型为ASYN_DNS_PARSE_T
 *
 * @param[in]  hContext        Handle of context
 * @param[in]  name_ptr   要解析的域名
 * @param[in]  time_out   超时时间，单位ms
 * @param[in]  fun        异步返回时调用的callback函数
 * @param[inout] hostIP  域名解析得到的IP地址，可以是IPV4/IPV6
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_NetGetHostByName(softwareHandle hContext, char* name_ptr, SOCK_IN_ADDR_T *hostIP, uint32 time_out, DNS_CALLBACK fun);


/**
 * 配置IPV4 dns
 *
 * @param[in]  hContext        Handle of context
 * @param[in]  dns        设置的dns
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_NetIpv4DnsSet(softwareHandle hContext, IPV4_DNS dns);

/**
 * 配置IPV6 dns
 *
 * @param[in]  hContext        Handle of context
 * @param[in]  dns        设置的dns
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_NetIpv6DnsSet(softwareHandle hContext, IPV6_DNS dns);

/**
 * 配置APN等相关信息
 *
 * @param[in] cid    PDP context id
 * @param[in] APN    指定的apn 如 cmnet
 * @param[in] user   用户名
 * @param[in] password 密码
 * @param[in] authType 认证类型 PPP_AUTH_NONE/PAP/CHAP
 * @param[in] ipType   IP_TYPE_V4  IP_TYPE_V6  IP_TYPE_V4V6
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_NetApnSet(uint32 cid, char* APN, char* user, char* password, uint32 authType, uint32 ipType);

/**
 * set Socket api mode
 *
 * @param[in]  flag    0 no native api mode, 1 native api mode.default set 0
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SocketNativeApiSet(int flag);

/**
 * 根据指定类型做域名解析。
 * APP通过指定SOCK_IN_ADDR_T中的type类型来明确
 * 期望解析得到的IP地址类型。解析得到的IP地址
 * 存放到SOCK_IN_ADDR_T中的u_addr返回给APP,最多返回4个。
 *
 * 异步返回时，API返回值为0，但hostIP直接被设置成NULL。
 * APP使用hostIP时应先检查hostIP是否为NULL。
 * 异步callback返回的数据类型为ASYN_MULTI_DNS_PARSE_T
 *
 * @param[in]  hContext        Handle of context
 * @param[in]  name_ptr   要解析的域名
 * @param[in]  time_out   超时时间，单位ms
 * @param[in]  fun        异步返回时调用的callback函数
 * @param[inout] hostIP  域名解析得到的IP地址，可以是IPV4/IPV6,输入时要分配4个IP的空间
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_NetGetAllHostByName(softwareHandle hContext, char* name_ptr, SOCK_IN_ADDR_T *hostIP, uint32 time_out, DNS_CALLBACK fun);

/**
 * Convert IP address string (both versions) to numeric.
 * The version is auto-detected from the string.
 *
 * @param[in] ipStr IP address string to convert
 * @param[out] ipAddr conversion result is stored here
 * @return 0 on success, -1 on error
 */
int KING_ipaton(char* ipStr, SOCK_IN_ADDR_T *ipAddr);

/**
 * Convert numeric IP address into decimal dotted ASCII representation.
 *
 * @param[in] ipAddr ip address in network order to convert
 * @param[out] ipStr pointer to the buffer that holds the ASCII
 *         representation of addr
 * @return 0 on success, -1 on error
 */
int KING_ipntoa(SOCK_IN_ADDR_T ipAddr, char **ipStr);


/*
 * On some platforms host byte order is Least Significant Byte first,
 * whereas the network byte order, as used on the Internet,
 * is Most Significant Byte first.
 */

/**
 * Converts the unsigned integer hostlong from host byte order to network byte order.
 
 * @param[in] hostlong  the address of host
 * @return 0 on success, -1 on error
 */
uint32 KING_htonl(uint32 hostlong);

/**
 * Converts the unsigned short integer hostshort from host byte order to network byte order.
 * @param[in] hostshort  the address of host
 * @return 0 on success, -1 on error
 */
uint16 KING_htons(uint16 hostshort);

/**
 * Converts the unsigned integer netlong from network byte order to host byte order.
 * @param[in] netlong  the address of net
 * @return 0 on success, -1 on error
 */
uint32 KING_ntohl(uint32 netlong);

/**
 * Converts the unsigned short integer netshort from network byte order to host byte order.
 * @param[in] netshort  the address of net
 * @return 0 on success, -1 on error
 */
uint16 KING_ntohs(uint16 netshort);


#endif

