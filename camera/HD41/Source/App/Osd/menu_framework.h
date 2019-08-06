/***************************************************************************************************
* FILE: menu_framework.h
*
* DESCRIPTION: --
*
* CREATED: 2019/1/3, by kevin
***************************************************************************************************/
#ifndef __MENU_FRAMEWORK_H__
#define __MENU_FRAMEWORK_H__

#include "stdint.h"
#include "llist.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MENU_REFRESH    (-1)
#define MENU_TRIGGER    (-2)
#define MENU_UP         (-3)
#define MENU_DOWN       (-4)
#define MENU_LEFT       (-5)
#define MENU_RIGHT      (-6)
#define MENU_SELECT     (-7)
#define MENU_ENTER      (-8)
#define MENU_BACK       (-9)
    
struct menu_item;

struct menu_content
{
    uint8_t text[20];
    int32_t (*func)(struct menu_item *pthis, int32_t todo);
};

struct menu_item
{
    struct list_head list;
    uint8_t index;
    uint8_t text[20];
    int32_t (*func)(struct menu_item *pthis, int32_t todo);
};

struct menu_head
{
    struct list_head list;
    struct list_head item_list;
    struct list_head *cur_item;
    uint8_t count;
    
};

int32_t NewMenu(void);
int32_t DeletMenu(void);
int32_t AddMenuItem(const struct menu_content *menuct);
void ShowCurMenu(void);
void ShowCurLine(void);
uint8_t * GetMenuCurLineText(int32_t line);
void MenuRollUp(void);
void MenuRollDown(void);
void MenuExeFunc(int32_t arg);

#ifdef __cplusplus
}
#endif

#endif
/****************************************** END OF FILE *******************************************/
