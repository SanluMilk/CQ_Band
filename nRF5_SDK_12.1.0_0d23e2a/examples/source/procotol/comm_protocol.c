 /*******************************************************************************************
*          ��Ȩ����, C ,2018- ,�ɶ������ֵ���Ϣ�������޹�˾
*
*����:Ƕ��ʽ������              ����:2018-9-1            �汾:V0.0
*˵��:
*    �ù��̻��ڣ�Nordic�ٷ�nRF5_SDK_12.1.0_0d23e2a.ʵ��BLE����һ��ͨ�š���
*������Ҫ����MPU6050,OLED��Ӳ����ʵ�ּƲ������ʲɼ��ȹ��ܡ�
*�޸�:
2019-3-13:
    1������Ѫ�������������ɼ����
		2�������Զ��ɼ�������
		3���������ӡ�
2019-4-1:
    1.���ӻ�ȡBeacon�Ƿ�ɨ������
*******************************************************************************************/

#include "comm_protocol.h"
#include "ble_salve.h"
#include "nrf_delay.h"
#include "app_error.h"
#include <string.h>
#include <stdio.h>
#include "utc.h"
#include "heat_sensor.h"
#include "oled_app.h"
#include "ble_config.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"

extern uint16_t collect_interval;       //�Զ��ɼ���� 
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
void BLE_Data_analysis(const uint8_t *data, const uint8_t len )
{
	   if(data[0] == 0xFF)                      //��ѯ�豸����
				{
					    NRF_LOG_INFO("CMD_Device_Device\r\n");
					    const uint8_t buff[] = {DEVICE_TYPE};
				
				     BLE_Salve_SendData(buff,1);   
				}
    else if( data[0] == PRODUCT_NUMBER )     //�жϲ�Ʒ���Ƿ���ȷ
    {
        switch( data[1] )
        {
            /********************************* �����豸 **************************************/
            case CMD_Device_Reset:
            {
                uint8_t buff[3] = {0};
                
                if( data[2] == 0 )    //�ж������Ƿ���ȷ
                {
                    NRF_LOG_INFO("CMD_Device_Reset\r\n");

                    buff[0] = PRODUCT_NUMBER;
                    buff[1] = CMD_Device_Reset;
                    buff[2] = Ret_Success;
                    BLE_Salve_SendData(buff,3);
                    nrf_delay_ms(1000);
                    APP_ERROR_CHECK(!NRF_SUCCESS);    //ͨ������У������
                }
            }
            break;

            /******************************* ��ȡ�汾�� ************************************/
            case CMD_Get_Version:
            {
                uint8_t buff[4] = {0};
                
                if( data[2] == 0 )    //�ж������Ƿ���ȷ
                {
                    NRF_LOG_INFO("CMD_Get_Version\r\n");

                    buff[0] = PRODUCT_NUMBER;
                    buff[1] = CMD_Get_Version;
                    buff[2] = HARDWARE_VERSION;
                    buff[3] = SOFTWARE_VERSION;
                    
                    BLE_Salve_SendData(buff,4);
                }
            }
            break;

            /******************************* ��ȡMAC��ַ **********************************/
            case CMD_Get_Mac:
            {
                uint8_t buff[8] = {0};
                ble_gap_addr_t device_addr;
                
                if( data[2] == 0 )    //�ж������Ƿ���ȷ
                {
                    NRF_LOG_INFO("CMD_Get_Mac\r\n");

	                  sd_ble_gap_addr_get(&device_addr);   //��ȡMAC��ַ
                    buff[0] = PRODUCT_NUMBER;
                    buff[1] = CMD_Get_Mac;
									  buff[2] = device_addr.addr[5];
									  buff[3] = device_addr.addr[4];
									  buff[4] = device_addr.addr[3];
									  buff[5] = device_addr.addr[2];
									  buff[6] = device_addr.addr[1];
									  buff[7] = device_addr.addr[0];
                    BLE_Salve_SendData(buff,8);
                }
            }
            break;

            /***************************** У׼ʱ�� *************************************/
            case CMD_Adjust_Time:
            {
                NRF_LOG_INFO("CMD_Adjust_Time\r\n");
													   uint8_t buff[3] = {0};
																
                if(len == 9)
                {
                   UTCTime_t tm;
                   union{
                       uint16_t a;
                       uint8_t b[2];
                   }c;

                   c.b[1] = data[2];
                   c.b[0] = data[3];
                  
                   tm.year   = c.a;
                   tm.month  = data[4]-1;
                   tm.day    = data[5]-1;
                   tm.hour   = data[6];
                   tm.minute = data[7];
                   tm.second = data[8];
                   NRF_LOG_INFO("%d-%d-%d %d:%d:%d\r\n",tm.year,tm.month+1,tm.day+1,tm.hour,tm.minute,tm.second);
                   SetClock_UTC(&tm);              //����UTCʱ��

                   buff[0] = PRODUCT_NUMBER;
                   buff[1] = CMD_Adjust_Time;
                   buff[2] = Ret_Success;
                   BLE_Salve_SendData(buff,3);
         
                }
                else
                {
                   buff[0] = PRODUCT_NUMBER;
                   buff[1] = CMD_Adjust_Time;
                   buff[2] = 1;
                   BLE_Salve_SendData(buff,3);
                }
                
            }
            break;
            
            /****************************** ��ȡʱ�� *************************************/
            case CMD_Get_Time:
            {
                uint8_t buff[9] = {0};
                UTCTime_t tm;
                
                if( data[2] == 0 )    //�ж������Ƿ���ȷ
                {
                   union{
                       uint16_t a;
                       uint8_t b[2];
                   }c;
                   
                    NRF_LOG_INFO("CMD_Get_Time\r\n");
                    GetClock_UTC(&tm);
                    c.a = tm.year;

                    buff[2] = c.b[1];
                    buff[3] = c.b[0];
                    buff[4] = tm.month+1;
                    buff[5] = tm.day+1;
                    buff[6] = tm.hour;
                    buff[7] = tm.minute;
                    buff[8] = tm.second;
                    
                    buff[0] = PRODUCT_NUMBER;
                    buff[1] = CMD_Get_Time;
	                  
                    BLE_Salve_SendData(buff,9);
                }    
            }
            break;
						
						/********************* �����Ƿ�ɨ��Beacon ***************************/
						case CMD_Set_Beacon:
						{
							  uint8_t buff[3] = {0};
								
						    save_param.OpenBeaconFlag = data[2];
							  
								/* �������� */	
                buff[0] = data[0];
                buff[1] = CMD_Set_Beacon;
                buff[2] = 0x00;	

                BLE_Salve_SendData(buff,3);		
						}
						break;

						/********************* ����Ŀ��Beacon��MAC ***************************/
						case CMD_Set_Beacon_Mac:
						{
						    uint8_t buff[3] = {0};   

								memcpy(save_param.Beacon_Mac, &data[2], 6);
								
                	/* �������� */	
                buff[0] = data[0];
                buff[1] = CMD_Set_Beacon_Mac;
                buff[2] = 0x00;	

                BLE_Salve_SendData(buff,3);		

                NRF_LOG_INFO("Beacon New MAC:%02X:%02X:%02X:%02X:%02X:%02X",save_param.Beacon_Mac[0],
								             save_param.Beacon_Mac[1],save_param.Beacon_Mac[2],save_param.Beacon_Mac[3],
								             save_param.Beacon_Mac[4],save_param.Beacon_Mac[5]);								
						}
						break;
						
						/************************* ��ȡBeacon�Ƿ�ɨ�� ******************************/
						case CMD_Get_Beacon:
						{
						    if( data[2] == 0 )
								{
								    uint8_t buff[3] = {0}; 
  
                    /* �������� */	
                    buff[0] = data[0];
                    buff[1] = CMD_Get_Beacon;
                    buff[2] = save_param.OpenBeaconFlag;	

                    BLE_Salve_SendData(buff,3);		
										
								}									
						}
						break;
						
            /************************* �����ɼ� *********************************/
            case CMD_Continuous_Collect:
            {
							  /* ���ܵ�ǰ��������棬�ֻ��ɼ����ȼ���� */
                if( data[2] == 0x01 )    
								{
								    timers_start_Max30102_timer();    //���������ɼ�
									
									  if( OLED_ctrl.oled_state_flag == OLED_OFF )
									      OLED_Display_On();  
										
										SetNewPageInfo(COLLECT_P, OLED_ON, OLED_CLOSE_EN, OLED_OFF_TIME);
										JumpPage();
								}
                else                     
								{	
								    timers_stop_Max30102_timer();    //�ر������ɼ�

									  SetLastPageInfo(DATE_P, OLED_OFF, OLED_CLOSE_EN, OLED_OFF_TIME);
									 
  									if( OLED_ctrl.oled_state_flag == 0)
									      OLED_Display_On();
										
										SetNewPageInfo(DATE_P, OLED_ON, OLED_CLOSE_EN, OLED_OFF_TIME);
										JumpPage();  //��ʾ	
										
										NRF_LOG_INFO("Finish Collect...\r\n");
								}									
            }
            break;

            /**************************** �����Զ��ɼ���� *************************/
            case CMD_Collect_Interval:
            {
                 collect_interval = save_param.Collect_interval =  data[2] * 60; 
								 
								 uint8_t buff[3] = {0};
									
									  /* �������� */	
                buff[0] = data[0];
                buff[1] = CMD_Collect_Interval;
                buff[2] = 0x00;	

                BLE_Salve_SendData(buff,3);			
            }
            break;
						
						 /************************* �������� *********************************/
            case CMD_Set_Alarm:
            {
							  uint8_t buff[3] = {0};
								
                save_param.alarm[0] = data[2];     //�������ӵ�ʱ 
                save_param.alarm[1] = data[3];     //�������ӵķ� 
                save_param.alarm[2] = data[4];     //�������ӵ��� 		

                /* �������� */	
                buff[0] = data[0];
                buff[1] = CMD_Set_Alarm;
                buff[2] = 0x00;	

                BLE_Salve_SendData(buff,3);								
            }
            break;
        }

    }
    
}








