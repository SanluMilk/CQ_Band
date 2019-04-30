#ifndef __OLED_APP_H__
#define __OLED_APP_H__

#include "oled.h"
#include "utc.h"

#define OLED_OFF_TIME     10     //Ϣ��ʱ��
/* ҳ�� */
typedef enum{
  DATE_P = 0,      //����ҳ��         0
	STEP_P ,         //����ҳ��         1
	DISTANCE_P,      //���ߵľ���ҳ��   2
	CALORIE_P,       //��������         3
	HEART_P,         //���ʽ���         4
	OXYGEN_P,        //Ѫ������         5
	TEMPER_P,        //�¶Ƚ���         6
	MAC_P,           //MAC��ַ����      7
	ALARM_P,         //���ӽ���         8
	COLLECT_P,       //�ֻ��ɼ�ҳ��     9
	BLE_CONNECT_P,   //��������         10
	BLE_DISCONNECT_P,//�����Ͽ�����     11
#ifdef BLE_SCAN
	BEACON_LOST_P,   //�ű궪ʧ         12
#endif
	
}page_e;

/* ��Ļ��Դ���� */
#define OLED_OFF          0
#define OLED_ON           1

/* ����Ϣ������ */
#define OLED_CLOSE_DISEN    0
#define OLED_CLOSE_EN       1

typedef struct
{
    uint8_t oled_state_flag:1;       //��ǰ��Ļ����������  0:��    1:��
    uint8_t oled_off_key:1;          //0�����ܿ�ʼ����ʱ      1:���Կ�ʼ����ʱ
    uint8_t time_off:6;              //Ϣ������ʱ   max = 64
	  uint8_t lastPageOledState:1;     //���뵱ǰҳ�棬��Ļ�����ŵĻ������ŵġ�  0:��    1:��
	  uint8_t lastPageKey:1;           //��һ��ҳ���ܷ�Ϣ��      //0�����ܿ�ʼ����ʱ      1:���Կ�ʼ����ʱ
	  uint8_t lastPageTime:6;          //��һ��ҳ��ʣ��ĵ���ʱʱ��
	  page_e  lastPageNum;             //������һ��ҳ�棬�����ʼ��Ļ��Ϩ��ģ���һ��ҳ���������ҳ��
	  page_e  page_num ;               //��ǰ������һ��ҳ��
	  
}oled_ctrl_t;

extern oled_ctrl_t OLED_ctrl;

typedef struct{  
    volatile s16 heart;                   //����
    volatile s16 high;                    //Ѫѹ��
    volatile s16 low;                     //Ѫѹ��
}heart_sensor_t;
extern heart_sensor_t heart_sensor_data;

/**********************************************************************
������:OLED_Change_Display
����:�ı�ҳ��Ͷ�̬ˢ��ҳ��
���������
@tm:ʱ������
�������:None
����ֵ:None
˵��:
***********************************************************************/
void OLED_Change_Display(const UTCTime_t *tm);

#ifdef BLE_SCAN
/**********************************************************************
������:OLEDE_dispaly_date
����:��ʾʱ�������״̬
���������
@tm:ʱ������
@connect_state:����״̬
@scan_state:ɨ��״̬
@batty:�����ȼ� 1:1��        2:2��   3:����    4:4��  0��0��
�������:None
����ֵ:None
˵��:
***********************************************************************/
void OLEDE_dispaly_date(const UTCTime_t *tm, const uint8_t scan_state, const uint8_t connect_state, const uint8_t batty);
#else
/**********************************************************************
������:OLEDE_dispaly_date
����:��ʾʱ�������״̬
���������
@tm:ʱ������
@state:����״̬  1������
@batty:�����ȼ� 1:1��        2:2��   3:����    4:4��  0��0��
�������:None
����ֵ:None
˵��:
***********************************************************************/
void OLEDE_dispaly_date(const UTCTime_t *tm, const uint8_t state, const uint8_t batty);
#endif

/**********************************************************************
������:OLEDE_dispaly_step
����:��ʾ����
���������
@step_num:����
�������:None
����ֵ:None
˵��:
***********************************************************************/
void OLEDE_dispaly_step(uint32_t step_num);

/**********************************************************************
������:OLEDE_dispaly_Distance
����:��ʾ���ߵľ���
���������
@distance:���ߵľ��룬(km*10)
�������:None
����ֵ:None
˵��:
***********************************************************************/
void OLEDE_dispaly_Distance(uint32_t distance);

