/*******************************************************************************************
*          ��Ȩ����, C ,2018- ,�ɶ������ֵ���Ϣ�������޹�˾
*
*����:Ƕ��ʽ������              ����:2018-9-1            �汾:V0.0
*˵��:
*    OLED��������ʵ��OLED�㡢�ߡ�ͼ���ַ�����ʾ��
*�޸�:
*    2018-11-6:������Ļ������ʾ��
*******************************************************************************************/
#include "oled.h"
#include "stdlib.h"
#include "oledfont.h"  	 
#include <stdio.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_error.h"
#include "nrf_delay.h"

#define REVERSE    1                                //�Ƿ�������ʾ
static uint8_t OLED_GRAM[Max_Column][Max_Row/8];	  //�Դ�

/**********************************************************************
������:OLED_WR_Byte
����:��OLEDд��һ���ֽ�
�������:
@Dat:��Ҫд�������
@Cmd:��Ҫд��ķ�ʽ
�������:None
����ֵ:None
˵��:
cmd:����/�����־ 0,��ʾ����;1,��ʾ����;
***********************************************************************/
static void OLED_WR_Byte(uint8_t Dat, OLED_Operation_t Cmd)
{	
    uint8_t i;
			  
    if(Cmd)
        DC_Set();
    else 
        DC_Reset();		  
	
    CS_Reset();

    /* �������� �������ط������� */
    for(i = 0; i < 8; i++)
    {			  
        SCLK_Reset();
        if(Dat & 0x80)
            SDIN_Set();
        else 
            SDIN_Reset();
		
        SCLK_Set();
		      Dat <<= 1;   
	   }
			 		  
    CS_Set();
    CS_Reset();   	  
} 

#if REVERSE
/**********************************************************************
������:OLED_Reverse
����:���Դ��е����ݷ�ת
�������:None
�������:None
����ֵ:None
˵��:
***********************************************************************/
static void OLED_Reverse(void)
{
    uint8_t OLED_Ram[Max_Column][Max_Row/8];
    uint8_t i = 0, j = 0;
    union{
        uint8_t tmp;
        struct t{
            uint8_t bit0:1;
            uint8_t bit1:1;
            uint8_t bit2:1;
            uint8_t bit3:1;
            uint8_t bit4:1;
            uint8_t bit5:1;
            uint8_t bit6:1;
            uint8_t bit7:1;
        }t1;
    }a;

    union{
        uint8_t tmp;
        struct T{
            uint8_t bit0:1;
            uint8_t bit1:1;
            uint8_t bit2:1;
            uint8_t bit3:1;
            uint8_t bit4:1;
            uint8_t bit5:1;
            uint8_t bit6:1;
            uint8_t bit7:1;
        }t1;
    }b;
    
    /* ��0 ��ʱ���� */
    for(i = 0; i < Max_Column; i++)
    {
        memset(OLED_Ram[i],0,Max_Row/8);
    }

    /* ���淭ת */
    for(i = 0; i < Max_Row/8; i++)
    {
        for(j = 0; j < Max_Column; j++)
        {
            OLED_Ram[j][i] = OLED_GRAM[Max_Column-j-1][Max_Row/8-i-1];
            a.tmp =  OLED_Ram[j][i];
            b.t1.bit0 = a.t1.bit7;
            b.t1.bit1 = a.t1.bit6;
            b.t1.bit2 = a.t1.bit5;
            b.t1.bit3 = a.t1.bit4;
            b.t1.bit4 = a.t1.bit3;
            b.t1.bit5 = a.t1.bit2;
            b.t1.bit6 = a.t1.bit1;
            b.t1.bit7 = a.t1.bit0;
            OLED_Ram[j][i] = b.tmp;
        }
    }

    /* ���»��� */
    for(i = 0; i < Max_Column; i++)
    {
        memcpy(OLED_GRAM[i],OLED_Ram[i],Max_Row/8);
    }
}

#endif

