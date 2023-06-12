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
//#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_VERBOSE
#include "hwregs.h"
#include "hal_iomux.h"
#include "boot_i2c.h"
#include "boot_platform.h"
#include "boot_compiler.h"
#include "boot_trace.h"
#include "boot_time.h"
#include "boot_delay.h"
#include <stdlib.h>

typedef struct
{
    HWP_I2C_MASTER_T *hwp;
    uint32_t irqn;
} i2cMasterHw_t;

static i2cMasterHw_t hw_masters[] = {
    {.hwp = hwp_i2c1, .irqn = SYS_IRQ_I2C1},
    {.hwp = hwp_i2c2, .irqn = SYS_IRQ_I2C2},
    {.hwp = hwp_i2c3, .irqn = SYS_IRQ_I2C3},
};

#define I2C_MASTER_COUNT BOOT_ARRAY_SIZE(hw_masters)

struct bootI2cMaster
{
    uint32_t name;
    uint32_t irqn;
    uint32_t ref_count;
    HWP_I2C_MASTER_T *hwp;
    bootI2cBps_t clk_mode;
    //bootMutex_t *access_lock;
};

static bootI2cMaster_t gI2cMaster[I2C_MASTER_COUNT] = {};

/// Max i2c OPERATE TIME 10ms
#define HAL_I2C_OPERATE_US 100000

static inline void _i2cSetClock(bootI2cMaster_t *d)
{
#define CONFIG_I2C_BASE_FREQ 52000000 //
    uint32_t new_clk_scale;
    switch (d->clk_mode)
    {
    case BOOT_I2C_BPS_400K:
        new_clk_scale = CONFIG_I2C_BASE_FREQ / (5 * 400000) - 1;
        break;

    case BOOT_I2C_BPS_100K:
    default:
        new_clk_scale = CONFIG_I2C_BASE_FREQ / (5 * 100000) - 1;
        break;
    }

    // Save control register
    REG_I2C_MASTER_CTRL_T ctrl_reg = {d->hwp->ctrl};

    // Disable before changing clock scal
    d->hwp->ctrl = 0;

    bootLog("set I2C Master Clock Prescale = %d\n", new_clk_scale);
    ctrl_reg.b.clock_prescale = new_clk_scale;

    // Restore initial config with new clock scal.
    d->hwp->ctrl = ctrl_reg.v;
}

static inline void _i2cEnable(bootI2cMaster_t *d, bool en)
{
    REG_I2C_MASTER_CTRL_T ctrl_reg = {d->hwp->ctrl};
    ctrl_reg.b.en = en ? 1 : 0;
    d->hwp->ctrl = ctrl_reg.v;
}

static inline void _i2cClearIrqStatus(bootI2cMaster_t *d)
{
    REG_I2C_MASTER_IRQ_CLR_T irq_clr = {};
    irq_clr.b.irq_clr = 1;
    d->hwp->irq_clr = irq_clr.v;
}

static inline bool _busyCheck(bootI2cMaster_t *d)
{
    REG_I2C_MASTER_STATUS_T status = {d->hwp->status};
    return (status.b.tip || status.b.busy);
}

static bool _i2cSendByte(bootI2cMaster_t *d, uint8_t data, int start, int stop)
{
    REG_I2C_MASTER_CMD_T master_cmd = {};

    master_cmd.b.wr = 1;

    if (start)
        master_cmd.b.sta = 1;
    if (stop)
        master_cmd.b.sto = 1;

    uint32_t sc = bootEnterCritical();
    d->hwp->txrx_buffer = (REG32)data;
    d->hwp->cmd = master_cmd.v;
    bootExitCritical(sc);

    bootLog("BOOT %4c data = 0x%08x, CMD = 0x%08x\n", d->name, data, master_cmd.v);

    uint32_t first_time = bootUpTimeUS(), second_time;
    REG_I2C_MASTER_STATUS_T status = {d->hwp->status};
    while (!status.b.irq_status)
    {
        second_time = bootUpTimeUS();
        if (second_time - first_time > HAL_I2C_OPERATE_US)
        {
            if (!stop)
            {
                master_cmd.b.sto = 1;
                d->hwp->cmd = master_cmd.v;
            }
            return false;
        }
        bootDelayUS(100);
        status.v = d->hwp->status;
    };
    _i2cClearIrqStatus(d);

    // Check RxACK
    status.v = d->hwp->status;
    if (status.b.rxack)
    {
        if (!stop)
        {
            // Abort the transfert
            master_cmd.b.sto = 1;
            d->hwp->cmd = master_cmd.v;
            status.v = d->hwp->status;
            while (status.b.rxack)
            {
                second_time = bootUpTimeUS();
                if (second_time - first_time > HAL_I2C_OPERATE_US)
                {
                    bootLog("i2c_send(0x%x) TIMEOUT for I2C_MASTER_RXACK!\n", data);
                    return false;
                }
                bootDelayUS(100);
                status.v = d->hwp->status;
            };
            _i2cClearIrqStatus(d);
        }
    }
    return true;
}

