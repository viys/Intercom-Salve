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

//#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

#include "hwregs.h"
#include "boot_adc.h"
#include "boot_adc_efuse.h"
//#include "boot_pmic_intr.h"
#include "boot_efuse_pmic.h"
#include "boot_adi_bus.h"
#include "boot_platform.h"

#define ADC_RESULT_NUM 7

#define DELAYUS(us) bootDelayUS(us)


#define AUXADC_CAL_EFUSE 1
#define ADC_NO_CAL 0
#define ADC_EFUSE_CAL_NO_USE 0xffffffff
#define RATIO(_n_, _d_) (_n_ << 16 | _d_)

static uint32_t anaChipId;
static bool gClock26MAdcFlag = false;

/**
 * @sample_speed:   0:quick mode, 1: slow mode
 * @scale:      0:little scale, 1:big scale
 * @hw_channel_delay:   0:disable, 1:enable
 * @channel_id:     channel id of software, Or dedicatid hw channel number
 * @channel_type:   0: software, 1: slow hardware , 2: fast hardware, recommended use 0
 */

struct adc_sample_data
{
    int32_t sample_speed;
    int32_t scale;
    int32_t hw_channel_delay;
    int32_t channel_id;
    int32_t channel_type;
    int32_t result;
};

static struct rda_adc_cal big_scale_cal = {
    4200,
    3310,
    3600,
    2832,
    ADC_NO_CAL,
};

static struct rda_adc_cal small_sclae_cal = {
    1000,
    3413,
    100,
    341,
    ADC_NO_CAL,
};

static struct rda_adc_cal headmic_sclae_cal = {
    1000,
    833,
    100,
    80,
    ADC_NO_CAL,
};

static struct rda_adc_cal scale01_cal = {
    2000,
    200,
    1000,
    50,
    ADC_NO_CAL,
};
static struct rda_adc_cal scale02_cal = {
    3000,
    395,
    2000,
    200,
    ADC_NO_CAL,
};

static const struct adc_efuse_para adcEffusePara = {
    .small_adc_p0 = 833,
    .small_adc_p1 = 80,
    .small_vol_p0 = 1000,
    .small_vol_p1 = 100,
    .small_cal_efuse_blk = 19,
    .small_cal_efuse_bit = 16,
    .scale01_cal_efuse_blk = ADC_EFUSE_CAL_NO_USE,
    .scale02_cal_efuse_blk = ADC_EFUSE_CAL_NO_USE,
    .big_adc_p0 = 733,
    .big_adc_p1 = 856,
    .big_vol_p0 = 3600,
    .big_vol_p1 = 4200,
    .big_cal_efuse_blk = 18,
    .big_cal_efuse_bit = 16,
    .headmic_adc_p0 = 818,
    .headmic_adc_p1 = 82,
    .headmic_vol_p0 = 1000,
    .headmic_vol_p1 = 100,
    .headmic_cal_efuse_blk = 20,
    .headmic_cal_efuse_bit = 16,
    .blk_width = 16,
    .adc_data_off = 128,
};

uint32_t _adcGetEffuseBits(int32_t bit_index, int32_t length)
{
    uint32_t val = 0;
    bootEfusePmicOpen();
    bootEfusePmicRead(bit_index, &val);
    bootEfusePmicClose();
    return val;
}

static bool _adcEffuseValid(void)
{
    uint32_t data = 0;
    data = _adcGetEffuseBits(adcEffusePara.big_cal_efuse_blk, adcEffusePara.big_cal_efuse_bit);
    data &= 0xffff;
    if (!data)
    {
        bootLog("adc: no calibration use default value. Efuse block %d, data: 0x%x",
                 adcEffusePara.big_cal_efuse_blk, data);
        return false;
    }
    else
    {
        bootLog("adc: have calibrated");
        return true;
    }
}

static void _adcEnable(void)
{
    REG_RDA2720M_GLOBAL_MODULE_EN0_T moudle_en0;
    REG_RDA2720M_GLOBAL_ARM_CLK_EN0_T arm_clk_en0;
    REG_RDA2720M_GLOBAL_XTL_WAIT_CTRL0_T xtl_wait_ctrl0;
    REG_RDA2720M_INT_INT_EN_T int_en;
    REG_RDA2720M_ADC_ADC_CFG_CTRL_T adc_cfg_ctrl;

    halAdiBusBatchChange(
        &hwp_rda2720mGlobal->module_en0,
        REG_FIELD_MASKVAL1(moudle_en0, adc_en, 1),
        &hwp_rda2720mGlobal->arm_clk_en0,
        REG_FIELD_MASKVAL2(arm_clk_en0, clk_auxadc_en, 1, clk_auxad_en, 1),
        &hwp_rda2720mGlobal->xtl_wait_ctrl0,
        REG_FIELD_MASKVAL1(xtl_wait_ctrl0, xtl_en, 1),
        &hwp_rda2720mInt->int_en,
        REG_FIELD_MASKVAL1(int_en, adc_int_en, 1),
        HAL_ADI_BUS_CHANGE_END);

    halAdiBusWrite(&hwp_rda2720mAdc->adc_cfg_int_en, 1);
    halAdiBusWrite(&hwp_rda2720mAdc->adc_cfg_int_clr, 1);
    halAdiBusBatchChange(
        &hwp_rda2720mAdc->adc_cfg_ctrl,
        REG_FIELD_MASKVAL1(adc_cfg_ctrl, adc_offset_cal_en, 1),
        HAL_ADI_BUS_CHANGE_END);
}

