/***************************************************************************************************
* FILE: ir_sony.h

* DESCRIPTION: 索尼红外遥控器解码库，
* 使用时需要实现3个接口：1.定时器开始计数。2定时结束计数。3.红外数据双边沿中断调用。

* CREATED: 2018/11/21, by kevin
***************************************************************************************************/
#ifndef __IR_SONY_H__
#define __IR_SONY_H__

#ifdef __cplusplus
 extern "C" {
#endif
     
#include "stdint.h"
     
/* 驱动回调函数 */
struct ir_decoder_ops
{
    /* 传入初始值开始计时，单位：us */
    void (*start_counting)(uint32_t initial_value);
    
    /* 停止计时，单位：us */
    uint32_t (*stop_counting)(void);
};

/* 获取驱动版本号 */
const char* SonyIr_GetVersion(void);


/* 初始化驱动，必须实现两个计时回调函数 */
int32_t SonyIr_Init(struct ir_decoder_ops *ops);


/* 获取当前缓冲区中的指令 */
uint32_t SonyIr_GetCmd(void);


/* 双边沿中断服务函数，需要传入当前引脚电平 */
void SonyIr_EdgeHandler(int32_t level);

#ifdef __cplusplus
 }
#endif
#endif
/****************************************** END OF FILE *******************************************/