/**********************************************************************
������:OLED_Refresh_Gram
����:ˢ��OLED����ʾ
�������:None
�������:None
����ֵ:None
˵��:
***********************************************************************/
void OLED_Refresh_Gram(void)
{
    uint8_t i,n;	
    
#if REVERSE
    OLED_Reverse();
#endif
    for(i = 0; i < (Max_Row/8); i++)  
	   {  
		    OLED_WR_Byte (0xb2+i,OLED_CMD);    //����ҳ��ַ��0~7��
		    OLED_WR_Byte (0x00,OLED_CMD);      //������ʾλ�á��е͵�ַ
		    OLED_WR_Byte (0x12,OLED_CMD);      //������ʾλ�á��иߵ�ַ   

        /* ����Ϊ��λ��� */
        for(n = 0; n < Max_Column; n++)
        {
             OLED_WR_Byte(OLED_GRAM[n][i],OLED_DATA); 
        }
	   }   
}
 	  
/**********************************************************************
������:OLED_Display_On
����:����OLED��ʾ
�������:None
�������:None
����ֵ:None
˵��:
***********************************************************************/
void OLED_Display_On(void)
{
    OLED_Init();                    //OLED��ʼ��
	
    OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
    OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
    OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}

/**********************************************************************
������:OLED_Display_Off
����:�ر�OLED��ʾ
�������:None
�������:None
����ֵ:None
˵��:
***********************************************************************/    
void OLED_Display_Off(void)
{
    OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
    OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
    OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
	
	  nrf_gpio_cfg_default(OLED_D0);
    nrf_gpio_cfg_default(OLED_D1);
    nrf_gpio_cfg_default(OLED_RES);
    nrf_gpio_cfg_default(OLED_DC);
    nrf_gpio_cfg_default(OLED_CS);
}

/**********************************************************************
������:OLED_Clear
����:�����ʾ
�������:None
�������:None
����ֵ:None
˵��:
��������,������,������Ļ�Ǻ�ɫ��!
***********************************************************************/
void OLED_Clear(void)  
{  
#if 0	  //ֱ������ 
    uint8_t i,n;	
  
    for(i = 0; i < (Max_Row/8); i++)  
    {  
        OLED_WR_Byte (0xb2 + i, OLED_CMD);    //����ҳ��ַ��0~7��
        OLED_WR_Byte (0x00, OLED_CMD);      //������ʾλ�á��е͵�ַ
        OLED_WR_Byte (0x12, OLED_CMD);      //������ʾλ�á��иߵ�ַ   
		
        for(n = 0; n < Max_Column; n++)
            OLED_WR_Byte(0, OLED_DATA); 

	   } //������ʾ
#else   //�建��
    memset(OLED_GRAM,0,sizeof(OLED_GRAM));
#endif

}

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
static void OLED_Set_Pos(uint8_t x,uint8_t y) 
{ 
    OLED_WR_Byte(0xb2 + y,OLED_CMD);
    OLED_WR_Byte((((x + 0x20) & 0xf0) >> 4) | 0x10, OLED_CMD);
    OLED_WR_Byte(( x & 0x0f ) | 0x01, OLED_CMD);  
}

/**********************************************************************
������:OLED_ShowDot
����:��ʾһ����
�������:
@x:��ʾ��X��
@y:��ʾ��Y��
@mode:0,������ʾ;1,������ʾ
�������:None
����ֵ:None
˵��:
***********************************************************************/
static void OLED_ShowDot(uint8_t x,uint8_t y,uint8_t mode)
{
    uint8_t pos,bx,temp=0;
	
    if((x > (Max_Column - 1)) || ( y > (Max_Row - 1)))
		      return;//������Χ��.
	
    pos = y / 8;
	   bx = y % 8;
	   temp = (1 << bx);
	
	   if(mode)
		      OLED_GRAM[x][pos] |= temp;
	   else 
		      OLED_GRAM[x][pos] &= ~temp;	      
}

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
void OLED_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    uint16_t t; 
    int xerr = 0, yerr = 0;
    int delta_x, delta_y, distance; 
    int incx, incy, uRow, uCol; 

	   delta_x = (x2 - x1); //������������ 
	   delta_y = (y2 - y1); 
	   uRow = x1; 
	   uCol = y1; 
	
    if( delta_x > 0 )
        incx = 1;      //���õ������� 
	   else if( delta_x == 0 )
        incx = 0;      //��ֱ�� 
	   else 
    {
         incx = -1;
         delta_x = -delta_x;
    } 
	
    if( delta_y > 0 )
       incy = 1; 
	   else if(delta_y == 0)
       incy = 0;//ˮƽ�� 
	   else
    {
       incy = -1;
       delta_y = -delta_y;
    } 
	
    if( delta_x > delta_y)
       distance = delta_x; //ѡȡ�������������� 
	   else
       distance = delta_y; 
      
    for(t = 0; t <= (distance+1); t++ )//������� 
	   {  
		      OLED_ShowDot(uRow,uCol,1);//���� 
		
        xerr += delta_x ; 
		      yerr += delta_y ; 
		
        if(xerr > distance) 
		      { 
			         xerr -= distance; 
			         uRow += incx; 
		      } 
		
        if(yerr > distance) 
		      { 
		          yerr -= distance; 
			         uCol += incy; 
		      } 
	   } 
