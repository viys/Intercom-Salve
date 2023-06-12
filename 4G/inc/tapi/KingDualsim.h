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
 * KH_DualSim ϵ�� API ֧��˫������¶�̬�л�����ҵ��.
 *
 * KH_DualSimInit ���� 0��˵���ײ�֧��˫�����ܣ������ʾ��֧�֡�
 *
 * KH_DualSimSet �������߹ر�˫�����ܡ������󣬵��ײ��⵽��ǰҵ���ź������
 * �����Ѿ�������GPRSģʽ���ͻ��ϱ� \p DUALSIM_EVENT_SWITCH_IND �¼���APP���о���
 * �Ƿ�ִ���л���
 * DUAL_SIM_RSP_DATA_S �е� ret = 1����ʾ�ɹ���ret = 0����ʾʧ�ܡ�
 * 
 * @attention
 * �л���ϲ��Ҹ�������ɹ����Ż᷵�� ret = 1�������� ret = 0����ˣ�����ret = 0��
 * �п�����SIM�л��ɹ�������ʧ�ܵ��л�������ͨ�� KING_DualSimCurrent ��ȡ��ǰҵ�񿨡�
 *
 *
 * KH_DualSimSwitch ִ������ҵ���л����л����첽������������ͨ��
 * \p DUALSIM_EVENT_SWITCH_RSP �¼�֪ͨ APP��
 * DUAL_SIM_IND_DATA_S �е� aim_sim �� cause��ʾϣ���л���Ŀ��SIM���л�ԭ��
 */

/**
 * ��ʼ��˫����������
 *
  * @return 0 SUCCESS  -1 FAIL or not support
 */
int KING_DualSimInit(void);


/**
 * ����˫������
 *
 * @param[in]  enable  0 -- �ر�˫������, 1 -- ����˫������
 * @param[in]  idDef  ����ʱĬ��ҵ��: SIM_0, SIM_1
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_DualSimSet(uint8 enable, SIM_ID idDef);


/**
 * ��ȡ˫����������
 *
 * @param[out]  enable  0 -- �ر�˫������, 1 -- ����˫������
 * @param[out]  idDef  ����ʱĬ��ҵ��: SIM_0, SIM_1
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_DualSimGet(uint8 *enable, SIM_ID *idDef);


/**
 * ����ҵ���л�, ˫�����ܿ�������ܵ��á�
 * ���л����첽���������ս��ͨ���ص���á�
 * DUALSIM_EVENT_SWITCH_RSP �¼���ִ�н��,
 * DUAL_SIM_RSP_DATA_S �ṹ���е� ret ��ʾ���: 0 -- ʧ��; 1 -- �ɹ�.
 *
 * @param[in]  id  SIM_0, SIM_1
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_DualSimSwitch(SIM_ID id);

/**
 * ��ȡ��ǰ��ҵ��, ˫�����ܿ�������ܵ��á�
 *
 * @param[out]  id  SIM_0, SIM_1
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_DualSimCurrent(SIM_ID *id);


#endif /*__KING_DUALSIM_H__*/

