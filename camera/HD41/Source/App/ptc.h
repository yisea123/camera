/***************************************************************************************************
* FILE: ptc.h
*
* DESCRIPTION: --
*
* CREATED: 2017/9/22, by kevin xu
***************************************************************************************************/
#ifndef PTC_H
#define PTC_H
#include "bsp.h"


/* ���ڽ��ջ������ֵ */
#define CAMERA_RX_BUFFER_MAX_SIZE  256
#define CAM_CACHE_BUFFER_MAX_SIZE  256
#define INPUT1_RX_BUFFER_MAX_SIZE  256
#define OUTPUT_RX_BUFFER_MAX_SIZE  256
#define INPUT2_RX_BUFFER_MAX_SIZE  256


//#define CUR_PTC             (stPtcInfo.type)
//#define CUR_ADDR            (stPtcInfo.addr)    
//#define CUR_BAUD            (stPtcInfo.baudrate)
//#define CUR_SPEED_GRADE     (stPtcInfo.speed_grade)
//#define CUR_ACC             (stPtcInfo.speed_acc)
//#define CUR_SPEED_LIMIT     (stPtcInfo.speed_limit)


typedef struct ptc_info
{
    uint8_t type;       //Э������
    uint8_t addr;       //����ͨ���и���Э��ĵ�ַ
    uint8_t baudrate;   //����ͨ���и���Э��Ĳ�����
    uint8_t speed_grade;  //�ٶ�ϸ�ֵȼ�visca 24/100 pelco 64/100
    uint8_t speed_acc;  //���ٶ�
    uint8_t speed_limit;//�ٶ����ƣ������ⲿ���ڿ�����̨ʱ�ٶ��Ƿ�;�ͷλ�ù���
} PTC_INFO_ST;



void Ptc_Init(void);

void CmdDebug(uint8_t onoff);
void LinkCam2In1(uint8_t onoff);



#endif
/****************************************** END OF FILE *******************************************/
