#ifndef __KING_ERR_CODE_H__
#define __KING_ERR_CODE_H__

/* ======================================================
 *                        Error Number
 * ======================================================
 */
#define EN_EPERM         1    /* Operation not permitted */
#define EN_ENOENT        2    /* No such file or directory */
#define EN_ESRCH         3    /* No such process */
#define EN_EINTR         4    /* Interrupted system call */
#define EN_EIO           5    /* I/O error */
#define EN_ENXIO         6    /* No such device or address */
#define EN_E2BIG         7    /* Argument list too long */
#define EN_ENOEXEC       8    /* Exec format error */
#define EN_EBADF         9    /* Bad file number */
#define EN_ECHILD        10    /* No child processes */
#define EN_EAGAIN        11    /* Try again */
#define EN_ENOMEM        12    /* Out of memory */
#define EN_EACCES        13    /* Permission denied */
#define EN_EFAULT        14    /* Bad address */
#define EN_ENOTBLK       15    /* Block device required */
#define EN_EBUSY         16    /* Device or resource busy */
#define EN_EEXIST        17    /* File exists */
#define EN_EXDEV         18    /* Cross-device link */
#define EN_ENODEV        19    /* No such device */
#define EN_ENOTDIR       20    /* Not a directory */
#define EN_EISDIR        21    /* Is a directory */
#define EN_EINVAL        22    /* Invalid argument */
#define EN_ENFILE        23    /* File table overflow */
#define EN_EMFILE        24    /* Too many open files */
#define EN_ENOTTY        25    /* Not a typewriter */
#define EN_ETXTBSY       26    /* Text file busy */
#define EN_EFBIG         27    /* File too large */
#define EN_ENOSPC        28    /* No space left on device */
#define EN_ESPIPE        29    /* Illegal seek */
#define EN_EROFS         30    /* Read-only file system */
#define EN_EMLINK        31    /* Too many links */
#define EN_EPIPE         32    /* Broken pipe */
#define EN_EDOM          33    /* Math argument out of domain of func */
#define EN_ERANGE        34    /* Math result not representable */
#define EN_EDEADLK       35    /* Resource deadlock would occur */
#define EN_ENAMETOOLONG  36    /* File name too long */
#define EN_ENOLCK        37    /* No record locks available */
#define EN_ENOSYS        38    /* Function not implemented */
#define EN_ENOTEMPTY     39    /* Directory not empty */
#define EN_ELOOP         40    /* Too many symbolic links encountered */
#define EN_EWOULDBLOCK   EN_EAGAIN    /* Operation would block */
#define EN_ENOMSG        42    /* No message of desired type */
#define EN_EIDRM         43    /* Identifier removed */
#define EN_ECHRNG        44    /* Channel number out of range */
#define EN_EL2NSYNC      45    /* Level 2 not synchronized */
#define EN_EL3HLT        46    /* Level 3 halted */
#define EN_EL3RST        47    /* Level 3 reset */
#define EN_ELNRNG        48    /* Link number out of range */
#define EN_EUNATCH       49    /* Protocol driver not attached */
#define EN_ENOCSI        50    /* No CSI structure available */
#define EN_EL2HLT        51    /* Level 2 halted */
#define EN_EBADE         52    /* Invalid exchange */
#define EN_EBADR         53    /* Invalid request descriptor */
#define EN_EXFULL        54    /* Exchange full */
#define EN_ENOANO        55    /* No anode */
#define EN_EBADRQC       56    /* Invalid request code */
#define EN_EBADSLT       57    /* Invalid slot */
#define EN_EDEADLOCK     EN_EDEADLK
#define EN_EBFONT        59    /* Bad font file format */
#define EN_ENOSTR        60    /* Device not a stream */
#define EN_ENODATA       61    /* No data available */
#define EN_ETIME         62    /* Timer expired */
#define EN_ENOSR         63    /* Out of streams resources */
#define EN_ENONET        64    /* Machine is not on the network */
#define EN_ENOPKG        65    /* Package not installed */
#define EN_EREMOTE       66    /* Object is remote */
#define EN_ENOLINK       67    /* Link has been severed */
#define EN_EADV          68    /* Advertise error */
#define EN_ESRMNT        69    /* Srmount error */
#define EN_ECOMM         70    /* Communication error on send */
#define EN_EPROTO        71    /* Protocol error */
#define EN_EMULTIHOP     72    /* Multihop attempted */
#define EN_EDOTDOT       73    /* RFS specific error */
#define EN_EBADMSG       74    /* Not a data message */
#define EN_EOVERFLOW     75    /* Value too large for defined data type */
#define EN_ENOTUNIQ      76    /* Name not unique on network */
#define EN_EBADFD        77    /* File descriptor in bad state */
#define EN_EREMCHG       78    /* Remote address changed */
#define EN_ELIBACC       79    /* Can not access a needed shared library */
#define EN_ELIBBAD       80    /* Accessing a corrupted shared library */
#define EN_ELIBSCN       81    /* .lib section in a.out corrupted */
#define EN_ELIBMAX       82    /* Attempting to link in too many shared libraries */
#define EN_ELIBEXEC      83    /* Cannot exec a shared library directly */
#define EN_EILSEQ        84    /* Illegal byte sequence */
#define EN_ERESTART      85    /* Interrupted system call should be restarted */
#define EN_ESTRPIPE      86    /* Streams pipe error */
#define EN_EUSERS        87    /* Too many users */
#define EN_ENOTSOCK      88    /* Socket operation on non-socket */
#define EN_EDESTADDRREQ  89    /* Destination address required */
#define EN_EMSGSIZE      90    /* Message too long */
#define EN_EPROTOTYPE    91    /* Protocol wrong type for socket */
#define EN_ENOPROTOOPT   92    /* Protocol not available */
#define EN_EPROTONOSUPPORT  93    /* Protocol not supported */
#define EN_ESOCKTNOSUPPORT  94    /* Socket type not supported */
#define EN_EOPNOTSUPP       95    /* Operation not supported on transport endpoint */
#define EN_EPFNOSUPPORT     96    /* Protocol family not supported */
#define EN_EAFNOSUPPORT     97    /* Address family not supported by protocol */
#define EN_EADDRINUSE       98    /* Address already in use */
#define EN_EADDRNOTAVAIL    99    /* Cannot assign requested address */
#define EN_ENETDOWN         100    /* Network is down */
#define EN_ENETUNREACH      101    /* Network is unreachable */
#define EN_ENETRESET        102    /* Network dropped connection because of reset */
#define EN_ECONNABORTED     103    /* Software caused connection abort */
#define EN_ECONNRESET       104    /* Connection reset by peer */
#define EN_ENOBUFS          105    /* No buffer space available */
#define EN_EISCONN          106    /* Transport endpoint is already connected */
#define EN_ENOTCONN         107    /* Transport endpoint is not connected */
#define EN_ESHUTDOWN        108    /* Cannot send after transport endpoint shutdown */
#define EN_ETOOMANYREFS     109    /* Too many references: cannot splice */
#define EN_ETIMEDOUT        110    /* Connection timed out */
#define EN_ECONNREFUSED     111    /* Connection refused */
#define EN_EHOSTDOWN        112    /* Host is down */
#define EN_EHOSTUNREACH     113    /* No route to host */
#define EN_EALREADY         114    /* Operation already in progress */
#define EN_EINPROGRESS      115    /* Operation now in progress */
#define EN_ESTALE           116    /* Stale file handle */
#define EN_EUCLEAN          117    /* Structure needs cleaning */
#define EN_ENOTNAM          118    /* Not a XENIX named type file */
#define EN_ENAVAIL          119    /* No XENIX semaphores available */
#define EN_EISNAM           120    /* Is a named type file */
#define EN_EREMOTEIO        121    /* Remote I/O error */
#define EN_EDQUOT           122    /* Quota exceeded */
#define EN_ENOMEDIUM        123    /* No medium found */
#define EN_EMEDIUMTYPE      124    /* Wrong medium type */
#define EN_ECANCELED        125    /* Operation Canceled */
#define EN_ENOKEY           126    /* Required key not available */
#define EN_EKEYEXPIRED      127    /* Key has expired */
#define EN_EKEYREVOKED      128    /* Key has been revoked */
#define EN_EKEYREJECTED     129    /* Key was rejected by service */
/* for robust mutexes */
#define EN_EOWNERDEAD       130    /* Owner died */
#define EN_ENOTRECOVERABLE  131    /* State not recoverable */
#define EN_ERFKILL          132    /* Operation not possible due to RF-kill */
#define EN_EHWPOISON        133    /* Memory page has hardware error */

