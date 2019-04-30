/*******************************************************************************************
*          ��Ȩ����, C ,2018- ,�ɶ������ֵ���Ϣ�������޹�˾
*
*����:Ƕ��ʽ������              ����:2018-9-1            �汾:V0.0
*˵��:
*    ADC�ɼ���ص�ѹ��
*�޸�:
2019-2-27:
1���޸�ADC�ĵ�ѹ�㷨�����ӵ�صȼ�����  
*******************************************************************************************/
#include <stdio.h>
#include <string.h>
#include "adc.h"
#include "nrf_drv_saadc.h"
#include "nrf_saadc.h"
#include "boards.h"
#include "app_gpio.h"
#include "timer.h"
#include "ble_salve.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#ifdef ADC_DEBUG
 #define adc_Msg    NRF_LOG_INFO
#else
 #define adc_Msg(...)
#endif

/* ����ADCͨ�� */
#define SAMPLES_IN_BUFFER        1                         //һ���ɼ��¼�
nrf_saadc_value_t    adc_buffer[2][SAMPLES_IN_BUFFER];     //˫����      
static uint8_t cache_num = 0;
static uint16_t adc_buff[10] = {0};    //����ADC�ɼ���ֵ
static uint8_t saadc_flag = 0;   //bit0:SAADC��ʼ�����   0:δ��ʼ��   1:�ѳ�ʼ��
                                 //bit1: ADC��ռ��        0:δռ��     1:ռ��
/**********************************************************************
������:ADC_Data_Dispose
����:ADC���ݴ���
�������:None
�������:None
����ֵ:None
˵��:
��ѹ������
4����4.0V-4.2V
3����3.8V-4.0V
2����3.6V-3.8V
1����3.5V-3.6V
0����3.4V-3.5V
3.4V����Ϊ�͵������رմ󲿷ֹ��ܡ�
***********************************************************************/
void ADC_Data_Dispose(void)
{
	  uint8_t i = 0,j = 0;
	  uint16_t temp = 0;
	  uint32_t Sum = 0;
	  uint16_t mean = 0;    //��ֵ
	
	  /* �����ݽ������� */
    for( i = 0; i < sizeof(adc_buff)/sizeof(adc_buff[0]); i++ )
	  {
		    for( j = 0; j < sizeof(adc_buff)/sizeof(adc_buff[0]) - 1 - i; j++ )
			  {
				    if( adc_buff[j] > adc_buff[j+1] )
						{
						    temp = adc_buff[j+1];
							  adc_buff[j+1] = adc_buff[j];
							  adc_buff[j] = temp;
						}							
				}
		}
   
		/* ȥ�����ֵ����Сֵ��ƽ��ֵ */
    for( i = 1; i < sizeof(adc_buff)/sizeof(adc_buff[0]) - 1; i++ )
		    Sum += adc_buff[i];
		
		mean = Sum/(sizeof(adc_buff)/sizeof(adc_buff[0]) - 2);
//		adc_Msg("mean = %d\r\n",mean);
		
    /* �����ѹ */
    sys_state.batty_Voltage = 0.1935 * mean;
		adc_Msg("voltage = %d.%02d\r\n",sys_state.batty_Voltage/100,sys_state.batty_Voltage%100);
//		char buff[10] = {0};
//		sprintf( buff,"V=%d.%02d", sys_state.batty_Voltage/100,sys_state.batty_Voltage%100);
//    BLE_Salve_SendData((uint8_t *)buff,strlen(buff));
		/* ���ʱֻ��ȡ��ѹֵ������ȡ���� */
		/* ��������ȼ� */	
		if(CHARGER_SATE != 0)
		{
		    if( sys_state.batty_Voltage > 400 )
		    {
		        if( sys_state.batty_grabe <4 )     //�����ǰ��صȼ�С��4,���ڼ��ĵ�صȼ�Ӧ����4��,�ͱ�������˴μ��ĵ�ѹֵ���ڵȼ���ѹ0.1V
				    {
				        if( sys_state.batty_Voltage > 405 ) 
                   sys_state.batty_grabe = 4;							
				    } 
            else	
                sys_state.batty_grabe = 4;					
		    }
		   
        else if( sys_state.batty_Voltage > 380 )
		    {
		        if( sys_state.batty_grabe <3 )     //�����ǰ��صȼ�С��3,���ڼ��ĵ�صȼ�Ӧ����3��,�ͱ�������˴μ��ĵ�ѹֵ���ڵȼ���ѹ0.1V
				    {
				    	  if( sys_state.batty_Voltage > 385 ) 
                   sys_state.batty_grabe = 3;							
				    }
				    else
				        sys_state.batty_grabe = 3;
		   }

		   else if( sys_state.batty_Voltage > 360 )
		   {
		        if( sys_state.batty_grabe <2 )     //�����ǰ��صȼ�С��2,���ڼ��ĵ�صȼ�Ӧ����2��,�ͱ�������˴μ��ĵ�ѹֵ���ڵȼ���ѹ0.1V
				    {
				    	 if( sys_state.batty_Voltage > 365 ) 
                   sys_state.batty_grabe = 2;							
				    }
				    else
				       sys_state.batty_grabe = 2;
		   }
		
		  else if( sys_state.batty_Voltage > 350 )
		  {
		       if( sys_state.batty_grabe <1 )     //�����ǰ��صȼ�С��1,���ڼ��ĵ�صȼ�Ӧ����1��,�ͱ�������˴μ��ĵ�ѹֵ���ڵȼ���ѹ0.05V
				   {
				    	 if( sys_state.batty_Voltage > 355 ) 
                   sys_state.batty_grabe = 1;							
				   }
				   else
				       sys_state.batty_grabe = 1;
		  }

		  else
		      sys_state.batty_grabe = 0;   
	  }
		
    /* �жϵ����Ƿ����(<3.4V),����������ͽ�ֻ����ʱ�����к�ADC���� */
    if( sys_state.batty_Voltage < 340 )		
		{
		    sys_state.power_low = 1;      //������
		}
		else 
		{
		    if(  sys_state.power_low == 1 )   //�����ʼ�ǵ͵���״̬,����ѹ�������3.45Vʱ�Ž���͵�ѹ״̬
        {
				    if( sys_state.batty_Voltage > 345 )
                sys_state.power_low = 0;							
				}					
		}
    	
    /* ������ݻ��� */		
		memset(adc_buff , 0, sizeof(adc_buff));
		cache_num = 0;
	  saadc_close();
		CLR_BIT( saadc_flag, 1 );
		timers_start_adc_timer();
		
		
}

