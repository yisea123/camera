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
  unsigned short front;   //����
  unsigned short rear;   //��β
  unsigned short count;  //������Ԫ�ظ���
  unsigned short size;   //���д�С
  uint8_t *buffer; //���л���ָ��
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
