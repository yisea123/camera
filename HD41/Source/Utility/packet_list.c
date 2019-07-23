/***************************************************************************************************
* FILE: packet_list.c

* DESCRIPTION:  --

* CREATED: 2018/9/3, by kevin
***************************************************************************************************/

/***************************************************************************************************
* INCLUDES
***************************************************************************************************/
#include "packet_list.h"
#include "intrinsics.h"

#define CLI() __set_PRIMASK(1)
#define SEI() __set_PRIMASK(0)

#define DISABLE_IRQ()   CLI()
#define ENABLE_IRQ()    SEI()

#ifdef __cplusplus
extern "C" {
#endif
    
#define EN_PACKLIST_DEBUG
#ifdef EN_PACKLIST_DEBUG
    static int32_t malloc_count = 0;
    static int32_t malloc_size = 0;
    static uint32_t max_alloc_addr = 0;
#endif
    
    
/***************************************************************************************************
* Description:  ��β�����һ���ڵ�
***************************************************************************************************/
int32_t PacketList_AddTail(struct packet_list *list, const uint8_t *buffer, uint32_t length)
{
    static struct packet *new_packet = NULL;
    
    if(buffer == NULL || length == 0)
    {
        return 1;
    }
    DISABLE_IRQ();   // �ر����ж�
    new_packet = (struct packet *)malloc(sizeof(struct packet));
    if(new_packet == NULL)
    {
        ENABLE_IRQ();   // �����ж�
        return 1;
    }
#ifdef EN_PACKLIST_DEBUG
    malloc_count++;
    malloc_size += sizeof(struct packet);
    if(max_alloc_addr < (uint32_t)new_packet) max_alloc_addr = (uint32_t)new_packet;
#endif
    
    new_packet->buffer = (uint8_t *)malloc(length);
    if(new_packet->buffer == NULL)
    {
        free(new_packet);
#ifdef EN_PACKLIST_DEBUG
        malloc_count--;
        malloc_size += sizeof(struct packet);
#endif
        ENABLE_IRQ();   // �����ж�
        return 1;
    }
#ifdef EN_PACKLIST_DEBUG
    malloc_count++;
    malloc_size += length;
    if(max_alloc_addr < (uint32_t)new_packet->buffer) max_alloc_addr = (uint32_t)new_packet->buffer;
#endif
    
    memcpy(new_packet->buffer, buffer, length);
    new_packet->length = length;
    new_packet->flag = 0;
    
    list_add_tail(&new_packet->list, &list->list);
    list->count++;
    list->total_size += length;
    
    ENABLE_IRQ();   // �����ж�
    return 0;
}

/***************************************************************************************************
* Description:  ȡ����ͷһ���ڵ�����
***************************************************************************************************/
int32_t PacketList_GetFirst(struct packet_list *list, uint8_t *buffer, uint32_t *length)
{
    struct packet *pk;
    
    if(list == NULL)
        return 1;
    if(list->count < 1)
        return 1;
    
    DISABLE_IRQ();   // �ر����ж�
    
    pk = list_first_entry(&list->list, struct packet, list);
    
    memcpy(buffer, pk->buffer, pk->length);
    (*length) = pk->length;
    
    list->count --;
    list->total_size -= pk->length;
    list_del(&pk->list);
    free(pk->buffer);
#ifdef EN_PACKLIST_DEBUG
    malloc_count--;
    malloc_size -= pk->length;
#endif
    free(pk);
#ifdef EN_PACKLIST_DEBUG
    malloc_count--;
    malloc_size -= sizeof(struct packet);
#endif
    
    ENABLE_IRQ();   // �����ж�
    return 0;
}

/***************************************************************************************************
* Description:  ɾ����һ���ڵ�
***************************************************************************************************/
void PacketList_DelFirst(struct packet_list *list)
{
    struct packet *pk;
    
    if(list->count < 1)
        return;
    
    DISABLE_IRQ();   // �ر����ж�
    
    pk = list_first_entry(&list->list, struct packet, list);
    list->count --;
    list->total_size -= pk->length;
    list_del(&pk->list);
    free(pk->buffer);
#ifdef EN_PACKLIST_DEBUG
    malloc_count--;
    malloc_size -= pk->length;
#endif
    free(pk);
#ifdef EN_PACKLIST_DEBUG
    malloc_count--;
    malloc_size -= sizeof(struct packet);
#endif
    ENABLE_IRQ();   // �����ж�
}

/***************************************************************************************************
* Description:  ɾ����������
***************************************************************************************************/
void PacketList_Clear(struct packet_list *list)
{
    struct packet *pk;
    struct list_head *tmp1, *tmp2;
    
    if(list->count < 1)
        return;
    
    DISABLE_IRQ();   // �ر����ж�
    
    list_for_each_safe(tmp1, tmp2, &list->list) 
    {
        list_del(tmp1);
        pk = list_entry(tmp1, struct packet, list);
        free(pk->buffer);
        free(pk); 
    }
    
    list->count = 0;
    list->total_size = 0;
    
    ENABLE_IRQ();   // �����ж�
}
#ifdef __cplusplus
}
#endif
/****************************************** END OF FILE *******************************************/