static void _adcDumpRegister(void)
{
    uint32_t base = (uint32_t)&hwp_rda2720mAdc->adc_version;
    uint32_t end = base + 0xb4;
    uint32_t value;
    bootLog("adc: adc_dump_register begin");
    for (; base < end; base += 4)
    {
        value = halAdiBusRead((uint32_t *)base);
        bootLog("adc: reg addr = 0x%x, value = 0x%x", base, value);
    }
    bootLog("adc: adc_dump_register end!");
}

static int32_t _adcConfig(struct adc_sample_data *adc)
{
    uint32_t delay_en;

    delay_en = (adc->hw_channel_delay ? 1 : 0);
    REG_RDA2720M_ADC_ADC_SW_CH_CFG_T adc_sw_ch_cfg;
    REG_RDA2720M_ADC_ADC_HW_CH_DELAY_T adc_hw_ch_delay;
    REG_RDA2720M_ADC_ADC_SLOW_HW_CH0_CFG_T adc_slow_hw_ch0_cfg;
    REG_RDA2720M_ADC_ADC_SLOW_HW_CH1_CFG_T adc_slow_hw_ch1_cfg;
    REG_RDA2720M_ADC_ADC_SLOW_HW_CH2_CFG_T adc_slow_hw_ch2_cfg;
    REG_RDA2720M_ADC_ADC_SLOW_HW_CH3_CFG_T adc_slow_hw_ch3_cfg;
    REG_RDA2720M_ADC_ADC_SLOW_HW_CH4_CFG_T adc_slow_hw_ch4_cfg;
    REG_RDA2720M_ADC_ADC_SLOW_HW_CH5_CFG_T adc_slow_hw_ch5_cfg;
    REG_RDA2720M_ADC_ADC_SLOW_HW_CH6_CFG_T adc_slow_hw_ch6_cfg;
    REG_RDA2720M_ADC_ADC_SLOW_HW_CH7_CFG_T adc_slow_hw_ch7_cfg;
    REG_RDA2720M_ADC_ADC_FAST_HW_CH0_CFG_T adc_fast_hw_ch0_cfg;
    REG_RDA2720M_ADC_ADC_FAST_HW_CH1_CFG_T adc_fast_hw_ch1_cfg;
    REG_RDA2720M_ADC_ADC_FAST_HW_CH2_CFG_T adc_fast_hw_ch2_cfg;
    REG_RDA2720M_ADC_ADC_FAST_HW_CH3_CFG_T adc_fast_hw_ch3_cfg;
    REG_RDA2720M_ADC_ADC_FAST_HW_CH4_CFG_T adc_fast_hw_ch4_cfg;
    REG_RDA2720M_ADC_ADC_FAST_HW_CH5_CFG_T adc_fast_hw_ch5_cfg;
    REG_RDA2720M_ADC_ADC_FAST_HW_CH6_CFG_T adc_fast_hw_ch6_cfg;
    REG_RDA2720M_ADC_ADC_FAST_HW_CH7_CFG_T adc_fast_hw_ch7_cfg;

    halAdiBusBatchChange(
        &hwp_rda2720mAdc->adc_sw_ch_cfg,
        REG_FIELD_MASKVAL3(adc_sw_ch_cfg, adc_slow, adc->sample_speed, adc_scale, adc->scale, adc_cs, adc->channel_id),
        HAL_ADI_BUS_CHANGE_END);

    if (adc->channel_type > 0)
    {

        halAdiBusBatchChange(
            &hwp_rda2720mAdc->adc_hw_ch_delay,
            REG_FIELD_MASKVAL1(adc_hw_ch_delay, hw_ch_delay, adc->hw_channel_delay),
            HAL_ADI_BUS_CHANGE_END);

        if (adc->channel_type == 1) // slow hw channel
        {

            if (adc->channel_id == 0)
            {
                halAdiBusBatchChange(
                    &hwp_rda2720mAdc->adc_slow_hw_ch0_cfg,
                    REG_FIELD_MASKVAL4(adc_slow_hw_ch0_cfg, req_slow, adc->sample_speed, req_scale, adc->scale, req_cs, adc->channel_id, req_delay_en, delay_en),
                    HAL_ADI_BUS_CHANGE_END);
            }
            else if (adc->channel_id == 1)
            {

                halAdiBusBatchChange(
                    &hwp_rda2720mAdc->adc_slow_hw_ch1_cfg,
                    REG_FIELD_MASKVAL4(adc_slow_hw_ch1_cfg, req_slow, adc->sample_speed, req_scale, adc->scale, req_cs, adc->channel_id, req_delay_en, delay_en),
                    HAL_ADI_BUS_CHANGE_END);
            }
            else if (adc->channel_id == 2)
            {

                halAdiBusBatchChange(
                    &hwp_rda2720mAdc->adc_slow_hw_ch2_cfg,
                    REG_FIELD_MASKVAL4(adc_slow_hw_ch2_cfg, req_slow, adc->sample_speed, req_scale, adc->scale, req_cs, adc->channel_id, req_delay_en, delay_en),
                    HAL_ADI_BUS_CHANGE_END);
            }
            else if (adc->channel_id == 3)
            {

                halAdiBusBatchChange(
                    &hwp_rda2720mAdc->adc_slow_hw_ch3_cfg,
                    REG_FIELD_MASKVAL4(adc_slow_hw_ch3_cfg, req_slow, adc->sample_speed, req_scale, adc->scale, req_cs, adc->channel_id, req_delay_en, delay_en),
                    HAL_ADI_BUS_CHANGE_END);
            }
            else if (adc->channel_id == 4)
            {

                halAdiBusBatchChange(
                    &hwp_rda2720mAdc->adc_slow_hw_ch4_cfg,
                    REG_FIELD_MASKVAL4(adc_slow_hw_ch4_cfg, req_slow, adc->sample_speed, req_scale, adc->scale, req_cs, adc->channel_id, req_delay_en, delay_en),
                    HAL_ADI_BUS_CHANGE_END);
            }
            else if (adc->channel_id == 5)
            {

                halAdiBusBatchChange(
                    &hwp_rda2720mAdc->adc_slow_hw_ch5_cfg,
                    REG_FIELD_MASKVAL4(adc_slow_hw_ch5_cfg, req_slow, adc->sample_speed, req_scale, adc->scale, req_cs, adc->channel_id, req_delay_en, delay_en),
                    HAL_ADI_BUS_CHANGE_END);
            }
            else if (adc->channel_id == 6)
            {

                halAdiBusBatchChange(
                    &hwp_rda2720mAdc->adc_slow_hw_ch6_cfg,
                    REG_FIELD_MASKVAL4(adc_slow_hw_ch6_cfg, req_slow, adc->sample_speed, req_scale, adc->scale, req_cs, adc->channel_id, req_delay_en, delay_en),
                    HAL_ADI_BUS_CHANGE_END);
            }
            else if (adc->channel_id == 7)
            {

                halAdiBusBatchChange(
                    &hwp_rda2720mAdc->adc_slow_hw_ch7_cfg,
                    REG_FIELD_MASKVAL4(adc_slow_hw_ch7_cfg, req_slow, adc->sample_speed, req_scale, adc->scale, req_cs, adc->channel_id, req_delay_en, delay_en),
                    HAL_ADI_BUS_CHANGE_END);
            }
        }
        else if (adc->channel_type == 2)
        {
            if (adc->channel_id == 0)
            {
                halAdiBusBatchChange(
                    &hwp_rda2720mAdc->adc_fast_hw_ch0_cfg,
                    REG_FIELD_MASKVAL4(adc_fast_hw_ch0_cfg, frq_slow, adc->sample_speed, frq_scale, adc->scale, frq_cs, adc->channel_id, frq_delay_en, delay_en),
                    HAL_ADI_BUS_CHANGE_END);
            }
            else if (adc->channel_id == 1)
            {

                halAdiBusBatchChange(
                    &hwp_rda2720mAdc->adc_fast_hw_ch1_cfg,
                    REG_FIELD_MASKVAL4(adc_fast_hw_ch1_cfg, frq_slow, adc->sample_speed, frq_scale, adc->scale, frq_cs, adc->channel_id, frq_delay_en, delay_en),
                    HAL_ADI_BUS_CHANGE_END);
            }
            else if (adc->channel_id == 2)
            {

                halAdiBusBatchChange(
                    &hwp_rda2720mAdc->adc_fast_hw_ch2_cfg,
                    REG_FIELD_MASKVAL4(adc_fast_hw_ch2_cfg, frq_slow, adc->sample_speed, frq_scale, adc->scale, frq_cs, adc->channel_id, frq_delay_en, delay_en),
                    HAL_ADI_BUS_CHANGE_END);
            }
            else if (adc->channel_id == 3)
            {

                halAdiBusBatchChange(
                    &hwp_rda2720mAdc->adc_fast_hw_ch3_cfg,
                    REG_FIELD_MASKVAL4(adc_fast_hw_ch3_cfg, frq_slow, adc->sample_speed, frq_scale, adc->scale, frq_cs, adc->channel_id, frq_delay_en, delay_en),
                    HAL_ADI_BUS_CHANGE_END);
            }
            else if (adc->channel_id == 4)
            {

                halAdiBusBatchChange(
                    &hwp_rda2720mAdc->adc_fast_hw_ch4_cfg,
                    REG_FIELD_MASKVAL4(adc_fast_hw_ch4_cfg, frq_slow, adc->sample_speed, frq_scale, adc->scale, frq_cs, adc->channel_id, frq_delay_en, delay_en),
                    HAL_ADI_BUS_CHANGE_END);
            }
            else if (adc->channel_id == 5)
            {

                halAdiBusBatchChange(
                    &hwp_rda2720mAdc->adc_fast_hw_ch5_cfg,
                    REG_FIELD_MASKVAL4(adc_fast_hw_ch5_cfg, frq_slow, adc->sample_speed, frq_scale, adc->scale, frq_cs, adc->channel_id, frq_delay_en, delay_en),
                    HAL_ADI_BUS_CHANGE_END);
            }
            else if (adc->channel_id == 6)
            {

                halAdiBusBatchChange(
                    &hwp_rda2720mAdc->adc_fast_hw_ch6_cfg,
                    REG_FIELD_MASKVAL4(adc_fast_hw_ch6_cfg, frq_slow, adc->sample_speed, frq_scale, adc->scale, frq_cs, adc->channel_id, frq_delay_en, delay_en),
                    HAL_ADI_BUS_CHANGE_END);
            }
            else if (adc->channel_id == 7)
            {

                halAdiBusBatchChange(
                    &hwp_rda2720mAdc->adc_fast_hw_ch7_cfg,
                    REG_FIELD_MASKVAL4(adc_fast_hw_ch7_cfg, frq_slow, adc->sample_speed, frq_scale, adc->scale, frq_cs, adc->channel_id, frq_delay_en, delay_en),
                    HAL_ADI_BUS_CHANGE_END);
            }
        }
    }
    return 0;
}