static int _i2cGetByte(bootI2cMaster_t *d, int start, int stop, uint8_t *data)
{
    uint32_t second_time, first_time;
    REG_I2C_MASTER_CMD_T cmd_reg = {};
    cmd_reg.b.rd = 1;
    if (start)
        cmd_reg.b.sta = 1;
    if (stop)
    {
        cmd_reg.b.ack = 1;
        cmd_reg.b.sto = 1;
    }
    d->hwp->cmd = cmd_reg.v;

    bootLog("BOOT %4c CMD  = 0x%08x\n", d->name, cmd_reg);

    first_time = bootUpTimeUS();
    REG_I2C_MASTER_STATUS_T status = {d->hwp->status};
    while (!status.b.irq_status)
    {
        second_time = bootUpTimeUS();
        if (second_time - first_time > HAL_I2C_OPERATE_US)
        {
            bootLog("i2c_send(0x%x, 0x%x) TIMEOUT for not I2C_MASTER_IRQ_STATUS!\n", data, cmd_reg);
            return false;
        }
        bootDelayUS(100);
        status.v = d->hwp->status;
    };

    _i2cClearIrqStatus(d);

    if (NULL != data)
        *data = (uint8_t)d->hwp->txrx_buffer;

    return true;
}

static bool _i2cSendStop(bootI2cMaster_t *d)
{
    uint32_t second_time, first_time;
    REG_I2C_MASTER_CMD_T cmd_reg = {};
    cmd_reg.b.sto = 1;
    d->hwp->cmd = cmd_reg.v;

    first_time = bootUpTimeUS();
    REG_I2C_MASTER_STATUS_T status = {d->hwp->status};
    while (!status.b.irq_status)
    {
        second_time = bootUpTimeUS();
        if (second_time - first_time > HAL_I2C_OPERATE_US)
        {
            bootLog("i2c_send stop TIMEOUT for not I2C_MASTER_IRQ_STATUS!\n");
            return false;
        }
        status.v = d->hwp->status;
    };

    _i2cClearIrqStatus(d);
    return true;
}


static bootI2cMaster_t *_i2cMasterInitGet(uint32_t name, bootI2cBps_t bps)
{
    uint8_t devnum;
    switch (name)
    {
    case BOOT_NAME_I2C1:
        devnum = 0;
        break;

    case BOOT_NAME_I2C2:
        devnum = 1;
        break;

    case BOOT_NAME_I2C3:
        devnum = 2;
        break;

    default:
        return NULL;
    }

    if (devnum >= I2C_MASTER_COUNT)
    {
        bootLog("I2C master acquire fail, no suce device %4c", name);
        return NULL;
    }

    if (bps > BOOT_I2C_BPS_3P5M)
    {
        return NULL;
    }

    uint32_t sc = bootEnterCritical();
    bootI2cMaster_t *d = &gI2cMaster[devnum];
    // should be initialize
    if (d->name != name)
    {
        d->name = name;
        d->hwp = hw_masters[devnum].hwp;
        d->irqn = hw_masters[devnum].irqn;
        d->ref_count = 0;
        d->clk_mode = bps;
        //d->access_lock = bootMutexCreate();
        //BOOT_ASSERT(d->access_lock && d->pm_source, "I2C master init fail");
    }
    bootExitCritical(sc);

    return d;
}

