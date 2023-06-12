/* Copyright (C) 2017 RDA Technologies Limited and/or its affiliates("RDA").
 * All rights reserved.
 *
 * This software is supplied "AS IS" without any warranties.
 * RDA assumes no responsibility or liability for the use of the software,
 * conveys no license or title under any patent, copyright, or mask work
 * right to the product. RDA reserves the right to make changes in the
 * software without notification.  RDA also make no representation or
 * warranty that such application will be suitable for the specified use
 * without further testing or modification.
 */

#ifndef _BOOT_UART_H_
#define _BOOT_UART_H_

#include "boot_compiler.h"
#include "boot_trace.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief UART data bits
 */
typedef enum
{
    BOOT_UART_DATA_BITS_7 = 7,
    BOOT_UART_DATA_BITS_8 = 8
} bootUartDataBits_t;

/**
 * @brief UART stop bits
 */
typedef enum
{
    BOOT_UART_STOP_BITS_1 = 1,
    BOOT_UART_STOP_BITS_2 = 2
} bootUartStopBits_t;

/**
 * @brief UART parity check mode
 */
typedef enum
{
    BOOT_UART_NO_PARITY,   ///< No parity check
    BOOT_UART_ODD_PARITY,  ///< Parity check is odd
    BOOT_UART_EVEN_PARITY, ///< Parity check is even
    BOOT_UART_SPACE_PARITY, ///< Parity check is always 0 (space)
    BOOT_UART_MARK_PARITY   ///< Parity check is always 1 (mark)
} bootUartParity_t;


typedef struct
{
    uint32_t baud;               ///< baudrate, 0 for auto baud
    bootUartDataBits_t data_bits; ///< data bits
    bootUartStopBits_t stop_bits; ///< stop bits
    bootUartParity_t parity;      ///< parity check mode
} bootUartCfg_t;



/**
 * \brief opaque data structure for uart
 */
typedef struct bootUart bootUart_t;

/**
 * \brief open uart
 *
 * When \p reconfig is false, the UART won't be re-configured, such as
 * iomux, uart config and baud rate. This is useful that it is known
 * that the uart is already configured. For example, when ROM jumps to
 * FDL, the uart is already configured, and the configured baud rate
 * should be used.
 *
 * When \p reconfig is false, \p baud will be ignored.
 *
 * \param name      uart device name
 * \param baud      baud rate
 * \param reconfig  false to ignore re-configuration
 * \return
 *      - the uart instance
 *      - NULL for failed
 */
bootUart_t *bootUartOpen(uint32_t name, bootUartCfg_t cfg, bool reconfig);

/**
 * \brief whether baud rate is supported
 *
 * \param uart      uart instance
 * \param baud      baud rate to be checked
 * \return
 *      - true on supported
 *      - false on unsupported
 */
bool bootUartBaudSupported(bootUart_t *uart, uint32_t baud);

/**
 * \brief set baud rate
 *
 * \param uart      uart instance
 * \param baud      baud rate to be set
 * \return
 *      - true on success
 *      - false on failed
 */
bool bootUartSetBaud(bootUart_t *uart, uint32_t baud);

/**
 * \brief get available size for read
 *
 * \param uart      uart instance
 * \return
 *      - available size for read
 */
int bootUartAvail(bootUart_t *uart);

/**
 * \brief read from the uart
 *
 * \param uart      uart instance
 * \param data      memory for read data
 * \param size      maximum read size
 * \return
 *      - actual read size
 */
int bootUartRead(bootUart_t *uart, void *data, size_t size);

/**
 * \brief write to the uart
 *
 * \param uart      uart instance
 * \param data      pointer of data to be written
 * \param size      maximum write size
 * \return
 *      - actual written size
 */
int bootUartWrite(bootUart_t *uart, const void *data, size_t size);

/**
 * \brief flush output
 *
 * This will wait all data will be sent out, and the peer is able to
 * receive all data.
 *
 * \param uart      uart instance
 */
void bootUartFlush(bootUart_t *uart);

/**
 * \brief write all data to the uart
 *
 * Comparing to \p bootUartWrite, it will loop and wait there are space
 * in uart tx fifo.
 *
 * \param uart      uart instance
 * \param data      pointer of data to be written
 * \param size      write size
 * \return
 *      - true on success
 *      - false on error, invalid parameter
 */
bool bootUartWriteAll(bootUart_t *uart, const void *data, size_t size);

#ifdef __cplusplus
}
#endif
#endif
