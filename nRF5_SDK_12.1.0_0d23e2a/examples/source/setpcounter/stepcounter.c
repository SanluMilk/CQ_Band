#include "nrf_gpio.h"
#include "nrf_error.h"
#include "nrf_delay.h"

#include <stdio.h>
#include <math.h>
#include <string.h>
#include "stepcounter.h"
#include "boards.h"
#include "lis3dh_driver.h"
#include "ble_salve.h"
#include "comm_protocol.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"

Movement_data_t movement = {
    .stepcount = 0,
	  .distance = 0,
	  .calorie = 0,
};

long   StepDetector_tick = 0;        //�Ʋ���������ʱ��

float oriValues[3] = {0};            //����������ݵļ��ٶȣ���λg 

int ValueNum = 4;                    //���ڴ�ż�����ֵ�Ĳ��岨�Ȳ�ֵ�ĸ���
float tempValue[4] = {0};            //���ڴ�ż�����ֵ�Ĳ��岨�Ȳ�ֵ
int tempCount = 0;                   //�ѻ�ȡ���ڴ�ż�����ֵ�Ĳ��岨�Ȳ�ֵ�ĸ���

bool isDirectionUp = false;          //�Ƿ������ı�־λ
int continueUpCount = 0;             //������������
int continueUpFormerCount = 0;       //��һ��ĳ��������Ĵ�����Ϊ�˼�¼�������������
bool lastStatus = false;             //��һ���״̬�����������½�
float peakOfWave = 0;                //����ֵ
float valleyOfWave = 0;              //����ֵ
long timeOfThisPeak = 0;             //�˴β����ʱ��

long timeOfLastPeak = 0;             //�ϴβ����ʱ��

long timeOfNow = 0;                  //��ǰ��ʱ��

float gravityNew = 0;                //��ǰ��������ֵ
float gravityOld = 0;                //�ϴδ�������ֵ
float InitialValue = (float) 1.3;    //��̬��ֵ��Ҫ��̬�����ݣ����ֵ������Щ��̬���ݵ���ֵ
float ThreadValue = (float) 2.0;     //��ʼ��ֵ
int TimeInterval = 25;               //���岨��ʱ��� 250ms

static int count = 0;                //��ʱ�Ʋ����������ڼ��10������ʱʹ��
static long timeOfLastPeak_1;          //��һ�β���ʱ��
static long timeOfThisPeak_1;          //��һ�β���ʱ��


/*************************************************************************
������:Step_Updata
����:ͨ�������ϴ�����
�������:None
�������:None
����ֵ:None
˵��:
������ʮ���ŻῪʼ�Ʋ�
��������9������,ͣ������3��,��������
*************************************************************************/
void Step_Updata(const UTCTime_t * tm)	
{
	   static uint32_t step_save = 0;
	
     /* ÿ������00:00:00,��0���� */
	  if( tm->hour == 0 && tm->minute == 0 && tm->second == 0 )
		{
		    step_save = movement.stepcount = 0;
			  movement.calorie = 0;
			  movement.distance = 0;
		}
    
	  /******************************************** �������б仯ʱ����Ҫ�ϴ���APP ******************************************/
    if( step_save != movement.stepcount )
    {
         union{
             uint32_t a;
             uint8_t b[4];
         }c;
                     
         c.a = movement.stepcount;
         uint8_t buff[6] = {0};

         buff[0] = PRODUCT_NUMBER;
         buff[1] = CMD_Updata_Step;
         buff[2] = c.b[3];
         buff[3] = c.b[2];
         buff[4] = c.b[1];
         buff[5] = c.b[0];
         BLE_Salve_SendData(buff, 6);
         step_save = movement.stepcount;
    }    
}

/*************************************************************************
������:countStep
����:���㲽��
�������:None
�������:None
����ֵ:None
˵��:
������ʮ���ŻῪʼ�Ʋ�
��������9������,ͣ������3��,��������
*************************************************************************/
static void countStep(void) 
{
        timeOfLastPeak_1 = timeOfThisPeak_1;                  //�ѵ�ǰ����ʱ�丳ֵ����һ�εĲ���ʱ�����
        timeOfThisPeak_1 = StepDetector_tick;                 //�ѵ�ǰʱ�丳ֵ����ǰ�����¼�����

        if (timeOfThisPeak_1 - timeOfLastPeak_1 <= 300L)      //�����ǰ����ʱ�����һ�β���ʱ����С��3s(300*10ms)
        {
            if(count<9)                                       //�����ʱ�Ʋ�С��9�����Լ�
            {
                count++;
            }
            else if(count == 9)                               //�����ʱ�Ʋ�����9���ͰѲ�������ʵ�ʲ�����
            {
                count++;
                movement.stepcount += count;
							  movement.distance = (int)((double)(((double)save_param.height/100) *movement.stepcount * (double)DISTANCE_CONST)/100);    //���ߵľ���(KM) *10
                movement.calorie = (int)(save_param.weight *(double)((double)(((double)save_param.height/100) *movement.stepcount * (double)DISTANCE_CONST)/1000) * (double)CALORIE_CONST);           //����(kCal * 10)
            }
            else                                             //�����ʱ��������9��ʵ�ʲ������Լ�
            {
                movement.stepcount++;
//                NRF_LOG_INFO("stepcount = %d\r\n", stepcount);
							
							  movement.distance = (int)((double)(((double)save_param.height/100) *movement.stepcount * (double)DISTANCE_CONST)/100);    //���ߵľ���(KM) *10
                movement.calorie = (int)(save_param.weight *(double)((double)(((double)save_param.height/100) *movement.stepcount * (double)DISTANCE_CONST)/1000) * (double)CALORIE_CONST);           //����(kCal * 10)
                
            }
        }
        else                                                //�����ǰ����ʱ�����һ�β���ʱ��������3s(300*10ms)�������ʱ����
        {
            count = 1;                                      //Ϊ1,����0
        }
}


