#ifndef __BLE_CONFIG_H__
#define __BLE_CONFIG_H__

#ifdef BLE_SCAN
#define CENTRAL_LINK_COUNT          1          //���������������
#else
#define CENTRAL_LINK_COUNT          0          //���������������
#endif
#define PERIPHERAL_LINK_COUNT       1          //���ӻ����������

#if (NRF_SD_BLE_API_VERSION == 3)
#define NRF_BLE_MAX_MTU_SIZE        GATT_MTU_SIZE_DEFAULT         //ʹ�õ�MTU�Ĵ�С
#endif


#define APP_FEATURE_NOT_SUPPORTED   BLE_GATT_STATUS_ATTERR_APP_BEGIN + 2          //�Բ�֧�ֵ������Ӧ������
#define DEVICE_TYPE     0x00                 //�ֻ����豸����

/*************************************************************************
������:ble_stack_init
����:����Э��ջ��ʼ��
�������:None
�������:None
����ֵ:None
*************************************************************************/
void ble_stack_init(void);

/*************************************************************************
������:db_discovery_init
����:������ʱ,��ѯ�ӻ������ܵĳ�ʼ��
�������:None
�������:None
����ֵ:None
*************************************************************************/
void db_discovery_init(void);

/*************************************************************************
������:adv_scan_start
����:�����㲥��ɨ��
�������:None
�������:None
����ֵ:None
*************************************************************************/
void adv_scan_start(void);

#endif