#define EN_EUNKNOWN         255    /* Unknown Error */

/* for cellular */
#define EN_ENOSVC           2000   /* No service */
#define EN_ENOCHANGED       2001   /* Already in request state */

/* user defined number from 5000 */
#define EN_EUSERDEF         5000

/* ======================================================
 *                        Error Class
 * ======================================================
 */
#define EC_COMMON        0x0000 /* Common Error */
#define EC_OS            0x0001 /* OS Error */
#define EC_FS            0x0002 /* File System Error */
#define EC_NW            0x0003 /* Network Error */
#define EC_SOCKET        0x0004 /* Socket Error */
#define EC_PLATFORM      0x0005 /* System/Platform Error */
#define EC_GPIO          0x0006 /* GPIO Error */
#define EC_UART          0x0007 /* UART Error */
#define EC_AT            0x0008 /* AT Error */
#define EC_IIC           0x0009 /* IIC Error */
#define EC_SPI           0x000A /* SPI Error */
#define EC_PWM           0x000B /* PWM Error */
#define EC_KEYPAD        0x000C /* KEYPAD Error */
#define EC_PM            0x000D /* Power Manager Error */
#define EC_MQTT          0x000E /* MQTT Error */
#define EC_COAP          0x000F /* COAP Error */
#define EC_LWM2M         0x0010 /* LWM2M Error */
#define EC_AES           0x0011 /* AES Error */
#define EC_RSA           0x0012 /* RSA Error */
#define EC_AUDIO         0x0013 /* Audio Error */
#define EC_SMS           0x0014 /* SMS Error */
#define EC_PB            0x0015 /* Phone Book Error */
#define EC_CHG           0x0016 /* Charge Error */
#define EC_SINK          0x0017 /* Sink Error */
#define EC_CC            0x0018 /* Call Control Error */
#define EC_BT            0x0019 /* BT Error */
#define EC_CAM           0x001A /* CAMERA Error */

