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

#include "boot_ifc.h"
#include "hwregs.h"
#include "boot_platform.h"

#define REG_SYS_IFC_CONTROL_T REG_AP_IFC_CONTROL_T
#define REG_SYS_IFC_STATUS_T REG_AP_IFC_STATUS_T
#define REG_SYS_IFC_TC_T REG_AP_IFC_TC_T

typedef volatile struct
{
    uint32_t control;
    uint32_t status;
    uint32_t start_addr;
    uint32_t tc;
} bootIfcChannelReg_t;

typedef struct
{
    uint32_t name;
    uint32_t mode;
    uint32_t reg_base;
    uint8_t ch_count;
    uint8_t request_id;
    uint8_t dir;
} bootIfcDesc_t;

static const bootIfcDesc_t gIfcDescs[] = {
    {DRV_NAME_DEBUGUART,
     (1 << 2) | (0 << 3) | (0 << 5) | (AON_APB_DMA_ID_RX_DEBUG_UART << 8),
     REG_ACCESS_ADDRESS(REG_AON_IFC_BASE), 2, AON_APB_DMA_ID_RX_DEBUG_UART, DRV_IFC_RX},
    {DRV_NAME_DEBUGUART,
     (1 << 2) | (0 << 3) | (0 << 5) | (AON_APB_DMA_ID_TX_DEBUG_UART << 8),
     REG_ACCESS_ADDRESS(REG_AON_IFC_BASE), 2, AON_APB_DMA_ID_TX_DEBUG_UART, DRV_IFC_TX},
    {DRV_NAME_SPI1,
     (1 << 2) | (0 << 3) | (0 << 5) | (AP_APB_DMA_ID_SPI_1_RX << 8),
     REG_ACCESS_ADDRESS(REG_AP_IFC_BASE), 7, AP_APB_DMA_ID_SPI_1_RX, DRV_IFC_RX},
    {DRV_NAME_SPI1,
     (1 << 2) | (0 << 3) | (0 << 5) | (AP_APB_DMA_ID_SPI_1_TX << 8),
     REG_ACCESS_ADDRESS(REG_AP_IFC_BASE), 7, AP_APB_DMA_ID_SPI_1_TX, DRV_IFC_TX},
    {DRV_NAME_SPI2,
     (1 << 2) | (0 << 3) | (0 << 5) | (AP_APB_DMA_ID_SPI_2_RX << 8),
     REG_ACCESS_ADDRESS(REG_AP_IFC_BASE), 7, AP_APB_DMA_ID_SPI_2_RX, DRV_IFC_RX},
    {DRV_NAME_SPI2,
     (1 << 2) | (0 << 3) | (0 << 5) | (AP_APB_DMA_ID_SPI_2_TX << 8),
     REG_ACCESS_ADDRESS(REG_AP_IFC_BASE), 7, AP_APB_DMA_ID_SPI_2_TX, DRV_IFC_TX},
    {DRV_NAME_SDMMC1,
     (1 << 2) | (0 << 3) | (1 << 5) | (AP_APB_DMA_ID_SDMMC1_RX << 8),
     REG_ACCESS_ADDRESS(REG_AP_IFC_BASE), 7, AP_APB_DMA_ID_SDMMC1_RX, DRV_IFC_RX},
    {DRV_NAME_SDMMC1,
     (1 << 2) | (0 << 3) | (1 << 5) | (AP_APB_DMA_ID_SDMMC1_TX << 8),
     REG_ACCESS_ADDRESS(REG_AP_IFC_BASE), 7, AP_APB_DMA_ID_SDMMC1_TX, DRV_IFC_TX},
    {DRV_NAME_CAMERA1,
     (1 << 2) | (0 << 3) | (1 << 5) | (AP_APB_DMA_ID_CAMERA_RX << 8),
     REG_ACCESS_ADDRESS(REG_AP_IFC_BASE), 7, AP_APB_DMA_ID_CAMERA_RX, DRV_IFC_RX},
};

//static osiSemaphore_t *gIfcWaitSema = NULL;

static const bootIfcDesc_t *_bootIfcDesc(uint32_t name, bootIfcDirection_t dir)
{
    for (int n = 0; n < sizeof(gIfcDescs) / sizeof(gIfcDescs[0]); n++)
    {
        if (gIfcDescs[n].name == name && gIfcDescs[n].dir == dir)
            return &gIfcDescs[n];
    }
    return NULL;
}

bool bootIfcChannelInit(bootIfcChannel_t *ch, uint32_t name, bootIfcDirection_t dir)
{
    ch->hwp_channel = NULL;
    ch->ifc_desc = _bootIfcDesc(name, dir);
    return (ch->ifc_desc != NULL);
}

bool bootIfcRequestChannel(bootIfcChannel_t *ch)
{
    if (ch->hwp_channel != NULL)
        return true;

    // register structure of all IFC are the same
    const bootIfcDesc_t *desc = (const bootIfcDesc_t *)ch->ifc_desc;
    HWP_SYS_IFC_T *hwp = (HWP_SYS_IFC_T *)desc->reg_base;
    REG_SYS_IFC_GET_CH_T get_ch = {hwp->get_ch};
    if (get_ch.b.ch_to_use >= desc->ch_count)
    {
        bootLog("IFC request channel failed, src/%d/%d",
                 desc->request_id, desc->dir);
        return false;
    }

    ch->hwp_channel = (void *)&hwp->std_ch[get_ch.b.ch_to_use];
    bootIfcChannelReg_t *reg = (bootIfcChannelReg_t *)ch->hwp_channel;
    reg->control = desc->mode;

    // It looks like a hack. And the purpose is to save the global
    // initialization of IFC management.
//    if (OSI_UNLIKELY(gIfcWaitSema == NULL))
//    {
//        uint32_t critical = bootEnterCritical();
//        if (gIfcWaitSema == NULL)
//            gIfcWaitSema = osiSemaphoreCreate(1, 1);
//        bootExitCritical(critical);
//    }
    return true;
}

