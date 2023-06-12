#ifndef __AT_CMD_GNSS_H__
#define __AT_CMD_GNSS_H__

#define GPS_NMEA_LEN_MAX    100
#define NMEA_BUFEER_LEN     1024

typedef struct
{
  char   gsa[GPS_NMEA_LEN_MAX];
  char   gga[GPS_NMEA_LEN_MAX];
  char   gsv1[GPS_NMEA_LEN_MAX];
  char   gsv2[GPS_NMEA_LEN_MAX];
  char   gsv3[GPS_NMEA_LEN_MAX];
  char   gsv4[GPS_NMEA_LEN_MAX];
  char   rmc[GPS_NMEA_LEN_MAX];
  char   vtg[GPS_NMEA_LEN_MAX];
  char   gll[GPS_NMEA_LEN_MAX];
} GPS_NMEA_DATA_T;

typedef struct
{
  char       utc_time[11]; //gga
  char       lat[15];
  char       lon[15];
  char       hop[5];
  char       alti[8];
  char       fix[2];  //gsa
  char       cog[8];    //vtg
  char       spkm[8];   //vtg
  char       spkn[8];   //vtg
  char       data[7];   //rmc
  char       nsat[3];   //gga
  uint32_t   snr_store[4];
  bool       bfix;
}GPS_LOC_DATA_T;

typedef struct
{
  char     buffer[NMEA_BUFEER_LEN];
  uint16   head;
  uint16   tail;
  uint16   count;
}NMEA_RING_BUFF_T;

typedef struct
{
    THREAD_HANDLE     taskId;
    MSG_HANDLE        msgHandle;
}GNSS_TASK_MSG_S;

typedef enum
{
    MSG_UART_DATA_READ = 1,
    MSG_AGPS_PDP_ACTIVE,
    MSG_AGPS_SOCKET_CONNECT_RSP,
    MSG_AGPS_SOCKET_READ_RSP,
    MSG_AGPS_DATA_UART_WRITE,
    MSG_TYPE_MAX,
}GNSS_MSG_TYPE_S;

#endif/*__AT_CMD_GNSS_H__*/

