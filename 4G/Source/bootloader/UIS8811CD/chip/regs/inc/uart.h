/* Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
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

#ifndef _UART_H_
#define _UART_H_

// Auto generated (v1.0-38-g167fa99). Don't edit it manually!

#define UART_RX_FIFO_SIZE (128)
#define UART_TX_FIFO_SIZE (16)
#define NB_RX_FIFO_BITS (7)
#define NB_TX_FIFO_BITS (4)

#define REG_UART1_BASE (0x41800000)
#define REG_UART2_BASE (0x41801000)
#define REG_UART3_BASE (0x41a04000)
#define REG_UART4_BASE (0x41a05000)
#define REG_UART5_BASE (0x41a06000)

typedef volatile struct
{
    uint32_t ctrl;        // 0x00000000
    uint32_t status;      // 0x00000004
    uint32_t rxtx_buffer; // 0x00000008
    uint32_t irq_mask;    // 0x0000000c
    uint32_t irq_cause;   // 0x00000010
    uint32_t triggers;    // 0x00000014
    uint32_t cmd_set;     // 0x00000018
    uint32_t cmd_clr;     // 0x0000001c
    uint32_t auto_ratio;  // 0x00000020
    uint32_t xon;         // 0x00000024
    uint32_t xoff;        // 0x00000028
} HWP_UART_T;

#define hwp_uart1 ((HWP_UART_T *)REG_ACCESS_ADDRESS(REG_UART1_BASE))
#define hwp_uart2 ((HWP_UART_T *)REG_ACCESS_ADDRESS(REG_UART2_BASE))
#define hwp_uart3 ((HWP_UART_T *)REG_ACCESS_ADDRESS(REG_UART3_BASE))
#define hwp_uart4 ((HWP_UART_T *)REG_ACCESS_ADDRESS(REG_UART4_BASE))
#define hwp_uart5 ((HWP_UART_T *)REG_ACCESS_ADDRESS(REG_UART5_BASE))

// ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t enable : 1;                // [0]
        uint32_t data_bits : 1;             // [1]
        uint32_t tx_stop_bits : 1;          // [2]
        uint32_t parity_enable : 1;         // [3]
        uint32_t parity_select : 2;         // [5:4]
        uint32_t soft_flow_ctrl_enable : 1; // [6]
        uint32_t __7_7 : 1;                 // [7]
        uint32_t auto_enable : 1;           // [8]
        uint32_t __11_9 : 3;                // [11:9]
        uint32_t data_bits_56 : 1;          // [12]
        uint32_t __18_13 : 6;               // [18:13]
        uint32_t divisor_mode : 2;          // [20:19]
        uint32_t irda_enable : 1;           // [21]
        uint32_t dma_mode : 1;              // [22]
        uint32_t auto_flow_control : 1;     // [23]
        uint32_t loop_back_mode : 1;        // [24]
        uint32_t rx_lock_err : 1;           // [25]
        uint32_t __27_26 : 2;               // [27:26]
        uint32_t rx_break_length : 4;       // [31:28]
    } b;
} REG_UART_CTRL_T;

// status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rx_fifo_level : 8;        // [7:0], read only
        uint32_t tx_fifo_space : 5;        // [12:8], read only
        uint32_t at_match_flag : 1;        // [13], read only
        uint32_t tx_active : 1;            // [14], read only
        uint32_t rx_active : 1;            // [15], read only
        uint32_t rx_overflow_err : 1;      // [16], read only
        uint32_t tx_overflow_err : 1;      // [17], read only
        uint32_t rx_parity_err : 1;        // [18], read only
        uint32_t rx_framing_err : 1;       // [19], read only
        uint32_t rx_break_int : 1;         // [20], read only
        uint32_t character_miscompare : 1; // [21], read only
        uint32_t auto_baud_locked : 1;     // [22], read only
        uint32_t __23_23 : 1;              // [23]
        uint32_t dcts : 1;                 // [24], read only
        uint32_t cts : 1;                  // [25], read only
        uint32_t auto_ratio_flag : 1;      // [26], read only
        uint32_t mask_tx_enable_flag : 1;  // [27], read only
        uint32_t dtr : 1;                  // [28], read only
        uint32_t __30_29 : 2;              // [30:29]
        uint32_t clk_enabled : 1;          // [31], read only
    } b;
} REG_UART_STATUS_T;

// rxtx_buffer
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rx_data : 8; // [7:0], read only
        uint32_t tx_data : 8; // [7:0]
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_UART_RXTX_BUFFER_T;

// irq_mask
typedef union {
    uint32_t v;
    struct
    {
        uint32_t tx_modem_status : 1;   // [0]
        uint32_t rx_data_available : 1; // [1]
        uint32_t tx_data_needed : 1;    // [2]
        uint32_t rx_timeout : 1;        // [3]
        uint32_t rx_line_err : 1;       // [4]
        uint32_t tx_dma_done : 1;       // [5]
        uint32_t rx_dma_done : 1;       // [6]
        uint32_t rx_dma_timeout : 1;    // [7]
        uint32_t dtr_rise : 1;          // [8]
        uint32_t dtr_fall : 1;          // [9]
        uint32_t auto_fail : 1;         // [10]
        uint32_t uart_dma_rx_adone : 1; // [11]
        uint32_t uart_dma_tx_adone : 1; // [12]
        uint32_t xoff_trig : 1;         // [13]
        uint32_t xon_trig : 1;          // [14]
        uint32_t start_det : 1;         // [15]
        uint32_t __31_16 : 16;          // [31:16]
    } b;
} REG_UART_IRQ_MASK_T;

// irq_cause
typedef union {
    uint32_t v;
    struct
    {
        uint32_t tx_modem_status : 1;     // [0], read only
        uint32_t rx_data_available : 1;   // [1], read only
        uint32_t tx_data_needed : 1;      // [2], read only
        uint32_t rx_timeout : 1;          // [3], read only
        uint32_t rx_line_err : 1;         // [4], read only
        uint32_t tx_dma_done : 1;         // [5]
        uint32_t rx_dma_done : 1;         // [6]
        uint32_t rx_dma_timeout : 1;      // [7]
        uint32_t dtr_rise : 1;            // [8]
        uint32_t dtr_fall : 1;            // [9]
        uint32_t auto_fail : 1;           // [10]
        uint32_t uart_dma_rx_adone : 1;   // [11]
        uint32_t uart_dma_tx_adone : 1;   // [12]
        uint32_t xoff_trig : 1;           // [13]
        uint32_t xon_trig : 1;            // [14]
        uint32_t start_det : 1;           // [15]
        uint32_t tx_modem_status_u : 1;   // [16], read only
        uint32_t rx_data_available_u : 1; // [17], read only
        uint32_t tx_data_needed_u : 1;    // [18], read only
        uint32_t rx_timeout_u : 1;        // [19], read only
        uint32_t rx_line_err_u : 1;       // [20], read only
        uint32_t tx_dma_done_u : 1;       // [21], read only
        uint32_t rx_dma_done_u : 1;       // [22], read only
        uint32_t rx_dma_timeout_u : 1;    // [23], read only
        uint32_t dtr_rise_u : 1;          // [24], read only
        uint32_t dtr_fall_u : 1;          // [25], read only
        uint32_t auto_fail_u : 1;         // [26], read only
        uint32_t uart_dma_rx_adone_u : 1; // [27]
        uint32_t uart_dma_tx_adone_u : 1; // [28]
        uint32_t xoff_trig_u : 1;         // [29]
        uint32_t xon_trig_u : 1;          // [30]
        uint32_t start_det_u : 1;         // [31]
    } b;
} REG_UART_IRQ_CAUSE_T;

// triggers
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rx_trigger : 7; // [6:0]
        uint32_t __7_7 : 1;      // [7]
        uint32_t tx_trigger : 4; // [11:8]
        uint32_t __15_12 : 4;    // [15:12]
        uint32_t afc_level : 7;  // [22:16]
        uint32_t __31_23 : 9;    // [31:23]
    } b;
} REG_UART_TRIGGERS_T;

// cmd_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ri : 1;               // [0], write set
        uint32_t dcd : 1;              // [1], write set
        uint32_t dsr : 1;              // [2], write set
        uint32_t tx_break_control : 1; // [3], write set
        uint32_t tx_finish_n_wait : 1; // [4], write set
        uint32_t rts : 1;              // [5], write set
        uint32_t rx_fifo_reset : 1;    // [6], read only
        uint32_t tx_fifo_reset : 1;    // [7], read only
        uint32_t __31_8 : 24;          // [31:8]
    } b;
} REG_UART_CMD_SET_T;

// cmd_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ri : 1;               // [0], write clear
        uint32_t dcd : 1;              // [1], write clear
        uint32_t dsr : 1;              // [2], write clear
        uint32_t tx_break_control : 1; // [3], write clear
        uint32_t tx_finish_n_wait : 1; // [4], write clear
        uint32_t rts : 1;              // [5], write clear
        uint32_t __31_6 : 26;          // [31:6]
    } b;
} REG_UART_CMD_CLR_T;

// auto_ratio
typedef union {
    uint32_t v;
    struct
    {
        uint32_t auto_ratio : 16; // [15:0], read only
        uint32_t __31_16 : 16;    // [31:16]
    } b;
} REG_UART_AUTO_RATIO_T;

// xon
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xon : 8;     // [7:0]
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_UART_XON_T;

// xoff
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xoff : 8;    // [7:0]
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_UART_XOFF_T;

// ctrl
#define UART_ENABLE_DISABLE (0 << 0)
#define UART_ENABLE_ENABLE (1 << 0)
#define UART_DATA_BITS (1 << 1)
#define UART_TX_STOP_BITS_1_BIT (0 << 2)
#define UART_TX_STOP_BITS_2_BITS (1 << 2)
#define UART_PARITY_ENABLE_NO (0 << 3)
#define UART_PARITY_ENABLE_YES (1 << 3)
#define UART_PARITY_SELECT(n) (((n)&0x3) << 4)
#define UART_PARITY_SELECT_ODD (0 << 4)
#define UART_PARITY_SELECT_EVEN (1 << 4)
#define UART_PARITY_SELECT_SPACE (2 << 4)
#define UART_PARITY_SELECT_MARK (3 << 4)
#define UART_SOFT_FLOW_CTRL_ENABLE (1 << 6)
#define UART_AUTO_ENABLE (1 << 8)
#define UART_DATA_BITS_56 (1 << 12)
#define UART_DIVISOR_MODE(n) (((n)&0x3) << 19)
#define UART_IRDA_ENABLE (1 << 21)
#define UART_DMA_MODE_DISABLE (0 << 22)
#define UART_DMA_MODE_ENABLE (1 << 22)
#define UART_AUTO_FLOW_CONTROL_ENABLE (1 << 23)
#define UART_AUTO_FLOW_CONTROL_DISABLE (0 << 23)
#define UART_LOOP_BACK_MODE (1 << 24)
#define UART_RX_LOCK_ERR (1 << 25)
#define UART_RX_BREAK_LENGTH(n) (((n)&0xf) << 28)

#define UART_ENABLE_V_DISABLE (0)
#define UART_ENABLE_V_ENABLE (1)
#define UART_TX_STOP_BITS_V_1_BIT (0)
#define UART_TX_STOP_BITS_V_2_BITS (1)
#define UART_PARITY_ENABLE_V_NO (0)
#define UART_PARITY_ENABLE_V_YES (1)
#define UART_PARITY_SELECT_V_ODD (0)
#define UART_PARITY_SELECT_V_EVEN (1)
#define UART_PARITY_SELECT_V_SPACE (2)
#define UART_PARITY_SELECT_V_MARK (3)
#define UART_DMA_MODE_V_DISABLE (0)
#define UART_DMA_MODE_V_ENABLE (1)
#define UART_AUTO_FLOW_CONTROL_V_ENABLE (1)
#define UART_AUTO_FLOW_CONTROL_V_DISABLE (0)

// status
#define UART_RX_FIFO_LEVEL(n) (((n)&0xff) << 0)
#define UART_TX_FIFO_SPACE(n) (((n)&0x1f) << 8)
#define UART_AT_MATCH_FLAG (1 << 13)
#define UART_TX_ACTIVE (1 << 14)
#define UART_RX_ACTIVE (1 << 15)
#define UART_RX_OVERFLOW_ERR (1 << 16)
#define UART_TX_OVERFLOW_ERR (1 << 17)
#define UART_RX_PARITY_ERR (1 << 18)
#define UART_RX_FRAMING_ERR (1 << 19)
#define UART_RX_BREAK_INT (1 << 20)
#define UART_CHARACTER_MISCOMPARE (1 << 21)
#define UART_AUTO_BAUD_LOCKED (1 << 22)
#define UART_DCTS (1 << 24)
#define UART_CTS (1 << 25)
#define UART_AUTO_RATIO_FLAG (1 << 26)
#define UART_MASK_TX_ENABLE_FLAG (1 << 27)
#define UART_DTR (1 << 28)
#define UART_CLK_ENABLED (1 << 31)

// rxtx_buffer
#define UART_RX_DATA(n) (((n)&0xff) << 0)
#define UART_TX_DATA(n) (((n)&0xff) << 0)

// irq_mask
#define UART_TX_MODEM_STATUS (1 << 0)
#define UART_RX_DATA_AVAILABLE (1 << 1)
#define UART_TX_DATA_NEEDED (1 << 2)
#define UART_RX_TIMEOUT (1 << 3)
#define UART_RX_LINE_ERR (1 << 4)
#define UART_TX_DMA_DONE (1 << 5)
#define UART_RX_DMA_DONE (1 << 6)
#define UART_RX_DMA_TIMEOUT (1 << 7)
#define UART_DTR_RISE (1 << 8)
#define UART_DTR_FALL (1 << 9)
#define UART_AUTO_FAIL (1 << 10)
#define UART_UART_DMA_RX_ADONE (1 << 11)
#define UART_UART_DMA_TX_ADONE (1 << 12)
#define UART_XOFF_TRIG (1 << 13)
#define UART_XON_TRIG (1 << 14)
#define UART_START_DET (1 << 15)

// irq_cause
#define UART_TX_MODEM_STATUS (1 << 0)
#define UART_RX_DATA_AVAILABLE (1 << 1)
#define UART_TX_DATA_NEEDED (1 << 2)
#define UART_RX_TIMEOUT (1 << 3)
#define UART_RX_LINE_ERR (1 << 4)
#define UART_TX_DMA_DONE (1 << 5)
#define UART_RX_DMA_DONE (1 << 6)
#define UART_RX_DMA_TIMEOUT (1 << 7)
#define UART_DTR_RISE (1 << 8)
#define UART_DTR_FALL (1 << 9)
#define UART_AUTO_FAIL (1 << 10)
#define UART_UART_DMA_RX_ADONE (1 << 11)
#define UART_UART_DMA_TX_ADONE (1 << 12)
#define UART_XOFF_TRIG (1 << 13)
#define UART_XON_TRIG (1 << 14)
#define UART_START_DET (1 << 15)
#define UART_TX_MODEM_STATUS_U (1 << 16)
#define UART_RX_DATA_AVAILABLE_U (1 << 17)
#define UART_TX_DATA_NEEDED_U (1 << 18)
#define UART_RX_TIMEOUT_U (1 << 19)
#define UART_RX_LINE_ERR_U (1 << 20)
#define UART_TX_DMA_DONE_U (1 << 21)
#define UART_RX_DMA_DONE_U (1 << 22)
#define UART_RX_DMA_TIMEOUT_U (1 << 23)
#define UART_DTR_RISE_U (1 << 24)
#define UART_DTR_FALL_U (1 << 25)
#define UART_AUTO_FAIL_U (1 << 26)
#define UART_UART_DMA_RX_ADONE_U (1 << 27)
#define UART_UART_DMA_TX_ADONE_U (1 << 28)
#define UART_XOFF_TRIG_U (1 << 29)
#define UART_XON_TRIG_U (1 << 30)
#define UART_START_DET_U (1 << 31)

// triggers
#define UART_RX_TRIGGER(n) (((n)&0x7f) << 0)
#define UART_TX_TRIGGER(n) (((n)&0xf) << 8)
#define UART_AFC_LEVEL(n) (((n)&0x7f) << 16)

// cmd_set
#define UART_RI (1 << 0)
#define UART_DCD (1 << 1)
#define UART_DSR (1 << 2)
#define UART_TX_BREAK_CONTROL (1 << 3)
#define UART_TX_FINISH_N_WAIT (1 << 4)
#define UART_RTS (1 << 5)
#define UART_RX_FIFO_RESET (1 << 6)
#define UART_TX_FIFO_RESET (1 << 7)

// cmd_clr
#define UART_RI (1 << 0)
#define UART_DCD (1 << 1)
#define UART_DSR (1 << 2)
#define UART_TX_BREAK_CONTROL (1 << 3)
#define UART_TX_FINISH_N_WAIT (1 << 4)
#define UART_RTS (1 << 5)

// auto_ratio
#define UART_AUTO_RATIO(n) (((n)&0xffff) << 0)

// xon
#define UART_XON(n) (((n)&0xff) << 0)

// xoff
#define UART_XOFF(n) (((n)&0xff) << 0)

#endif // _UART_H_