/*
 * err_num:   [15 - 0]
 * err_class: [30 - 16]
 */
#define ERROR_CODE(err_class, err_num)    ((err_class << 16) | err_num | 0x80000000)
#define GET_ERR_CLASS_FROM_ERR_CODE(code) (((code & 0x7FFF0000) >> 16) & 0xFFFF)
#define GET_ERR_NUM_FROM_ERR_CODE(code)   ( code & 0xFFFF)

//common
#define GLOBAL_ERR_UNKNOWN                  ERROR_CODE(EC_COMMON, EN_EUNKNOWN)  //unknown. should never use.
#define GLOBAL_ERR_OPERATION_NOT_ALLOWED    ERROR_CODE(EC_COMMON, EN_EPERM)  //operation not allowed
#define GLOBAL_ERR_OPERATION_NOT_SUPPORTED  ERROR_CODE(EC_COMMON, EN_ENOSYS)  //operation not supported
#define GLOBAL_ERR_INVALID_PARAM            ERROR_CODE(EC_COMMON, EN_EINVAL)  //Invalid Param

//RTOS ERR CODE
#define OS_ERR_FAIL                         ERROR_CODE(EC_OS, EN_EAGAIN) /* 0x1 -operation failed code */
#define OS_ERR_TIMEOUT                      ERROR_CODE(EC_OS, EN_ETIME) /* 0x2 -Timed out waiting for a resource */
#define OS_ERR_NO_RESOURCES                 ERROR_CODE(EC_OS, EN_EBUSY) /* 0x3 -Internal OS resources expired */
#define OS_ERR_INVALID_POINTER              ERROR_CODE(EC_OS, EN_EINVAL) /* 0x4 -0 or out of range pointer value */
#define OS_ERR_INVALID_MEMORY               ERROR_CODE(EC_OS, EN_ENOMEM) /* 0x8 -invalid memory pointer */
#define OS_ERR_INVALID_PRIORITY             ERROR_CODE(EC_OS, EN_EINVAL) /* 0xB, 11 -out of range task priority */
#define OS_ERR_NO_MEMORY                    ERROR_CODE(EC_OS, EN_ENOMEM) /* 0x10, 16 -no memory left */
#define OS_ERR_NO_TASKS                     ERROR_CODE(EC_OS, EN_ESRCH) /* 0x18, 24 -exceeded max # of tasks in the system */
#define OS_ERR_NO_QUEUES                    ERROR_CODE(EC_OS, EN_ENOMEM) /* 0x1C, 28 -exceeded max # of msg queues in the system */
#define OS_ERR_MALLOC_ERR                   ERROR_CODE(EC_OS, EN_ENOMEM)
#define OS_ERR_INPUT_PARA_ERR               ERROR_CODE(EC_OS, EN_EINVAL)
#define OS_ERR_IS_IRQ                       ERROR_CODE(EC_OS, EN_EINTR)

