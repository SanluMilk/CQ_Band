#ifdef BLE_SCAN
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include "app_error.h"
#include "sdk_errors.h"
#include "ble_conn_state.h"
#include "ble_hci.h"

#include "ble_config.h"
#include "ble_master.h"
#include "ble_salve.h"
#include "boards.h"
#include "oled_app.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"


#ifdef BLE_M_DEBUG
 #define BLE_M_Msg       NRF_LOG_INFO
#else
  #define BLE_M_Msg(...)
#endif



ble_nus_c_t        m_ble_nus_c;                                         //�ӻ�����ʵ��
uint16_t           m_conn_handle_nus_c = BLE_CONN_HANDLE_INVALID;       //����״̬

ble_db_discovery_t m_ble_db_discovery[CENTRAL_LINK_COUNT + PERIPHERAL_LINK_COUNT];  //�������ǣ��ӻ��ķ����б�

//static const char m_target_periph_name[] = "cqxd-Beacon" ;                           //��Ҫ���ӵ��豸��

/* ɨ����� */
static const ble_gap_scan_params_t m_scan_params =
{
    .active   = 1,
    .interval = SCAN_INTERVAL,
    .window   = SCAN_WINDOW,
    .timeout  = SCAN_TIMEOUT,
#if (NRF_SD_BLE_API_VERSION == 2)
        .selective   = 0,
        .p_whitelist = NULL,
#endif
#if (NRF_SD_BLE_API_VERSION == 3)
        .use_whitelist = 0,
#endif
};

////���Ӳ���
//static const ble_gap_conn_params_t m_connection_param =
//{
//    (uint16_t)MIN_CONNECTION_INTERVAL,
//    (uint16_t)MAX_CONNECTION_INTERVAL,
//    0,
//    (uint16_t)SUPERVISION_TIMEOUT
//};


/**********************************************************************
����:����ɨ��
***********************************************************************/
void scan_start(void)
{
    ret_code_t err_code;

    (void) sd_ble_gap_scan_stop();

    err_code = sd_ble_gap_scan_start(&m_scan_params);
    if (err_code != NRF_ERROR_INVALID_STATE)
    {
        APP_ERROR_CHECK(err_code);
    }
}

#if 0
/**********************************************************************
����:�����㲥��Ϣ
�������:
    type:��Ҫ������
    p_advdata:�㲥����
�������:
    p_typedata:�������ȡ�����͵�����
***********************************************************************/
static uint32_t adv_report_parse(uint8_t type, data_t * p_advdata, data_t * p_typedata)
{
    uint32_t  index = 0;
    uint8_t * p_data;

    p_data = p_advdata->p_data;

    while (index < p_advdata->data_len)
    {
        uint8_t field_length = p_data[index];
        uint8_t field_type   = p_data[index + 1];

        if (field_type == type)
        {
            p_typedata->p_data   = &p_data[index + 2];
            p_typedata->data_len = field_length - 1;
            return NRF_SUCCESS;
        }
        index += field_length + 1;
    }
    return NRF_ERROR_NOT_FOUND;
}


/**********************************************************************
����:��������ͨ��NAME����Ҫ���ӵ��豸
�������:
    p_adv_report���յ��Ĺ㲥����
    uuid_to_find: ��Ҫ���ҵ�NAME
***********************************************************************/
static bool find_adv_name(const ble_gap_evt_adv_report_t *p_adv_report, const char * name_to_find)
{
    uint32_t err_code;
    data_t   adv_data;
    data_t   dev_name;

    // Initialize advertisement report for parsing
    adv_data.p_data     = (uint8_t *)p_adv_report->data;
    adv_data.data_len   = p_adv_report->dlen;

    //search for advertising names
    err_code = adv_report_parse(BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME,
                                &adv_data,
                                &dev_name);
    if (err_code == NRF_SUCCESS)
    {
        if (memcmp(name_to_find, dev_name.p_data, dev_name.data_len )== 0)
        {
            return true;
        }
    }
    else
    {
        // Look for the short local name if it was not found as complete
        err_code = adv_report_parse(BLE_GAP_AD_TYPE_SHORT_LOCAL_NAME,
                                    &adv_data,
                                    &dev_name);
        if (err_code != NRF_SUCCESS)
        {
            return false;
        }
        if (memcmp(m_target_periph_name, dev_name.p_data, dev_name.data_len )== 0)
        {
            return true;
        }
    }
    return false;
}