static uint32_t _adcSmallScaleTovol(uint16_t adcvalue)
{
    int32_t vol;

    vol = small_sclae_cal.p0_vol - small_sclae_cal.p1_vol;
    vol = vol * (adcvalue - small_sclae_cal.p0_adc);
    vol = vol / (small_sclae_cal.p0_adc - small_sclae_cal.p1_adc);
    vol = vol + small_sclae_cal.p0_vol;
    if (vol < 0)
        vol = 0;

    return vol;
}

static uint32_t _adcScale01Tovol(uint16_t adcvalue)
{
    int32_t vol;

    vol = scale01_cal.p0_vol - scale01_cal.p1_vol;
    vol = vol * (adcvalue - scale01_cal.p0_adc);
    vol = vol / (scale01_cal.p0_adc - scale01_cal.p1_adc);
    vol = vol + scale01_cal.p0_vol;
    if (vol < 0)
        vol = 0;

    return vol;
}

static uint32_t _adcScale02Tovol(uint16_t adcvalue)
{
    int32_t vol;

    vol = scale02_cal.p0_vol - scale02_cal.p1_vol;
    vol = vol * (adcvalue - scale02_cal.p0_adc);
    vol = vol / (scale02_cal.p0_adc - scale02_cal.p1_adc);
    vol = vol + scale02_cal.p0_vol;
    if (vol < 0)
        vol = 0;

    return vol;
}

