/*******************************************************************************************
*          ��Ȩ����, C ,2018- ,�ɶ������ֵ���Ϣ�������޹�˾
*
*����:Ƕ��ʽ������              ����:2018-11-6            �汾:V0.0
*˵��:
*    ��оƬ�ڲ�flash������
*    ����ʱ����ĵ�ַʱ��Ե�ַ
*�޸�:
*    2018-11-5:���ӹ̼��������ܡ�
*******************************************************************************************/
#include "sdk_config.h"
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "boards.h"
#include "ble_nus.h"
#include "fstorage.h"
#include "fstorage_internal_defs.h"
#include "fds.h"
#include "app_fds.h"
#include "app_uart_ex.h"
#include "nrf_delay.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#ifdef FLASH_DEBUG
    #define mem_dbg NRF_LOG_INFO
#else
    #define mem_dbg(...)
#endif

#define NUM_PAGES 1                                   //������ҳ
#define PAGE_SIZE_WORDS     FS_PAGE_SIZE_WORDS        //ÿҳ��С 4096/4 ��

static uint8_t fs_callback_flag;                      //�ص�������ʶ
static void fs_evt_handler(fs_evt_t const *const evt, fs_ret_t result);

/* Ĭ������FS */
FS_REGISTER_CFG(fs_config_t fs_config) =
{
    .callback= fs_evt_handler,                        // Function for event callbacks.
    .num_pages= NUM_PAGES,                            // Number of physicalflash pages required.
    .priority = 0xFE                                  // Priority for flash usage.
};
/**********************************************************************
������:fs_evt_handler
����:flash�¼��ص�����
�������:
@evt:�¼�
@result:���
�������:None
����ֵ:None
˵��:
***********************************************************************/
static void fs_evt_handler(fs_evt_t const *const evt, fs_ret_t result)
{
    if (result != FS_SUCCESS)
    {
//        mem_dbg("\r\nfstorage command Error full ycompleted\r\n")
    }
    else
    {
//        mem_dbg("\r\nfstorage command success full ycompleted\r\n");
        fs_callback_flag = 0;
    }
}

/**********************************************************************
������:Flash_Init
����:��ʼ��Flash
�������:None
�������:None
����ֵ:None
˵��:
***********************************************************************/
void Flash_Init(void)
{
    fs_ret_t ret = fs_init();
    if(ret != FS_SUCCESS)
    {
        mem_dbg("fs init error\r\n");
					   return;
    }

    mem_dbg("Flash address 0x%X: \r\n", (uint32_t)fs_config.p_start_addr);
}


/**********************************************************************
������:Flash_Read
����:��ָ����ַ��ȡ��ָ�����ȵ�����
�������:
@start_addr:��ʼ��ȡ�ĵ�ַ(��Ե�ַ,��0��ʼ)
@len:��Ҫ��ȡ�ĳ���(����λ��λ)
�������:
@pData:��ȡ�����ݷ���
����ֵ:fs_ret_t
˵��:
Flash��ȡ���ݣ��������ֶ�(32λ)
***********************************************************************/
fs_ret_t Flash_Read(const uint32_t start_addr, uint32_t *pData, const uint16_t len)
{
	   if( (start_addr + len) > (NUM_PAGES * PAGE_SIZE_WORDS))
				{
					   mem_dbg("fs Read ADDR ERR\r\n");
				    return FS_ERR_INVALID_ADDR;
				}
				
	   mem_dbg("Flash Read Data:0x%X\r\n",(uint32_t)(fs_config.p_start_addr + start_addr));
    for(int i=0; i<len; i++)
    {
        pData[i]= *(fs_config.p_start_addr + start_addr + i);
    }
				
				return FS_SUCCESS;
}

/**********************************************************************
������:Flash_Erase_Page
����:��ָ����ҳ����ʼ����������ҳ
�������:
@start_page:��ʼ��ҳ
@number:��Ҫ������ҳ��
�������:None
����ֵ:fs_ret_t
˵��:
***********************************************************************/
fs_ret_t Flash_Erase_Page(const uint8_t start_page, const uint8_t number )
{
	   if( (start_page + number) > NUM_PAGES )
				{
					   mem_dbg("fs Erase ADDR ERR\r\n");
				    return FS_ERR_INVALID_ADDR;
				}
				
    mem_dbg("Erasinga flash page at address 0x%X\r\n", (uint32_t)(fs_config.p_start_addr + start_page * PAGE_SIZE_WORDS));

    fs_callback_flag= 1;

    fs_ret_t ret= fs_erase(&fs_config, (uint32_t *)(fs_config.p_start_addr + start_page * PAGE_SIZE_WORDS), number, NULL);
    if(ret != FS_SUCCESS)
    {
        mem_dbg("fs erase error\r\n");
					   return ret;
    }

				uint32_t time_out = 1000;
    while(fs_callback_flag== 1 && time_out--)
    {
					    nrf_delay_ms(1);
    }
				if( time_out != 0 )
        return FS_SUCCESS;		
    else
				{
					   mem_dbg("fs erase timeout\r\n");
        return FS_ERR_OPERATION_TIMEOUT;		
				}					
}

