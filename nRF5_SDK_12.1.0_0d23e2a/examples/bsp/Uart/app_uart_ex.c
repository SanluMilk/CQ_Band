/*******************************************************************************************
*          ��Ȩ����, C ,2018- ,�ɶ������ֵ���Ϣ�������޹�˾
*
*����:Ƕ��ʽ������              ����:2018-9-1            �汾:V0.0
*˵��:
*    ������غ���
*�޸�:
*******************************************************************************************/
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "boards.h"
#include "app_uart.h"
#include "app_uart_ex.h"
#include "ble_salve.h"
#include "timer.h"
#include "app_timer.h"

#include "ble_config.h"
#include "ble_master.h"
#include "ble_salve.h"

#include "heat_sensor.h"
#include "oled_app.h"
#include "hard_config.h"
#include "comm_protocol.h"
#include "stepcounter.h"
#include "app_gpio.h"


#include "nrf_log.h"
#include "nrf_log_ctrl.h"

/**********************************************************************
������:uart_event_handle
����:�����¼��ص�����
�������:
@p_event:�����¼�
�������:None
����ֵ:None
˵��:
***********************************************************************/
extern uint8_t adjust_flag ;
extern heart_sensor_t Save_heart_sensor_data;  //�����ϴλ�ȡ������Ѫѹ���ݣ����ڹ㲥
static void uart_event_handle(app_uart_evt_t * p_event)
{
   static uint8_t data[UART_RX_BUF_SIZE] = {0}; 
   static uint16_t index = 0;
   static uint8_t rec_flag = 0;     //0:���ɽ���        1:�ɽ���
		                                   /*
                                       bit0:����Ѫѹ���ݿɽ���
                                       bit1:У׼���ݷ���
                                      */
		static uint8_t collect_num = 0;  //�ɼ����ݵĴ���
		static uint8_t collect_data[3][3];//�ɼ������ݻ���
    uint8_t buff[9] = {0};
                   
    switch (p_event->evt_type)
    {
        case APP_UART_DATA_READY:
         {
            UNUSED_VARIABLE(app_uart_get(&data[index]));      //����������

            /* �ɼ�����Ѫѹ���� */
            if( data[index]  == 0xFD || READ_FLAG(rec_flag,0))
            {
                SET_FLAG(rec_flag,0);
                index++;
						    
                if(index == 6)
                {
                    NRF_LOG_HEXDUMP_INFO(data,6);            //������Ϣ����ӡ���յ�����
//										BLE_Salve_SendData(data,6);             //���ظ�APP 		  

                    if((data[1] != 0 && data[1] != 0xFF && data[2] != 0 && data[2] != 0xFF && data[3] != 0 && data[3] != 0xFF) &&   //ȷ��������Ч,����С��150
                        ((OLED_ctrl.page_num == HEART_P && heart_sensor_data.heart >= 0 )) )         //ȷ���ѹ�����ʱ       
                    {
										    NRF_LOG_INFO("data[1] = %02x,data[2] = %02x,data[3] = %02x",data[1],data[2],data[3]);
												sys_state.bool_preeeure_valid = 1;
											
											  /* �����ݷ��뻺�� */
												collect_data[0][collect_num] = data[1];    //0�ռ�������ѹ
                        collect_data[1][collect_num] = data[2];    //1�ռ�������ѹ
                        collect_data[2][collect_num] = data[3];    //2�ռ�������
                        collect_num++;	
																					  
												if( collect_num >= 3 )
												{
												    Close_Heat_Sensor();
														Motor_Drive(); 
														collect_num = 0;
																									
														heart_sensor_data.high   = collect_data[0][0];       //����ѹ
														heart_sensor_data.low    = collect_data[1][0];       //����ѹ
														heart_sensor_data.heart  = collect_data[2][0];	      //����
																									
														/* ��Ч����ȡ�ɼ������ݵ���Сֵ */
														for(uint8_t i = 0; i < 3; i++)
														{
														    if( heart_sensor_data.high > collect_data[0][i] )
																    heart_sensor_data.high  = collect_data[0][i];       //����ѹ
																																
																if( heart_sensor_data.low > collect_data[1][i] )
                                    heart_sensor_data.low = collect_data[1][i];       //����ѹ
																																
																if( heart_sensor_data.heart > collect_data[2][i] )
                                    heart_sensor_data.heart = collect_data[2][i];	      //����
	
                            }
																												
                            union{
                                uint32_t a;
                                uint8_t b[4];
                            }c;

                            c.a = stepcount;
                            buff[0] = PRODUCT_NUMBER;
                            buff[1] = CMD_Adjust_Heart_Sensor;
                            buff[2] = c.b[3];
                            buff[3] = c.b[2];
                            buff[4] = c.b[1];
                            buff[5] = c.b[0];
                            Save_heart_sensor_data.high = buff[6] = heart_sensor_data.high;
                            Save_heart_sensor_data.low = buff[7] = heart_sensor_data.low;
                             Save_heart_sensor_data.heart = buff[8] = heart_sensor_data.heart;
                        
                            BLE_Salve_SendData(buff,9);           //���ظ�APP 
										   }
																						 
                   }
									 else     //��û�л�ȡ������
									 {
										    sys_state.bool_preeeure_valid = 0;
										    if( sys_state.bool_preeeure_flag != 0 )
												{
										        if( data[1] == 0xFF && data[2] == 0xFF &&data[3] == 0xFF )     //δ�Ӵ�Ƥ��
												    {
												        heart_sensor_data.heart = 0xFF;
                                heart_sensor_data.high = 0xFF;
                                heart_sensor_data.low = 0xFF;													
												    }
												    else if( (data[1] == 0) || (data[2] == 0) || (data[3] == 0) )        //��Ч���ݻ�û�г���
												    {
												        /* ��һ������� */
                            		heart_sensor_data.heart = 50+rand()%30;    //50+30�ڵ������
                                heart_sensor_data.high = 100+rand()%40;    //100+40�ڵ������
                                heart_sensor_data.low = 50+rand()%30;    //50+30�ڵ������													
												    }
												}
									 }

                   index = 0;                                     //������ձ��
									 memset(data,0,UART_RX_BUF_SIZE);               //������ջ���
                   CLEAN_FLAG(rec_flag,0);                        //����ɽ��ձ��
								}
					  }

            /* ������У׼���� */
            if( data[index]  == 0xFE || READ_FLAG(rec_flag,1))
            {
                SET_FLAG(rec_flag,1);
                index++;
						    
                if(index == 6)
                {
                    NRF_LOG_HEXDUMP_INFO(data,6);                //������Ϣ����ӡ���յ�����
                
                    if( data[3] == 0x00 || data[3] == 0x01 )
                    {
										    if( data[3] == 0x00 )       //У׼�ɹ�
												    Close_Heat_Sensor();
												else
												    adjust_flag = 1;        //У׼��
											
																								
                        buff[0] = PRODUCT_NUMBER;
                        buff[1] = CMD_Adjust_Heart_Sensor;
	                      buff[2] = data[3];
                        BLE_Salve_SendData(buff,3);          //���ظ�APP
                    }

                    
                    index = 0;                               //������ձ��
									  memset(data,0,UART_RX_BUF_SIZE);         //������ջ���
                    CLEAN_FLAG(rec_flag,1);                  //����ɽ��ձ��
							 }
					}
        }
        break;

        case APP_UART_COMMUNICATION_ERROR:
            APP_ERROR_HANDLER(p_event->data.error_communication);
            break;

        case APP_UART_FIFO_ERROR:
            APP_ERROR_HANDLER(p_event->data.error_code);
            break;

        default:
            break;
    }
}

