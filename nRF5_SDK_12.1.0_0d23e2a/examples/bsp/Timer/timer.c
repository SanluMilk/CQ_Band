/*******************************************************************************************
*          ��Ȩ����, C ,2018- ,�ɶ������ֵ���Ϣ�������޹�˾
*
*����:Ƕ��ʽ������              ����:2018-9-1            �汾:V0.0
*˵��:
*    ������ʱ���������и���ʱ���Ļص�������
*�޸�:
2019-2-26:
1������ˢ�¹㲥��Ϣ��ʱ����
2019-2-27:
1���޸ĳ��״̬�߼�����
2�����ӵ���ģʽ���رչ㲥���رռƲ���ʱ����
*******************************************************************************************/
#include <stdio.h>

#include "timer.h"
#include "boards.h"
#include "adc.h"
#include "utc.h"
#include "oled_app.h"
#include "ble_salve.h"
#include "ble_master.h"
#include "comm_protocol.h"
#include "lis3dh_driver.h"
#include "app_gpio.h"
#include "spi.h"
#include "lis3dh_driver.h"
#include "ble_advertising.h"
#include "MAX3010x.h"
#include "stepcounter.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#ifdef TIME_DEBUG
 #define TIME_Msg    NRF_LOG_INFO
#else
 #define TIME_Msg(...)
#endif

APP_TIMER_DEF(m_period_task_timer_id);              //1S��ʱ��
APP_TIMER_DEF(m_adc_task_timer_id);                 //ADCת����ʱ��
APP_TIMER_DEF(m_step_10_task_timer_id);             //�Ʋ�10ms��ʱ��
APP_TIMER_DEF(m_step_50_task_timer_id);             //�Ʋ�20ms��ʱ��
APP_TIMER_DEF(m_GapData_task_timer_id);             //�㲥���ݸ��¶�ʱ��
APP_TIMER_DEF(m_max30102_task_timer_id);            //10m��ȡһ��MAX30102������

uint16_t collect_interval = 0;       //�Զ��ɼ����  

/**********************************************************************
������:timers_init
����:��ʱ�ӽ��г�ʼ��
�������:None
�������:None
����ֵ:None
˵��:
***********************************************************************/
void timers_init(void)
{
    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, NULL);
}

/**********************************************************************
������:Low_Power_State
����:����Ƿ��ǵ͵���״̬������ǣ�����Ӧ�Ĵ���
�������:None
�������:None
����ֵ:None
˵��:
***********************************************************************/
static void Low_Power_State(void)
{
     /*************************************** ��ص���������� ************************************************/
		 static uint8_t low_power_flag = 0;      //0:δ��ǵ͵���   1:�ѱ�ǵ͵���
		 uint32_t err_code;
		 
		 if( sys_state.power_low == 1 )  //��������
		 {
			   if( low_power_flag == 0 )
				 {
					   TIME_Msg("Low Power\r\n");
				     low_power_flag = 1;
					   /* �رչ㲥,��ʱ�� */
					   sd_ble_gap_adv_stop();   //�ع㲥
					   (void)app_timer_stop(m_step_10_task_timer_id);    //�رն�ʱ��
					   (void)app_timer_stop(m_step_50_task_timer_id);
					   (void)app_timer_stop(m_GapData_task_timer_id);
				 }
			 
		 }
		 else     //��������
		 {
		     if( low_power_flag == 1 )
				 {
					   TIME_Msg("Not Low Power\r\n");
				     low_power_flag = 0;
					   
	           err_code = app_timer_start(m_step_10_task_timer_id, STEP_10MS_TASK_TIMEOUT, NULL);  //�����Ʋ�10msʱ�������ʱ��
	           APP_ERROR_CHECK(err_code);
	
	           err_code = app_timer_start(m_step_50_task_timer_id, STEP_50MS_TASK_TIMEOUT, NULL);  //�����Ʋ�50ms�㷨��ʱ��
	           APP_ERROR_CHECK(err_code);
	
	           err_code = app_timer_start(m_GapData_task_timer_id, GAPDATA_TASK_TIMEOUT, NULL);  //�����޸Ĺ㲥���ݶ�ʱ��
	           APP_ERROR_CHECK(err_code);   
					 
					    //�����㲥,��ʱ��
					   err_code = ble_advertising_start(BLE_ADV_MODE_FAST);    //���¹㲥
             APP_ERROR_CHECK(err_code);
				 }
		 }    
}