//FS
#define FS_ERR_INVALID_INPUT_VALUE          ERROR_CODE(EC_FS, EN_EINVAL)
#define FS_ERR_DRIVE_FULL                   ERROR_CODE(EC_FS, EN_ENOMEM)
#define FS_ERR_FILE_NOT_FOUND               ERROR_CODE(EC_FS, EN_ENOENT)
#define FS_ERR_INVALID_FILE_NAME            ERROR_CODE(EC_FS, EN_EINVAL)
#define FS_ERR_FILE_ALREADY_EXISTS          ERROR_CODE(EC_FS, EN_EEXIST)
#define FS_ERR_WRITE_FILE_FAIL              ERROR_CODE(EC_FS, EN_EIO)
#define FS_ERR_OPEN_FILE_FAIL               ERROR_CODE(EC_FS, EN_ENOENT)
#define FS_ERR_READ_FILE_FAIL               ERROR_CODE(EC_FS, EN_EIO)
#define FS_ERR_LIST_FILE_FAIL               ERROR_CODE(EC_FS, EN_EIO)
#define FS_ERR_NO_SPACE                     ERROR_CODE(EC_FS, EN_ENOSPC)
#define FS_ERR_FILE_TOO_LARGE               ERROR_CODE(EC_FS, EN_EFBIG)
#define FS_ERR_INVALID_PARAMETER            ERROR_CODE(EC_FS, EN_EINVAL)
#define FS_ERR_OPERATION_NOT_ALLOWED        ERROR_CODE(EC_FS, EN_EPERM)
#define FS_ERR_OPERATION_NOT_SUPPORTED      ERROR_CODE(EC_FS, EN_ENOSYS)
#define FS_ERR_IS_DIRECTORY                 ERROR_CODE(EC_FS, EN_EISDIR)
#define FS_ERR_IS_NOT_DIRECTORY             ERROR_CODE(EC_FS, EN_ENOTDIR)
#define FS_ERR_DIR_NOT_EMPTY                ERROR_CODE(EC_FS, EN_ENOTEMPTY)
#define FS_ERR_MOUNT_ALREADY                ERROR_CODE(EC_FS, EN_EBUSY)
#define FS_ERR_NOT_MOUNTED                  ERROR_CODE(EC_FS, EN_ENOENT)