/**********************************************************************
����:��������ͨ��UUID����Ҫ���ӵ��豸
�������:
    p_adv_report���յ��Ĺ㲥����
    uuid_to_find: ��Ҫ���ҵ�UUID
***********************************************************************/
static bool find_adv_uuid(const ble_gap_evt_adv_report_t *p_adv_report, const uint16_t uuid_to_find)
{
    uint32_t err_code;
    data_t   adv_data;
    data_t   type_data;

    // Initialize advertisement report for parsing.
    adv_data.p_data     = (uint8_t *)p_adv_report->data;
    adv_data.data_len   = p_adv_report->dlen;

    err_code = adv_report_parse(BLE_GAP_AD_TYPE_128BIT_SERVICE_UUID_MORE_AVAILABLE,
                                &adv_data,
                                &type_data);

    if (err_code != NRF_SUCCESS)
    {
        // Look for the services in 'complete' if it was not found in 'more available'.
        err_code = adv_report_parse(BLE_GAP_AD_TYPE_128BIT_SERVICE_UUID_COMPLETE,
                                    &adv_data,
                                    &type_data);

        if (err_code != NRF_SUCCESS)
        {
            // If we can't parse the data, then exit.
            return false;
        }
    }

    // Verify if any UUID match the given UUID.
    for (uint32_t u_index = 0; u_index < (type_data.data_len / UUID16_SIZE); u_index++)
    {
        uint16_t    extracted_uuid;

        UUID16_EXTRACT(&extracted_uuid, &type_data.p_data[u_index * UUID16_SIZE]);

        if (extracted_uuid == uuid_to_find)
        {
            return true;
        }
    }
    return false;
}

#endif

static struct scan_flag_t{
	  uint8_t startScanFlag:1;       //0:δ����ɨ��      1:�Ѿ�����ɨ�� 
	  uint8_t timeCount:2;           //ʱ���¼����ʱ�����ڿ�ʱɨ��Ͳ�ɨ��ļ��	
}scanFlag; 

#define SCAN_VALUE_NUM      5      //��¼ɨ��Ļ������
/* ���ڻ���ɨ�赽�ű���ź�ǿ�� */
static struct scan_value_t{
    int rssi;
	  UTCTime timeNode;
}scanValue[SCAN_VALUE_NUM];
static uint8_t buffCount = 0;  //���ڼ�¼��ǰ���Ļ���ռ䣬�������ռ�������ˡ�����ǰ������

