#ifndef __COMM_PROTOCOL_H__
#define __COMM_PROTOCOL_H__


#include "boards.h"

/* �������� */
typedef enum{
    CMD_Device_Reset           = 0x00,//�����ֻ�
    CMD_Get_Version            = 0x01,//��ȡ�豸�汾��
    CMD_Get_Mac                = 0x02,//��ȡMAC��ַ
    CMD_Adjust_Time            = 0x03,//У׼ʱ��
    CMD_Get_Time               = 0x04,//��ȡʱ��
	  CMD_Updata_Step            = 0x05,//ʵʱ�ϴ��Ʋ�����
	  CMD_Set_Beacon             = 0x06,//�����Ƿ��Beaconɨ��
	  CMD_Set_Beacon_Mac         = 0x07,//����Ŀ��Beacon��MAC��ַ
	  CMD_Get_Beacon             = 0x08,//��ȡBeaconɨ���Ƿ��
    CMD_Continuous_Collect     = 0x80,//�ֻ��ɼ�
    CMD_Collect_Interval       = 0x81,//���òɼ����
	  CMD_Set_Alarm              = 0x82,//��������
}CMD_type_e;

/* �������� */
typedef enum{
    Ret_Success   = 0,                //���سɹ�
    Ret_operation = 1,                //������
    Ret_Error     = 2,                //���ش���
}Operation_type_t;


/**********************************************************************
������:BLE_Data_analysis
����:�������ݰ���Э�����
�������:
@data:�������յ�����
@len:�������յ����ݳ���
�������:None
����ֵ:None
˵��:
***********************************************************************/
void BLE_Data_analysis(const uint8_t *data, const uint8_t len );











#endif

