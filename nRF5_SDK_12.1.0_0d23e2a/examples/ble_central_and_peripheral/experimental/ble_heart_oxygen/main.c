/*******************************************************************************************
*          ��Ȩ����, C ,2019- ,�ɶ������ֵ���Ϣ�������޹�˾
*
*����:Ƕ��ʽ������              ����:2019-3-7            �汾:V1.0
*˵��:
   1.�ù��̻��ڣ�Nordic�ٷ�nRF5_SDK_12.1.0_0d23e2a.ʵ��BLE����һ��ͨ�š���
������Ҫ����LI3DH,OLED��Ӳ����ʵ�ּƲ���ʹ��MAX30102ʵ������Ѫ���Ĳɼ��ȹ��ܡ�
   2.�ù��̻���Ѫѹ�ɼ��ֻ����޸Ĺ�����
   3.��������ɵĹ�����ʵ����ʾʱ�䣬�ɼ����ʡ�Ѫ�����¶ȵȣ��ɼ��������������ӵȹ��ܡ�
*�޸�:
2019-3-12:
    �������߾��룬������Ѫ��Ũ�ȣ����ʣ��¶ȣ����ӵȽ��档
2019-3-14:
    ����DFU(�̼���������)��
2019-3-18:
    �������ݴ洢���ܣ���һЩ�û�����д��оƬ�ڲ�Flash�С�
2019-3-19:
    1.Flash�洢����DFU�г�ͻ�����²��ܹ㲥��������ʱ��ʹ��Flash��
		2.�޸ô�����ʹ�ܿ��Ƶ�IO�ڣ����͹��ġ�˯�߹���Ϊ(150us-300us�䲨��)�������㲥��
		3.�ر�NRF_LOG���ܡ�
		4.ʹ��bootloader�Ĺ��ıȲ�ʹ��bootloader���ĸ�100uA���ҡ�
2019-3-20:
    1.�޸������㷨���ɼ��������֮ǰ�ȶ����١�
		2.�ڲɼ����ʱ�����𶯹��ܡ�
2019-3-25:
    1.�����������ӺͶϿ����档
2019-3-26:
    1.����ɨ��Beacon���ܡ�
2019-3-27:
    1.����Beaconɨ��
		2.�޸���ʾ�����߼�
2019-3-29:
    1.�޸������߼��ϵ�BUG��
		2.�����������롣
		3.�����˿���Beaconɨ��������롣
2019-4-1:
    1.���ӻ�ȡBeacon�Ƿ�ɨ������
2019-4-5:
    1.�޸�Bug:������Ӧ�󡣲�����֮ǰ��ҳ�档
2019-4-23:
		1.�޸��ֻ��ɼ�ʱ���ʳ���̫��������
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
#include "ble_conn_state.h"
#include "fstorage.h"
#include "fds.h"
#include "nrf_drv_gpiote.h"
#include "nrf_delay.h"
#include "app_fds.h"
#include "ble_config.h"
#include "ble_salve.h"
#include "ble_master.h"
#include "timer.h"
#include "adc.h"
#include "utc.h"
#include "key.h"
#include "oled.h"
#include "stepcounter.h"
#include "oled_app.h"
#include "app_gpio.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "MAX3010x.h"

//#define FLASH        //��Ҫ���籣������

/* ��Ҫ����Ĳ��� */
save_param_t save_param = {
	  .save_flag = SAVE_FLAG,
    .sex = 1,
	  .height = 170,
	  .weight = 65,
	  .age = 25,
#ifdef BLE_SCAN
	  .OpenBeaconFlag = 0,
	  .Beacon_Mac = {0x04, 0xED, 0x68, 0x69, 0x20, 0x68},
#endif
};


/* ��������״̬����ص���״̬�� */
sys_state_t sys_state = {
#ifdef BLE_SCAN
	  .Beacon_scan_state = 0,
#endif
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

#ifdef FLASH
/**********************************************************************
������:cheack_param
����:У׼ϵͳ��Ҫ����Ĳ���
�������:None
�������:None
����ֵ:None
˵��:
***********************************************************************/
#define SAVE_PARAM_ADDR       0     //�洢�����ĵ�ַ
static void cheack_param(void)
{
	  save_param_t temp_param;
	  uint32_t err_code;
	
	  memset( &temp_param, 0, sizeof( temp_param ) );
    err_code = Flash_Read( SAVE_PARAM_ADDR, (uint32_t *)&temp_param , sizeof( temp_param )/4+1  );   //��ȡ����Ĳ���
    APP_ERROR_CHECK(err_code);
	
	  NRF_LOG_INFO("save_flag = 0x%02X\r\n",temp_param.save_flag);
	  if( temp_param.save_flag == SAVE_FLAG )     //���������Ч
		{
		    memcpy( &save_param, &temp_param, sizeof( temp_param ) );
			  NRF_LOG_INFO("System param Read Success!\r\n");
			
			  NRF_LOG_INFO("height = %d\r\n",save_param.height);
			  NRF_LOG_INFO("weight = %d\r\n",save_param.weight);
			  NRF_LOG_INFO("age = %d\r\n",save_param.age);
			  NRF_LOG_INFO("Collect_interval = %d\r\n",save_param.Collect_interval);
			  NRF_LOG_INFO("Alarm time =%d:%d:%d\r\n",save_param.alarm[0],save_param.alarm[1],save_param.alarm[2]);
 
		}
    else     //������Ч����ʹ��Ĭ�ϲ�������д�뵽��flash
		{
		    err_code = Flash_Write(SAVE_PARAM_ADDR , (uint32_t *)&save_param , sizeof( save_param )/4+1); //д�����
			  APP_ERROR_CHECK(err_code);
			  NRF_LOG_INFO("System param Read Fail!,Write param\r\n");
		}			
}

#endif

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
	
	  NRF_POWER->DCDCEN = 1;      //ʹ��DCDCת��
	
    err_code = NRF_LOG_INIT(NULL);  //��ʼ����־����
    APP_ERROR_CHECK(err_code);
	
	  NRF_LOG_INFO("Device Type:0x%02X,HardWare:%d,SoftWare:%d\r\n",PRODUCT_NUMBER,HARDWARE_VERSION,SOFTWARE_VERSION);
	
	
#ifdef FLASH	
	  Flash_Init();                                       //flash��ʼ��
		cheack_param();                                     //У�����
#endif		
	  timers_init();                                      //��ʼ��ʱ��
		
		  /* ���ø������ж�,��Ȼ���Ļ�Ƚϸ� */
	  NVIC_SetPriority(FPU_IRQn, APP_IRQ_PRIORITY_LOW);
    NVIC_EnableIRQ(FPU_IRQn);
		GPIO_UnInit_All();                                  //�ر����е�IO��
		
	
    ble_stack_init();                                   //��ʼ��Э��ջ
    gap_params_init();                                  //��ʼ��GAP����
    conn_params_init();                                 //��ʼ�����Ӳ���
    services_init();                                    //��ʼ������
    advertising_init();                                 //��ʼ���㲥	

#ifdef BLE_SCAN
	  db_discovery_init();
    nus_c_init();                                       //��ʼ���ͻ���
#endif

    buttons_init();                                     //��ʼ������
	  GPIO_Init();                                        //GPIO��ʼ��
    timer_task_create();                                //������ʱ������
    SetClock_UTC(&tm);                                  //����UTCʱ��

		adv_scan_start();                                   //�����㲥
		timers_start();                                     //������ʱ��	

		Motor_Drive();
		
		for (;;)
    {
        power_manage();
    }
}
      