#define AVERAGE_RSSI_MIN      (70)      //�ж��ź�ǿ�ȵ���С�ź�ǿ��ƽ��ֵ
#define RSSI_MIN              (90)      //�ж��ź�ǿ�ȵ���С�ź�ǿ��
#define INVAIN_RSSI_NUM        3        //�ж���Ч�ź�ǿ�ȵ�������
#define INTERVAL_TIME          20        //�ж���Ч�ĳ�ʱʱ��(S)
/**********************************************************************
����:��ȡ�ű��״̬��1S����һ�θú���
˵����
    ���¼�������Ӧ
***********************************************************************/
void Get_Beacon_State(void)
{
	  ret_code_t err_code;
	
	  /* ��Ъ�Կ����͹ر�ɨ�蹦�ܡ�0��ʱ����ɨ�裬1-3��ʱ��ر�ɨ�衣��0-3֮��ѭ�� */
    if( scanFlag.timeCount == 0 && scanFlag.startScanFlag == 0 && save_param.OpenBeaconFlag == 1 )   //���δ����ɨ��,��ͳ��ʱ��Ϊ0,�ҿ���ɨ�裬�Ϳ���ɨ��
		{
		     err_code = sd_ble_gap_scan_start(&m_scan_params);
			
         if (err_code != NRF_ERROR_INVALID_STATE)
         {
             APP_ERROR_CHECK(err_code);
         } 
         scanFlag.startScanFlag = 1;				 
				 scanFlag.timeCount++; 
		}
	  else if( scanFlag.timeCount == 1 )     //ɨ��ʱ�������ֹͣɨ��
		{
			   if( scanFlag.startScanFlag != 0 )
				 {
		         (void) sd_ble_gap_scan_stop();  //�ر�ɨ��
			        scanFlag.startScanFlag = 0;     //��ǹر�ɨ��
				 }
			   scanFlag.timeCount++;
		}	
		else
		{
		     scanFlag.timeCount++;
         if( scanFlag.timeCount == 4 )
             scanFlag.timeCount = 0;					 
		}
		
		/* ����ر���ɨ�蹦�� */
		if( save_param.OpenBeaconFlag == 0 )
		{
		    sys_state.Beacon_scan_state = 0;
        return;			
		}
		
    /* �����ű��Ƿ�ɨ�赽�ű� */
		uint8_t beBeacon = 0;    //�����ֵ���Ϊ0,��ʾδɨ�赽�ű�
		int rssiSum = 0;
		uint8_t count = 0;
		//(0)�жϻ����Ƿ�����,�жϲ�����ͷ���
		if( buffCount <  SCAN_VALUE_NUM)
		{
	      beBeacon |= (1<<0);	   
  			return;
		}
		
		//(1)�жϾ�����һ��ɨ�赽�ű��Ƿ�ʱ,�жϲ�������beBeacon |= (1<<0)
		if( INTERVAL_TIME < (GetClock_seconds() - scanValue[SCAN_VALUE_NUM-1].timeNode) )
		{
		    beBeacon |= (1<<0);	
        NRF_LOG_INFO(" scan Beacon timeout\r\n ");			
  			goto judge;    
		}
		
		//(2)�ж�ƽ���ź�ǿ���Ƿ�����,�жϲ�������beBeacon |= (1<<1)
		
		for( uint8_t i = 0; i < SCAN_VALUE_NUM; i++ )
		{
		    rssiSum += scanValue[i].rssi;
		}
		if( abs( rssiSum/SCAN_VALUE_NUM ) > AVERAGE_RSSI_MIN )
		{
		    beBeacon |= (1<<1);	  
        NRF_LOG_INFO(" average rssi min\r\n ");			
  			goto judge;     
		}
		
		//(3)�����ź�ǿ�ȳ��ֵĴ���,�жϲ�������beBeacon |= (1<<2)
		
		for( uint8_t i = 0; i < SCAN_VALUE_NUM; i++ )
		{
		    if(abs(scanValue[i].rssi) > RSSI_MIN )
				    count++;
		}
		if( count >= INVAIN_RSSI_NUM )
		{
		    beBeacon |= (1<<2);	  
        NRF_LOG_INFO(" invain rssi number\r\n ");				
  			goto judge;    
		}
		
		sys_state.Beacon_scan_state = 1;
		return;   //��������㣬�ͷ���
		
judge:	     
     /* ������� */
		buffCount = 0;
		
    if( beBeacon != 0 && sys_state.Beacon_scan_state == 1)  // δɨ�赽�ű� 
		{
			    /* ����ű궪ʧ */
		      sys_state.Beacon_scan_state = 0;
			
					/* �����ǰҳ�治���ֻ��ɼ��������Ͽ����������ӣ�����ҳ�� */
					if(   OLED_ctrl.page_num != COLLECT_P 
							&&  OLED_ctrl.page_num != BLE_DISCONNECT_P 
						  && OLED_ctrl.page_num != BLE_CONNECT_P 
						  && OLED_ctrl.page_num != ALARM_P)
				  {
								if( OLED_ctrl.oled_state_flag == OLED_OFF )  //�����ǰ��ĻϨ���
								 {
								      OLED_Display_On();
									    SetLastPageInfo(DATE_P, OLED_ON, OLED_CLOSE_EN, 1);  //�����ǰҳ����Ϩ��ģ��ͽ���һ������Ϣ����Ϊ����ҳ�档��ʱ��Ϊ1S��ȷ���Ͽ����Ӻ�
									                                                         //�޲�������Ϣ����
								 }
								 else     //�����ǰ��Ļ������
								 {
								      SetLastPageInfo(OLED_ctrl.page_num, OLED_ctrl.oled_state_flag, OLED_ctrl.oled_off_key, OLED_OFF_TIME);  //��¼֮ǰ��ҳ����Ϣ
								 }
								
								 SetNewPageInfo(BEACON_LOST_P, OLED_ON, OLED_CLOSE_EN, OLED_OFF_TIME);   //���õ�ǰҪ��ʾ��ҳ����Ϣ
								 JumpPage();
					}
		}			
					  
					 
	
}