/*************************************************************************
������:setSteps
����:���ò���
�������:
@initValue:���õĲ���
�������:None
����ֵ:None
˵��:
*************************************************************************/
void setSteps(int initValue) 
{
        movement.stepcount = initValue;
        count = 0;
        timeOfLastPeak_1 = 0;
        timeOfThisPeak_1 = 0;      
}

/*************************************************************************
������:detectorPeak
����:��Ⲩ��,��¼����ֵ
�������:
@newValue:�µļ��ٶ���Ϣ
@oldValue:�ɵļ��ٶ���Ϣ
�������:None
����ֵ:None
˵��:
�����ĸ������ж�Ϊ���壺
1.Ŀǰ��Ϊ�½������ƣ�isDirectionUpΪfalse
2.֮ǰ�ĵ�Ϊ���������ƣ�lastStatusΪtrue
3.������Ϊֹ�������������ڵ���2��
4.����ֵ����1.2g (11.76),С��2g (19.6)
��¼����ֵ
1.�۲첨��ͼ�����Է����ڳ��ֲ��ӵĵط������ȵ���һ�����ǲ��壬�бȽ����Ե������Լ���ֵ
2.����Ҫ��¼ÿ�εĲ���ֵ��Ϊ�˺��´εĲ������Ա�
*************************************************************************/
static bool detectorPeak(float newValue, float oldValue) 
{
    lastStatus = isDirectionUp;                                         //��ȡ��һ���״̬

    if (newValue >= oldValue)                                           //�µļ��ٶ���Ϣ���ڵ��ھɵļ��ٶ���Ϣ
    {
        isDirectionUp = true;                                           //��ǲ���״̬������
        continueUpCount++;                                              //������������+1
    }
    else                                                                //�µļ��ٶ���ϢС�ھɵļ��ٶ���Ϣ
    {
        continueUpFormerCount = continueUpCount;                        //������һ�������������                   
        continueUpCount = 0;                                            //����������������0
        isDirectionUp = false;                                          //��ǲ���û������
    }

    /* ����������������ҵ�ǰ״̬��������������������������2����һ�μ��ٶ���Ϣ��1.2G~2G֮�� */
    if (!isDirectionUp && lastStatus && (continueUpFormerCount >= 2 && (oldValue >= (float)11.76 && oldValue < (float)19.6))) 
    {
        peakOfWave = oldValue;                                          //����һ�μ��ٶ�ֵ��ֵ������ֵ
        return true;
    } 
    else if (!lastStatus && isDirectionUp)                              //�����һ�����½�����һ��������
    {
        valleyOfWave = oldValue;                                        //����һ�μ��ٶ�ֵ��ֵ������ֵ                  
        return false;
    } 
    else 
    {
        return false;
    }
}

/*************************************************************************
������:averageValue
����:ͨ���ݶȷ������µ���ֵ
�������:
@value[]:����Ĳ����벨��֮��
@n:����ռ��С
�������:None
����ֵ:������µ���ֵ
˵��:
*************************************************************************/
static float averageValue(float value[], int n) 
{
    float ave = 0;

    /* ���㲨���벨��֮���ƽ��ֵ */
    for (int i = 0; i < n; i++) 
    {
        ave += value[i];
    }
    ave = ave / ValueNum;

    /* �����ݶȷ����µ���ֵ */
    if (ave >= 8)
        ave = (float)(4.3);
    else if (ave >= 7 && ave < 8)
        ave = (float)(3.3);
    else if (ave >= 4 && ave < 7)
        ave = (float)(2.3);
    else if (ave >= 3 && ave < 4)
        ave = (float)(2.0);
    else 
        ave = (float)(1.3);
        
		return ave;
}


