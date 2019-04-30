#ifndef __BLE_SALVE_H__
#define __BLE_SALVE_H__

#include "ble_nus.h"
#include "ble_dfu.h"

#define BLE_S_DEBUG


#define APP_COMPANY_IDENTIFIER           0x1D1E                                        //�ض������̺�(�˺��뱾Ӧ����������������;����������ԭ�򣬹�˾�޷�����;
                                                                                       //�����ڹ�˾�ڲ����ú��붨��Ϊ����˾(�ɶ������ֵ���Ϣ�������޹�˾)�ض����롣���ܸ��ġ�
																																											 //һ�����ģ������²����豸�޷�ͨ��)

#define APP_ADV_INTERVAL                 MSEC_TO_UNITS(500,UNIT_0_625_MS)              //�㲥���ʱ��500*0.625ms
#define APP_ADV_TIMEOUT_IN_SECONDS       MSEC_TO_UNITS(4800,UNIT_0_625_MS)              //�㲥��ʱʱ��3S  0:һֱ�㲥  


#define MIN_CONNECTION_INTERVAL          (uint16_t) MSEC_TO_UNITS(7.5, UNIT_1_25_MS)   //��С���Ӽ��
#define MAX_CONNECTION_INTERVAL          (uint16_t) MSEC_TO_UNITS(30, UNIT_1_25_MS)    //������Ӽ��
#define SLAVE_LATENCY                    0                                             //�ӻ���ʱ
#define SUPERVISION_TIMEOUT              (uint16_t) MSEC_TO_UNITS(4000, UNIT_10_MS)    //���ӳ�ʱʱ��

#define FIRST_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(5000, APP_TIMER_PRESCALER)    //��ʼ����ɵ���һ�ε���sd_ble_gap_conn_param_update()��ʱ��
#define NEXT_CONN_PARAMS_UPDATE_DELAY    APP_TIMER_TICKS(30000, APP_TIMER_PRESCALER)   //����sd_ble_gap_conn_param_update()�ļ��
#define MAX_CONN_PARAMS_UPDATE_COUNT     3                                             //��������

#define DEVICE_NAME                      "cq-Band"                               //�㲥���豸��
#define MANUFACTURER_NAME                "cqxd.com"                                    //��������




#define m_ble_nus_max_data_len   20  //�����������ݳ���

extern ble_nus_t  m_nus;             //���ӻ�ʱʵ��
extern ble_dfu_t m_dfus;        //�̼���������ʵ��
/**********************************************************************
������:on_ble_peripheral_evt
����:��������Χ�豸ʱ���������¼��Ĵ���
�������:
@p_ble_evt:�¼�ָ��
�������:None
����ֵ:None
˵��:
***********************************************************************/
void on_ble_peripheral_evt(ble_evt_t * p_ble_evt);

/**********************************************************************
������:gap_params_init
����:�㲥������ʼ��
�������:None
�������:None
����ֵ:None
˵��:
***********************************************************************/
void gap_params_init(void);

/**********************************************************************
������:conn_params_init
����:���Ӳ���������ʼ��
�������:None
�������:None
����ֵ:None
˵��:
***********************************************************************/
void conn_params_init(void);

/**********************************************************************
������:services_init
����:���ڷ����ʼ��
�������:None
�������:None
����ֵ:None
˵��:
***********************************************************************/
void services_init(void);


/**********************************************************************
������:advertising_init
����:�㲥��ʼ��
�������:None
�������:None
����ֵ:None
˵��:
***********************************************************************/
void advertising_init(void);

/**********************************************************************
������:BLE_Salve_SendData
����:��������Χ�豸��������
�������:
@pdata:��Ҫ���͵�����
@len:��Ҫ���͵����ݳ���
�������:None
����ֵ:None
˵��:
***********************************************************************/
void BLE_Salve_SendData(const uint8_t *pdata, const uint16_t len);


#endif