/**********************************************************************
������:UartInit
����:���ڳ�ʼ��
�������:None
�������:None
����ֵ:None
˵��:
��ʼ������ʱ�����ڵ�RX��TX����������
***********************************************************************/
void UartInit(void)
{
    uint32_t err_code;
	  
    app_uart_comm_params_t comm_params =
    {
        .rts_pin_no = RTS_PIN_NUMBER,
        .cts_pin_no = CTS_PIN_NUMBER,
        .rx_pin_no = RX_PIN_NUMBER,
        .tx_pin_no = TX_PIN_NUMBER,
        .flow_control = APP_UART_FLOW_CONTROL_DISABLED,
        .use_parity = false,
        .baud_rate = UARTE_BAUDRATE_BAUDRATE_Baud115200,
    };
		
    APP_UART_FIFO_INIT( &comm_params,           //��ʼ������
                        UART_RX_BUF_SIZE,
                        UART_TX_BUF_SIZE,
                        uart_event_handle,
                        APP_IRQ_PRIORITY_LOWEST,
                        err_code);
    APP_ERROR_CHECK(err_code);
}

/**********************************************************************
������:fputc
����:֧��C��׼��printf
�������:
@ch:��Ҫ���͵�����
p_file��
�������:None
����ֵ:���͵�����
˵��:
***********************************************************************/
//int fputc(int ch, FILE * p_file)
//{
//    UNUSED_PARAMETER(p_file);

//    UNUSED_VARIABLE(app_uart_put((uint8_t)ch));
//    return ch;
//}

/**********************************************************************
������:Uart_SendData
����:����ֱ�ӷ�������
�������:
@pData:��Ҫ���͵����ݵ�ָ��
@Len:��Ҫ���͵����ݵĳ���
�������:None
����ֵ:None
˵��:
***********************************************************************/
void Uart_SendData(const uint8_t *pData, const uint16_t Len)
{
    for(uint16_t i = 0; i < Len; i++)
	  {
		    app_uart_put((uint8_t)pData[i]);    
		}
}