/**********************************************************************
����:��ɨ����豸����У�飬�ж��Ƿ�Ϊ��Ҫ���豸
˵����
    ���¼�������Ӧ
***********************************************************************/
extern save_param_t save_param;
void Cheack_Beacon(const ble_gap_evt_adv_report_t *p_adv_report)
{
    if( p_adv_report->peer_addr.addr[5] == save_param.Beacon_Mac[0] && \
			  p_adv_report->peer_addr.addr[4] == save_param.Beacon_Mac[1] && \
		    p_adv_report->peer_addr.addr[3] == save_param.Beacon_Mac[2] && \
		    p_adv_report->peer_addr.addr[2] == save_param.Beacon_Mac[3] && \
		    p_adv_report->peer_addr.addr[1] == save_param.Beacon_Mac[4] && \
		    p_adv_report->peer_addr.addr[0] == save_param.Beacon_Mac[5] )      
		{
			  BLE_M_Msg("==============================\r\n");
			  BLE_M_Msg("rssi:%d\r\n",p_adv_report->rssi );
			
			  if( buffCount < SCAN_VALUE_NUM )    //�������δ����
				{
				    scanValue[buffCount++].rssi =  p_adv_report->rssi;
        		scanValue[buffCount++].timeNode = GetClock_seconds();			
				}
				else                                //��������,��ǰ������뻺��
				{
				    for(int i = 0; i < (SCAN_VALUE_NUM - 1); i++)
            {
						    scanValue[i].rssi = scanValue[i+1].rssi;
                scanValue[i].timeNode = scanValue[i+1].timeNode;							
						}	
						scanValue[SCAN_VALUE_NUM - 1].rssi =  p_adv_report->rssi;
        		scanValue[SCAN_VALUE_NUM - 1].timeNode = GetClock_seconds();	
				}
		}
}

