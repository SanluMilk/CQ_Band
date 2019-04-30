/**************************************************************************
    ��Ȩ (C), 2018- ,�ɶ�������Ϣ�������޹���

����Ӳ���d       
����:polaris.zhao@foxmail.com
�汾:V0.0
��������:2019-1-14
�ļ�����:
   ��ʼ��IICͨ�ţ�ʹ��ģ��IIC
�޸�:
**************************************************************************/
#include "IIC.h"
#include "nrf_delay.h"


/******************************************************************************
Function   : IIC_Start
Description: MCU�������߿�ʼ�Ņ�
Input      : None
Output     : None
Return     : None
Others     : ��SCL�ߵ�ƽʱ��SDA����һ�������ر�ʾI2C���������ź�
*******************************************************************************/
void IIC_Start(void)
{
    SDA_OUT();      //sda������
    IIC_SDA_1();;	  	  
    IIC_SCL_1();
    nrf_delay_us(1);
    IIC_SDA_0();     //START:when CLK is high,DATA change form high to low 
    nrf_delay_us(1);
    IIC_SCL_0();     //ǯסI2C���ߣ�׼�����ͻ�������� 
}

/******************************************************************************
Function   : IIC_Stop
Description: MCU��������ֹͣ�ź�
Input      : None
Output     : None
Return     : None
Others     : ��SCL�ߵ�ƽʱ��SDA����һ�������ر�ʾI2C����ֹͣ�ź�
*******************************************************************************/
void IIC_Stop(void)
{
    SDA_OUT();    //sda������
    IIC_SCL_0();
    IIC_SDA_0();    //STOP:when CLK is high DATA change form low to high
    nrf_delay_us(1);
    IIC_SCL_1(); 
    IIC_SDA_1();    //����I2C���߽����ź�
    nrf_delay_us(1);		
}

/******************************************************************************
Function   : IIC_SendByte
Description: MCU��IIC���߷��TByte������
Input      : data:��Ҫ���͵�����
Output     : None
Return     : None
Others     : SCL�½��ص�ʱ�򣬽�SDA�����ݷ��ͳ���
*******************************************************************************/
void IIC_SendByte(uint8_t data)
{
    uint8_t t;   
    
	  SDA_OUT(); 	    
    IIC_SCL_0();          //����ʱ�ӿ�ʼ���ݴ��{
    
	   for(t=0;t<8;t++)
    {              
        if(0 != ((data & 0x80) >> 7))
				    IIC_SDA_1();
				else
					  IIC_SDA_0();
				
        data <<= 1; 	  
        nrf_delay_us(1);   //��TEA5767��������ʱ���Ǳ����
        IIC_SCL_1();
        nrf_delay_us(1); 
        IIC_SCL_0();	
        nrf_delay_us(1);
    }	 
}


/******************************************************************************
Function   : IIC_ReadByte
Description: MCU��IIC���߶�ȡ1Byte������
Input      : None
Output     : None
Return     : None
Others     : SCL�½��ص�ʱ�򣬽�SDA�����ݷ��ͳ���
*******************************************************************************/
uint8_t IIC_ReadByte(void)
{
    uint8_t i = 0;
	   uint8_t value = 0;
	
	   SDA_IN();
	   
	   /* ��ȡ��һ��bitΪbit7 */
	   for(i = 0;  i < 8; i++ )
	   {
        IIC_SCL_0(); 
        nrf_delay_us(1);
        IIC_SCL_1();
        value <<= 1;
        
				if(READ_SDA())
            value++;   
		    nrf_delay_us(1); 
		 }
			return value;
}

/******************************************************************************
Function   : IIC_WaitACK
Description: MCU����һ��ʱ�ӣ���ȡ������ACKӦ���ź�
Input      : None
Output     : None
Return     : 0:Ӧ������ 
             ����:��Ӧ�q
Others     : 
*******************************************************************************/
uint8_t IIC_WaitACK(void)
{
    uint32_t ucErrTime=0;
    
	  SDA_OUT();      //SDA����Ϊ��� 
    IIC_SDA_1();
	  nrf_delay_us(1);	   
	  IIC_SCL_1();
	  nrf_delay_us(1);	 
	
	  SDA_IN();       //SDA����Ϊ��� 
	  while(READ_SDA())
	  {
		    ucErrTime++;
        if(ucErrTime > 16*250)
		      {
			         IIC_Stop();
			         return 1;
	      	}
	   }
	   IIC_SCL_0();//ʱ�����0 	   
	   return 0;  
}

/******************************************************************************
Function   : IIC_ACK
Description: MCU����һACK�ź�
Input      : None
Output     : None
Return     : None
Others     : 
*******************************************************************************/
void IIC_ACK(void)
{
    IIC_SCL_0();
    SDA_OUT();
    IIC_SDA_0();
    nrf_delay_us(1);
    IIC_SCL_1();
    nrf_delay_us(1);
    IIC_SCL_0();
}

/******************************************************************************
Function   : IIC_NACK
Description: MCU����һNACK�ź�
Input      : None
Output     : None
Return     : None
Others     : 
*******************************************************************************/
void IIC_NACK(void)
{
    IIC_SCL_0();
    SDA_OUT();
    IIC_SDA_1();;
    nrf_delay_us(1);
    IIC_SCL_1();
    nrf_delay_us(1);
    IIC_SCL_0();
}

/******************************************************************************
Function   : IIC_Init
Description: ��ʼ��IICʹ�õ�IO��
Input      : None
Output     : None
Return     : 0:�ɹ�   
             ����:ʧ��
Others     : 
*******************************************************************************/
int IIC_Init(void)
{
	   nrf_gpio_cfg_output(PN532_IIC_SCL);
	   nrf_gpio_cfg_output(PN532_IIC_SDA);
	   IIC_Stop();
     return 0;
}

/******************************************************************************
Function   : IIC_unInit
Description: �ͷ�IICʹ�õ�IO��
Input      : None
Output     : None
Return     : 0:�ɹ�   
             ����:ʧ��
Others     : 
*******************************************************************************/
int IIC_unInit(void)
{
	   nrf_gpio_cfg_default(PN532_IIC_SCL);
	   nrf_gpio_cfg_default(PN532_IIC_SDA);
	   IIC_Stop();
     return 0;
}








