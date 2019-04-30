#ifndef __BLE_MASTER_H__
#define __BLE_MASTER_H__

#ifdef BLE_SCAN
#include "ble_config.h"
#include "ble_db_discovery.h"
#include "ble_nus_c.h"

#define BLE_M_DEBUG

#define SCAN_INTERVAL               0x0050                            //ɨ����(����ɨ��ļ��ʱ��)*0.625ms
#define SCAN_WINDOW                 0x0050                            //ɨ�贰��(ɨ���ʱ��)
#define SCAN_TIMEOUT                0                                 //ɨ�賬ʱʱ��  0:һֱɨ��

#define UUID16_SIZE                 2             //UUID�Ĵ�С
#define UUID16_EXTRACT(DST, SRC)   do{(*(DST))   = (SRC)[1];(*(DST)) <<= 8;(*(DST))  |= (SRC)[0];} while (0)   //����UUID

typedef struct
{
    uint8_t     * p_data;    //����ָ��
    uint16_t      data_len;  //���ݴ�С
} data_t;


extern ble_nus_c_t   m_ble_nus_c;                      //�ӻ�����ʵ��
extern uint16_t      m_conn_handle_nus_c;              //����״̬
extern ble_db_discovery_t m_ble_db_discovery[CENTRAL_LINK_COUNT + PERIPHERAL_LINK_COUNT];

void nus_c_init(void);
void scan_start(void);
void on_ble_central_evt(const ble_evt_t * const p_ble_evt);
void BLE_Master_SendData(const uint8_t *pdata, const uint16_t len);

/**********************************************************************
����:��ȡ�ű��״̬��1S����һ�θú���
˵����
    ���¼�������Ӧ
***********************************************************************/
void Get_Beacon_State(void);

#endif

#endif