/**********************************************************************
������:OLEDE_dispaly_Calorie
����:��ʾ���ĵĿ�·��
���������
@Cal:����(cal)
�������:None
����ֵ:None
˵��:
    �������ߵľ���������ĵ�����
***********************************************************************/
void OLEDE_dispaly_Calorie(uint32_t Cal);

/**********************************************************************
������:OLEDE_dispaly_HeartRate
����:��ʾ����
���������
@heart_num:����
�������:None
����ֵ:None
˵��:
heart_num < 0,��ʾ���ʲɼ�׼��������ʱ
heart_num = 0,��ʾ���ڲɼ�
***********************************************************************/
void OLEDE_dispaly_HeartRate(s16 heart_num);

/**********************************************************************
������:OLEDE_dispaly_Oxygen
����:��ʾѪ��
���������
@oxygen:Ѫ��
�������:None
����ֵ:None
˵��:
***********************************************************************/
void OLEDE_dispaly_Oxygen(s16 oxygen);

/**********************************************************************
������:OLEDE_dispaly_Temperature
����:��ʾ�¶�
���������
@oxygen:Ѫ��
�������:None
����ֵ:None
˵��:
***********************************************************************/
void OLEDE_dispaly_Temperature(s16 temp);

/**********************************************************************
������:OLEDE_dispaly_alarm
����:��ʾ����
�������:None
�������:None
����ֵ:None
˵��:
***********************************************************************/
void OLEDE_dispaly_alarm(void);

/**********************************************************************
������:OLEDE_dispaly_mac
����:��ʾMAC
�������:None
�������:None
����ֵ:None
˵��:
hig, 0x< 0,��ʾ���ʲɼ�׼��������ʱ
hig, 0x= 0,��ʾ���ڲɼ�
***********************************************************************/
void OLEDE_dispaly_mac(void);

/**********************************************************************
������:OLEDE_dispaly_Temperature
����:��ʾ�¶�
���������
@oxygen:Ѫ��
�������:None
����ֵ:None
˵��:
***********************************************************************/
void OLEDE_dispaly_Phone_Collect(void);

/**********************************************************************
������:OLEDE_dispaly_BLE_Connect
����:��ʾ�������ӽ���
���������
@time:��ʾʱ�䣬Ϊ0ʱ����֮ǰ��ҳ��
�������:None
����ֵ:None
˵��:
***********************************************************************/
void OLEDE_dispaly_BLE_Connect(int time);

/**********************************************************************
������:OLEDE_dispaly_BLE_Disconnect
����:��ʾ�����Ͽ�����
���������
�������:None
����ֵ:None
˵��:
    �����Ͽ�ҳ����Ҫ�ֶ��л������ߵ���ʱʱ�䵽��������ʾ��ʱ����Ҫ�𶯡�
***********************************************************************/
void OLEDE_dispaly_BLE_Disconnect(void);

#ifdef BLE_SCAN
/**********************************************************************
������:OLEDE_dispaly_BeaconLost
����:��ʾ�����ű궪ʧ
���������
�������:None
����ֵ:None
˵��:
    �����Ͽ�ҳ����Ҫ�ֶ��л������ߵ���ʱʱ�䵽��������ʾ��ʱ����Ҫ�𶯡�
***********************************************************************/
void OLEDE_dispaly_BeaconLost(void);

/**********************************************************************
������:JumpPage
����:ҳ����ת,����ת��ʱ��ˢ��ҳ��
���������None
�������:None
����ֵ:None
˵��:
***********************************************************************/
void JumpPage(void);

/**********************************************************************
������:SetLastPageInfo
����:������һ��ҳ�����Ϣ
���������
@page:��Ҫ��ת��ҳ��
@state:ҳ��״̬����������Ϩ��
@closeSwitch:Ϣ�����ء��ܷ�Ϣ��
@closeTime:����ʱ��
�������:None
����ֵ:None
˵��:
***********************************************************************/
void SetLastPageInfo(const page_e page, const int8_t state, const int8_t closeSwitch, const int8_t closeTime);

/**********************************************************************
������:SetNewPageInfo
����:���õ�ǰҳ�����Ϣ
���������
@page:��Ҫ��ת��ҳ��
@state:ҳ��״̬����������Ϩ��
@closeSwitch:Ϣ�����ء��ܷ�Ϣ��
@closeTime:����ʱ��
�������:None
����ֵ:None
˵��:
***********************************************************************/
void SetNewPageInfo(const page_e page, const int8_t state, const int8_t closeSwitch, const int8_t closeTime);

#endif

















#endif