/**********************************************************************
����:���������¼��Ļص�����
˵����
    ���¼�������Ӧ
***********************************************************************/
void on_ble_central_evt(const ble_evt_t * const p_ble_evt)
{
    const ble_gap_evt_t   * const p_gap_evt = &p_ble_evt->evt.gap_evt;
    ret_code_t                    err_code;

    switch (p_ble_evt->header.evt_id)
    {
        /* �����¼� */
        case BLE_GAP_EVT_CONNECTED:
        {
            BLE_M_Msg("Central Connected \r\n");
            
					  //���û������
            if ((m_conn_handle_nus_c == BLE_CONN_HANDLE_INVALID))
            {
                BLE_M_Msg("try to find NUS on conn_handle 0x%x\r\n", p_gap_evt->conn_handle);

                APP_ERROR_CHECK_BOOL(p_gap_evt->conn_handle < CENTRAL_LINK_COUNT + PERIPHERAL_LINK_COUNT);
                err_code = ble_db_discovery_start(&m_ble_db_discovery[p_gap_evt->conn_handle], p_gap_evt->conn_handle);
                APP_ERROR_CHECK(err_code);
            }

            if (ble_conn_state_n_centrals() == CENTRAL_LINK_COUNT)    //��ǰ���ӵ�����
            {
               
            }
            else
            {
                scan_start();
            }
        } break; 

				/* �Ͽ����� */
        case BLE_GAP_EVT_DISCONNECTED:
        {
            uint8_t n_centrals;

            if (p_gap_evt->conn_handle == m_conn_handle_nus_c)
            {
                BLE_M_Msg("NUS central disconnected (reason: %d)\r\n",
                       p_gap_evt->params.disconnected.reason);

                m_conn_handle_nus_c = BLE_CONN_HANDLE_INVALID;
            }

            if ((m_conn_handle_nus_c == BLE_CONN_HANDLE_INVALID))
            {
                scan_start();
            }
            n_centrals = ble_conn_state_n_centrals();    //��ѯ����������

            if (n_centrals == 0)
            {
							
            }
        } break; 

				/* ɨ�赽�㲥 */
        case BLE_GAP_EVT_ADV_REPORT:
        {
				    Cheack_Beacon(&p_gap_evt->params.adv_report);        
#if 0					
            if (strlen(m_target_periph_name) != 0)   //ͨ�����ֲ���Ҫ���ӵ��豸
            {
                if (find_adv_name(&p_gap_evt->params.adv_report, m_target_periph_name))
                {
									  BLE_M_Msg("==============================\r\n");
									  BLE_M_Msg("MAC:%02x:%02x:%02x:%02x:%02x:%02x ",   \
									  p_gap_evt->params.adv_report.peer_addr.addr[5],  \
									  p_gap_evt->params.adv_report.peer_addr.addr[4],  \
									  p_gap_evt->params.adv_report.peer_addr.addr[3],  \
									  p_gap_evt->params.adv_report.peer_addr.addr[2],  \
									  p_gap_evt->params.adv_report.peer_addr.addr[1],  \
									  p_gap_evt->params.adv_report.peer_addr.addr[0]);
									  BLE_M_Msg("rssi:%d\r\n",p_gap_evt->params.adv_report.rssi );
									
//                    err_code = sd_ble_gap_connect(&p_gap_evt->params.adv_report.peer_addr,   //��������
//                                                  &m_scan_params,
//                                                  &m_connection_param);
//                    if (err_code != NRF_SUCCESS)
//                    {
//                        BLE_M_Msg("Connection Request Failed, reason %d\r\n", err_code);
//                    }
                }
            }
            else
            {
                if ((find_adv_uuid(&p_gap_evt->params.adv_report, BLE_UUID_NUS_SERVICE)&&    //ͨ��UUID����Ҫ���ӵ��豸
                     (m_conn_handle_nus_c == BLE_CONN_HANDLE_INVALID)))
                {
//                    err_code = sd_ble_gap_connect(&p_gap_evt->params.adv_report.peer_addr,   //��������
//                                                  &m_scan_params,
//                                                  &m_connection_param);
//                    if (err_code != NRF_SUCCESS)
//                    {
//                        BLE_M_Msg("Connection Request Failed, reason %d\r\n", err_code);
//                    }
                }
            }
#endif
        } break; 

        case BLE_GAP_EVT_TIMEOUT:
        {
            // We have not specified a timeout for scanning, so only connection attemps can timeout.
            if (p_gap_evt->params.timeout.src == BLE_GAP_TIMEOUT_SRC_CONN)
            {
                BLE_M_Msg("Connection Request timed out.\r\n");
            }
        } break; 

        case BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST:
        {
            // Accept parameters requested by peer.
            err_code = sd_ble_gap_conn_param_update(p_gap_evt->conn_handle,
                                        &p_gap_evt->params.conn_param_update_request.conn_params);
            APP_ERROR_CHECK(err_code);
        } break; 

        case BLE_GATTC_EVT_TIMEOUT:
            // Disconnect on GATT Client timeout event.
            BLE_M_Msg("GATT Client Timeout.\r\n");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break; 

        case BLE_GATTS_EVT_TIMEOUT:
            // Disconnect on GATT Server timeout event.
            BLE_M_Msg("GATT Server Timeout.\r\n");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break; 

#if (NRF_SD_BLE_API_VERSION == 3)
        case BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST:
            err_code = sd_ble_gatts_exchange_mtu_reply(p_ble_evt->evt.gatts_evt.conn_handle,
                                                       NRF_BLE_MAX_MTU_SIZE);
            APP_ERROR_CHECK(err_code);
            break; 
#endif
        default:
            break;
    }
}


