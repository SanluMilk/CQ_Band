/*******************************************************************************************
*          ��Ȩ����, C ,2018- ,�ɶ������ֵ���Ϣ�������޹�˾
*
*����:Ƕ��ʽ������              ����:2018-9-1            �汾:V0.0
*˵��:
*    �ù��̻��ڣ�Nordic�ٷ�nRF5_SDK_12.1.0_0d23e2a.ʵ��BLE����һ��ͨ�š���
*������Ҫ����LI3DH,OLED��Ӳ����ʵ�ּƲ������ʲɼ��ȹ��ܡ�
*�޸�:
2018-11-5:
1������flash�洢���ܡ�
2019-2-26:
1��������Ҫ��������Ȼ��ʼ�����ɹ�(Ӳ������)��
2��������Ѫѹҳ������Ϊһ��ҳ�档
2019-2-27:
1���޸ĵ�ص�ѹ�ɼ����͵����ȼ��㷨��
2�����ӵ͵������򡣵���ص�����ѹ3.4Vʱ���رչ㲥���رռƲ����ر���ʾ������ʱ������С�
2019-3-5��
1���������������һЩ������Ϣ
*******************************************************************************************/
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "nordic_common.h"
#include "softdevice_handler.h"
#include "app_timer.h"
#include "app_uart.h"
#include "boards.h"
#include "app_uart_ex.h"
#include "ble.h"
#include "ble_hci.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "ble_db_discovery.h"
#include "ble_nus.h"
#include "ble_nus_c.h"
#include "app_uart_ex.h"
#include "ble_conn_state.h"
#include "fstorage.h"
#include "fds.h"
#include "nrf_drv_gpiote.h"
#include "nrf_delay.h"
#include "app_fds.h"

#include "ble_config.h"
#include "ble_master.h"
#include "ble_salve.h"
#include "timer.h"
#include "adc.h"
#include "utc.h"
#include "key.h"
#include "oled.h"
#include "stepcounter.h"
#include "oled_app.h"
#include "heat_sensor.h"
#include "app_gpio.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"

/* ��������״̬����ص���״̬�� */
sys_state_t sys_state = {
    .ble_connect_state = 0,             //��������״̬
	  .batty_grabe = 4,                   //��صȼ�
	  .power_low = 0,                     //��ǵ�ص�����
};

/**********************************************************************
������:assert_nrf_callback
����:ϵͳ��װ�ص�����
�������:
@line_num:�к�
@p_file_name:�ļ���
�������:None
����ֵ:None
˵��:
�ú�������ʵ��ʲô���ܣ�δ֪
***********************************************************************/
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(0xDEADBEEF, line_num, p_file_name);
}

/**********************************************************************
������:power_manage
����:��Դ����
�������:None
�������:None
����ֵ:None
˵��:
�ú���ͨ����Դ����ʵ��оƬ�͹�������
***********************************************************************/
static void power_manage(void)
{
    ret_code_t err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);
}

/**********************************************************************
������:FPU_IRQHandler
����:�������жϣ����fpscr
�������:None
�������:None
����ֵ:None
˵��:
***********************************************************************/
#define FPU_EXCEPTION_MASK 0x0000009F
void FPU_IRQHandler(void)
{
    uint32_t *fpscr = (uint32_t *)(FPU->FPCAR+0x40);
    (void)__get_FPSCR();

    *fpscr = *fpscr & ~(FPU_EXCEPTION_MASK);
}

/**********************************************************************
������:main
����:������������������ڣ��Ը�����ģ����г�ʼ��
�������:None
�������:None
����ֵ:None
˵��:
***********************************************************************/
int main(void)
{
    uint32_t err_code;
    /****************utc test*******************************/
    UTCTime_t tm;
    tm.year   = 2019;
    tm.month  = 2-1;
    tm.day    = 26-1;
    tm.hour   = 11;
    tm.minute = 8;
    tm.second = 20;
    /*******************************************************/
	
    err_code = NRF_LOG_INIT(NULL);  //��ʼ����־����
    APP_ERROR_CHECK(err_code);
		
	  /* ���ø������ж� */
	  NVIC_SetPriority(FPU_IRQn, APP_IRQ_PRIORITY_LOW);
    NVIC_EnableIRQ(FPU_IRQn);
	
	  timers_init();                  //��ʼ��ʱ��
    ble_stack_init();               //��ʼ��Э��ջ
    gap_params_init();              //��ʼ��GAP����
    conn_params_init();             //��ʼ�����Ӳ���
    services_init();                //��ʼ������
    advertising_init();             //��ʼ���㲥	
   

    buttons_init();                 //��ʼ������
//    Flash_Init();                   //flash��ʼ��
	  GPIO_Init();                    //GPIO��ʼ��
    timer_task_create();            //������ʱ������
		Heat_sensor_Init();             //��ʼ�����ʴ������ɼ�
    SetClock_UTC(&tm);              //����UTCʱ��
   
		adv_scan_start();               //�����㲥
		timers_start();                 //������ʱ��	

		Motor_Drive();
		
		for (;;)
    {
        power_manage();
    }
}
      
