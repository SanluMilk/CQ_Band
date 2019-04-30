/*******************************************************************************************
*          ��Ȩ����, C ,2018- ,�ɶ������ֵ���Ϣ�������޹�˾
*
*����:Ƕ��ʽ������              ����:2018-9-1            �汾:V0.0
*˵��:
*    ������ͬ��ѹ�ķ�ʽ������Ӧ�Ĵ���
*�޸�:
2019-2-25:
1���رճ�������
2019-2-27:
1�����ӵ͵���ģʽ������������
2���޸�Ϣ��ʱ��Ϊ1S    
*******************************************************************************************/
#include <stdio.h>
#include "app_button.h"
#include "app_timer.h"

#include "key.h"
#include "oled_app.h"
#include "stepcounter.h"
#include "heat_sensor.h"
#include "app_gpio.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "MAX3010x.h"
#include "nrf_delay.h"

#define KEY_LONG      0     //��������  0:������    1:����
#ifdef KEY_DEBUG
 #define key_Msg     NRF_LOG_INFO
#else
 #define key_Msg(...)
#endif

#if KEY_LONG
APP_TIMER_DEF(m_button_timer_id);
#endif
/**********************************************************************
��������bsp_button_event_handler
����:�����¼��ص�����
�������:
    pin_no:����IO
    button_action:�����
�������:None
����ֵ��None
˵����
    ���������Ӧ����϶�ʱ�����������ֳ������Ƕ̰���
***********************************************************************/
static void bsp_button_event_handler(uint8_t pin_no, uint8_t button_action)
{  
    if (pin_no == KEY_PIN)                                          //�����������Ҫ�İ���
    {
        switch (button_action)                                      //�����״̬
        {
            case APP_BUTTON_RELEASE:                                //�����ͷ�
            {         
								/******************************* ������Ļ ***********************************/
					       if(OLED_ctrl.oled_state_flag == OLED_ON)    //��ǰ��Ļ������
					       {
									     
                        switch((uint8_t)(OLED_ctrl.page_num))   //��ѯ��ǰ����һ��ҳ��
                        {
													  
													  /*********************************����***************************************/
													  /*
                                �����ǰ������ҳ�棬�������º󣬾�Ӧ����������ҳ�棬���µ�������ʱ��													
   													*/
												    case DATE_P:      
														{
															   SetLastPageInfo(DATE_P, OLED_OFF, OLED_CLOSE_EN, OLED_OFF_TIME);   //����Ϊ����ҳ������Ϊ������Щ����Ϣ���󣬶���ص����ڽ���
															   SetNewPageInfo(STEP_P, OLED_ON, OLED_CLOSE_EN, OLED_OFF_TIME);
															   JumpPage();
														}
														break;
														
														 /*********************************����***************************************/
														 /*
                                �����ǰ�ڲ���ҳ�棬�������º󣬾�Ӧ����������ҳ�棬���µ�������ʱ��													
   													*/
	                          case STEP_P:    
                            {
															  SetLastPageInfo(DATE_P, OLED_OFF, OLED_CLOSE_EN, OLED_OFF_TIME);   //����Ϊ����ҳ������Ϊ������Щ����Ϣ���󣬶���ص����ڽ���
															  SetNewPageInfo(DISTANCE_P, OLED_ON, OLED_CLOSE_EN, OLED_OFF_TIME);
															  JumpPage();
														}
       											break;
														
                            /*********************************����***************************************/		
                            /*
                                �����ǰ�ھ���ҳ�棬�������º󣬾�Ӧ����������ҳ�棬���µ�������ʱ��													
   													*/														
	                          case DISTANCE_P: 
														{
															  SetLastPageInfo(DATE_P, OLED_OFF, OLED_CLOSE_EN, OLED_OFF_TIME);   //����Ϊ����ҳ������Ϊ������Щ����Ϣ���󣬶���ص����ڽ���
															  SetNewPageInfo(CALORIE_P, OLED_ON, OLED_CLOSE_EN, OLED_OFF_TIME);
															  JumpPage();
														}
 														break;
														
														/*********************************����***************************************/	
														 /*
                                �����ǰ������ҳ�棬�������º󣬾�Ӧ����������ҳ�棬���µ�������ʱ��													
   													*/	
	                          case CALORIE_P:
                            {
															  max30102_data.Heart = -3;
															  SetLastPageInfo(DATE_P, OLED_OFF, OLED_CLOSE_EN, OLED_OFF_TIME);   //����Ϊ����ҳ������Ϊ������Щ����Ϣ���󣬶���ص����ڽ���
															  SetNewPageInfo(HEART_P, OLED_ON, OLED_CLOSE_DISEN, OLED_OFF_TIME);
															  JumpPage();  
														}		
                            break;
														
														/*********************************����***************************************/
														/*
                                �����ǰ������ҳ�棬�������º󣬾�Ӧ������Ѫ��ҳ�棬���µ�������ʱ�䡣û�вɼ���
                                ���ݣ��Ͳ�Ϣ��														
   													*/	
	                          case HEART_P:         
														{
															  max30102_data.SpO2 = -3;
															  SetLastPageInfo(DATE_P, OLED_OFF, OLED_CLOSE_EN, OLED_OFF_TIME);   //����Ϊ����ҳ������Ϊ������Щ����Ϣ���󣬶���ص����ڽ���
															  SetNewPageInfo(OXYGEN_P, OLED_ON, OLED_CLOSE_DISEN, OLED_OFF_TIME);
															  JumpPage();  
															  
														}
														break;
														
														/*********************************Ѫ��***************************************/
														/*
                                �����ǰ��Ѫ��ҳ�棬�������º󣬾�Ӧ�������¶�ҳ�棬���µ�������ʱ�䡣û�вɼ���
                                ���ݣ��Ͳ�Ϣ��													
   													*/	
	                          case OXYGEN_P:       
														{
															  max30102_data.Temper = -3;
															  SetLastPageInfo(DATE_P, OLED_OFF, OLED_CLOSE_EN, OLED_OFF_TIME);   //����Ϊ����ҳ������Ϊ������Щ����Ϣ���󣬶���ص����ڽ���
															  SetNewPageInfo(TEMPER_P, OLED_ON, OLED_CLOSE_DISEN, OLED_OFF_TIME);
															  JumpPage();
														}
														break;
														
														/*********************************�¶�***************************************/
														/*
                                �����ǰ���¶�ҳ�棬�������º󣬾�Ӧ������MACҳ�棬���µ�������ʱ�䡣û�вɼ���
                                ���ݣ��Ͳ�Ϣ��													
   													*/
	                          case TEMPER_P:        
														{
															  SetLastPageInfo(DATE_P, OLED_OFF, OLED_CLOSE_EN, OLED_OFF_TIME);   //����Ϊ����ҳ������Ϊ������Щ����Ϣ���󣬶���ص����ڽ���
															  SetNewPageInfo(MAC_P, OLED_ON, OLED_CLOSE_EN, OLED_OFF_TIME);
															  JumpPage();
														}
														break;
														
														/*********************************MAC***************************************/
														/*
                                �����ǰ��MACҳ�棬�������º󣬾�Ӧ����������ҳ�棬���µ�������ʱ�䡣û�вɼ���
                                ���ݣ��Ͳ�Ϣ��													
   													*/
	                          case MAC_P:           
														{
															  SetLastPageInfo(DATE_P, OLED_OFF, OLED_CLOSE_EN, OLED_OFF_TIME);   //����Ϊ����ҳ������Ϊ������Щ����Ϣ���󣬶���ص����ڽ���
															  SetNewPageInfo(DATE_P, OLED_ON, OLED_CLOSE_EN, OLED_OFF_TIME);
															  JumpPage();
														}
														break;
														
														/*********************************����***************************************/
														case ALARM_P:
														{
															  SetNewPageInfo(OLED_ctrl.lastPageNum, OLED_ctrl.lastPageOledState, OLED_ctrl.lastPageKey, OLED_ctrl.lastPageTime);
															  JumpPage(); 
														}
														break;
														
														/*********************************�����Ͽ�����***************************************/
														case BLE_DISCONNECT_P:
														{
															  SetNewPageInfo(OLED_ctrl.lastPageNum, OLED_ctrl.lastPageOledState, OLED_ctrl.lastPageKey, OLED_OFF_TIME);
															  JumpPage(); 	  
														}
														break;
#ifdef BLE_SCAN														
												   	/*********************************Beacon����***************************************/
														case BEACON_LOST_P:
														{
															 SetNewPageInfo(OLED_ctrl.lastPageNum, OLED_ctrl.lastPageOledState, OLED_ctrl.lastPageKey, OLED_OFF_TIME);
															 JumpPage(); 
														}
														break;
#endif
						            }
					          }
										/************************************��ǰ��Ļ�����**********************************/
					          else    
					          {
											   if( sys_state.power_low == 0 )    //��������
												 {
													   OLED_Display_On();   
												     if( COLLECT_P != OLED_ctrl.page_num )            //��ǰ�����ֻ��ɼ�ҳ��
														 {		
                                 /* ������Ļ��ʾ���� */              
                                 SetLastPageInfo(DATE_P, OLED_OFF, OLED_CLOSE_EN, OLED_OFF_TIME);   //����Ϊ����ҳ������Ϊ������Щ����Ϣ���󣬶���ص����ڽ���
															   SetNewPageInfo(DATE_P, OLED_ON, OLED_CLOSE_EN, OLED_OFF_TIME);
															   JumpPage();
														 }
														 else     //��ǰ���ֻ��ɼ�ҳ��
														 {
                                 SetNewPageInfo(COLLECT_P, OLED_ON, OLED_CLOSE_EN, OLED_OFF_TIME);
															   JumpPage();														 
														 }
												 }
					          }
             }
             break;
        }
    }
}

/**********************************************************************
��������bsp_button_event_handler
����:�����¼��ص�����
�������:
    pin_no:����IO
    button_action:�����
�������:None
����ֵ��None
˵����
    ���������Ӧ����϶�ʱ�����������ֳ������Ƕ̰���
***********************************************************************/
void buttons_init(void)
{
    uint32_t err_code = NRF_SUCCESS;

    static const app_button_cfg_t app_buttons[] =
    {
        {
				    KEY_PIN,                                //��������
					  APP_BUTTON_ACTIVE_LOW,                 //������Ծ״̬
					  NRF_GPIO_PIN_NOPULL,                  //IO������
					  bsp_button_event_handler                //�����¼��ص�����
				},
    };
				
				/* ������ʼ�� */
    err_code = app_button_init((app_button_cfg_t *)app_buttons,
                                sizeof(app_buttons) / sizeof(app_buttons[0]),
                                APP_TIMER_TICKS(10, APP_TIMER_PRESCALER));
    
    if (err_code == NRF_SUCCESS)
    {
        err_code = app_button_enable();           //ʹ�ܰ���
    }  
}