//    OLED_Refresh_Gram();//������ʾ
}

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
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t size, uint8_t mode)
{      	
    uint8_t temp,t,t1;
	   uint8_t y0 = y;
	
	   chr = chr-' ';//�õ�ƫ�ƺ��ֵ			
	
    for(t = 0; t < size; t++)
    {   
		      if(size == 12)
            temp = asc2_1206[chr][t];  //����1206����
		      else if(size == 24)
            temp = asc2_2416[chr][t];  //����2412���� 
        else if(size == 32)
			         temp = asc2_3224[chr][t];  //����3224���� 
        else
			         temp = asc2_1608[chr][t];  //����1608���� 
        
        for(t1 = 0; t1 < 8; t1++)
		      {
            if( temp & 0x80 )
                OLED_ShowDot(x,y,mode);
            else 
                OLED_ShowDot(x,y,!mode);
			
            temp <<= 1;
            y++;
            
			         if((y-y0) == size)
            {
                y = y0;
                x++;
                break;
            }
        }  	 
    }          
//    OLED_Refresh_Gram();//������ʾ	
}

/**********************************************************************
������:oled_pow
����:m^n����
�������:
@m:
@n:
�������:None
����ֵ:None
˵��:
***********************************************************************/
static uint32_t oled_pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;	 
	
    while(n--)
        result *= m;    
	
     return result;
}
				  
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
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size)
{         	
    uint8_t t,temp;
    uint8_t enshow = 0;
						   
    for(t = 0; t < len; t++)
    {
        temp = (num/oled_pow(10,len-t-1))%10;
        if( (enshow == 0) && (t < (len-1)) )
        {
            if(temp == 0)
            {
                OLED_ShowChar(x+(size/2)*t,y,' ',size,1);
                continue;
			         }
            else 
                enshow = 1; 
		      }
	 	     OLED_ShowChar(x+(size/2)*t,y,temp+'0',size,1); 
	   }
}

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
void OLED_ShowString(uint8_t x, uint8_t y, const char *p, uint8_t size, uint8_t mode)
{         
    while( *p!='\0' )
    {       
        if(x > MAX_CHAR_POSX)
        {
             x = 0;
             y += 16;
         }
        if(y > MAX_CHAR_POSY)
        {
             y = x = 0;
             OLED_Clear();
         }
        OLED_ShowChar(x,y,*p,size,mode);
		      if(size == 12)
		          x += 6;
		      else if(size == 32)
		          x += 10;
		      else
		          x += 8;	
       
        p++;
    }  
}

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
void OLED_ShowCHinese(uint8_t x, uint8_t y, uint8_t no, uint8_t size, uint8_t mode)
{    
    uint8_t temp,t,t1;
	   uint8_t y0 = y;  			    
	
    switch( size )
    {
        case 16:
       {
           for (t = 0; t < (2*size); t++)
          {
               temp = Hzk16_16[no][t];
               
               for(t1 = 0; t1 < 8; t1++)
               {
                   if(temp&0x80)
                       OLED_ShowDot(x,y,mode);
			                else 
                       OLED_ShowDot(x,y,!mode);
                   
                   temp <<= 1;
               
				               y++;
				               if ((y-y0) == size)
				               {
					                  y = y0;
					                  x++;
					                  break;
				               }
               }  
           }
       }
       break;
       
       case 12:
       {
          
          for (t = 0; t < 2*size; t++)
          {
               temp = Hzk12_12[no][t];
               
               for(t1 = 0; t1 < 8; t1++)
               {
                   if(temp&0x80)
                       OLED_ShowDot(x,y,mode);
			                else 
                       OLED_ShowDot(x,y,!mode);
                   
                   temp<<=1;
               
				               y++;
				               if ((y-y0) == size)
				               {
					                  y = y0;
					                  x++;
					                  break;
				               }
               }
          }
        
       }
       break;

       default:
       break;
    }
   
//    OLED_Refresh_Gram();//������ʾ          				
}

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
void OLED_DrawBMP(uint8_t x, uint8_t y,uint8_t x_1, uint8_t y_1, const unsigned char *BMP, uint16_t dataLen, uint8_t mode)
{

     uint8_t temp,t,t1;
	    uint8_t y0 = y;

	    for(t = 0; t < dataLen; t++ )
	    {
	        temp = BMP[t];
	        for(t1 = 0; t1 < 8; t1++)
		      {
				      if( temp & 0x80 )
	                OLED_ShowDot(x,y,mode);
				      else 
	                OLED_ShowDot(x,y,!mode);
				
	            temp <<= 1;
				      y++;
	            
				      if(y == y_1)
				      {
					         y = y0;
					         x++;
					         break;
				      }
		       }  
	    }
//	 OLED_Refresh_Gram();
} 