// Network Err Codes
#define NW_ERR_INP_NULL                     ERROR_CODE(EC_NW, EN_EINVAL)  //输入的参数为空
#define NW_ERR_OUT_MEMORY                   ERROR_CODE(EC_NW, EN_ENOMEM)  //内存分配失败
#define NW_ERR_INVALID_PARAM                ERROR_CODE(EC_NW, EN_EINVAL)  //输入参数无效
#define NW_ERR_INVALID_PDP_ID               ERROR_CODE(EC_NW, EN_EINVAL)  //输入pdp上下文标识无效
#define NW_ERR_INVALID_PDP_TYPE             ERROR_CODE(EC_NW, EN_EINVAL)  //输入pdp type参数无效
#define NW_ERR_INVALID_APN                  ERROR_CODE(EC_NW, EN_EINVAL)  //输入APN参数无效
#define NW_ERR_PDP_ACT_FAIL                 ERROR_CODE(EC_NW, EN_EIO)  // active PDP fail
#define NW_ERR_PDP_ACTIVING                 ERROR_CODE(EC_NW, EN_EINPROGRESS)  // PDP active is in processing
#define NW_ERR_PDP_ACTIVED                  ERROR_CODE(EC_NW, EN_EISCONN)  // PDP has been actived
#define NW_ERR_PDP_DEACT_FAIL               ERROR_CODE(EC_NW, EN_EIO)  // deactive PDP fail
#define NW_ERR_PDP_DEACTIVING               ERROR_CODE(EC_NW, EN_EINPROGRESS)  // PDP deactive is in processing
#define NW_ERR_PDP_DEACTED                  ERROR_CODE(EC_NW, EN_ENOTCONN)  // PDP has been deactived
#define NW_ERR_GET_STATE_FAIL               ERROR_CODE(EC_NW, EN_EIO)  // get network state fail
#define NW_ERR_GPRS_ATTACH_FAIL             ERROR_CODE(EC_NW, EN_EIO)  // GPRS attach fail
#define NW_ERR_GPRS_DETACH_FAIL             ERROR_CODE(EC_NW, EN_EIO)  // GPRS detach fail
#define NW_ERR_GPRS_ATTACH_NODO             ERROR_CODE(EC_NW, EN_ENOCHANGED)  // GPRS no change
#define NW_ERR_PS_ON_FAIL                   ERROR_CODE(EC_NW, EN_EIO)  // PS ON fail
#define NW_ERR_PS_OFF_FAIL                  ERROR_CODE(EC_NW, EN_EIO)  // PS OFF fail
#define NW_ERR_GET_INFO_FAIL                ERROR_CODE(EC_NW, EN_EIO)  // get network info fail
#define NW_ERR_NO_CELL                      ERROR_CODE(EC_NW, EN_ENOSVC)  // no service

// Socket Err Codes
#define SOCKET_ERR_NO_BUFS                  ERROR_CODE(EC_SOCKET, EN_ENOBUFS)  /* No buffer space available */
#define SOCKET_ERR_TIMED_OUT                ERROR_CODE(EC_SOCKET, EN_ETIMEDOUT)  /* Connection timed out */
#define SOCKET_ERR_IS_CONN                  ERROR_CODE(EC_SOCKET, EN_EISCONN)  /* Transport endpoint is already connected */
#define SOCKET_ERR_OP_NOT_SUPP              ERROR_CODE(EC_SOCKET, EN_EOPNOTSUPP)  /* Operation not supported on transport endpoint */
#define SOCKET_ERR_CONN_ABORTED             ERROR_CODE(EC_SOCKET, EN_ECONNABORTED)  /* Software caused connection abort */
#define SOCKET_ERR_WOULD_BLOCK              ERROR_CODE(EC_SOCKET, EN_EWOULDBLOCK)  /* Operation would block */
#define SOCKET_ERR_AGAIN                    ERROR_CODE(EC_SOCKET, EN_EAGAIN)  /* Try again */
#define SOCKET_ERR_CONN_REFUSED             ERROR_CODE(EC_SOCKET, EN_ECONNREFUSED)  /* Connection refused */
#define SOCKET_ERR_CONN_RESET               ERROR_CODE(EC_SOCKET, EN_ECONNRESET)  /* Connection reset by peer */
#define SOCKET_ERR_NOT_CONN                 ERROR_CODE(EC_SOCKET, EN_ENOTCONN)  /* Transport endpoint is not connected */
#define SOCKET_ERR_ALREADY_INPROGRESS       ERROR_CODE(EC_SOCKET, EN_EALREADY)  /* Operation already in progress */
#define SOCKET_ERR_INVAL_ARG                ERROR_CODE(EC_SOCKET, EN_EINVAL)  /* Invalid argument */
#define SOCKET_ERR_MSG_SIZE                 ERROR_CODE(EC_SOCKET, EN_EMSGSIZE)  /* Message too long */
#define SOCKET_ERR_BROKEN_PIPE              ERROR_CODE(EC_SOCKET, EN_EPIPE)  /* Broken pipe */
#define SOCKET_ERR_DEST_ADDR_REQ            ERROR_CODE(EC_SOCKET, EN_EDESTADDRREQ)  /* Destination address required */
#define SOCKET_ERR_SHUTDOWN                 ERROR_CODE(EC_SOCKET, EN_ESHUTDOWN)  /* Cannot send after transport endpoint shutdown */
#define SOCKET_ERR_NO_PROTO_OPT             ERROR_CODE(EC_SOCKET, EN_ENOPROTOOPT)  /* Protocol not available */
#define SOCKET_ERR_NO_MEM                   ERROR_CODE(EC_SOCKET, EN_ENOMEM)  /* Out of memory */
#define SOCKET_ERR_ADDR_NOT_AVAIL           ERROR_CODE(EC_SOCKET, EN_EADDRNOTAVAIL)  /* Cannot assign requested address */
#define SOCKET_ERR_ADDR_INUSE               ERROR_CODE(EC_SOCKET, EN_EADDRINUSE)  /* Address already in use */
#define SOCKET_ERR_AF_NO_SUPPORT            ERROR_CODE(EC_SOCKET, EN_EAFNOSUPPORT)  /* Address family not supported by protocol */
#define SOCKET_ERR_INPROGRESS               ERROR_CODE(EC_SOCKET, EN_EINPROGRESS)  /* Operation now in progress */
#define SOCKET_ERR_NOT_SOCK                 ERROR_CODE(EC_SOCKET, EN_ENOTSOCK)  /* Socket operation on non-socket */
#define SOCKET_ERR_TOO_MANY_REFS            ERROR_CODE(EC_SOCKET, EN_ETOOMANYREFS)  /* Too many references: cannot splice */
#define SOCKET_ERR_BAD_ADDR                 ERROR_CODE(EC_SOCKET, EN_EFAULT)  /* Bad address */
#define SOCKET_ERR_NET_UNREACH              ERROR_CODE(EC_SOCKET, EN_ENETUNREACH)  /* Network is unreachable */

