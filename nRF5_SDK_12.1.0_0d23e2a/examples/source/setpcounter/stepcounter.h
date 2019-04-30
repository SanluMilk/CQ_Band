#ifndef _STEPCOUNTER_H_
#define _STEPCOUNTER_H_

#include <stdint.h>
#include "boards.h"
#include "utc.h"

#define DISTANCE_CONST        0.4
#define CALORIE_CONST         6.050//8.214      //(0.8214 * 10)

/* �˶����ݽṹ�� */
typedef struct{
    uint32_t stepcount;      //����
	  uint16_t distance;       //���ߵľ���(km)*10    ���� = ����(Cm) * K(ϵ��) *����   K = 0.4
	  uint16_t calorie;        //��·��(kcal)*10      ���� = ����(kg) * ����(KM) * K(ϵ��)   K = 0.8214
}Movement_data_t;

extern Movement_data_t movement;              //������¼�˶�������
extern long   StepDetector_tick;              //�Ʋ���������ʱ��

#define  GRAVITY_EARTH         9.80665f      /* �������ٶȳ���        */
#define  GSENSOR_RESOLUTION    16384         /* 1G���ٶȶ�Ӧ�����ֵ */

void setSteps(int initValue) ;
void get_Gsenson_Value(void);

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
void Step_Updata(const UTCTime_t * tm);

#endif