/**********************************************************************
������:OLED_Init
����:��ʼ��OLED
�������:None
�������:None
����ֵ:None
˵��: 
***********************************************************************/
void OLED_Init(void)
{ 	
    nrf_gpio_cfg_output(OLED_D0);
    nrf_gpio_cfg_output(OLED_D1);
    nrf_gpio_cfg_output(OLED_RES);
    nrf_gpio_cfg_output(OLED_DC);
    nrf_gpio_cfg_output(OLED_CS);
    
    RES_Set();
	  nrf_delay_ms(100);
	  RES_Reset();
	  nrf_delay_ms(100);
	  RES_Set(); 
	
    OLED_WR_Byte(0xAE,OLED_CMD);//--turn off oled panel
	  OLED_WR_Byte(0x00,OLED_CMD);//---set low column address
	  OLED_WR_Byte(0x12,OLED_CMD);//---set high column address
	  OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	  OLED_WR_Byte(0x81,OLED_CMD);//--set contrast control register
	  OLED_WR_Byte(0xCF,OLED_CMD);// Set SEG Output Current Brightness
	  OLED_WR_Byte(0xA1,OLED_CMD);//--Set SEG/Column Mapping     0xa0�����෴       0xa1����
	  OLED_WR_Byte(0xC8,OLED_CMD);//Set COM/Row Scan Direction   0xc0�����෴       0xc8����
	  OLED_WR_Byte(0xA6,OLED_CMD);//--set normal display
		OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
	  OLED_WR_Byte(0x3f,OLED_CMD);//--1/64 duty
	  OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	  OLED_WR_Byte(0x00,OLED_CMD);//-not offset
	  OLED_WR_Byte(0xd5,OLED_CMD);//--set display clock divide ratio/oscillator frequency
	  OLED_WR_Byte(0x80,OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
	  OLED_WR_Byte(0xD9,OLED_CMD);//--set pre-charge period
	  OLED_WR_Byte(0xF1,OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	  OLED_WR_Byte(0xDA,OLED_CMD);//--set com pins hardware configuration
	  OLED_WR_Byte(0x12,OLED_CMD);
	  OLED_WR_Byte(0xDB,OLED_CMD);//--set vcomh
	  OLED_WR_Byte(0x40,OLED_CMD);//Set VCOM Deselect Level
	  OLED_WR_Byte(0x20,OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
	  OLED_WR_Byte(0x02,OLED_CMD);//
	  OLED_WR_Byte(0x8D,OLED_CMD);//--set Charge Pump enable/disable
	  OLED_WR_Byte(0x14,OLED_CMD);//--set(0x10) disable
	  OLED_WR_Byte(0xA4,OLED_CMD);// Disable Entire Display On (0xa4/0xa5)
	  OLED_WR_Byte(0xA6,OLED_CMD);// Disable Inverse Display On (0xa6/a7) 
	  OLED_WR_Byte(0xAF,OLED_CMD);//--turn on oled panel
	
	  OLED_WR_Byte(0xAF,OLED_CMD); /*display ON*/ 
	  OLED_Clear();
	  OLED_Set_Pos(0,0); 	
}  


