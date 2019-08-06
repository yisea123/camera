/***************************************************************************************************
* FILE: menu_framework.c
*
* DESCRIPTION:  --
*
* CREATED: 2019/1/2, by kevin
***************************************************************************************************/
#include "menu_framework.h"
#include "ccp.h"
#include "visca2camera.h"
#include "string.h"

static uint8_t sel_line = 0;

static LIST_HEAD(MenuHead);
struct list_head *pCurMenuHead = NULL;
/***************************************************************************************************
* Description:  �����б�
***************************************************************************************************/
int32_t NewMenu(void)
{
    struct menu_head *head;
    
    /* �б� */
    head = (struct menu_head *)malloc(sizeof(struct menu_head));
    if(head == NULL)
        return 1;
    
    head->item_list.prev = &head->item_list;
    head->item_list.next = &head->item_list;
    head->cur_item = NULL;
    head->count = 0;
    list_add(&head->list, &MenuHead);
    
    return 0;
}

/***************************************************************************************************
* Description:  ɾ����ǰ���б�
***************************************************************************************************/
int32_t DeletMenu(void)
{
    struct menu_head *menu_list;
    struct list_head *it, *temp;
    struct menu_item *item;
    
    if(list_empty(&MenuHead))
        return 1;
    menu_list = list_first_entry(&MenuHead, struct menu_head, list);
    
    list_for_each_safe(it, temp, &menu_list->item_list)
    {
        item = list_entry(it, struct menu_item, list);
        list_del(it);
        free(item);
    }
    
    list_del(MenuHead.next);
    free(menu_list);
    
    if(list_empty(&MenuHead))
        return 1;

    return 0;
}

/***************************************************************************************************
* Description:  ���һ���ڵ�
***************************************************************************************************/
int32_t AddMenuItem(const struct menu_content *menuct)
{
    struct menu_head *menu_list;
    struct menu_item *item;
    
    uint8_t index;
    
    if(menuct == NULL)
        return 1;
    
    if(list_empty(&MenuHead))
        return 1;
    menu_list = list_first_entry(&MenuHead, struct menu_head, list);
    
    /* �����ڴ� */
    item = (struct menu_item *)malloc(sizeof(struct menu_item));
    if(item == NULL)
        return 1;
    
    /* �����½ڵ�����к� */
    if(list_empty(&menu_list->item_list))
    {
        index = 0;
        menu_list->cur_item = &item->list;
    }
    else
    {
        struct menu_item *temp;
        temp = list_last_entry(&menu_list->item_list, struct menu_item, list);
        index = temp->index + 1;
    }
    
    /* ������� */
    item->index = index;
    memcpy(item->text, menuct, sizeof(struct menu_content));
    item->func = menuct->func;
    
    list_add_tail(&item->list, &menu_list->item_list);
    return 0;
}

/***************************************************************************************************
* Description:  ��ʾ�˵�
***************************************************************************************************/
void ShowCurMenu(void)
{
    struct menu_head *menu_list;
    struct list_head *it;
    struct menu_item *item;
    uint8_t count = 0;
    
    if(list_empty(&MenuHead))
        return;
    menu_list = list_first_entry(&MenuHead, struct menu_head, list);
    
    if(list_empty(&menu_list->item_list))
        return;
    
    visca_title_cmd(0x0F, TITLE_CLEAR, 1);
    visca_title_set(sel_line, 0, TITLE_WHITE, 0, 1);
    visca_title_cmd(0x0F, TITLE_OPEN, 1);
    list_for_each(it, &menu_list->item_list)
    {
        /* ��ʾ���� */
        item = list_entry(it, struct menu_item, list);
        if(item->func != NULL)
        item->func(item, MENU_REFRESH);
        if(&item->list == menu_list->cur_item)
        {
            sel_line = item->index;
            visca_title_set(sel_line, 0, TITLE_RED, 0, 1);
        }
        visca_title_show(count, item->text, 1);
        if(++count > 11)
            break;
    }
}

