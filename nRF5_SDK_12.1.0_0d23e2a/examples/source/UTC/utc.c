/*******************************************************************************************
*          ��Ȩ����, C ,2018- ,�ɶ������ֵ���Ϣ�������޹�˾
*
*����:Ƕ��ʽ������              ����:2018-9-1            �汾:V0.0
*˵��:
*    UTCʱ����ع��ܺ���
*�޸�:
*
*******************************************************************************************/
#include "utc.h"

static UTCTime base_time = 0;     //ʱ�ӻ���

/******************************************************************************
Function   : Time_Run
Description: ʱ������
Input      : Null
Output     : Null
Return     : 
Others     : ʱ����1s�ƣ�����һ��1s�Ķ�ʱ�� 
*******************************************************************************/
void Time_Run(void)
{
    base_time++;    
}

/******************************************************************************
Function   : SetClock_seconds
Description: ��������ʱ��
Input      : newTime:����ǵ���ʱ��
Output     : Null
Return     : 
Others     : 
*******************************************************************************/
void SetClock_seconds( UTCTime newTime )
{
    base_time = newTime;
}

/******************************************************************************
Function   : SetClock_UTC
Description: ��ʱ���ʽ����ʱ��
Input      : tm:ʱ���ʽ����ʱ��
Output     : Null
Return     : 
Others     : 
*******************************************************************************/
void SetClock_UTC(UTCTime_t *tm)
{
    base_time = UTC_To_seconds(tm);   
}

/******************************************************************************
Function   : GetClock_seconds
Description: �����ȡʱ��
Input      : Null
Output     : Null
Return     : ��ȡ����ʱ�� 
Others     : 
*******************************************************************************/
UTCTime GetClock_seconds(void )
{
     return base_time;
}

/******************************************************************************
Function   : SetClock_UTC
Description: ��ʱ���ʽ��ȡʱ��
Input      : tm:ʱ���ʽ����ʱ��
Output     : Null
Return     : 
Others     : 
*******************************************************************************/
void GetClock_UTC(UTCTime_t *tm)
{
    seconds_To_UTC(tm, base_time);   
}

/******************************************************************************
Function   : monthLength
Description: �������»�ȡ����
Input      : lpyr:һ�������
             mon:��
Output     : Null
Return     : ת���������
Others     : 
*******************************************************************************/
static uint8_t monthLength( uint8_t lpyr, uint8_t mon )
{
    uint8_t days = 31;

    if ( mon == 1 )     //����Ƕ���
        days = ( 28 + lpyr );
    else
    {
        if ( mon > 6 )   // 8��--12��
            mon--;

        if ( mon & 1 )  //������
            days = 30;
    }
    return ( days );
}

/******************************************************************************
Function   : seconds_To_UTC
Description: ��ת����ʱ���ʽ
Input      : secTime:ʱ����
Output     : tm������֮���ʽ
Return     : 
Others     : 
*******************************************************************************/
void seconds_To_UTC( UTCTime_t *tm, UTCTime secTime )
{
    uint32_t day = secTime % SECOND_PER_DAY;
    
    /* ��ȡʱ���� */
    tm->second = day % 60UL;
    tm->minute = (day % 3600UL) / 60UL;
    tm->hour = (day / 3600UL);
  
    uint16_t numDays = secTime / SECOND_PER_DAY;
    
    /* ��ȡ�� */
    tm->weekday = (numDays + week_Thursday) % 7;    // 1970-1-1 is Thursday
    
    /* ��ȡ�� */
    tm->year = BEGYEAR;
    while ( numDays >= YearLength( tm->year ) )
    {
      numDays -= YearLength( tm->year );
      tm->year++;
    }

    /* ��ȡ�� */
    tm->month = 0;
    while ( numDays >= monthLength( IsLeapYear( tm->year ), tm->month ) )
    {
      numDays -= monthLength( IsLeapYear( tm->year ), tm->month );
      tm->month++;
    }

    /* ��ȡ�� */
    tm->day = numDays;
}

/******************************************************************************
Function   : UTC_To_seconds
Description: ʱ���ʽת������
Input      : Null
Output     : tm��ʱ���ʽ��ʱ��
Return     : 
Others     : 
*******************************************************************************/
UTCTime UTC_To_seconds( UTCTime_t *tm )
{
    uint32_t second;

    /* ��ȡһ���Ѿ����˶����� */
    second = (((tm->hour * 60UL) + tm->minute) * 60UL) + tm->second;

    uint16_t days = tm->day;
    int8_t month = tm->month;
     
    /* ������ */
    while ( --month >= 0 )
    {
         days += monthLength( IsLeapYear( tm->year ), month );
    }
    
    uint16_t year = tm->year;
    while ( --year >= BEGYEAR )
    {
        days += YearLength( year );
    }
    
    /* ��ȡ�ꡢ�¡��ռ������ */
    second += (days * SECOND_PER_DAY);
	 
    return ( second );
}