static bool prvSetIomux(bootI2cMaster_t *d)
{
    if (d->name == BOOT_NAME_I2C1)
    {
        halIomuxSetFunction(HAL_IOMUX_FUN_I2C1_SCL);
        halIomuxSetFunction(HAL_IOMUX_FUN_I2C1_SDA);
    }
    else if (d->name == BOOT_NAME_I2C2)
    {
        halIomuxSetFunction(HAL_IOMUX_FUN_I2C2_SCL);
        halIomuxSetFunction((HAL_IOMUX_FUN_I2C2_SDA));
    }
    else if (d->name == BOOT_NAME_I2C3)
    {
        halIomuxSetFunction(HAL_IOMUX_FUN_I2C3_SCL);
        halIomuxSetFunction((HAL_IOMUX_FUN_I2C3_SDA));
    }
    else
    {
        return false;
    }

    return true;
}

bootI2cMaster_t *bootI2cMasterAcquire(uint32_t name, bootI2cBps_t bps)
{
    bootI2cMaster_t *d = _i2cMasterInitGet(name, bps);
    if (d == NULL)
        return NULL;

    if (d->clk_mode != bps)
    {
        bootLog("I2C master[%4c] acquire fail, bps mismatch(current %u, require %u)", d->clk_mode, bps);
        return NULL;
    }
    
    prvSetIomux(d);

    d->ref_count += 1;
    if (d->ref_count == 1)
    {
        _i2cEnable(d, false);
        _i2cSetClock(d);
        _i2cEnable(d, true);
    }

    return d;
}


void bootI2cMasterRelease(bootI2cMaster_t *d)
{
    if (d == NULL)
        return;

    d->ref_count -= 1;
    if (d->ref_count == 0)
    {
        d->name = 0;
        _i2cEnable(d, false);
    }
}

static bool _i2cSetAddress(bootI2cMaster_t *d, const bootI2cSlave_t *slave, bool read)
{
    if (!_i2cSendByte(d, (slave->addr_device << 1), 1, 0))
        return false;

    if (!_i2cSendByte(d, slave->addr_data, 0, 0))
        return false;

    if (slave->reg_16bit)
    {
        if (!_i2cSendByte(d, slave->addr_data_low, 0, 0))
            return false;
    }
    if (read)
    {
        if (!_i2cSendStop(d))
            return false;
        if (!_i2cSendByte(d, ((slave->addr_device << 1) | 0x1), 1, 0))
            return false;
    }

    return true;
}

bool bootI2cWrite(bootI2cMaster_t *d, const bootI2cSlave_t *slave, const uint8_t *data, uint32_t length)
{
    if (d == NULL || slave == NULL)
        return false;

    if (length == 0 || data == NULL)
        return true;

    if (_busyCheck(d))
    {
        bootLog("I2C master[%4c] send fail, busy", d->name);
        return false;
    }

    bool result = false;
    //bootMutexLock(d->access_lock);

    // clear status bit in case previous transfer (Raw) hasn't cleared it.
    _i2cClearIrqStatus(d);

    if (!_i2cSetAddress(d, slave, false))
    {
        bootLog("I2C master[%4c] send fail, set address", d->name);
        goto end;
    }

    // write data before last one.
    for (uint32_t c = 0; c < length - 1; c++)
    {
        if (!_i2cSendByte(d, data[c], 0, 0))
        {
            bootLog("I2C master[%4c] send fail, send data fail", d->name);
            goto end;
        }
    }

    // Send last byte with stop condition
    if (!_i2cSendByte(d, data[length - 1], 0, 1))
    {
        bootLog("I2C master[%4c] send last byte fail", d->name);
        goto end;
    }

    result = true;

end:
    //bootMutexUnlock(d->access_lock);
    return result;
}

