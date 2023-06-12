#ifndef __KING_DUALSIM_H__
#define __KING_DUALSIM_H__

#include "KingDef.h"
#include "KingUsim.h"

/*---------------------------------------------------------------------------
 ** Defines
---------------------------------------------------------------------------*/
#define DUALSIM_EVENT_SWITCH_RSP        0x0001
#define DUALSIM_EVENT_SWITCH_IND        0x0002


/*******************************************************************************
 ** Type Definitions
 ******************************************************************************/
/**
 * KH_DualSim 系列 API 支持双卡情况下动态切换数据业务卡.
 *
 * KH_DualSimInit 返回 0，说明底层支持双卡功能，否则表示不支持。
 *
 * KH_DualSimSet 开启或者关闭双卡功能。开启后，当底层检测到当前业务卡信号质量差，
 * 或者已经工作于GPRS模式，就会上报 \p DUALSIM_EVENT_SWITCH_IND 事件，APP自行决定
 * 是否执行切换。
 * DUAL_SIM_RSP_DATA_S 中的 ret = 1，表示成功；ret = 0，表示失败。
 * 
 * @attention
 * 切换完毕并且附着网络成功，才会返回 ret = 1，否则都是 ret = 0。因此，返回ret = 0，
 * 有可能是SIM切换成功但附着失败的切换，可以通过 KING_DualSimCurrent 获取当前业务卡。
 *
 *
 * KH_DualSimSwitch 执行数据业务卡切换。切换是异步操作，具体解决通过
 * \p DUALSIM_EVENT_SWITCH_RSP 事件通知 APP。
 * DUAL_SIM_IND_DATA_S 中的 aim_sim 和 cause表示希望切换的目标SIM和切换原因。
 */

/**
 * 初始化双卡基本环境
 *
  * @return 0 SUCCESS  -1 FAIL or not support
 */
int KING_DualSimInit(void);


/**
 * 设置双卡功能
 *
 * @param[in]  enable  0 -- 关闭双卡功能, 1 -- 开启双卡功能
 * @param[in]  idDef  开机时默认业务卡: SIM_0, SIM_1
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_DualSimSet(uint8 enable, SIM_ID idDef);


/**
 * 获取双卡功能设置
 *
 * @param[out]  enable  0 -- 关闭双卡功能, 1 -- 开启双卡功能
 * @param[out]  idDef  开机时默认业务卡: SIM_0, SIM_1
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_DualSimGet(uint8 *enable, SIM_ID *idDef);


/**
 * 启动业务卡切换, 双卡功能开启后才能调用。
 * 卡切换是异步操作，最终结果通过回调获得。
 * DUALSIM_EVENT_SWITCH_RSP 事件是执行结果,
 * DUAL_SIM_RSP_DATA_S 结构体中的 ret 表示结果: 0 -- 失败; 1 -- 成功.
 *
 * @param[in]  id  SIM_0, SIM_1
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_DualSimSwitch(SIM_ID id);

/**
 * 获取当前的业务卡, 双卡功能开启后才能调用。
 *
 * @param[out]  id  SIM_0, SIM_1
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_DualSimCurrent(SIM_ID *id);


#endif /*__KING_DUALSIM_H__*/

