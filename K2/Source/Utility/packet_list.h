/***************************************************************************************************
* FILE: packet_list.h
*
* DESCRIPTION:  --
*
* CREATED: 2018/9/3, by kevin
***************************************************************************************************/
#ifndef __PACKET_LIST_H_
#define __PACKET_LIST_H_


#include "llist.h"
//#include "stdint.h"
/***************************************************************************************************
* DEFINES
***************************************************************************************************/
struct packet
{
    struct list_head list;
    uint32_t length;
    uint8_t flag;
    uint8_t *buffer;
};

struct packet_list
{
    struct list_head list;
    uint32_t count;
    uint32_t total_size;
};

/* ���岢��ʼ��һ�������� */
#define PACKET_LIST_HEAD(name) struct packet_list name = {{ &(name).list, &(name).list }, 0, 0}

/* ��ȡ�ڵ���� */
#define PackList_GetCount(name) (name.count)

/* ��ȡ�����ܴ�С */
#define PackList_GetSize(name) (name.total_size)

/***************************************************************************************************
* VARIABLES DECLARATION
***************************************************************************************************/

/***************************************************************************************************
* FUNCTION DECLARATION
***************************************************************************************************/
int32_t PacketList_Add(struct packet_list *list, const uint8_t *buffer, uint32_t length);
int32_t PacketList_Get(struct packet_list *list, uint8_t **buffer, uint32_t *length);
void PacketList_DelFirst(struct packet_list *list);
int32_t PacketList_Destroy(struct packet_list *list);



#endif
/****************************************** END OF FILE *******************************************/