static uint32_t _adcHeadmicTovol(uint16_t adcvalue)
{
    int32_t vol;

    vol = small_sclae_cal.p0_vol - small_sclae_cal.p1_vol;
    vol = vol * (adcvalue - small_sclae_cal.p0_adc);
    vol = vol / (small_sclae_cal.p0_adc - small_sclae_cal.p1_adc);
    vol = vol + small_sclae_cal.p0_vol;
    if (vol < 0)
        vol = 0;

    return vol;
}

static uint32_t _adcBigScaleAdcToVol(uint16_t adcvalue)
{
    int32_t vol;

    vol = big_scale_cal.p0_vol - big_scale_cal.p1_vol;
    vol = vol * (adcvalue - big_scale_cal.p0_adc);
    vol = vol / (big_scale_cal.p0_adc - big_scale_cal.p1_adc);
    vol = vol + big_scale_cal.p0_vol;
    if (vol < 0)
        vol = 0;

    return vol;
}

static int32_t _adcRatio(int32_t channel, int32_t scale, int32_t con_mode)
{
    switch (channel)
    {
    case ADC_CHANNEL_BAT_DET:
        return RATIO(1, 1);
    case ADC_CHANNEL_VBATSENSE:
        return RATIO(100, 406);
    case ADC_CHANNEL_VCHGSEN:
        return RATIO(68, 900);
    case ADC_CHANNEL_HEADMIC:
        switch (con_mode)
        {
        case HEADMICIN_DET_SCALE1:
        case HEADMICIN_L_INT_SCALE1:
        case HP_L_SCALE1:
        case HP_R_SCALE1:
            return RATIO(1, 4);
        case VDD_VB_SCALE1:
        case VDD_PA_SCALE1:
        case MICBIAS_SCALE1:
        case HEADMIC_BIAS_SCALE1:
            return RATIO(1, 16);
        case HEADMICIN_DET_SCALE0:
        case HEADMICIN_L_INT_SCALE0:
        case HP_L_SCALE0:
        case HP_R_SCALE0:
            return RATIO(1, 1);
        case VDD_VB_SCALE0:
        case VDD_PA_SCALE0:
        case MICBIAS_SCALE0:
        case HEADMIC_BIAS_SCALE0:
            return RATIO(1, 4);
        default:
            return RATIO(1, 1);
        }
    case ADC_CHANNEL_DCDC_CALOUT:
        switch (con_mode)
        {
        case DCDC_VCORE:
            return RATIO(1, 1);
        case DCDC_VGEN:
            return RATIO(18, 37);
        case DCDC_VPA:
            return RATIO(18, 68);
        default:
            return RATIO(1, 1);
        }
    case ADC_CHANNEL_LDO_CALOUT0:
    case ADC_CHANNEL_LDO_CALOUT1:
    case ADC_CHANNEL_LDO_CALOUT2:
        return RATIO(3, 8);
    case ADC_CHANNEL_PROG2ADC:
        return RATIO(48, 100);

    default:
        switch (scale)
        {
        case ADC_SCALE_1V250:
            return RATIO(1, 1);
        case ADC_SCALE_2V444:
            return RATIO(1000, 1955);
        case ADC_SCALE_3V233:
            return RATIO(1000, 2586);
        case ADC_SCALE_5V000:
            return RATIO(100, 406);
        default:
            return RATIO(1, 1);
        }
        return RATIO(1, 1);
    }
    return RATIO(1, 1);
}