/**********************************************************************
������:Flash_Write
����:��ָ����ַ����д����
�������:
@start_addr:д��Ŀ�ʼ��ַ(��Ե�ַ,��0��ʼ)
@p_data:��Ҫд�������(����λ��λ)
@len:��Ҫд������ݵĳ���(����λ��λ)
�������:None
����ֵ:fs_ret_t
˵��:
***********************************************************************/
fs_ret_t Flash_Write(const uint32_t start_addr, const uint32_t *p_data, const uint16_t len )
{
	   uint16_t i = 0;
	
	   if( (start_addr + len) > (NUM_PAGES * PAGE_SIZE_WORDS))
				{
					   mem_dbg("fs Write ADDR ERR\r\n");
				    return FS_ERR_INVALID_ADDR;
				}
				
				uint32_t time_out = 1000;
				
        mem_dbg("Flash Write Data:0x%X\r\n", (uint32_t)(fs_config.p_start_addr + start_addr));
    
				uint8_t start_page_number  = start_addr/PAGE_SIZE_WORDS;                                        //�������ݴ洢�Ŀ�ʼҳ
				uint8_t end_page_number = (start_addr + len)/PAGE_SIZE_WORDS;                                   //�������ݴ洢�Ľ���ҳ
				
				mem_dbg("start_page_nuber = %d, end_page_nuber = %d\r\n",start_page_number, end_page_number);
				uint32_t data_buff[PAGE_SIZE_WORDS] = {0};                                                      //����ҳ���ݻ���
					
				/* Read page data */
				uint32_t read_addr = start_page_number * PAGE_SIZE_WORDS;                                       //�����ҳ���׵�ַ
				Flash_Read(read_addr, data_buff, PAGE_SIZE_WORDS);                                                 //��ȡ����ҳ������
				
				Flash_Erase_Page(start_page_number, 1);                                                         //�������ҳ
    
				/* ��������ӵ����� */
				uint16_t write_len = ((start_page_number + 1) * PAGE_SIZE_WORDS - start_addr);
				write_len = ( write_len > len )?len:write_len;
				
				for( i = 0; i < write_len; i++ )
				{
				     data_buff[(start_addr - start_page_number*PAGE_SIZE_WORDS)+i] = p_data[i];    
				}
   
				/* д������ */
				fs_callback_flag = 1;
				fs_ret_t ret= fs_store(&fs_config, (fs_config.p_start_addr + start_page_number * PAGE_SIZE_WORDS), (uint32_t *)data_buff, FS_PAGE_SIZE_WORDS, NULL);
    if(ret != FS_SUCCESS)
    {
        mem_dbg("fs write error\r\n");
					   return ret;
    }

    while(fs_callback_flag== 1 && time_out--)
    {
					    nrf_delay_ms(1);
    }
				time_out = 1000;
				if( time_out == 0 )
				{
					   mem_dbg("fs write timeout\r\n");
        return FS_ERR_OPERATION_TIMEOUT;	
				}
				
				/* ����˴δ洢��Ҫʹ�ö��ҳ */
				while(start_page_number != end_page_number)  
				{
					   start_page_number++;                                                          //������һҳ
					   memset(data_buff, 0, sizeof(data_buff));                                      //�������
				    	
					   /* Read page data */
				    uint32_t read_addr = start_page_number * PAGE_SIZE_WORDS;                                       //�����ҳ���׵�ַ
				    Flash_Read(read_addr, data_buff, PAGE_SIZE_WORDS);                                                 //��ȡ����ҳ������
				
				    Flash_Erase_Page(start_page_number, 1);                                                         //�������ҳ
    
			    	/* ��������ӵ����� */
				    uint16_t write_len = len - start_page_number * PAGE_SIZE_WORDS;
					   write_len = ( write_len > PAGE_SIZE_WORDS )?PAGE_SIZE_WORDS:write_len;
				
				    for( i = 0; i < write_len; i++ )
				    {
				         data_buff[i] = p_data[start_page_number * PAGE_SIZE_WORDS - start_addr + i];    
				    }
   
				    /* д������ */
				    fs_callback_flag = 1;
				    fs_ret_t ret= fs_store(&fs_config, (fs_config.p_start_addr + start_page_number * PAGE_SIZE_WORDS), (uint32_t *)data_buff, FS_PAGE_SIZE_WORDS, NULL);
        if(ret != FS_SUCCESS)
        {
            mem_dbg("fs write error\r\n");
					       return ret;
        }

        while(fs_callback_flag== 1 && time_out--)
        {
					       nrf_delay_ms(1);
        }
				    time_out = 1000;
				    if( time_out == 0 )
				    {
					       mem_dbg("fs write timeout\r\n");
            return FS_ERR_OPERATION_TIMEOUT;	
				    }
				}
				return FS_SUCCESS;
}