//GPIO ERR CODE
#define GPIO_ERR_AMOUNT_OUT_OF_RANGE        ERROR_CODE(EC_GPIO, EN_EINVAL)
#define GPIO_ERR_INVALID_PIN_NUM            ERROR_CODE(EC_GPIO, EN_EINVAL)
#define GPIO_ERR_OPERATE_ERROR              ERROR_CODE(EC_GPIO, EN_EIO)

//UART ERR CODE
#define UART_ERR_PORT_NUM_ERROR             ERROR_CODE(EC_UART, EN_ENXIO) /*Error in the UART port number*/
#define UART_ERR_ILLEGAL_BAUD_RATE          ERROR_CODE(EC_UART, EN_EINVAL) /*Error in the UART Bayd Rate*/
#define UART_ERR_PARITY_BITS_ERROR          ERROR_CODE(EC_UART, EN_EINVAL) /*Error in parity bit*/
#define UART_ERR_ONE_STOP_BIT_ERROR         ERROR_CODE(EC_UART, EN_EINVAL) /*Error in one stop bit*/
#define UART_ERR_ONE_HALF_STOP_BIT_ERROR    ERROR_CODE(EC_UART, EN_EINVAL) /*Error in one and half stop bit*/
#define UART_ERR_TWO_STOP_BIT_ERROR         ERROR_CODE(EC_UART, EN_EINVAL) /*Error in two stop bit*/
#define UART_ERR_UART_NOT_AVAILABLE         ERROR_CODE(EC_UART, EN_EBUSY) /*Error in try to open UART that is open*/
#define UART_ERR_IRDA_CONFIG_ERR            ERROR_CODE(EC_UART, EN_EINVAL) /*Illegal IrDA configuration */
#define UART_ERR_FLOWCONTROL                ERROR_CODE(EC_UART, EN_EINVAL) /*Error flow control configuration */

//IIC ERR CODE
#define IIC_ERR_OPERATE_ERROR               ERROR_CODE(EC_IIC, EN_EIO)
#define IIC_ERR_OPEN_NUM_MAX                ERROR_CODE(EC_IIC, EN_ENXIO)
#define IIC_ERR_OPERATE_NO_SUPPORT          ERROR_CODE(EC_IIC, EN_ENOSYS)
#define IIC_ERR_BAD_SLAVE_ADDRESS           ERROR_CODE(EC_IIC, EN_ENXIO)