static uint16_t _adcChanToCaliRaw(int32_t scale, uint16_t adcvalue)
{
    uint32_t temp;
    temp = adcvalue;
    switch (scale)
    {
    case ADC_SCALE_1V250:
        return (uint16_t)temp;
    case ADC_SCALE_2V444:
        return (uint16_t)(temp * 1000 / 1955);
    case ADC_SCALE_3V233:
        return (uint16_t)(temp * 1000 / 2586);
    case ADC_SCALE_5V000:
        return (uint16_t)(temp * 100 / 406);

    default:
        return adcvalue;
    }
}

static void _adcGetVolRatio(uint32_t channel_id, int32_t scale, int32_t con_mode,
                            uint32_t *div_numerators,
                            uint32_t *div_denominators)
{
    uint32_t ratio;

    ratio = _adcRatio(channel_id, scale, con_mode);
    *div_numerators = ratio >> 16;
    *div_denominators = ratio << 16 >> 16;
}

static void _adcGetSmallScaleCal(uint32_t *p_cal_data)
{
    uint32_t deta = 0;
    uint16_t adc_data = 0;
    deta = _adcGetEffuseBits(adcEffusePara.small_cal_efuse_blk, adcEffusePara.small_cal_efuse_bit);
    deta &= 0xFFFF;

    bootLog("adc: get small calib efuse block %d, deta: 0x%x", adcEffusePara.small_cal_efuse_blk, deta);
    /* adc 0.1V */
    adc_data =
        (((deta >> 8) & 0x00FF) + adcEffusePara.small_adc_p1 -
         adcEffusePara.adc_data_off) *
        4;
    bootLog("adc: 0.1V adc data 0x%x", adc_data);
    p_cal_data[1] = (adcEffusePara.small_vol_p1) | (adc_data << 16);

    /* adc 1.0V */
    adc_data =
        ((deta & 0x00FF) + adcEffusePara.small_adc_p0 -
         adcEffusePara.adc_data_off) *
        4;
    bootLog("adc: 1.0V adc data 0x%x", adc_data);
    p_cal_data[0] = (adcEffusePara.small_vol_p0) | (adc_data << 16);
}

static void _adcGetBigScaleCal(uint32_t *p_cal_data)
{
    uint32_t deta = 0;
    uint32_t cal_data = 0;

    deta = _adcGetEffuseBits(adcEffusePara.big_cal_efuse_blk, adcEffusePara.big_cal_efuse_bit);
    deta &= 0xFFFF;

    bootLog("adc: get big scale efuse block %d, deta: 0x%x", adcEffusePara.big_cal_efuse_blk, deta);
    /*adc 3.6V */
    cal_data = ((deta >> 8) & 0x00FF) + adcEffusePara.big_adc_p0 - adcEffusePara.adc_data_off;
    p_cal_data[1] = (adcEffusePara.big_vol_p0) | ((cal_data << 2) << 16);
    /*adc 4.2V */
    cal_data = (deta & 0x00FF) + adcEffusePara.big_adc_p1 - adcEffusePara.adc_data_off;
    p_cal_data[0] = (adcEffusePara.big_vol_p1) | ((cal_data << 2) << 16);
}
static void _adcHeadmicScaleCal(uint32_t *p_cal_data)
{
    uint32_t deta = 0;
    uint16_t adc_data = 0;
    deta = _adcGetEffuseBits(adcEffusePara.headmic_cal_efuse_blk, adcEffusePara.headmic_cal_efuse_bit);
    deta &= 0xFFFF;

    bootLog("adc: get headmic scale efuse block %d, deta: 0x%x", adcEffusePara.headmic_cal_efuse_blk, deta);
    /* adc 0.1V */
    adc_data =
        ((deta & 0x00FF) + adcEffusePara.small_adc_p1 -
         adcEffusePara.adc_data_off) *
        4;
    bootLog("adc: 0.1V adc data 0x%x", adc_data);
    p_cal_data[1] = (adcEffusePara.small_vol_p1) | (adc_data << 16);

    /* adc 1.0V */
    adc_data =
        (((deta >> 8) & 0x00FF) + adcEffusePara.small_adc_p0 -
         adcEffusePara.adc_data_off) *
        4;
    bootLog("adc: 1.0V adc data 0x%x", adc_data);
    p_cal_data[0] = (adcEffusePara.small_vol_p0) | (adc_data << 16);
}