/**********************************************************************
����:������ʱ���ӻ��¼��ص�����
�������:
    p_adv_report���յ��Ĺ㲥����
    uuid_to_find: ��Ҫ���ҵ�NAME
***********************************************************************/
static void nus_c_evt_handler(ble_nus_c_t * p_nus_c, const ble_nus_c_evt_t * p_nus_c_evt)
{
    switch (p_nus_c_evt->evt_type)
    {
			  /* �ӻ��豸�����¼� */
        case BLE_NUS_C_EVT_DISCOVERY_COMPLETE:
        {
            if (m_conn_handle_nus_c == BLE_CONN_HANDLE_INVALID)
            {
                ret_code_t err_code;

                m_conn_handle_nus_c = p_nus_c_evt->conn_handle;       //��ǰ���Ӿ��
                BLE_M_Msg("NUS discovered on conn_handle 0x%x\r\n",m_conn_handle_nus_c);

							  //�󶨷�����
							  err_code = ble_nus_c_handles_assign(p_nus_c,m_conn_handle_nus_c,&p_nus_c_evt->handles);   //p_nus_c_evt->handles:�ڴӻ��Ϸ��ֵķ�����
                APP_ERROR_CHECK(err_code);

                // nus service discovered. Enable notification of nus.
                err_code = ble_nus_c_rx_notif_enable(p_nus_c);
                APP_ERROR_CHECK(err_code);
            }
        } break; 

				/* �����¼� */
        case BLE_NUS_C_EVT_NUS_RX_EVT:
        {

            BLE_M_Msg("NUS rx len = %d\r\n", p_nus_c_evt->data_len);

            BLE_Salve_SendData(p_nus_c_evt->p_data, p_nus_c_evt->data_len);    //ͨ����������
            
        } break; 
        
				/* �ӻ��Ͽ������¼� */
				case BLE_NUS_C_EVT_DISCONNECTED:
				{
				    BLE_M_Msg("NUS server disconnect...\r\n");
				}
				break;
				
        default:
           
            break;
    }
}


/**********************************************************************
����:��ǰ�豸���������ͻ��ˣ���ʼ��
˵����
ȷ�ϵ�ǰ�豸�������Ļص�����
***********************************************************************/
void nus_c_init(void)
{
    uint32_t         err_code;
    ble_nus_c_init_t nus_c_init_obj;

    nus_c_init_obj.evt_handler = nus_c_evt_handler;

    err_code = ble_nus_c_init(&m_ble_nus_c, &nus_c_init_obj);
    APP_ERROR_CHECK(err_code);
}



/**********************************************************************
����:������������������
���������
pdata:Ҫ���͵�����
len:���ݳ���
˵��
   ͨ�������������������ݡ�
***********************************************************************/
void BLE_Master_SendData(const uint8_t *pdata, const uint16_t len)
{
	  uint32_t  err_code;
    uint8_t frame_number = 0;
    uint8_t residue_number = 0;
	  uint8_t i = 0;
	
	  frame_number = len/m_ble_nus_max_data_len;
	  residue_number = len%m_ble_nus_max_data_len;
	
	  for(i = 0; i < frame_number; i++ )
	  {
				 err_code = ble_nus_c_string_send(&m_ble_nus_c, (uint8_t *)(pdata+i*m_ble_nus_max_data_len), m_ble_nus_max_data_len);
         if ( (err_code != NRF_ERROR_INVALID_STATE) && (err_code != NRF_ERROR_BUSY) )
         {
              APP_ERROR_CHECK(err_code);
         }		
		}
		
		if(residue_number != 0)
		{
			   err_code = ble_nus_c_string_send(&m_ble_nus_c, (uint8_t *)(pdata+i*m_ble_nus_max_data_len), residue_number);
			   if ( (err_code != NRF_ERROR_INVALID_STATE) && (err_code != NRF_ERROR_BUSY) )
			  {
						APP_ERROR_CHECK(err_code);
			  }	
		}	
}

#endif


