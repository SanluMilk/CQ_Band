#ifndef __OLED_H__
#define __OLED_H__

#include "boards.h"

#define Max_Column	64
#define Max_Row		48

typedef enum{
    OLED_CMD  = 0,
    OLED_DATA = 1,
}OLED_Operation_t;



#define     SCLK_Set()        nrf_gpio_pin_set(OLED_D0)
#define     SCLK_Reset()      nrf_gpio_pin_clear(OLED_D0)

#define     SDIN_Set()        nrf_gpio_pin_set(OLED_D1)
#define     SDIN_Reset()      nrf_gpio_pin_clear(OLED_D1)

#define     RES_Set()         nrf_gpio_pin_set(OLED_RES)
#define     RES_Reset()       nrf_gpio_pin_clear(OLED_RES)

#define     DC_Set()          nrf_gpio_pin_set(OLED_DC)
#define     DC_Reset()        nrf_gpio_pin_clear(OLED_DC)

#define     CS_Set()          nrf_gpio_pin_set(OLED_CS)
#define     CS_Reset()        nrf_gpio_pin_clear(OLED_CS)

/**********************************************************************
������:OLED_Refresh_Gram
����:ˢ��OLED����ʾ
�������:None
�������:None
����ֵ:None
˵��:
***********************************************************************/
void OLED_Refresh_Gram(void);

/**********************************************************************
������:OLED_Display_On
����:����OLED��ʾ
�������:None
�������:None
����ֵ:None
˵��:
***********************************************************************/
void OLED_Display_On(void);

/**********************************************************************
������:OLED_Display_Off
����:�ر�OLED��ʾ
�������:None
�������:None
����ֵ:None
˵��:
***********************************************************************/    
void OLED_Display_Off(void);

/**********************************************************************
������:OLED_Clear
����:�����ʾ
�������:None
�������:None
����ֵ:None
˵��:
��������,������,������Ļ�Ǻ�ɫ��!
***********************************************************************/
void OLED_Clear(void);

/**********************************************************************
������:OLED_Set_Pos
����:����һ����
�������:
@x:��ʾ��X��
@y:��ʾ��Y��
�������:None
����ֵ:None
˵��:
***********************************************************************/
static void OLED_Set_Pos(uint8_t x,uint8_t y);

/**********************************************************************
������:OLED_DrawLine
����:��ʾһ����
�������:
@x1:��ʾ��X�����
@y1:��ʾ��Y�����
@x2:��ʾ��X���յ�
@y2:��ʾ��Y���յ�
�������:None
����ֵ:None
˵��:
***********************************************************************/
void OLED_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

/**********************************************************************
������:OLED_ShowChar
����:��ʾһ���ַ�
�������:
@x:��ʾ��X��
@y:��ʾ��Y��
@chr:�ַ�
@size:��ʾ�����С16/12 
@mode:0,������ʾ;1,������ʾ
�������:None
����ֵ:None
˵��:
***********************************************************************/
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t size, uint8_t mode);

/**********************************************************************
������:OLED_ShowNum
����:��ʾ����
�������:
@x:��ʾ��X��
@y:��ʾ��Y��
@num:����
@len:���ֳ���
@size:�����С
�������:None
����ֵ:None
˵��:
num:��ֵ(0~4294967295);	 
***********************************************************************/
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size);

/**********************************************************************
������:OLED_ShowString
����:��ʾ�ַ���
�������:
@x:��ʾ��X��
@y:��ʾ��Y��
@p:�ַ���
@len:���ֳ���
@size:�����С
@mode:0,������ʾ;1,������ʾ
�������:None
����ֵ:None
˵��: 
***********************************************************************/
#define MAX_CHAR_POSX 64
#define MAX_CHAR_POSY 48 
void OLED_ShowString(uint8_t x, uint8_t y, const char *p, uint8_t size, uint8_t mode);

/**********************************************************************
������:OLED_ShowCHinese
����:��ʾ����
�������:
@x:��ʾ��X��
@y:��ʾ��Y��
@no:�ַ���
@size:�����С
@mode:0,������ʾ;1,������ʾ
�������:None
����ֵ:None
˵��: 
***********************************************************************/
void OLED_ShowCHinese(uint8_t x, uint8_t y, uint8_t no, uint8_t size, uint8_t mode);

/**********************************************************************
������:OLED_DrawBMP
����:��ʾ��ʾBMPͼƬ
�������:
@x:����ͼƬ����ʼ��x����
@y:����ͼƬ����ʼ��y����
@x_1:ͼƬ�Ľ�������x��
@y_1:ͼƬ�Ľ�������y��
@BMP:ͼƬ����
@dataLen:ͼƬ���ݳ���
@mode:��ʾģʽ
�������:None
����ֵ:None
˵��: 
��дx,y��x_1,y_1ʱ������׼ȷ����ȻͼƬ��ʾ���׳�����
***********************************************************************/
void OLED_DrawBMP(uint8_t x, uint8_t y,uint8_t x_1, uint8_t y_1, const unsigned char *BMP, uint16_t dataLen, uint8_t mode);

/**********************************************************************
������:OLED_Init
����:��ʼ��OLED
�������:None
�������:None
����ֵ:None
˵��: 
***********************************************************************/
void OLED_Init(void);

















#endif