void bootIfcWaitChannel(bootIfcChannel_t *ch)
{
    for (;;)
    {
        if (bootIfcRequestChannel(ch))
            return;

//        osiSemaphoreAcquire(gIfcWaitSema);
    }
}

bool bootIfcReady(bootIfcChannel_t *ch)
{
    return (ch->hwp_channel != NULL);
}

void bootIfcReleaseChannel(bootIfcChannel_t *ch)
{
    if (ch->hwp_channel == NULL)
        return;

    bootIfcChannelReg_t *reg = (bootIfcChannelReg_t *)ch->hwp_channel;
    REG_SYS_IFC_CONTROL_T control = {reg->control};
    control.b.disable = 1;
    reg->control = control.v;
    ch->hwp_channel = NULL;

//    osiSemaphoreRelease(gIfcWaitSema);
}

void bootIfcFlush(bootIfcChannel_t *ch)
{
    if (ch->hwp_channel == NULL)
        return;

    bootIfcChannelReg_t *reg = (bootIfcChannelReg_t *)ch->hwp_channel;
    REG_SYS_IFC_CONTROL_T control = {reg->control};
    control.b.flush = 1;
    reg->control = control.v;

    REG_SYS_IFC_STATUS_T status;
    REG_WAIT_FIELD_NEZ(status, reg->status, fifo_empty);
}

void bootIfcClearFlush(bootIfcChannel_t *ch)
{
    if (ch->hwp_channel == NULL)
        return;

    bootIfcChannelReg_t *reg = (bootIfcChannelReg_t *)ch->hwp_channel;
    REG_SYS_IFC_CONTROL_T control = {reg->control};
    control.b.flush = 0;
    reg->control = control.v;
}

uint32_t bootIfcGetTC(bootIfcChannel_t *ch)
{
    bootIfcChannelReg_t *reg = (bootIfcChannelReg_t *)ch->hwp_channel;
    REG_SYS_IFC_TC_T tc = {reg->tc};
    return tc.b.tc;
}

void bootIfcStop(bootIfcChannel_t *ch)
{
    if (ch->hwp_channel == NULL)
        return;

    uint32_t critical = bootEnterCritical();
    bootIfcChannelReg_t *reg = (bootIfcChannelReg_t *)ch->hwp_channel;
    REG_SYS_IFC_CONTROL_T control = {reg->control};
    control.b.enable = 0;
    control.b.disable = 1;
    reg->control = control.v;
    (void)reg->control;

    reg->tc = 0;
    (void)reg->tc;

    control.b.enable = 1;
    control.b.disable = 0;
    reg->control = control.v;
    (void)reg->control;
    bootExitCritical(critical);
}

void bootIfcStart(bootIfcChannel_t *ch, const void *address, uint32_t size)
{
    if (ch->hwp_channel == NULL || address == NULL || size == 0)
        return;

    uint32_t critical = bootEnterCritical();
    const bootIfcDesc_t *desc = (const bootIfcDesc_t *)ch->ifc_desc;
    bootIfcChannelReg_t *reg = (bootIfcChannelReg_t *)ch->hwp_channel;
    REG_SYS_IFC_CONTROL_T control = {desc->mode};
    control.b.enable = 0;
    control.b.disable = 1;
    reg->control = control.v;
    (void)reg->control;

    reg->start_addr = (uint32_t)address;
    reg->tc = size;
    (void)reg->tc;

    control.b.enable = 1;
    control.b.disable = 0;
    reg->control = control.v;
    (void)reg->control;
    bootExitCritical(critical);
}

void bootIfcExtend(bootIfcChannel_t *ch, uint32_t size)
{
    bootIfcChannelReg_t *reg = (bootIfcChannelReg_t *)ch->hwp_channel;
    reg->tc = size;
}

bool bootIfcIsFifoEmpty(bootIfcChannel_t *ch)
{
    bootIfcChannelReg_t *reg = (bootIfcChannelReg_t *)ch->hwp_channel;
    REG_SYS_IFC_STATUS_T status = {reg->status};
    return (status.b.fifo_empty == 1);
}

bool bootIfcIsRunning(bootIfcChannel_t *ch)
{
    if (ch->hwp_channel == NULL)
        return false;

    bootIfcChannelReg_t *reg = (bootIfcChannelReg_t *)ch->hwp_channel;
    REG_SYS_IFC_STATUS_T status = {reg->status};
    if (status.b.enable == 0)
        return false;
    if (reg->tc == 0)
        return false;
    return true;
}

void bootIfcWaitDone(bootIfcChannel_t *ch)
{
    if (ch->hwp_channel == NULL)
        return;

    bootIfcChannelReg_t *reg = (bootIfcChannelReg_t *)ch->hwp_channel;
    REG_SYS_IFC_STATUS_T status;
    REG_WAIT_COND(REG_FIELD_GET(reg->status, status, enable) == 0 || reg->tc == 0);
}