/**********************************************************************
������:Alarm_Response
����:������Ӧ������ʱ�䵽�˺���Ӧ
�������:None
�������:None
����ֵ:None
˵��:
***********************************************************************/
static void Alarm_Response(const UTCTime_t *tm)
{
     /*************************************** ������� ************************************************/
		 if( tm->hour == save_param.alarm[0] && tm->minute == save_param.alarm[1] && tm->second == save_param.alarm[2] )
		 {
		     if( OLED_ctrl.page_num != BLE_DISCONNECT_P &&  OLED_ctrl.page_num != BLE_CONNECT_P && OLED_ctrl.page_num != BEACON_LOST_P )    //��ǰ���������Ͽ�ҳ�棬Ҳ������������ҳ�棬Ҳ�����ű궼��ҳ��
				 {
				       if( OLED_ctrl.oled_state_flag == OLED_OFF )    //�����ǰ��ĻϨ��
               {  
							      OLED_Display_On();      //������Ļ
								    SetLastPageInfo(DATE_P, OLED_ON, OLED_CLOSE_EN, 1);  //�����ǰҳ����Ϩ��ģ��ͽ���һ������Ϣ����Ϊ����ҳ�档��ʱ��Ϊ1S��ȷ���Ͽ����Ӻ�	                                                         //�޲�������Ϣ����
							 }
							 else     //�����ǰ��Ļ����
							 {
							      SetLastPageInfo(OLED_ctrl.page_num, OLED_ctrl.oled_state_flag, OLED_ctrl.oled_off_key, OLED_OFF_TIME);  //��¼֮ǰ��ҳ����Ϣ
							 }
							 
							 SetNewPageInfo(ALARM_P, OLED_ON, OLED_CLOSE_EN, OLED_OFF_TIME);   //���õ�ǰҪ��ʾ��ҳ����Ϣ
								 JumpPage();
							 
							 /* ������ */
				       Motor_Drive();
						   
				 }

				 
		 }		     
}

/**********************************************************************
������:Auto_Collection
����:�Զ��ɼ���⣬������ʱ�䵽�����Զ������ɼ�
�������:None
�������:None
����ֵ:None
˵��:
***********************************************************************/
static void Auto_Collection(void)
{
     /*************************************** �Զ��ɼ���� ************************************************/
		 if( collect_interval != 0 )
		 {
				   if( (--collect_interval == 0) && !( OLED_ctrl.page_num == HEART_P || OLED_ctrl.page_num == OXYGEN_P || OLED_ctrl.page_num == TEMPER_P))   //����ʱʱ�䵽���ҵ�ǰ���ڲɼ�ҳ��
					 {
					     collect_interval = save_param.Collect_interval;    //���¸�ֵ����

                timers_start_Max30102_timer();
									
						    if( OLED_ctrl.oled_state_flag == 0 )
								    OLED_Display_On(); 
                
						    max30102_data.Heart = -3;
								OLEDE_dispaly_HeartRate(max30102_data.Heart);   //��ʾ����
						    OLED_ctrl.oled_off_key = 0;                     //�����Ե���ʱϢ��
						    OLED_ctrl.oled_state_flag = 1;                  //��ǵ�ǰ��Ļ�����ŵ�
						    OLED_ctrl.time_off = OLED_OFF_TIME;             //���¸�ֵ����ʱʱ��
						    OLED_ctrl.page_num = HEART_P;                   //��ǵ�ǰҳ��						 
					 }
		}	  
}

/**********************************************************************
������:period_timeout_handler
����:1s���ڶ�ʱ��,ʵ��UTCʱ�������,��LED��̬��ʾ
�������:
@p_context:��������ָ��
�������:None
����ֵ:None
˵��:
***********************************************************************/
static void period_timeout_handler(void * p_context)
{
   
    UTCTime_t tm;
	  
    Time_Run();                              //����UTC
	  GetClock_UTC(&tm);                       //��ȡUTCʱ��(������Ҫ����Ȼʱ��Ҫ����)
	
	  Step_Updata(&tm);                        //�ϴ�����
	  Change_Chatger_State();                  //�ı���״̬
		OLED_Change_Display(&tm);                //ˢ����ʾҳ��
		Low_Power_State();                    	 //����Ƿ��ǵ͵���״̬
	  Alarm_Response(&tm);                     //���Ӽ��
    Auto_Collection();                       //�Զ��ɼ����
	
#ifdef BLE_SCAN
	      Get_Beacon_State();
#endif
}

