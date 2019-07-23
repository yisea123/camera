/***************************************************************************************************
* FILE: ir_sony.h

* DESCRIPTION: �������ң��������⣬
* ʹ��ʱ��Ҫʵ��3���ӿڣ�1.��ʱ����ʼ������2��ʱ����������3.��������˫�����жϵ��á�

* CREATED: 2018/11/21, by kevin
***************************************************************************************************/
#ifndef __IR_SONY_H__
#define __IR_SONY_H__

#ifdef __cplusplus
 extern "C" {
#endif
     
#include "stdint.h"
     
/* �����ص����� */
struct ir_decoder_ops
{
    /* �����ʼֵ��ʼ��ʱ����λ��us */
    void (*start_counting)(uint32_t initial_value);
    
    /* ֹͣ��ʱ����λ��us */
    uint32_t (*stop_counting)(void);
};

/* ��ȡ�����汾�� */
const char* SonyIr_GetVersion(void);


/* ��ʼ������������ʵ��������ʱ�ص����� */
int32_t SonyIr_Init(struct ir_decoder_ops *ops);


/* ��ȡ��ǰ�������е�ָ�� */
uint32_t SonyIr_GetCmd(void);


/* ˫�����жϷ���������Ҫ���뵱ǰ���ŵ�ƽ */
void SonyIr_EdgeHandler(int32_t level);

#ifdef __cplusplus
 }
#endif
#endif
/****************************************** END OF FILE *******************************************/
