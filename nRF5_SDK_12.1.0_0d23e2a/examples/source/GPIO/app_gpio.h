/*******************************************************************************************
*          ��Ȩ����, C ,2018- ,�ɶ������ֵ���Ϣ�������޹�˾
*
*����:Ƕ��ʽ������              ����:2018-11-12            �汾:V0.0
*˵��:
*    ��ʼ��GPIO������������ܡ�
*�޸�:
*******************************************************************************************/

#ifndef APP_GPIO_H__
#define APP_GPIO_H__

#include "boards.h"
#include "nrf_gpio.h"
#include "IIC.h"

/* ������� */
#define MOTOR_ON          {nrf_gpio_pin_set(MOTOR_DRIVE);}
#define MOTOR_OFF         {nrf_gpio_pin_clear(MOTOR_DRIVE);}

/* ������ʹ�� */
#define SENSOR_ON         {nrf_gpio_cfg_output(SENSOR_POWER_EN); nrf_gpio_pin_set(SENSOR_POWER_EN);}
#define SENSOR_OFF        {nrf_gpio_pin_clear(SENSOR_POWER_EN); nrf_gpio_cfg_default(SENSOR_POWER_EN); IIC_unInit();}

/* ���״̬ */
#define CHARGER_SATE      nrf_gpio_pin_read(CHARGER_STATUS)
/**********************************************************************
������:GPIO_Init
����:GPIO��ʼ��
�������:None
�������:None
����ֵ:None
˵��:
�ú�����ʼ��GPIO�����������
***********************************************************************/
void GPIO_Init(void);

/**********************************************************************
������:Motor_Drive
����:�����
�������:None
�������:None
����ֵ:None
˵��:
***********************************************************************/
void Motor_Drive(void);

/**********************************************************************
������:GPIO_UnInit_All
����:�ر����е�IO��
�������:None
�������:None
����ֵ:None
˵��:
***********************************************************************/
void GPIO_UnInit_All(void);





#endif