//SPI ERR CODE
#define SPI_ERR_OPEN_NUM_MAX                ERROR_CODE(EC_SPI, EN_ENXIO)
#define SPI_ERR_OPERATE_ERROR               ERROR_CODE(EC_SPI, EN_EIO)

// audio Error codes
#define PLAY_ERR_INVALID_PARAM              ERROR_CODE(EC_AUDIO, EN_EINVAL)  /**invalid input param */
#define PLAY_ERR_IS_PLAYING                 ERROR_CODE(EC_AUDIO, EN_EBUSY)  /** the play state is playing */
#define PLAY_ERR_SET_PARAM_ERROR            ERROR_CODE(EC_AUDIO, EN_EINVAL)  /** set param error*/
#define PLAY_ERR_GET_PARAM_FAIL             ERROR_CODE(EC_AUDIO, EN_EIO)  /** get param fail*/
#define PLAY_ERR_CALL_ACTIVE                ERROR_CODE(EC_AUDIO, EN_EBUSY)  /** call acitve*/

//sms Error codes
#define SMS_ERR_ME_FAIL                     ERROR_CODE(EC_SMS, EN_EUNKNOWN)
#define SMS_ERR_INVALID_PDU_PARAM           ERROR_CODE(EC_SMS, EN_EINVAL)
#define SMS_ERR_INVALID_TXT_PARAM           ERROR_CODE(EC_SMS, EN_EINVAL)
#define SMS_ERR_MEM_FAIL                    ERROR_CODE(EC_SMS, EN_ENOMEM)
#define SMS_ERR_INVALID_TXT_CHAR            ERROR_CODE(EC_SMS, EN_EINVAL)
#define SMS_ERR_INVALID_PDU_CHAR            ERROR_CODE(EC_SMS, EN_EINVAL)
#define SMS_ERR_INVALID_LEN                 ERROR_CODE(EC_SMS, EN_EINVAL)
#define SMS_ERR_SIM_COND_NO_FULLFILLED      ERROR_CODE(EC_SMS, EN_EPERM)

//PB Error codes
#define PB_ERR_UNKNOWN                      ERROR_CODE(EC_PB, EN_EUNKNOWN)
#define PB_ERR_IN_RUNING                    ERROR_CODE(EC_PB, EN_EINPROGRESS)
#define PB_ERR_NUMBER_INVALID               ERROR_CODE(EC_PB, EN_EINVAL)
#define PB_ERR_NAME_INVALID                 ERROR_CODE(EC_PB, EN_EINVAL)

//charge 
#define CHARGE_ERR_INVALID_PARAM            ERROR_CODE(EC_CHG, EN_EINVAL)  /**invalid input param */

//SINK Error codes
#define SINK_ERR_INVALID_TYPE               ERROR_CODE(EC_SINK, EN_EINVAL) /** invalid sink type */
#define SINK_ERR_INVALID_LEVEL              ERROR_CODE(EC_SINK, EN_EINVAL) /** invalid level value */

//call control
#define CC_ERR_NO_SERVICE                   ERROR_CODE(EC_CC, EN_ENOSVC)
#define CC_ERR_INVALID_PARAM                ERROR_CODE(EC_CC, EN_EINVAL)
#define CC_ERR_INVALID_NUMBER               ERROR_CODE(EC_CC, EN_EINVAL)
#define CC_ERR_DAILING                      ERROR_CODE(EC_CC, EN_EINPROGRESS)
#define CC_ERR_DAIL_FAIL                    ERROR_CODE(EC_CC, EN_EAGAIN)
#define CC_ERR_OPERATION_NOT_ALLOWED        ERROR_CODE(EC_CC, EN_EPERM)
#define CC_ERR_MAX_CALL_NUM                 ERROR_CODE(EC_CC, EN_EBUSY)
#define CC_ERR_OPERATION_NOT_SUPPORTED      ERROR_CODE(EC_CC, EN_ENOSYS)
#define CC_ERR_UNKNOWN                      ERROR_CODE(EC_CC, EN_EUNKNOWN)
#endif