/**********************************************************************
������:ADC_timeout_handler
����:��ص�ѹADC�ɼ�����ʱ��
�������:
@p_context:��������ָ��
�������:None
����ֵ:None
˵��:
��ص�ѹ���̶�һ��ʱ��ɼ�һ�Σ�ͨ����ʱ��ʵ��
***********************************************************************/
static void ADC_timeout_handler(void * p_context)
{
     /* �ɼ�����Ѫѹʱ���� */
	   if( OLED_ctrl.page_num != HEART_P && OLED_ctrl.page_num != OXYGEN_P && OLED_ctrl.page_num != TEMPER_P)
		 {
         start_adc_convert();               //����ADCת�����
		 }
}

/**********************************************************************
������:step_10ms_handler
����:�Ʋ�10ms��ʱ��
�������:
@p_context:��������ָ��
�������:None
����ֵ:None
˵��:
�Ʋ��㷨����Ҫ10ms��ʱ������Ϊ�����㷨������ʱ��
***********************************************************************/
static void step_10ms_handler(void * p_context)
{
    StepDetector_tick++;
}

/**********************************************************************
������:step_20ms_handler
����:�Ʋ�20ms��ʱ��
�������:
@p_context:��������ָ��
�������:None
����ֵ:None
˵��:
�Ʋ��㷨����Ҫ50ms��ʱ������Ϊ�ɼ����ݻ�׼
***********************************************************************/
static void step_50ms_handler(void * p_context)
{	  
    uint32_t err_code;
	
	  /* ��ʼ��Ӳ�� */
	  SPI_Init();                     //SPI��ʼ��
    LIS3DH_Init();                  //G_Sensor��ʼ��
	
	  /* �ɼ�����,�������� */
    get_Gsenson_Value(); 
	
	  /* �ر�Ӳ�� */
    SPI_UnInit();
	
	  /* ���¿�����ʱ�� */
    err_code = app_timer_start(m_step_50_task_timer_id, STEP_50MS_TASK_TIMEOUT, NULL);
    APP_ERROR_CHECK(err_code);
}

/**********************************************************************
������:max30102_10ms_handler
����:�ɼ�max30102������
�������:
@p_context:��������ָ��
�������:None
����ֵ:None
˵��:
���10ms�ɼ����ݣ����ڼ���Ѫ��������
***********************************************************************/
static uint8_t Max30102_flag = 0;                        
//bit0:�������ɼ����        0��δ�ɼ�   1:�ڲɼ�
//bit1:�Ƿ��Ѿ���ʼ��������  0:δ��ʼ��  1:�ѳ�ʼ��
static void max30102_10ms_handler(void * p_context)
{
	  uint32_t err_code;
	
	  if( IS_SET( Max30102_flag, 1 ) == 0 )
		{
        MAX3010x_Init();  
        SET_BIT( Max30102_flag, 1 );			
		}
		else
		    Get_SensorData();    //�ɼ�����
	  
	  err_code = app_timer_start(m_max30102_task_timer_id, MAX30102_10MS_TASK_TIMEOUT, NULL);            //����ADC��ʱ��
	  APP_ERROR_CHECK(err_code);
}

/**********************************************************************
������:GapData_handler
����:���¹㲥���ݵĶ�ʱ��
�������:
@p_context:��������ָ��
�������:None
����ֵ:None
˵��:
***********************************************************************/
static void GapData_handler(void * p_context)
{	  
    if( sys_state.ble_connect_state == 0 )    //����δ����
		{
	      uint32_t err_code;
	
	      sd_ble_gap_adv_stop();   //�ع㲥
		    advertising_init();      //���¹㲥����
	
	      err_code = ble_advertising_start(BLE_ADV_MODE_FAST);    //���¹㲥
        APP_ERROR_CHECK(err_code);
		}
}

