/***************************************************************************************************
* FILE: ui_framework.h
*
* DESCRIPTION:  --
*
* CREATED: 2018/8/21, by kevin
***************************************************************************************************/
#ifndef __UI_FRAMEWORK_H_
#define __UI_FRAMEWORK_H_

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "llist.h"

#include "GUI.h"
#include "FRAMEWIN.h"
#include "TEXT.h"





/***************************************************************************************************
* DEFINES
***************************************************************************************************/
#define X_SIZE			400
#define Y_SIZE			240

#define ALIGN_LEFT		10
#define ALIGN_RIGHT		10
#define ALIGN_TOP		10
#define ALIGN_BOTTOM	10

#define GET_TEXT_HEIGHT(n)	(n+n/2)

/* 颜色定义 */
#define BK_COLOR_0		GUI_MAKE_COLOR(0x2d57b5)
#define BK_COLOR_1		GUI_MAKE_COLOR(0x5073c1)

#define FONT_COLOR_0	GUI_WHITE
#define FONT_COLOR_1	0xFFA0A0A0

#define PBAR_COLOR		GUI_WHITE

enum onoff
{
	e_off,
	e_on
};

enum roll_dir
{
	roll_up,
	roll_down
};
enum func_arg
{
	arg_getbrief = -1,
    arg_getin = -2,
    arg_back = -3,
    arg_enter = -4,
    arg_up = -5,
    arg_down = -6,
	arg_left = -7,
	arg_right = -8,
};

enum uitem_type
{
    uitem_func,
    uitem_list,
    uitem_sel,
    uitem_sel_no_sign,
};


struct menu_item
{
	char text[2][30];
	int32_t (*func)(int32_t);
};

struct menu_data
{
	char title[2][30];
	int32_t type;
	int32_t leng;
	void *item;
};

struct ip_item
{
    struct list_head list;
    char ip_addr[20];
    int32_t index;
};

struct uitem
{
    int32_t type;
    int32_t genius;
	char text[32];
};

struct list_item
{
	struct list_head list;
	int32_t index;
    int32_t type;
    int32_t genius;
	char text[32];
};

struct ulist_handle
{
	char title[30];		//标题
	int32_t type;			//类型，0--打开是下级菜单，1--打开是应用
	int32_t count;			//元素个数
    int32_t sel_index;      //选中的元素
	int32_t screen_pos;		//当前元素在屏幕中显示的位置
    int32_t show_index;     //显示序列号
	struct list_head item_list;	//元素链表头
	struct list_head *cur_item;	//当前元素指针
};

#define LIST_HANDLE(name) \
	struct ulist_handle name = {"", 0, 0, {&name.item_list, &name.item_list}, 0};


struct menu_stack
{
	struct list_head list;
	struct ulist_handle *menu;
};

#define LIST_STACK_HEAD(name)\
	struct menu_stack name = {{&name.list, &name.list}, NULL};
/***************************************************************************************************
* VARIABLES DECLARATION
***************************************************************************************************/
//字体
extern GUI_CONST_STORAGE GUI_FONT GUI_FontArial24;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontArial28;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontArial36;

extern GUI_CONST_STORAGE GUI_FONT GUI_FontMicrosoft_YaHei_UI23;

extern GUI_CONST_STORAGE GUI_FONT GUI_FontConsolas24;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontConsolas25;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontConsolas26;

/***************************************************************************************************
* FUNCTION DECLARATION
***************************************************************************************************/
int32_t UList_Init(struct ulist_handle **phandle, const char *title, int32_t index_onoff);
int32_t Ulist_AppendItem(struct ulist_handle *phandle, const struct uitem *item, int32_t length);


//int32_t UList_Build(struct ulist_handle **menu, const struct menu_data *data, enum onoff indextext);
//int32_t UList_Build2(struct ulist_handle **menu, const char (*str)[30], int32_t leng, int32_t sel_index, enum onoff indextext);
//int32_t UList_Build3(struct ulist_handle **menu, struct list_head *cam_list);
//int32_t UList_Build4(struct ulist_handle **menu, uint32_t *arr, uint32_t length);

int32_t UList_ShowList(struct ulist_handle* plist);
int32_t UList_Show(struct ulist_handle *plist);
int32_t ShowHint(const char *title, const char *str);

int32_t UList_Roll(struct ulist_handle *plist, enum onoff cyclic);
int32_t Ulist_FindSelectedItem(struct ulist_handle *plist);
int32_t UList_RollSelect(struct ulist_handle *plist, int32_t *reg);
int32_t UList_Handle(struct ulist_handle *plist, int32_t (*call_back)(int32_t));
int32_t Ulist_SetSel(struct ulist_handle *plist);

int32_t UList_Destroy(struct ulist_handle *plist);

int32_t UList_StackPush(struct list_head *stack_list, struct ulist_handle *menu);
int32_t UList_StackPop(struct list_head *stack_list, struct ulist_handle **menu);

uint32_t Edit_IpAddress(uint32_t ip);

#endif
/****************************************** END OF FILE *******************************************/
