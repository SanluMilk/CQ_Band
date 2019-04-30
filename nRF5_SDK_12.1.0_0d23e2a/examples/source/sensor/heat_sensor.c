/***************************************************************************
 ��Ȩ (c), 2018- ,�ɶ������ֵ���Ϣ�������޹�˾

����:Ӳ����                ����:2018-10-11              �汾:V0.0


����:
    ���ڴ�����/Ѫѹ�������ж�ȡ����ֵ,Ѫѹֵ
***************************************************************************/
#include "app_uart_ex.h"
#include "nrf_delay.h"
#include "heat_sensor.h"
#include "timer.h"
#include "app_timer.h"
#include "oled_app.h"
#include "stepcounter.h"
#include "comm_protocol.h"
#include "ble_salve.h"
#include "app_gpio.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

/* ���� */
                                /*����ѹ ����ѹ ����       */
static uint8_t adjust[]   = {0xFE, 0x6E, 0x4C, 0x46, 0x00, 0x00 };     //У׼Ѫѹ����
static uint8_t readData[] = {0xFD, 0x00, 0x00, 0x00, 0x00, 0x00 };     //��ȡѪѹ����
static uint8_t readECG[]  = {0xF9, 0x00, 0x00, 0x00, 0x00, 0x00 };     //��ȡ�ĵ�����

Send_Cmd_type_e SendCMDType = SENSOR_READ;      //����Ѫѹ���͵��������ͣ�Ĭ��Ϊ��ȡ����

/* ����һ����ʱ�� */
APP_TIMER_DEF(m_sensor_task_timer_id);              //1S��ʱ��
APP_TIMER_DEF(m_CloseSensor_task_timer_id);         //�رմ��������ܶ�ʱ��
static uint8_t sensor_state = 0;                    //������״̬         0:û�вɼ�        1:�ɼ��� 
static uint8_t sendNum[SENSOR_MAX] = {0};           //������͵ĳ�ʱ����

/*************************************************************************
������:Open_Heat_Sensor
����:������Ѫѹ�������ɼ�
�������:
@ms:�ɼ����
@mode:��ʱ������ģʽ
APP_TIMER_MODE_SINGLE_SHOT     //����
APP_TIMER_MODE_REPEATED        //ѭ��
�������:None
����ֵ:None
*************************************************************************/
void Open_Heat_Sensor(uint32_t ms,app_timer_mode_t mode)
{
	   if( sensor_state == 0 )
		 {
			   SENSOR_ON;             //������������Դ
			   nrf_delay_ms(500);     //�ȴ�500ms
			   /* ��ʼ������ */
		     UartInit();                     //��ʼ������		 
			 
	       /* �����ɼ���ʱ�� */
         app_timer_start(m_sensor_task_timer_id, APP_TIMER_TICKS(ms, mode), NULL);    //����������ʱ��
			   sensor_state = 1; 
		 }
}

/*************************************************************************
������:Close_Heat_Sensor
����:�ر�����Ѫѹ�������ɼ�
�������:None
�������:None
����ֵ:None
*************************************************************************/
void Close_Heat_Sensor(void)
{
	  if( sensor_state == 1 )
		{
        /* �رղɼ���ʱ�� */
	      (void)app_timer_stop(m_sensor_task_timer_id);
	      
			  sensor_state = 0;     //���û�вɼ�

        /* �����رմ�������Դ��ʱ�� */
        app_timer_start(m_CloseSensor_task_timer_id,  APP_TIMER_TICKS(1000, APP_TIMER_PRESCALER), NULL);     //1S��ر�
		}
}

