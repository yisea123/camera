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
#include "stdint.h"
#include "string.h"

#ifdef __cplusplus
extern "C" {
#endif
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

/* 定义并初始化一个包链表 */
#define PACKET_LIST(name) struct packet_list name = {{ &(name).list, &(name).list }, 0, 0}

/* 获取节点个数 */
#define PackList_GetCount(name) (name.count)

/* 获取链表总大小 */
#define PackList_GetSize(name) (name.total_size)


/***************************************************************************************************
* FUNCTION DECLARATION
***************************************************************************************************/
int32_t PacketList_AddTail(struct packet_list *list, const uint8_t *buffer, uint32_t length);
int32_t PacketList_GetFirst(struct packet_list *list, uint8_t *buffer, uint32_t *length);
void PacketList_DelFirst(struct packet_list *list);
void PacketList_Clear(struct packet_list *list);


#ifdef __cplusplus
}
#endif
#endif
/****************************************** END OF FILE *******************************************/