/**********************************************************************
������:timer_task_create
����:������ʱ������
�������:None
�������:None
����ֵ:None
˵��:
***********************************************************************/
void timer_task_create(void)
{
	  uint32_t err_code;
	
	  /* ����Ϊ1S�Ķ�ʱ�������ڴ���һЩ1S������ص��¼� */
    err_code = app_timer_create(&m_period_task_timer_id,     //��ʱ�����
								APP_TIMER_MODE_REPEATED,                     //�����Զ�ʱ��
								period_timeout_handler);                     //��ʱ���ص�����
	  APP_ERROR_CHECK(err_code);  

	  /* ADC���ڲɼ���ʱ�� */
    err_code = app_timer_create(&m_adc_task_timer_id,        //��ʱ�����
								APP_TIMER_MODE_SINGLE_SHOT,                  //���ζ�ʱ��
								ADC_timeout_handler);                        //��ʱ���ص�����
	  APP_ERROR_CHECK(err_code);   
	
	  /* �Ʋ��㷨10msʱ����� */
	  err_code = app_timer_create(&m_step_10_task_timer_id,    //��ʱ�����
								APP_TIMER_MODE_REPEATED,                     //�����Զ�ʱ��
								step_10ms_handler);                          //��ʱ���ص�����
	 APP_ERROR_CHECK(err_code); 
	
	 /* �Ʋ��㷨50ms�㷨��ʱ�� */
	 err_code = app_timer_create(&m_step_50_task_timer_id,     //��ʱ�����
								APP_TIMER_MODE_SINGLE_SHOT,                  //���ζ�ʱ��
								step_50ms_handler);                          //��ʱ���ص�����
	 APP_ERROR_CHECK(err_code); 
	 
	  /* ���¹㲥���ݵĶ�ʱ�� */
	 err_code = app_timer_create(&m_GapData_task_timer_id,     //��ʱ�����
								APP_TIMER_MODE_REPEATED,                     //���ڶ�ʱ��
								GapData_handler);                            //��ʱ���ص�����
	 APP_ERROR_CHECK(err_code); 
	 
	 /* max30102�ɼ����� */
	 err_code = app_timer_create(&m_max30102_task_timer_id,     //��ʱ�����
								APP_TIMER_MODE_SINGLE_SHOT,                   //���ζ�ʱ��
								max30102_10ms_handler);                       //��ʱ���ص�����
	 APP_ERROR_CHECK(err_code); 
}

/**********************************************************************
������:timers_start
����:������ʱ������
�������:None
�������:None
����ֵ:None
˵��:
***********************************************************************/
void timers_start(void)
{
	uint32_t err_code;

	err_code = app_timer_start(m_period_task_timer_id, PERIOD_TASK_TIMEOUT, NULL);      //����1S���ڶ�ʱ��
	APP_ERROR_CHECK(err_code);
	
	err_code = app_timer_start(m_adc_task_timer_id, ADC_TASK_TIMEOUT, NULL);            //����ADC��ʱ��
	APP_ERROR_CHECK(err_code);
	
	err_code = app_timer_start(m_step_10_task_timer_id, STEP_10MS_TASK_TIMEOUT, NULL);  //�����Ʋ�10msʱ�������ʱ��
	APP_ERROR_CHECK(err_code);
	
	err_code = app_timer_start(m_step_50_task_timer_id, STEP_50MS_TASK_TIMEOUT, NULL);  //�����Ʋ�50ms�㷨��ʱ��
	APP_ERROR_CHECK(err_code);
	
	err_code = app_timer_start(m_GapData_task_timer_id, GAPDATA_TASK_TIMEOUT, NULL);  //�����޸Ĺ㲥���ݶ�ʱ��
	APP_ERROR_CHECK(err_code);

}

/**********************************************************************
������:timers_start_adc_timer
����:����ADC��ʱ��
�������:None
�������:None
����ֵ:None
˵��:
***********************************************************************/
void timers_start_adc_timer(void)
{
	  uint32_t err_code;
	
    err_code = app_timer_start(m_adc_task_timer_id, ADC_TASK_TIMEOUT, NULL);            //����ADC��ʱ��
	  APP_ERROR_CHECK(err_code);    
}

/**********************************************************************
������:timers_start_Max30102_timer
����:����max30102���������ݲɼ���ʱ��
�������:None
�������:None
����ֵ:None
˵��:
***********************************************************************/
void timers_start_Max30102_timer(void)
{
	   uint32_t err_code;
	
	   if( IS_SET( Max30102_flag,0) == 0 )
		 {
			  SENSOR_ON;                                          //�򿪴�������Դ
        IIC_Init();                                           //IIC��ʼ��
	
        err_code = app_timer_start(m_max30102_task_timer_id, MAX30102_10MS_TASK_TIMEOUT, NULL); //����max30102���ݶ�ʱ��
	      APP_ERROR_CHECK(err_code); 
			 
			  SET_BIT( Max30102_flag ,0);
		 }			 
}

/**********************************************************************
������:timers_stop_Max30102_timer
����:�ر�max30102���������ݲɼ���ʱ��
�������:None
�������:None
����ֵ:None
˵��:
***********************************************************************/
void timers_stop_Max30102_timer(void)
{
	  uint32_t err_code;
	  
	  if( IS_SET( Max30102_flag,0) != 0 )
		{
        err_code = app_timer_stop(m_max30102_task_timer_id); 
	      APP_ERROR_CHECK(err_code); 
        SENSOR_OFF;
       
   			Max30102_flag = 0;			
		}			
}


