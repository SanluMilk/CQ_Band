/*******************************************************************************************
*          ��Ȩ����, C ,2018- ,�ɶ������ֵ���Ϣ�������޹�˾
*
*����:Ƕ��ʽ������              ����:2018-9-1            �汾:V0.0
*˵��:
    ���ŷ���
*�޸�:
*******************************************************************************************/
#ifndef __HART_CONFIG__
#define __HART_CONFIG__

#ifdef __cplusplus
extern "C" {
#endif

//IO����
#define RX_PIN_NUMBER   8                           /*< ����RX��        */
#define TX_PIN_NUMBER   6                           /*< ����TX��        */
#define CTS_PIN_NUMBER  7                           /*< ����CTS��       */
#define RTS_PIN_NUMBER  5                           /*< ����RTS��       */

#define BATT_ADC        2                           /*< ���ADC�ɼ����� */

#define KEY_PIN         12                          /*< ��������        */

#define OLED_D0         23                          /*< OLED_D0����     */
#define OLED_D1         22                          /*< OLED_D1����     */
#define OLED_RES        26                          /*< OLED_RES����    */
#define OLED_DC         24                          /*< OLED_DC����     */
#define OLED_CS         30                          /*< OLED_CS����     */

#define SPI_SS_PIN      29                          /*< SPI_SS����      */
#define SPI_MOSI_PIN    4                           /*< SPI_MOSI����    */
#define SPI_MISO_PIN    28                          /*< SPI_MISO����    */
#define SPI_SCK_PIN     3                           /*< SPI_SCK����     */

#define CHARGER_STATUS  11                          /*< ���״̬��      */
#define MOTOR_DRIVE     25                          /*< ���������      */
#define SENSOR_POWER_EN 13                          /*< ��������Դʹ��  */

#define PN532_IIC_SDA         14
#define PN532_IIC_SCL         15


//��ʱ������
#define APP_TIMER_PRESCALER         0               //RTCԤ��ֵ
#define APP_TIMER_MAX_TIMERS        10              //��ʱ����������ֵ
#define APP_TIMER_OP_QUEUE_SIZE     15              //��ʱ�����д�С



//������õ�Ƶʱ��
#define NRF_CLOCK_LFCLKSRC      {.source        = NRF_CLOCK_LF_SRC_XTAL,            \
                                 .rc_ctiv       = 0,                                \
                                 .rc_temp_ctiv  = 0,                                \
                                 .xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_20_PPM}

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;
	
typedef signed char    s8;
typedef signed short   s16;
typedef signed int     s32;

typedef enum{ENABLE = 1, DISABLE = !ENABLE}Enable_e;

#define SET_FLAG(flag, bit)        (flag |= (1 << bit))
#define READ_FLAG(flag, bit)       ((flag >>bit) & 1)
#define CLEAN_FLAG(flag, bit)      (flag &= ~(1 << bit))

#ifdef __cplusplus
}
#endif

#endif