/*************************************************************************
������:sensor_timeout_handler
����:������Ӧ������
�������:
@pcontext:���ݲ�����ָ��
�������:None
����ֵ:None
˵��:
    ͨ������ķ��ʹ��������õĶ�ʱ���¼���ȷ��һ������ĳ�ʱʱ��
*************************************************************************/
uint8_t adjust_flag = 0;   //0:δУ׼     1:У׼��
static void sensor_timeout_handler(void * pcontext)
{       
    switch(SendCMDType)
    {
        // У׼������
        case SENSOR_ADJUST:    
        {
            sendNum[SENSOR_ADJUST]++;                    //���ʹ���++
            if(sendNum[SENSOR_ADJUST] <= 30)              //����û�г���30��
						{
						     if( adjust_flag == 0 )
                    Uart_SendData(adjust, sizeof(adjust));   //��������
						}
            else              //���ʹ�������3��
            {
                NRF_LOG_INFO("adjust sensor timout...\r\n");
                /* 0. �رն�ʱ��,ֹͣ���͡��رն�ʱ�������ֿ���:<1>���յ���ȷ�ķ���        <2>��ʱ���˴������ڵڶ���      */
                Close_Heat_Sensor();
                
								uint8_t buff[3] = {0};
                /* 1. ���ֻ����ش��� */
								buff[0] = PRODUCT_NUMBER;
                buff[1] = CMD_Adjust_Heart_Sensor;
	              buff[2] = 0x02;
                BLE_Salve_SendData(buff,3);          //���ظ�APP    
            }
                
        }
        break;  

        // ��ȡѪѹ�������� 
        case SENSOR_READ  :   
        {
				     Uart_SendData(readData, sizeof(readData));	
					
            
            if(sendNum[SENSOR_READ] < 15)               //����û�г���10��(��ʱ�¼�ʱ3*1 = 30s),��ʼ��ʾ������
						{
						    sendNum[SENSOR_READ]++;                      //���ʹ���++
                sys_state.bool_preeeure_flag = 0;							
						}
						else
						     sys_state.bool_preeeure_flag = 1;		    
						
//            else
//            {
//                 NRF_LOG_INFO("read data sensor timout...\r\n");
//                 /* 0. �رն�ʱ��,ֹͣ���͡��رն�ʱ�������ֿ���:<1>���յ���ȷ�ķ���        <2>��ʱ���˴������ڵڶ���        */
//                 Close_Heat_Sensor();    

//                 /* 1. ��ʾ�ɼ�ʧ�� */
//                 if( OLED_ctrl.page_num == HEART_P )    //�����ǰ������ҳ��
//                 {
//                     heart_sensor_data.heart = 1;       //��ʾ�ɼ�ʧ��
//                 }
//                 else if(OLED_ctrl.page_num == BLOOD_P) //��ǰ������Ѫѹҳ��
//                 {
//                     heart_sensor_data.high = 1;       //��ʾ�ɼ�ʧ��
//                 }
//                 /* 2.��APP�������� */
//                  union{
//                            uint32_t a;
//                            uint8_t b[4];
//                  }c;
//                  uint8_t buff[9] = {0};

//                  c.a = stepcount;
//                  buff[0] = PRODUCT_NUMBER;
//                  buff[1] = CMD_Adjust_Heart_Sensor;
//                  buff[2] = c.b[3];
//                  buff[3] = c.b[2];
//                  buff[4] = c.b[1];
//                  buff[5] = c.b[0];
//                  buff[6] = 0xFF;
//                  buff[7] = 0XFF;
//                  buff[8] = 0XFF;
//                        
//                  BLE_Salve_SendData(buff,9);          //���ظ�APP 
//            }
        }
        break; 

        // ��ȡ�ĵ�ͼ����
        case SENSOR_ECG   :  
        {
            Uart_SendData(readECG, sizeof(readECG));  
        }break;     
    } 
}

