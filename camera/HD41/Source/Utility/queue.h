/***************************************************************************************************
* FILE: queue.h

* DESCRIPTION:
  --

* CREATED: 2017/9/22, by kevin xu
****************************************************************************************************
* MODIFY:  --

* DATE: --

* DESCRIPTION: --

***************************************************************************************************/
#ifndef __QUEUE_H_
#define __QUEUE_H_
#include "stdint.h"
/***************************************************************************************************
* DEFINES
***************************************************************************************************/
typedef struct ByteQueue
{
  unsigned short front;   //队首
  unsigned short rear;   //队尾
  unsigned short count;  //队列中元素个数
  unsigned short size;   //队列大小
  uint8_t *buffer; //队列缓存指针
} *pByteQueue, ByteQueue;
/***************************************************************************************************
* VARIABLES DECLARATION
***************************************************************************************************/

/***************************************************************************************************
* FUNCTION DECLARATION
***************************************************************************************************/
uint8_t ByteQueue_Init(pByteQueue pQueue, uint8_t *buf, unsigned short size);
void ByteQueue_Clear(pByteQueue pQueue);
uint8_t ByteQueue_IsEmpty(pByteQueue pQueue);
void ByteQueue_EnQueue(pByteQueue pQueue, uint8_t data);
uint8_t ByteQueue_DeQueue(pByteQueue pQueue, uint8_t *data);


#endif
/****************************************** END OF FILE *******************************************/