/***************************************************************************************************
* Description:  ��ʾ��ǰ��
***************************************************************************************************/
void ShowCurLine(void)
{
    struct menu_head *menu_list;
    struct menu_item *item;
    
    if(list_empty(&MenuHead))
        return;
    menu_list = list_first_entry(&MenuHead, struct menu_head, list);
    
    if(list_empty(&menu_list->item_list))
        return;
    item = list_entry(menu_list->cur_item, struct menu_item, list);
    
    if(item->func != NULL)
        item->func(item, MENU_REFRESH);
    visca_title_show(item->index, item->text, 1);
    
}

/***************************************************************************************************
* Description:  ��ȡ��ǰ�˵�ָ���е��ı�buffer��ַ
***************************************************************************************************/
uint8_t * GetMenuCurLineText(int32_t line)
{
    struct menu_head *menu_list;
    struct menu_item *item;
    struct list_head *it;
    
    if(list_empty(&MenuHead))
        return NULL;
    menu_list = list_first_entry(&MenuHead, struct menu_head, list);
    
    if(list_empty(&menu_list->item_list))
        return NULL;
    
    if(line == -1)
    {
        item = list_entry(menu_list->cur_item, struct menu_item, list);
        return item->text;
    }
    else
    {
        list_for_each(it, &menu_list->item_list)
        {
            /* ��ʾ���� */
            item = list_entry(it, struct menu_item, list);
            
            if(item->index == line)
                return item->text;
        }
        return NULL;
    }
}

/***************************************************************************************************
* Description:  ��ǰ�б����Ϲ���
***************************************************************************************************/
void MenuRollUp(void)
{
    struct menu_head *menu_list;
    struct menu_item *item;
    
    if(list_empty(&MenuHead))
        return ;
    menu_list = list_first_entry(&MenuHead, struct menu_head, list);
    
    if(list_empty(&menu_list->item_list))
        return;
    
    visca_title_set(sel_line, 0, TITLE_WHITE, 0, 1);
    
    menu_list->cur_item = menu_list->cur_item->prev;
    if(menu_list->cur_item == &menu_list->item_list)
        menu_list->cur_item = menu_list->cur_item->prev;
    
    item = list_entry(menu_list->cur_item, struct menu_item, list);
    sel_line = item->index;
    visca_title_set(sel_line, 0, TITLE_RED, 0, 1);
}

/***************************************************************************************************
* Description:  ��ǰ�б����¹���
***************************************************************************************************/
void MenuRollDown(void)
{
    struct menu_head *menu_list;
    struct menu_item *item;
    
    if(list_empty(&MenuHead))
        return;
    menu_list = list_first_entry(&MenuHead, struct menu_head, list);
    
    if(list_empty(&menu_list->item_list))
        return;
    
    visca_title_set(sel_line, 0, TITLE_WHITE, 0, 1);
    
    menu_list->cur_item = menu_list->cur_item->next;
    if(menu_list->cur_item == &menu_list->item_list)
        menu_list->cur_item = menu_list->cur_item->next;
    
    item = list_entry(menu_list->cur_item, struct menu_item, list);
    sel_line = item->index;
    visca_title_set(sel_line, 0, TITLE_RED, 0, 1);
}

/***************************************************************************************************
* Description:  ָ���б����
***************************************************************************************************/
void MenuExeFunc(int32_t arg)
{
    struct menu_head *menu_list;
    struct menu_item *item;
    
    if(list_empty(&MenuHead))
        return;
    menu_list = list_first_entry(&MenuHead, struct menu_head, list);
    
    if(list_empty(&menu_list->item_list))
        return;
    item = list_entry(menu_list->cur_item, struct menu_item, list);
    
    if(item->func != NULL)
        item->func((void *)(item->func), arg);    
}










/****************************************** END OF FILE *******************************************/