/**********************************************************************
������:saadc_callback
����:ADC�ص�����
�������:
@p_event:�¼�ָ��
�������:None
����ֵ:None
˵��:
***********************************************************************/
void saadc_callback(nrf_drv_saadc_evt_t const * p_event)
{
    if(p_event->type == NRF_DRV_SAADC_EVT_DONE)
    {
        ret_code_t err_code;
        err_code = nrf_drv_saadc_buffer_convert(p_event->data.done.p_buffer,SAMPLES_IN_BUFFER);
			  APP_ERROR_CHECK(err_code);
			
//			  adc_Msg("ADC Value:%d\r\n",p_event->data.done.p_buffer[0]);
			  adc_buff[cache_num++] = p_event->data.done.p_buffer[0];    //��仺��
					  
			  /* �Բɼ��ĵ�ѹ���д��� */
			  if( cache_num == sizeof(adc_buff)/sizeof(adc_buff[0]) )    //��������������������
				{
				    ADC_Data_Dispose();

				}
				else
				{
				    APP_ERROR_CHECK(nrf_drv_saadc_sample());    //������δ����,����ת��
				}
		
    }
}

/**********************************************************************
������:Change_Chatger_State
����:��س��״̬�ĸı�
�������:None
�������:None
����ֵ:None
˵��:
***********************************************************************/
void Change_Chatger_State(void)
{
    /***************************************************** ���״̬�л� **************************************************/
    static uint8_t chatger_state_lock = 0;     //bit0:���״̬��,0:δ���������ʱ,���ͼ�궯̬��䡣  1:���������ʱ�����ͼ�겻����
		                                           //bit1:���״̬ȷ��  0:��ǰ��Ϊ���    1����ǰ�ڳ��
		                                           //bit2:���ڳ�����ʱ�����״̬һֱ�仯���˱�����ڳ��״̬��ʾ�жϡ��жϳ���Ƿ���ɣ���Ҫ��ʱ�ж�
		static uint8_t debounce_time = 0;          //���״̬������ʱ��
		//��������:���״̬�ɳ��(CHARGER_SATE = 0),�仯Ϊ�����(CHARGER_SATE = 1)����ѹ>4.1Vʱ������
		//����������:��ѹ<4.1Vʱ��������
		
		/* �����ǰ��ѹС��4.1V���������״̬�� */
		if( sys_state.batty_Voltage < 410 )
		    CLR_BIT(chatger_state_lock,0);
    
		/* ������״̬������ʾ�ڳ��,�ҳ��״̬��Ϊ0,��̬��ʾ������ */
		if( (CHARGER_SATE == 0) && (IS_SET(chatger_state_lock,0) == 0))
    {
			  debounce_time = 0;
			
				sys_state.batty_grabe++;
        if( sys_state.batty_grabe > 4 )
            sys_state.batty_grabe = 0;	

        if( IS_SET(chatger_state_lock,1) == 0 )   //��ǰ״̬Ϊδ���	    
				{
				     SET_BIT(chatger_state_lock,1);       //����ڳ��    
              Motor_Drive();
											   
						 if( OLED_ctrl.oled_state_flag == 0 ) //�����ǰ��Ļ��Ϩ���
						 {
								 /* ������Ļ��ʾ���� */
					       OLED_Display_On();
						     OLED_ctrl.oled_off_key = 1;
						     OLED_ctrl.oled_state_flag = 1;
						     OLED_ctrl.time_off = OLED_OFF_TIME;
						     OLED_ctrl.page_num = DATE_P;
						 }					
				}					
		}		

    /* �����ǰ�������״̬δ���*/
    if( CHARGER_SATE != 0 )
		{
			    if( (IS_SET(chatger_state_lock,1) != 0) && (IS_SET(chatger_state_lock,2) != 0) )   //��ǰ״̬Ϊ��磬�����жϳ�������ʱ
					{
						   debounce_time = 0;
					     CLR_BIT(chatger_state_lock,1);       //���δ��� 
						   CLR_BIT(chatger_state_lock,2); 	    //����ж���ʱ���
                Motor_Drive();
						
						   if( OLED_ctrl.oled_state_flag == 0 ) //�����ǰ��Ļ��Ϩ���
						   {
								    sys_state.batty_grabe = 4;
								    /* ������Ļ��ʾ���� */
					          OLED_Display_On();
						        OLED_ctrl.oled_off_key = 1;
						        OLED_ctrl.oled_state_flag = 1;
						        OLED_ctrl.time_off = OLED_OFF_TIME;
						        OLED_ctrl.page_num = DATE_P;
						   }											
					}	
          
          /* ��ѹ>400V ,��ʾ������� */					
			    if( sys_state.batty_Voltage > 400 )
					{
		          SET_BIT(chatger_state_lock,0);	   //�������״̬�� 
					}
				  start_adc_convert();                   //����ADCת��,��ȡ��ѹ�ȼ�	
					
					if( (++debounce_time) > 3 )            //����ʱ��3S
					    SET_BIT(chatger_state_lock,2);	   //��ʾ���жϹ�һ�γ����
       				
		}					
}