bool bootI2cRead(bootI2cMaster_t *d, const bootI2cSlave_t *slave, uint8_t *buf, uint32_t length)
{
    if (d == NULL || slave == NULL)
        return false;

    if (buf == NULL || length == 0)
        return true;

    if (_busyCheck(d))
    {
        bootLog("I2C master[%4c] read fail, busy", d->name);
        return false;
    }

    bool result = false;
    //bootMutexLock(d->access_lock);

    // clear status bit in case previous transfer (Raw) hasn't cleared it.
    _i2cClearIrqStatus(d);

    // write slave address (write mode, to write memory address)
    if (!_i2cSetAddress(d, slave, true))
    {
        bootLog("I2C master[%4c] read fail, send address", d->name);
        goto end;
    }

    // read all values but the last one
    for (uint32_t c = 0; c < length - 1; c++)
    {
        // Read value
        if (!_i2cGetByte(d, 0, 0, &buf[c]))
        {
            bootLog("I2C master[%4c] read fail, read data fail", d->name);
            goto end;
        }
    }

    // Read last value - send no acknowledge - send stop condition/bit
    if (!_i2cGetByte(d, 0, 1, &buf[length - 1]))
    {
        bootLog("I2C master[%4c] read fail, read last byte fail", d->name);
        goto end;
    }

    result = true;

end:
    //bootMutexUnlock(d->access_lock);
    return result;
}

bool bootI2cWriteRawByte(bootI2cMaster_t *d, uint8_t send_byte, uint32_t cmd_mask)
{
    if (d == NULL)
        return false;

    if (_busyCheck(d))
    {
        bootLog("I2C master[%4c] write raw fail, busy", d->name);
        return false;
    }

    bool result = false;
    uint32_t second_time, first_time;
    //bootMutexLock(d->access_lock);

    first_time = bootUpTimeUS();
    REG_I2C_MASTER_STATUS_T status = {d->hwp->status};

    while (status.b.tip)
    {
        second_time = bootUpTimeUS();
        if (second_time - first_time > HAL_I2C_OPERATE_US)
        {
            bootLog("I2C master[%4c] send raw byte fail, timeout", d->name);
            goto end;
        }
        status.v = d->hwp->status;
    }

    // Write slave address
    d->hwp->txrx_buffer = (REG32)send_byte;
    d->hwp->cmd = (REG32)cmd_mask;
    result = true;

end:
    //bootMutexUnlock(d->access_lock);
    return result;
}

bool bootI2cReadRawByte(bootI2cMaster_t *d, uint8_t *data, uint32_t cmd_mask)
{
    if (d == NULL || data == NULL)
        return false;

    if (_busyCheck(d))
    {
        bootLog("I2C master[%4c] read raw fail, busy", d->name);
        return false;
    }

    bool result = false;
    uint32_t second_time, first_time;
    //bootMutexLock(d->access_lock);

    first_time = bootUpTimeUS();
    REG_I2C_MASTER_STATUS_T status = {d->hwp->status};
    while (status.b.tip)
    {
        second_time = bootUpTimeUS();
        if (second_time - first_time > HAL_I2C_OPERATE_US)
        {
            bootLog("I2C master[%4c] read raw fail, wait tip timeout", d->name);
            goto end;
        }
        status.v = d->hwp->status;
    }

    _i2cClearIrqStatus(d);

    // read value - send no acknowledge - send stop condition/bit
    d->hwp->cmd = cmd_mask;

    // polling on the TIP flag
    status.v = d->hwp->status;
    while (!(status.b.irq_status))
    {
        second_time = bootUpTimeUS();
        if (second_time - first_time > HAL_I2C_OPERATE_US)
        {
            bootLog("I2C master[%4c] read raw fail, wait read done timeout", d->name);
            goto end;
        }
        status.v = d->hwp->status;
    };

    _i2cClearIrqStatus(d);
    *data = (uint8_t)d->hwp->txrx_buffer;
    result = true;

end:
    //bootMutexUnlock(d->access_lock);
    return result;
}

bool bootI2cChangeFreq(bootI2cMaster_t *d, bootI2cBps_t bps)
{
    if (d == NULL || bps > BOOT_I2C_BPS_400K)
        return false;

    //bootMutexLock(d->access_lock);
    d->clk_mode = bps;
    //bootMutexUnlock(d->access_lock);
    return true;
}

