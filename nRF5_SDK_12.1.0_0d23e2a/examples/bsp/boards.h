/* Copyright (c) 2014 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */
#ifndef BOARDS_H
#define BOARDS_H

#include "nrf_gpio.h"

#if defined(BOARD_NRF6310)
  #include "nrf6310.h"
#elif defined(BOARD_PCA10000)
  #include "pca10000.h"
#elif defined(BOARD_PCA10001)
  #include "pca10001.h"
#elif defined(BOARD_PCA10002)
  #include "pca10000.h"
#elif defined(BOARD_PCA10003)
  #include "pca10003.h"
#elif defined(BOARD_PCA20006)
  #include "pca20006.h"
#elif defined(BOARD_PCA10028)
  #include "pca10028.h"
#elif defined(BOARD_PCA10031)
  #include "pca10031.h"
#elif defined(BOARD_PCA10036)
  #include "pca10036.h"
#elif defined(BOARD_PCA10040)
  #include "pca10040.h"
#elif defined(BOARD_CQXD_NETWORK)
  #include "hard_config.h"
#else
#error "Board is not defined"

#endif

#define PRODUCT_NUMBER       0X01  //��Ʒ����
#define HARDWARE_VERSION     2     //Ӳ���汾
#define SOFTWARE_VERSION     8     //����汾,BLE4.1
/*
����汾˵��:
0 : 2019-3-19���������������ɣ�������flash�洢�����Ļ��ܼ����Ż����˰汾����Ϊ���Գ��򡣲�������������
1 : 2019-3-20�����������ɣ��޸�һ��BUG��
        1.�޸������㷨���ɼ��������֮ǰ�ȶ����١�
		    2.�ڲɼ����ʱ�����𶯹��ܡ�
2 : 2019-3-21������޸�BUG.
        1.�޸��������㹫ʽ��
3 : 2019-3-25����������������ӣ��Ͽ���ͼ��
4 : 2019-3-26����������ű�ɨ��ʹ�á�
5 : 2019-3-29���������Beacon���ܡ�
6 : 2019-4-3����������������롣
7 : 2019-4-4������޸�Bug.������Ӧ�󡣲�����֮ǰ��ҳ�档
8 : 2019-4-23������޸��ֻ��ɼ�ʱ���ʳ���̫��������
*/

/* ��ǰϵͳ״̬ */
typedef struct{
#ifdef BLE_SCAN
	  uint8_t Beacon_scan_state:1;    //�ű�ɨ��״̬  0:û���ű�   1���ű�
#endif
	  uint8_t ble_connect_state:1;    //��������״̬  0:����δ����   1:����������
	  uint8_t batty_grabe:3;
	  uint8_t power_low:1;            //��ص�����  0:��������    1:������
	  uint8_t bool_preeeure_valid:1;  //Ѫѹ������Ч���  0:��Ч    1:��Ч
	  uint8_t bool_preeeure_flag:1;   //Ѫѹ������ʾ      0:����ʾ   1:��ʾ
	  volatile uint16_t batty_Voltage;
}sys_state_t;


#define SAVE_FLAG     0x55         //���ڴ���ڴ洢����У�ÿ�ο����������жϸ�ֵ��
                                   //�����ȱ�ʾ�洢�Ĳ�����Ч���������ȱ�ʾ�洢�Ĳ�����Ч��������Ĭ�ϲ�����
/* ��Ҫ����Ĳ��� */
typedef struct{
	  uint8_t save_flag;               //�洢���  
    uint8_t sex;                   //�Ա� 0:��    1:Ů 
    uint8_t height;                //���(cm)
    uint8_t	weight;                //����(kg)
	  uint8_t age;                   //����
	  uint8_t alarm[3];              // alarm[0]-->ʱ    alarm-->��    alarm-->��
	  uint16_t Collect_interval;     //�Զ��ɼ����
#ifdef BLE_SCAN
	  uint8_t OpenBeaconFlag;        //��Beacon���   0:δ��   1:��
	  uint8_t Beacon_Mac[6];         //�󶨵�BeaconMAC��ַ,����1����
#endif
}save_param_t;

extern save_param_t save_param;
extern sys_state_t sys_state;
#endif
