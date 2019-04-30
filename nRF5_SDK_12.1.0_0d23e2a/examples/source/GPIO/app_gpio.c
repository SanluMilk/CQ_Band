/*******************************************************************************************
*          ��Ȩ����, C ,2018- ,�ɶ������ֵ���Ϣ�������޹�˾
*
*����:Ƕ��ʽ������              ����:2018-11-12            �汾:V0.0
*˵��:
*    ��ʼ��GPIO������������ܡ�
*�޸�:
2019-3-19:
    �޸Ĵ�������Դʹ�����ŵĿ��ƣ��Խ��͹��ġ�
*******************************************************************************************/
#include "app_gpio.h"
#include "timer.h"

APP_TIMER_DEF(m_Motor_Timer_id);    //���������ʱ��

/*************************************************************************
������:Motor_handler
����:���������ʱ��
�������:
@pcontext:���ݲ�����ָ��
�������:None
����ֵ:None
˵��:
*************************************************************************/
static void Motor_handler(void * pcontext)
{
    MOTOR_OFF;
	   nrf_gpio_cfg_default(MOTOR_DRIVE);
}

/**********************************************************************
������:GPIO_Init
����:GPIO��ʼ��
�������:None
�������:None
����ֵ:None
˵��:
�ú�����ʼ��GPIO�����������
***********************************************************************/
void GPIO_Init(void)
{
    /* ����GPIOΪ��� */ 
//    nrf_gpio_cfg_output(SENSOR_POWER_EN);    	

//    SENSOR_OFF;
	
    /* ����GPIOΪ���� */
    nrf_gpio_cfg_input(CHARGER_STATUS,NRF_GPIO_PIN_PULLUP);      //������������
	
	  /* ��ӵ����ʱ�� */
	  uint32_t err_code;
	
    err_code = app_timer_create(&m_Motor_Timer_id,               //��ʱ�����
							                         	APP_TIMER_MODE_SINGLE_SHOT,     //���ζ�ʱ��
								                         Motor_handler);                 //��ʱ���ص�����
	   APP_ERROR_CHECK(err_code);
    
}

/**********************************************************************
������:Motor_Drive
����:�����
�������:None
�������:None
����ֵ:None
˵��:
***********************************************************************/
void Motor_Drive(void)
{
	  nrf_gpio_cfg_output(MOTOR_DRIVE);   
    MOTOR_ON;
    app_timer_start(m_Motor_Timer_id, APP_TIMER_TICKS(300, APP_TIMER_PRESCALER), NULL);     //�ر�	
}

/**********************************************************************
������:GPIO_UnInit_All
����:�ر����е�IO��
�������:None
�������:None
����ֵ:None
˵��:
***********************************************************************/
void GPIO_UnInit_All(void)
{
    for( int i = 0; i < 32; i++ )
	      nrf_gpio_cfg_default(i);    
}
