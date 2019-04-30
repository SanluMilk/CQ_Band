/*******************************************************************************************
*          ��Ȩ����, C ,2018- ,�ɶ������ֵ���Ϣ�������޹�˾
*
*����:Ƕ��ʽ������              ����:2018-9-1            �汾:V0.0
*˵��:
*    UTCʱ����ع���ͷ�ļ�������һЩ�����Ҫ�����ݽṹ
*�޸�:
*
*******************************************************************************************/
#ifndef __UTC_H__
#define __UTC_H__

#include <stdint.h>

/* һ������� */
#define	SECOND_PER_DAY   86400UL  // 24 hours * 60 minutes * 60 seconds

/* �������� */
#define	IsLeapYear(yr)	(!((yr) % 400) || (((yr) % 100) && !((yr) % 4)))
#define	YearLength(yr)	(IsLeapYear(yr) ? 366 : 365)

/* ʱ���������1970/1/1 0��0��0��ʼ���� */
#define	BEGYEAR	        1970     // UTC started at 00:00:00 January 1, 1970   max year 2100

/* ʱ������ */
typedef uint32_t UTCTime;

/* ���� */
typedef enum{
    week_Monday = 0,    //����һ
    week_Tuesday,
    week_Wednesday,
    week_Thursday,
    week_Friday,
    week_Saturday,
    week_Sunday        //������
}week_day_t;

/* ʱ�� */
typedef struct{
    uint16_t    year;    // 1970+
    uint8_t     month;   // 0-11
    uint8_t     day;     // 0-30
    uint8_t     weekday; // 0:Monday
    uint8_t     hour;    // 0-23
    uint8_t     minute;  // 0-59
    uint8_t     second;  // 0-59
}UTCTime_t;

/******************************************************************************
Function   : Time_Run
Description: ʱ������
Input      : Null
Output     : Null
Return     : 
Others     : ʱ����1s�ƣ�����һ��1s�Ķ�ʱ�� 
*******************************************************************************/
void Time_Run(void);

/******************************************************************************
Function   : SetClock_seconds
Description: ��������ʱ��
Input      : newTime:����ǵ���ʱ��
Output     : Null
Return     : 
Others     : 
*******************************************************************************/
void SetClock_seconds( UTCTime newTime );

/******************************************************************************
Function   : SetClock_UTC
Description: ��ʱ���ʽ����ʱ��
Input      : tm:ʱ���ʽ����ʱ��
Output     : Null
Return     : 
Others     : 
*******************************************************************************/
void SetClock_UTC(UTCTime_t *tm);

/******************************************************************************
Function   : GetClock_seconds
Description: �����ȡʱ��
Input      : Null
Output     : Null
Return     : ��ȡ����ʱ�� 
Others     : 
*******************************************************************************/
UTCTime GetClock_seconds(void );

/******************************************************************************
Function   : SetClock_UTC
Description: ��ʱ���ʽ��ȡʱ��
Input      : tm:ʱ���ʽ����ʱ��
Output     : Null
Return     : 
Others     : 
*******************************************************************************/
void GetClock_UTC(UTCTime_t *tm);

/******************************************************************************
Function   : monthLength
Description: �������»�ȡ����
Input      : lpyr:һ�������
             mon:��
Output     : Null
Return     : ת���������
Others     : 
*******************************************************************************/
void seconds_To_UTC( UTCTime_t *tm, UTCTime secTime );

/******************************************************************************
Function   : seconds_To_UTC
Description: ��ת����ʱ���ʽ
Input      : secTime:ʱ����
Output     : tm������֮���ʽ
Return     : 
Others     : 
*******************************************************************************/
UTCTime UTC_To_seconds( UTCTime_t *tm );

#endif