static void _adcGetAdcScale01cal(uint32_t *p_cal_data)
{
    uint32_t deta = 0;
    uint32_t cal_data = 0;
    if (adcEffusePara.scale01_cal_efuse_blk == ADC_EFUSE_CAL_NO_USE)
    {
        p_cal_data[0] = ADC_EFUSE_CAL_NO_USE;
        return;
    }
    deta = _adcGetEffuseBits(adcEffusePara.scale01_cal_efuse_blk, adcEffusePara.scale01_cal_efuse_bit);
    deta &= 0xFFFF;

    bootLog("adc: get scale01 efuse block %d, deta: 0x%x", adcEffusePara.scale01_cal_efuse_blk, deta);

    /*adc 1V */
    cal_data =
        ((deta >> 8) & 0x00FF) + adcEffusePara.big_adc_p0 - adcEffusePara.adc_data_off;
    p_cal_data[1] = (adcEffusePara.big_vol_p0) | ((cal_data << 2) << 16);
    /*adc 2V */
    cal_data = (deta & 0x00FF) + adcEffusePara.big_adc_p1 - adcEffusePara.adc_data_off;
    p_cal_data[0] = (adcEffusePara.big_vol_p1) | ((cal_data << 2) << 16);
}

static void _adcGetAdcScale02Cal(uint32_t *p_cal_data)
{
    uint32_t deta = 0;
    uint32_t cal_data = 0;

    if (adcEffusePara.scale02_cal_efuse_blk == ADC_EFUSE_CAL_NO_USE)
    {
        p_cal_data[0] = ADC_EFUSE_CAL_NO_USE;
        return;
    }
    deta = _adcGetEffuseBits(adcEffusePara.scale02_cal_efuse_blk, adcEffusePara.scale02_cal_efuse_bit);
    deta &= 0xFFFF;

    bootLog("adc: get scale02 efuse block %d, deta: 0x%x", adcEffusePara.scale01_cal_efuse_blk, deta);

    /*adc 2V */
    cal_data =
        ((deta >> 8) & 0x00FF) + adcEffusePara.big_adc_p0 - adcEffusePara.adc_data_off;
    p_cal_data[1] = (adcEffusePara.big_vol_p0) | ((cal_data << 2) << 16);
    /*adc 3V */
    cal_data = (deta & 0x00FF) + adcEffusePara.big_adc_p1 - adcEffusePara.adc_data_off;
    p_cal_data[0] = (adcEffusePara.big_vol_p1) | ((cal_data << 2) << 16);
}

static void _adcBigScaleEffuseGet(void)
{
    uint32_t efuse_cal_data[2] = {0};

    _adcGetBigScaleCal(efuse_cal_data);

    big_scale_cal.p0_vol = efuse_cal_data[0] & 0xffff;
    big_scale_cal.p0_adc = (efuse_cal_data[0] >> 16) & 0xffff;
    big_scale_cal.p1_vol = efuse_cal_data[1] & 0xffff;
    big_scale_cal.p1_adc = (efuse_cal_data[1] >> 16) & 0xffff;
    big_scale_cal.cal_type = AUXADC_CAL_EFUSE;
    bootLog("adc: efuse big cal %d,%d,%d,%d,cal_type:%d",
             big_scale_cal.p0_vol, big_scale_cal.p0_adc,
             big_scale_cal.p1_vol, big_scale_cal.p1_adc,
             big_scale_cal.cal_type);
}

static void _adcSmallScaleEffuseGet(void)
{
    uint32_t efuse_cal_data[2] = {0};

    _adcGetSmallScaleCal(efuse_cal_data);

    small_sclae_cal.p0_vol = efuse_cal_data[0] & 0xffff;
    small_sclae_cal.p0_adc = (efuse_cal_data[0] >> 16) & 0xffff;
    small_sclae_cal.p1_vol = efuse_cal_data[1] & 0xffff;
    small_sclae_cal.p1_adc = (efuse_cal_data[1] >> 16) & 0xffff;
    small_sclae_cal.cal_type = AUXADC_CAL_EFUSE;
    bootLog("adc: efuse small cal %d,%d,%d,%d,cal_type:%d",
             small_sclae_cal.p0_vol, small_sclae_cal.p0_adc,
             small_sclae_cal.p1_vol, small_sclae_cal.p1_adc,
             small_sclae_cal.cal_type);
}

static void _adcScale01EffuseGet(void)
{
    uint32_t efuse_cal_data[2] = {0};

    _adcGetAdcScale01cal(efuse_cal_data);
    if (efuse_cal_data[0] == ADC_EFUSE_CAL_NO_USE)
        return;
    scale01_cal.p0_vol = efuse_cal_data[0] & 0xffff;
    scale01_cal.p0_adc = (efuse_cal_data[0] >> 16) & 0xffff;
    scale01_cal.p1_vol = efuse_cal_data[1] & 0xffff;
    scale01_cal.p1_adc = (efuse_cal_data[1] >> 16) & 0xffff;
    scale01_cal.cal_type = AUXADC_CAL_EFUSE;
    bootLog("adc: efuse small cal %d,%d,%d,%d,cal_type:%d",
             scale01_cal.p0_vol, scale01_cal.p0_adc,
             scale01_cal.p1_vol, scale01_cal.p1_adc,
             scale01_cal.cal_type);
}

