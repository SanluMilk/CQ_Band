/*******************************************************************************************
*          ��Ȩ����, C ,2018- ,�ɶ������ֵ���Ϣ�������޹�˾
*
*����:Ƕ��ʽ������              ����:2018-9-1            �汾:V0.0
*˵��:
*    SPIͨ�ţ���������LIS3dH
*�޸�:
*
*******************************************************************************************/
#ifndef __SPI_H__
#define __SPI_H__


#include "nrf_drv_spi.h"
#include "app_util_platform.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "boards.h"
#include "app_error.h"
#include <string.h>


/*************************************************************************
������:SPI_Init
����:SPI�¼��ص�����
�������:
@p_event:�����¼���ָ��
�������:None
����ֵ:None
˵��:
*************************************************************************/
void SPI_Init(void);

/*************************************************************************
������:SPI_UnInit
����:�ر�SPI
�������:None
�������:None
����ֵ:None
˵��:
*************************************************************************/
void SPI_UnInit(void);

/************************************************************************
* Function Name		: SPI_WriteReg
* Description		: Generic Writing function. It must be fullfilled with either
*			: I2C or SPI writing function
* Input			: Register Address, Data to be written
* Output		: None
* Return		: None
*************************************************************************/
uint8_t SPI_WriteReg(uint8_t WriteAddr, uint8_t Data);

/************************************************************************
* Function Name		: LIS3DH_ReadReg
* Description		: Generic Reading function. It must be fullfilled with either
*			: I2C or SPI reading functions					
* Input			: Register Address
* Output		: Data REad
* Return		: None
**************************************************************************/
uint8_t SPI_ReadReg(uint8_t Reg, uint8_t* Data);


#endif

