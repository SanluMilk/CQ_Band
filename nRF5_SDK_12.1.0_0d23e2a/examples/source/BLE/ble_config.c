#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "nordic_common.h"
#include "softdevice_handler.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"

#include "ble_conn_state.h"
#include "fstorage.h"
#include "boards.h"

#include "ble_config.h"
#include "ble_master.h"
#include "ble_salve.h"


/*************************************************************************
������:ble_evt_dispatch
����:�Բ�ͬ���¼����з��䴦��
�������:
@p_ble_evt:�¼�ָ��
�������:None
����ֵ:None
*************************************************************************/
static void ble_evt_dispatch(ble_evt_t * p_ble_evt)
{
    uint16_t conn_handle;
    uint16_t role;

    ble_conn_state_on_ble_evt(p_ble_evt);

    conn_handle = p_ble_evt->evt.gap_evt.conn_handle;    //��ȡ�¼����
    role        = ble_conn_state_role(conn_handle);      //��ȡ��ǰ�豸�������еĽ�ɫ         

    //���ݲ�ͬ��ɫ���¼��������ͬ�ص�����
    if (role == BLE_GAP_ROLE_PERIPH)                     //�����ǰ�豸���ӻ�
    {
        on_ble_peripheral_evt(p_ble_evt);

        ble_advertising_on_ble_evt(p_ble_evt);
        ble_conn_params_on_ble_evt(p_ble_evt);
        ble_dfu_on_ble_evt(&m_dfus,p_ble_evt );
        ble_nus_on_ble_evt (&m_nus, p_ble_evt);
    }
    else if ((role == BLE_GAP_ROLE_CENTRAL) || (p_ble_evt->header.evt_id == BLE_GAP_EVT_ADV_REPORT))    //��ǰ�豸������
    {
        if (p_ble_evt->header.evt_id != BLE_GAP_EVT_DISCONNECTED)
        {
            on_ble_central_evt(p_ble_evt);               //�������Ӿ��
        }

        if (conn_handle < CENTRAL_LINK_COUNT + PERIPHERAL_LINK_COUNT)
        {
            ble_db_discovery_on_ble_evt(&m_ble_db_discovery[conn_handle], p_ble_evt);
        }
        ble_nus_c_on_ble_evt(&m_ble_nus_c, p_ble_evt);

        // ���ʱ�Ͽ������¼����������Ӿ��
        if (p_ble_evt->header.evt_id == BLE_GAP_EVT_DISCONNECTED)
        {
            on_ble_central_evt(p_ble_evt);
        }
    }
}

/*************************************************************************
������:sys_evt_dispatch
����:ϵͳ�¼�����
�������:
@sys_evt:�¼�ָ��
�������:None
����ֵ:None
*************************************************************************/
static void sys_evt_dispatch(uint32_t sys_evt)
{
    fs_sys_event_handler(sys_evt);
    ble_advertising_on_sys_evt(sys_evt);
}

/*************************************************************************
������:ble_stack_init
����:����Э��ջ��ʼ��
�������:None
�������:None
����ֵ:None
*************************************************************************/
void ble_stack_init(void)
{
    ret_code_t err_code;

    // ��ʼ������
    nrf_clock_lf_cfg_t clock_lf_cfg = NRF_CLOCK_LFCLKSRC;
    SOFTDEVICE_HANDLER_INIT(&clock_lf_cfg, NULL);

    ble_enable_params_t ble_enable_params;
    err_code = softdevice_enable_get_default_config(CENTRAL_LINK_COUNT,
                                                    PERIPHERAL_LINK_COUNT,
                                                    &ble_enable_params);
    APP_ERROR_CHECK(err_code);

	
	  ble_enable_params.common_enable_params.vs_uuid_count = 2;
	
    //�������ӵ�����������RAM
    CHECK_RAM_START_ADDR(CENTRAL_LINK_COUNT,PERIPHERAL_LINK_COUNT);
   
    //ʹ��BLEЭ��ջ
#if (NRF_SD_BLE_API_VERSION == 3)
    ble_enable_params.gatt_enable_params.att_mtu = NRF_BLE_MAX_MTU_SIZE;
#endif
    err_code = softdevice_enable(&ble_enable_params);
    APP_ERROR_CHECK(err_code);

    //��BLE�¼��ص�����
    err_code = softdevice_ble_evt_handler_set(ble_evt_dispatch);
    APP_ERROR_CHECK(err_code);

    //��BLEϵͳ�¼��ص�����
    err_code = softdevice_sys_evt_handler_set(sys_evt_dispatch);
    APP_ERROR_CHECK(err_code);
}

/*************************************************************************
������:db_disc_handler
����:������ʱ����ѯ���ӻ�����Ļص�����
�������:
@p_evt:�¼�ָ��
�������:None
����ֵ:None
*************************************************************************/
static void db_disc_handler(ble_db_discovery_evt_t * p_evt)
{
    ble_nus_c_on_db_disc_evt(&m_ble_nus_c, p_evt);
}

/*************************************************************************
������:db_discovery_init
����:������ʱ,��ѯ�ӻ������ܵĳ�ʼ��
�������:None
�������:None
����ֵ:None
*************************************************************************/
void db_discovery_init(void)
{
    ret_code_t err_code = ble_db_discovery_init(db_disc_handler);
    APP_ERROR_CHECK(err_code);
}


/*************************************************************************
������:adv_scan_start
����:�����㲥��ɨ��
�������:None
�������:None
����ֵ:None
*************************************************************************/
void adv_scan_start(void)
{
    ret_code_t err_code;
    uint32_t count;

    //����û�в���flash
    err_code = fs_queued_op_count_get(&count);
    APP_ERROR_CHECK(err_code);

    if (count == 0)
    {
			  //�򿪹㲥
        err_code = ble_advertising_start(BLE_ADV_MODE_FAST);
        APP_ERROR_CHECK(err_code);
			
			 
    }
}