static void _adcScale02EffuseGet(void)
{
    uint32_t efuse_cal_data[2] = {0};

    _adcGetAdcScale02Cal(efuse_cal_data);
    if (efuse_cal_data[0] == ADC_EFUSE_CAL_NO_USE)
        return;

    scale02_cal.p0_vol = efuse_cal_data[0] & 0xffff;
    scale02_cal.p0_adc = (efuse_cal_data[0] >> 16) & 0xffff;
    scale02_cal.p1_vol = efuse_cal_data[1] & 0xffff;
    scale02_cal.p1_adc = (efuse_cal_data[1] >> 16) & 0xffff;
    scale02_cal.cal_type = AUXADC_CAL_EFUSE;
    bootLog("adc: efuse small cal %d,%d,%d,%d,cal_type:%d",
             scale02_cal.p0_vol, scale02_cal.p0_adc,
             scale02_cal.p1_vol, scale02_cal.p1_adc,
             scale02_cal.cal_type);
}

static void _adcheadmicEffuseGet(void)
{
    uint32_t efuse_cal_data[2] = {0};

    _adcHeadmicScaleCal(efuse_cal_data);

    headmic_sclae_cal.p0_vol = efuse_cal_data[0] & 0xffff;
    headmic_sclae_cal.p0_adc = (efuse_cal_data[0] >> 16) & 0xffff;
    headmic_sclae_cal.p1_vol = efuse_cal_data[1] & 0xffff;
    headmic_sclae_cal.p1_adc = (efuse_cal_data[1] >> 16) & 0xffff;
    headmic_sclae_cal.cal_type = AUXADC_CAL_EFUSE;
    bootLog("adc: efuse small cal %d,%d,%d,%d,cal_type:%d",
             headmic_sclae_cal.p0_vol, headmic_sclae_cal.p0_adc,
             headmic_sclae_cal.p1_vol, headmic_sclae_cal.p1_adc,
             headmic_sclae_cal.cal_type);
}

static bool _adcGetValue(struct adc_sample_data *adc)
{
    int32_t cnt_timeout = 50;
    int32_t num = 0;
    uint32_t ret = 0;
    REG_RDA2720M_ADC_ADC_CFG_CTRL_T adc_cfg_ctrl;

    if (!adc)
        return false;

    halAdiBusBatchChange(
        &hwp_rda2720mAdc->adc_cfg_ctrl,
        REG_FIELD_MASKVAL3(adc_cfg_ctrl, adc_en, 0, sw_ch_run, 0, rg_auxad_average, 0),
        HAL_ADI_BUS_CHANGE_END);

    _adcConfig(adc);
    
    halAdiBusWrite(&hwp_rda2720mAdc->adc_cfg_int_clr, 1);

    halAdiBusBatchChange(
        &hwp_rda2720mAdc->adc_cfg_ctrl,
        REG_FIELD_MASKVAL3(adc_cfg_ctrl, sw_ch_run_num, num, adc_en, 1, sw_ch_run, 1),
        HAL_ADI_BUS_CHANGE_END);

    while ((ret == 0) && cnt_timeout--)
    {
        DELAYUS(40);
        ret = halAdiBusRead(&hwp_rda2720mAdc->adc_cfg_int_raw);
    }

    if (cnt_timeout == -1)
    {
        _adcDumpRegister();

        halAdiBusBatchChange(
            &hwp_rda2720mAdc->adc_cfg_ctrl,
            REG_FIELD_MASKVAL1(adc_cfg_ctrl, adc_en, 0),
            HAL_ADI_BUS_CHANGE_END);

        bootLog("_adcGetValue timeout!");
        return false;
    }

    adc->result = halAdiBusRead(&hwp_rda2720mAdc->adc_dat) & (0xfff);

    halAdiBusBatchChange(
        &hwp_rda2720mAdc->adc_cfg_ctrl,
        REG_FIELD_MASKVAL1(adc_cfg_ctrl, adc_en, 0),
        HAL_ADI_BUS_CHANGE_END);

    return true;
}