/*************************************************************************
������:Close_sensor_handler
����:�رմ��������ܵĶ�ʱ��
�������:
@pcontext:���ݲ�����ָ��
�������:None
����ֵ:None
˵��:
    ��Ҫ�رյ��д��ڣ��ʹ�������Դ�����ڹرմ�����������һ�������ڴ��ڻص�����ִ�У�
��������ִ��ڴ���������Ҫ�����������񣬽��лص����رմ��ں͵�Դ��
*************************************************************************/
static void Close_sensor_handler(void * pcontext)
{
    /* ���ݲɼ���ɹرմ��� */
    app_uart_close();      //�رղɼ�
					    
    /* �رմ�������Դ */
    SENSOR_OFF;  
    NRF_LOG_INFO("Close Sensor Power");	
}

/*************************************************************************
������:Heat_sensor_Init
����:��ʼ������Ѫѹ�������ɼ�
�������:None
�������:None
����ֵ:None
����:��ʼ���ɼ����������Ǵ����ɼ���ʱ��
*************************************************************************/
void Heat_sensor_Init(void)
{
    uint32_t err_code;
	
	  /* ���ڲɼ���ʱ�� */
    err_code = app_timer_create(&m_sensor_task_timer_id,         //��ʱ�����
								APP_TIMER_MODE_REPEATED,                         //�����Զ�ʱ��
								sensor_timeout_handler);                         //��ʱ���ص�����
	  APP_ERROR_CHECK(err_code);  
	
	  /* �رղɼ�ʱ,ʹ�ö�ʱ�����ӳ�1S�ر� */
	  err_code = app_timer_create(&m_CloseSensor_task_timer_id,    //��ʱ�����
								APP_TIMER_MODE_SINGLE_SHOT,                      //���ζ�ʱ��
								Close_sensor_handler);                           //��ʱ���ص�����
	  APP_ERROR_CHECK(err_code);  
}

/*************************************************************************
������:Sensor_Adjust
����:У׼������
�������:
@sysstolic:����ѹ
@diastole:����ѹ
@heart:����
�������:None
����ֵ:None
˵��:
    У׼��������3S����һ������
*************************************************************************/
void Sensor_Adjust(const uint8_t sysstolic, const uint8_t diastole, const uint8_t heart )
{
    /* 1.������� */
    adjust[1] = sysstolic;
    adjust[2] = diastole;
    adjust[3] = heart;

    /* 2.�޸ķ�����������,���㷢�ʹ��� */
    SendCMDType = SENSOR_ADJUST;
    sendNum[SENSOR_ADJUST] = 0;

    /* 3.������ʱ����ѭ������ */
    Open_Heat_Sensor(2000,APP_TIMER_MODE_REPEATED);
}

/*************************************************************************
������:Sensor_ReadData
����:��ȡ����Ѫѹֵ
�������:None
�������:None
����ֵ:None
˵����
    ��ȡ���ݣ�1S��һ��
*************************************************************************/
void Sensor_ReadData(void)
{
    /* 1.�޸ķ�����������,���㷢�ʹ��� */
    SendCMDType = SENSOR_READ;
    sendNum[SENSOR_READ] = 0;

    /* 2.������ʱ����ѭ������ */
    Open_Heat_Sensor(1000,APP_TIMER_MODE_REPEATED);
}

/*************************************************************************
������:Sensor_ReadECG
����:��ȡ�ĵ�����
�������:
@EN:�򿪲ɼ���رղɼ�
�������:None
����ֵ:None
˵����
    ��ȡ�ĵ����ݣ�10mS��һ��
    ECGֵ��Ҫ�����ɼ���������Ҫ���ÿ�ʼ�ͽ���
*************************************************************************/
void Sensor_ReadECG(Enable_e EN)
{
    if( EN == ENABLE )
    {
        /* 1.�޸ķ�����������,���㷢�ʹ��� */
        SendCMDType = SENSOR_READ;
        sendNum[SENSOR_READ] = 0;

        /* 2.������ʱ����ѭ������ */
        Open_Heat_Sensor(10,APP_TIMER_MODE_REPEATED);
    }
    else
    {
        Close_Heat_Sensor();     
    }
}