/*************************************************************************
������:peakValleyThread
����:��ֵ�ļ���
�������:
@value:�����벨��֮��
�������:None
����ֵ:������µ���ֵ
˵��:
��ֵ�ļ���
1.ͨ�����岨�ȵĲ�ֵ������ֵ
2.��¼4��ֵ������tempValue[]������
3.�ڽ����鴫�뺯��averageValue�м�����ֵ
*************************************************************************/
static float peakValleyThread(float value) 
{
    float tempThread = ThreadValue;                                  //����һ����ʱ��ֵ������ǰ��ʼ��ֵ����
    
    if (tempCount < ValueNum)                                        //�����ȡ�Ĳ����벨��֮����������
    {
        tempValue[tempCount] = value;                                //���˴β����벨��֮��������뻺��
        tempCount++;
    }
    else                                                             //�����ȡ�Ĳ����벨��֮����������
    {
        tempThread = averageValue(tempValue, ValueNum);              //�����µĳ�ʼ��ֵ

        /* �������벨��֮���ռ����ƣ�������ֵ���Ա����¼����µĳ�ʼ��ֵ */
        for (int i = 1; i < ValueNum; i++)
        {
            tempValue[i - 1] = tempValue[i];
        }
        tempValue[ValueNum - 1] = value;
    }
    return tempThread;                                               //���ؼ�����µ���ֵ

}

/*************************************************************************
������:detectorNewStep
����:��ⲽ�ӣ�����ʼ�Ʋ�
�������:
@values:���ٶ���Ϣ
�������:None
����ֵ:None
˵��:
1.����sersor�е�����
2.�����⵽�˲��壬���ҷ���ʱ����Լ���ֵ�����������ж�Ϊ1��
3.����ʱ������������岨�Ȳ�ֵ����initialValue���򽫸ò�ֵ������ֵ�ļ�����
*************************************************************************/
static void detectorNewStep(float values) 
{
    if (gravityOld == 0)                                                            //�����һ�δ�����ֵΪ0���ͰѴ˴ε�ֵ��ֵ����һ�δ�����ֵ�������
    {
         gravityOld = values;
    } 
    else 
    {
        if (detectorPeak(values, gravityOld))                                      //�����⵽���� 
        {
            timeOfLastPeak = timeOfThisPeak;                                       //��ǰ����ֵ��ֵ����һ�β���ֵ
            timeOfNow = StepDetector_tick;                                         //��ȡ��ǰʱ��                         
            if (timeOfNow - timeOfLastPeak >= TimeInterval && (peakOfWave - valleyOfWave >= ThreadValue))  //�˴β�������һ�β�����ʱ�����Ԥ��ֵ�ң������벨��֮����ڳ�ʼ��ֵ
            {
                timeOfThisPeak = timeOfNow;                                        //���浱ǰʱ��Ϊ��ǰ����ʱ��
                countStep();                                                       //���㲽��                                                 
            }
            if (timeOfNow - timeOfLastPeak >= TimeInterval && (peakOfWave - valleyOfWave >= InitialValue)) //�˴β�������һ�β�����ʱ�����Ԥ��ֵ�ң������벨��֮����ڶ�̬��ֵ
            {
                    timeOfThisPeak = timeOfNow;                                   //���浱ǰʱ��Ϊ��ǰ����ʱ��
                    ThreadValue = peakValleyThread(peakOfWave - valleyOfWave);    //���»�ȡ��ʼ��ֵ
            }
       }
    }
    gravityOld = values;                                                          //�Ѵ˴ε�ֵ��ֵ����һ�δ�����ֵ�������                           
}

/*************************************************************************
������:get_Gsenson_Value
����:��ȡ������ֵ
�������:None
�������:None
����ֵ:None
˵��:
*************************************************************************/
void get_Gsenson_Value(void)
{
     AxesRaw_t dataXYZ;

     if(LIS3DH_GetAccAxesRaw(&dataXYZ) == MEMS_SUCCESS)                           //�ɼ�������x/y/z���ٶ�ֵ
     {
         oriValues[0]= ((float)dataXYZ.AXIS_X/GSENSOR_RESOLUTION)*GRAVITY_EARTH;         //��ȡX������������ٵĵļ��ٶ�ֵ(��λ:G) 
			   oriValues[1]= ((float)dataXYZ.AXIS_Y/GSENSOR_RESOLUTION)*GRAVITY_EARTH;         //��ȡY������������ٵĵļ��ٶ�ֵ(��λ:G)
			   oriValues[2]= ((float)dataXYZ.AXIS_Z/GSENSOR_RESOLUTION)*GRAVITY_EARTH;         //��ȡZ������������ٵĵļ��ٶ�ֵ(��λ:G)

         gravityNew = (float) sqrt(oriValues[0] * oriValues[0] + oriValues[1] * oriValues[1] + oriValues[2] * oriValues[2]);

         detectorNewStep(gravityNew);                                              //��ⲽ�� 
     }
}