/**********************************************************************
������:saddc_init
����:��ʼ��ADC
�������:None
�������:None
����ֵ:None
˵��:
***********************************************************************/
void saddc_init(void)
{
    /* ����ADC */
	  nrf_drv_saadc_config_t saadc_config = NRF_DRV_SAADC_DEFAULT_CONFIG;
	  saadc_config.resolution = NRF_SAADC_RESOLUTION_12BIT;    //����12bitADC
    APP_ERROR_CHECK(nrf_drv_saadc_init(&saadc_config, saadc_callback));
    
	  /* ���õ��ADCͨ�� */
    nrf_saadc_channel_config_t bat_channel_config = NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(nrf_drv_saadc_gpio_to_ain(BATT_ADC));  //ʹ��Ĭ������
    bat_channel_config.gain = NRF_SAADC_GAIN1;       //ʹ���ڲ��ο���ѹ0.6V,���Դ˴�Ҫ�����뽵�ͣ�ѡ��1������
    APP_ERROR_CHECK(nrf_drv_saadc_channel_init(nrf_drv_saadc_gpio_to_ain(BATT_ADC)-1, &bat_channel_config));

	  /* ����˫������� */
	  APP_ERROR_CHECK(nrf_drv_saadc_buffer_convert(adc_buffer[0],SAMPLES_IN_BUFFER));
	  APP_ERROR_CHECK(nrf_drv_saadc_buffer_convert(adc_buffer[1],SAMPLES_IN_BUFFER));
}

/**********************************************************************
������:saadc_close
����:�ر�ADC����
�������:None
�������:None
����ֵ:None
˵��:
***********************************************************************/
void saadc_close(void)
{
	  if( IS_SET( saadc_flag, 0 ) != 0)
		{
        nrf_drv_saadc_uninit();
			  CLR_BIT( saadc_flag, 0 );
		}
}

/**********************************************************************
������:start_adc_convert
����:����ת��
�������:None
�������:None
����ֵ:None
˵��:
***********************************************************************/
void start_adc_convert(void)
{
	  if( IS_SET( saadc_flag, 0 ) == 0)
		{
	      saddc_init();
			  SET_BIT( saadc_flag, 0 );
		}
    
		if( IS_SET( saadc_flag, 1 ) == 0)
		{
		    APP_ERROR_CHECK(nrf_drv_saadc_sample()); 
        SET_BIT( saadc_flag, 1 );			
		}			
}