static int32_t _adcChangAdcToVol(uint16_t channel, int32_t scale, int32_t con_mode, int32_t adc_value)
{
    uint32_t result;
    uint32_t vol = 0;
    uint32_t numerators, denominators;
    uint16_t adcvalue;
    adcvalue = (uint16_t)(adc_value & 0xffff);

    if (scale == ADC_SCALE_1V250)
    {
        if (channel == ADC_CHANNEL_HEADMIC)
        {
            vol = _adcHeadmicTovol(adcvalue);
        }
        else
        {
            vol = _adcSmallScaleTovol(adcvalue);
        }
    }
    else if (scale == ADC_SCALE_2V444)
    {

        if (adcEffusePara.scale01_cal_efuse_blk == ADC_EFUSE_CAL_NO_USE)
        {
            vol = _adcSmallScaleTovol(adcvalue * 1955 / 1000);
        }
        else
        {
            vol = _adcScale01Tovol(adcvalue);
        }
    }
    else if (scale == ADC_SCALE_3V233)
    {

        if (adcEffusePara.scale01_cal_efuse_blk == ADC_EFUSE_CAL_NO_USE)
            vol = _adcSmallScaleTovol(adcvalue * 2586 / 1000);
        else
            vol = _adcScale02Tovol(adcvalue);
    }
    else if (scale == ADC_SCALE_5V000)
    {
        vol = _adcBigScaleAdcToVol(adcvalue);
    }
    else
    {
        bootLog("adc: _adcChangAdcToVol, invalide scale");
    }

    _adcGetVolRatio(channel, scale, con_mode, &numerators, &denominators);
    result = vol * denominators / numerators;

    result = _adcChanToCaliRaw(scale, result);
    return (int32_t)result;
}

static void _adcClock26MRequest()
{
    if(gClock26MAdcFlag == true)
        return;
    
    uint32_t sc = bootEnterCritical();
    gClock26MAdcFlag = true;
    hwp_rfReg->ana_ctrl_reg32 = 1;
    REG_CP_SYSREG_RF_ANA_26M_CTRL_T ana_26m_ctrl = {};
    ana_26m_ctrl.b.enable_clk_26m_audio = 1;
    hwp_sysreg->rf_ana_26m_ctrl_set = ana_26m_ctrl.v;
    bootExitCritical(sc);
}

static void _adcClock26MRelease()
{
    if(gClock26MAdcFlag == false)
        return;
    
    uint32_t sc = bootEnterCritical();
    gClock26MAdcFlag = false;
    REG_CP_SYSREG_RF_ANA_26M_CTRL_T ana_26m_ctrl = {};

    ana_26m_ctrl.b.enable_clk_26m_audio = 1;
    hwp_sysreg->rf_ana_26m_ctrl_clr = ana_26m_ctrl.v;
    hwp_rfReg->ana_ctrl_reg32 = 0;
    bootExitCritical(sc);
}

static void _adcOpenPmic26MclkAdc(void)
{
    _adcClock26MRequest();
}

static void _adcClosePmic26MclkAdc(void)
{
    _adcClock26MRelease();
}

void bootAdcInit(void)
{
    _adcEnable();

    anaChipId = ((uint32_t)halAdiBusRead(&hwp_rda2720mGlobal->chip_id_high) << 16) |
                ((uint32_t)halAdiBusRead(&hwp_rda2720mGlobal->chip_id_low) & 0xFFFF);
    bootLog("adc: the pmic chip id is 0x%x", anaChipId);

    if (true == _adcEffuseValid())
    {
        _adcSmallScaleEffuseGet();
        _adcScale01EffuseGet();
        _adcScale02EffuseGet();
        _adcheadmicEffuseGet();
        _adcBigScaleEffuseGet();
    }
}

static int32_t _bootGetAdcAverage(uint32_t channel, int32_t scale)
{
    int32_t i, j, temp, ret;
    int32_t adc_result[ADC_RESULT_NUM];

    for (i = 0; i < ADC_RESULT_NUM; i++)
    {
        ret = bootAdcGetRawValue(channel, scale);
        if (ret == -1)
            return -1;
        adc_result[i] = ret;
        bootLog("adc: GetAdcAverage i %d, adc:%d ", i, adc_result[i]);
    }

    for (j = 1; j <= ADC_RESULT_NUM - 1; j++)
    {
        for (i = 0; i < ADC_RESULT_NUM - j; i++)
        {
            if (adc_result[i] > adc_result[i + 1])
            {
                temp = adc_result[i];
                adc_result[i] = adc_result[i + 1];
                adc_result[i + 1] = temp;
            }
        }
    }

    bootLog("adc: AdcAverage   adc:%d ", adc_result[ADC_RESULT_NUM / 2]);

    return adc_result[ADC_RESULT_NUM / 2];
}

int32_t bootAdcGetRawValue(uint32_t channel, int32_t scale)
{
    struct adc_sample_data adc;
    adc.channel_id = channel;
    adc.channel_type = 0;
    adc.hw_channel_delay = 0;
    adc.sample_speed = 1;
    adc.scale = scale;
    _adcOpenPmic26MclkAdc();

    if (_adcGetValue(&adc) == false)
    {
        bootLog("adc: Get adc value, error");
        _adcClosePmic26MclkAdc();
        return -1;
    }

    bootLog("adc:  channel = %d,raw = %d", channel, adc.result);
    _adcClosePmic26MclkAdc();
    return adc.result;
}

int32_t bootAdcGetChannelVolt(uint32_t channel, int32_t scale)
{
    int32_t value, adc;
    if (channel > ADC_CHANNEL_MAX)
        return ADC_CHANNEL_INVALID;
    if (scale > ADC_SCALE_MAX)
        return -1;

    adc = _bootGetAdcAverage(channel, scale);
    if (adc == -1)
        return -1;
    value = _adcChangAdcToVol(channel, scale, 0, adc);

    bootLog("adc: channel= %d,vol= %dmv", channel, value);
    return value;
}
