/***************************************************************************************************
* FILE: ui_framework.c
*
* DESCRIPTION:  --
*
* CREATED: 2018/8/21, by kevin
***************************************************************************************************/

/***************************************************************************************************
* INCLUDES
***************************************************************************************************/
#include "ui_framework.h"

#include "rocker.h"
#include "key.h"
#include "led.h"
#include "system.h"

/***************************************************************************************************
* DEFINES
***************************************************************************************************/

/***************************************************************************************************
* VARIABLES
***************************************************************************************************/

/***************************************************************************************************
* FUNCTION DECLARATION
***************************************************************************************************/
int32_t SelectCustomAddress(int32_t arg);



/***************************************************************************************************
* Description:  初始化一个列表
***************************************************************************************************/
int32_t UList_Init(struct ulist_handle **phandle, const char *title, int32_t index_onoff)
{
    struct ulist_handle *new_list;

	//申请列表句柄内存，并初始化
	new_list = (struct ulist_handle *)malloc(sizeof(struct ulist_handle));
	if (new_list == NULL)
    {
        SYS_LOG("ERROR: Malloc Failure!");
		return ERR_MEM;
    }
    
	memset(new_list, 0, sizeof(struct ulist_handle));
	new_list->item_list.next = &new_list->item_list;
	new_list->item_list.prev = &new_list->item_list;
    
	//填充标题文本
	strcat(new_list->title, title);
    
    //初始化位置信息
	new_list->screen_pos = 0;
	new_list->cur_item = new_list->item_list.next;
    new_list->show_index = index_onoff;
	*phandle = new_list;
    return ERR_NO;
}

/***************************************************************************************************
* Description:  在列表尾部添加列表项
* Arguments:    lhandle：列表指针
*               item：列表项数组指针
*               length：列表项个数
* Returns:      错误代码
***************************************************************************************************/
int32_t Ulist_AppendItem(struct ulist_handle *phandle, const struct uitem *item, int32_t length)
{
    struct list_item *new_item;

    for(int32_t i=0; i<length; i++)
    {
        //申请列表节点内存并初始化
		new_item = (struct list_item *)malloc(sizeof(struct list_item));
		if (new_item == NULL)
        {
			SYS_LOG("ERROR: Malloc Failure!");
            return ERR_MEM;
        }
		memset(new_item, 0, sizeof(struct list_item));
        
        /* 填充索引号 */
        new_item->index = phandle->count;
        phandle->count++;

        /* 复制数据 */
        memcpy(&(new_item->type), &item[i], sizeof(struct uitem));
        
        list_add_tail(&(new_item->list), &(phandle->item_list));
    }
    
    if(phandle->cur_item == &phandle->item_list)
    {
        if(!list_empty(&phandle->item_list))
        {
           phandle->cur_item = phandle->item_list.next;
        }
    }

    return ERR_NO;
}

/***************************************************************************************************
* Description:  显示列表,不包括标题
* Arguments:    plist:列表句柄
* Returns:      none
***************************************************************************************************/
int32_t UList_ShowList(struct ulist_handle* plist)
{
	int32_t height;
	int32_t xs, xe, ys, ye;
    
	int32_t start_index;
	int32_t show_count = 0;
    
	struct list_item *cur_item;
	struct list_head *it;
	struct list_item *tmp;
    
    
	cur_item = list_entry(plist->cur_item, struct list_item, list);
	start_index = cur_item->index - plist->screen_pos;
    
	height = GET_TEXT_HEIGHT(24);
    
	xs = 0;
	xe = plist->count > 5 ? 394 : 399;
	ys = 42 + 3;
	ye = ys + height;
    
    
	list_for_each(it, &plist->item_list)
	{
		tmp = list_entry(it, struct list_item, list);
		if (tmp->index >= start_index)
		{
			/* 填充背景 */
			if (tmp->index == cur_item->index)
				GUI_SetColor(0xFF33AA77);
			else
				GUI_SetColor(BK_COLOR_1);
			GUI_FillRect(xs, ys, xe, ye);
            
			/* 显示字符串 */
			GUI_SetColor(FONT_COLOR_0);
			GUI_SetFont(&GUI_FontArial24);
            if(plist->show_index)
            {
                char num_str[5];
                sprintf(num_str, "%d. ", tmp->index+1);
                GUI_DispStringAt(num_str, xs + 5, ys + 6);
                GUI_DispString(tmp->text);
            }
            else
            {
                GUI_DispStringAt(tmp->text, xs + 5, ys + 6);
            }
            
            switch(tmp->type)
            {
                case uitem_func:
                if (tmp->genius)
                {
                    GUI_SetTextAlign(GUI_TA_RIGHT);
                    const char *str = (const char *)((int32_t (*)(int32_t))tmp->genius)(arg_getbrief);
					GUI_DispStringAt(str, xs + 365, ys + 6);
					GUI_SetTextAlign(GUI_TA_LEFT);
                    GUI_DispCharAt('>', 370, ys + 6);
                }
                
                break;
                case uitem_list:
                GUI_DispCharAt('>', 370, ys + 6);
                break;
                case uitem_sel:
                if(tmp->index == plist->sel_index)
                    GUI_DispCharAt(127, 370, ys + 6);
                break;
                case uitem_sel_no_sign:
                
                break;
                default:
                break;
            }

			ys = ye + 3;
			ye = ys + height;
            
			if (++show_count >= 5)
				break;
		}
	}
    
	/* 显示进度条 */
	if (plist->count > 5)
	{
		int32_t TotalLen = 193;
		int32_t BlankLen1 = 0;	//进度条上方空白长度
		int32_t PBLen = 0;		//进度条长度
		int32_t BlankLen2 = 0;	//进度条下方空白长度
		float per = 5.0f / plist->count;
		if (per < 0.1f) per = 0.1f;
        
		PBLen = (int32_t)(TotalLen * per);
		BlankLen1 = (TotalLen - PBLen) * (cur_item->index - plist->screen_pos) / (plist->count - 5);
		BlankLen2 = TotalLen - PBLen - BlankLen1;
        
		ys = 45 + BlankLen1;
		ye = 237 - BlankLen2;
        
		GUI_SetColor(BK_COLOR_0);
		GUI_FillRect(397, 45, 399, 239);
		GUI_SetColor(PBAR_COLOR);
		GUI_FillRect(397, ys, 399, ye);
	}
    
	return 0;
}

/***************************************************************************************************
* Description:  显示列表
* Arguments:    plist:列表句柄
* Returns:      none
***************************************************************************************************/
int32_t UList_Show(struct ulist_handle *plist)
{
	int32_t height;
	int32_t xs, xe, ys, ye;
    
	GUI_SetTextMode(GUI_TM_TRANS);
    
	/* 填充背景色 */
	GUI_SetColor(BK_COLOR_0);
	GUI_FillRect(0, 0, 399, 239);
    
	height = GET_TEXT_HEIGHT(28);
    
	xs = 0;
	xe = 399;
	ys = 0;
	ye = ys + height;
    
	/* 填充标题背景 */
	GUI_SetColor(BK_COLOR_1);
	GUI_FillRect(xs, ys, xe, ye);
    
	/* 显示标题 */
	GUI_SetColor(FONT_COLOR_0);
	GUI_SetFont(&GUI_FontArial28);
	GUI_DispStringHCenterAt(plist->title, 199, xs + 7);
    
	UList_ShowList(plist);
    
	return 0;
}

/***************************************************************************************************
* Description:  
***************************************************************************************************/
int32_t ShowHint(const char *title, const char *str)
{
    int32_t height;
	int32_t xs, xe, ys, ye;
    
	GUI_SetTextMode(GUI_TM_TRANS);
    
	/* 填充背景色 */
	GUI_SetColor(BK_COLOR_0);
	GUI_FillRect(0, 0, 399, 239);
    
	height = GET_TEXT_HEIGHT(28);
    
	xs = 0;
	xe = 399;
	ys = 0;
	ye = ys + height;
    
	/* 填充文字背景 */
	GUI_SetColor(BK_COLOR_1);
	GUI_FillRect(xs, ys, xe, ye);
    
    ys = ye + 3;
    ye = 239;
    GUI_FillRect(xs, ys, xe, ye);
    
	/* 显示标题 */
	GUI_SetColor(FONT_COLOR_0);
	GUI_SetFont(&GUI_FontArial28);
	GUI_DispStringHCenterAt(title, 199, xs + 7);

    GUI_RECT rect = {xs, ys, xe, ye-30};
    GUI_DispStringInRect(str, &rect, GUI_TA_HCENTER|GUI_TA_VCENTER);
        
    return 0;
}

/***************************************************************************************************
* Description:  上下滚动列表
* Arguments:    plist:列表句柄
*               dir:滚动方向
*               cyclic:循环滚动使能,0-禁止循环,其他值使能
* Returns:      none
***************************************************************************************************/
int32_t UList_Roll(struct ulist_handle *plist, enum onoff cyclic)
{
	struct list_head *it;
    int8_t rocker_val;
	if (plist == NULL)
		return -1;
    
    if(Rocker_GetTiltVal(&rocker_val, 2))
    {
        if(rocker_val > 0)
        {
            it = plist->cur_item->prev;
            if (&(plist->item_list) == it)
            {
                /* 使能循环 */
                if (cyclic)
                {
                    it = plist->item_list.prev;
                    plist->screen_pos = 4;
                }
                else
                {
                    return 2;
                }
            }
            else
            {
                if (--plist->screen_pos < 0) plist->screen_pos = 0;
            }
            
            plist->cur_item = it;
            UList_ShowList(plist);
        }
        else if(rocker_val < 0)
        {
            it = plist->cur_item->next;
            if (&(plist->item_list) == it)
            {
                /* 使能循环 */
                if (cyclic)
                {
                    it = plist->item_list.next;
                    plist->screen_pos = 0;
                }
                else
                {
                    return 2;
                }
            }
            else
            {
                if (++plist->screen_pos > 4) plist->screen_pos = 4;
            }
            plist->cur_item = it;
            UList_ShowList(plist);
        }
    }
    return 0;
}

int32_t Ulist_SetSel(struct ulist_handle *plist)
{
    struct list_head *it;
    struct list_item *tmp;
    list_for_each(it, &plist->item_list)
    {
        tmp = list_entry(it, struct list_item, list);
        if(tmp->type == uitem_sel)
        {
            if(it == plist->cur_item)
            {
                tmp->genius = 1;
            }
            else
            {
                tmp->genius = 0;
            }
        }
    }
    
    plist->sel_index = (list_entry(plist->cur_item, struct list_item, list))->index;
    return plist->sel_index;
}

/***************************************************************************************************
* Description:  
***************************************************************************************************/
int32_t EnterFuncCallBack(struct ulist_handle *plist)
{
    
    return 0;
}
int32_t EnterListCallBack(struct ulist_handle *plist)
{
        
    return 0;
}
int32_t SingleSelCallBack(struct ulist_handle *plist)
{

//    *reg = Ulist_SetSel(plist);
//                UList_ShowList(plist);
    return 0;
}
int32_t IpSelCallBack(struct ulist_handle *plist)
{
//    struct list_item *tmp = list_entry(plist->cur_item, struct list_item, list);
    return 0;
}

int32_t UList_RollSelect(struct ulist_handle *plist, int32_t *reg)
{
    int8_t rocker_val;

    UList_Roll(plist, e_on);
    
    if(Rocker_GetPanVal(&rocker_val, 3))
    {
        if(rocker_val > 0)
        {
            struct list_item *tmp = list_entry(plist->cur_item, struct list_item, list);
            if(reg) *reg = tmp->index;
            Ulist_SetSel(plist);
            UList_ShowList(plist);
        }
        else if(rocker_val < 0)
        {
            return 1;
        }
    }
    
    if(GetKey(KEY_ROCKER, 0))
    {
        struct list_item *tmp = list_entry(plist->cur_item, struct list_item, list);
        if(reg) *reg = tmp->index;
        Ulist_SetSel(plist);
        UList_ShowList(plist);
    }
    
    if(GetKey(KEY_BACK, 0))
    {
        LED_Ctrl(KEY_BACK, LED_1S);
        return 1;
    }
    
    return 0;
}

int32_t UList_Handle(struct ulist_handle *plist, int32_t (*call_back)(int32_t))
{
    int8_t rocker_val;
    struct list_head *it;
    
    while(1)
    {
        /* 上下 */
        if(Rocker_GetTiltVal(&rocker_val, 2))
        {
            if(rocker_val > 0)
            {
                it = plist->cur_item->prev;
                if (&(plist->item_list) == it)
                {
                    it = plist->item_list.prev;
                    plist->screen_pos = 4;
                }
                else
                {
                    if (--plist->screen_pos < 0) plist->screen_pos = 0;
                }
                
                plist->cur_item = it;
                UList_ShowList(plist);
            }
            else if(rocker_val < 0)
            {
                it = plist->cur_item->next;
                if (&(plist->item_list) == it)
                {
                    it = plist->item_list.next;
                    plist->screen_pos = 0;
                }
                else
                {
                    if (++plist->screen_pos > 4) plist->screen_pos = 4;
                }
                
                plist->cur_item = it;
                UList_ShowList(plist);
            }
        }
        
        /* 左右 */
        if(Rocker_GetPanVal(&rocker_val, 3))
        {
            if(rocker_val > 0)
            {
                if(call_back(arg_right))
                    return 1;
            }
            else if(rocker_val < 0)
            {
                if(call_back(arg_left))
                    return 1;
            }
        }
        
        /* 摇杆按键 */
        if(GetKey(KEY_ROCKER, 0))
        {
            if(call_back(arg_enter))
                return 1;
        }
        
        /* 返回按键 */
        if(GetKey(KEY_BACK, 0))
        {
            LED_Ctrl(KEY_BACK, LED_1S);
            if(call_back(arg_back))
                return 1;
        }
        
        vTaskDelay(10);
    }
}

/***************************************************************************************************
* Description:  设置列表指针为选择的项目
***************************************************************************************************/
int32_t Ulist_FindSelectedItem(struct ulist_handle *plist)
{
    struct list_head *it;
    struct list_item *tmp;
    
    list_for_each(it, &plist->item_list)
	{
		tmp = list_entry(it, struct list_item, list);
        if(tmp->index == plist->sel_index)
        {
            plist->cur_item = it;
            break;
        }
    }
    
    int32_t offset = plist->count-plist->sel_index;
    if(offset > 5) offset = 5;
    plist->screen_pos = 5 - offset;
    return 0;
}

/***************************************************************************************************
* Description:  删除整个列表，包括表头
***************************************************************************************************/            
int32_t UList_Destroy(struct ulist_handle *plist)
{
    list_destroy(&plist->item_list, struct list_item, list);
	free(plist);
    plist = NULL;
    return 0;
}

/***************************************************************************************************
* Description:  列表句柄入栈
***************************************************************************************************/
int32_t UList_StackPush(struct list_head *stack_list, struct ulist_handle *menu)
{
    struct menu_stack *stack;
    
    stack = (struct menu_stack *)malloc(sizeof(struct menu_stack));
    if (stack == NULL)
        return 1;

    stack->menu = menu;
    list_add(&stack->list, stack_list);
    return 0;
}

/***************************************************************************************************
* Description:  列表句柄出栈
***************************************************************************************************/
int32_t UList_StackPop(struct list_head *stack_list, struct ulist_handle **menu)
{
    struct menu_stack *stack;
    
    if (list_empty(stack_list))
        return 1;
    
    stack = list_first_entry(stack_list, struct menu_stack, list);
    *menu = stack->menu;
    
    struct list_head *tmp = stack_list->next;
    list_del(stack_list->next);
    free(list_entry(tmp, struct menu_stack, list));

    return 0;
}

/***************************************************************************************************
* Description:  编辑IP地址，ip：传入的初始值
***************************************************************************************************/
uint32_t Edit_IpAddress(uint32_t ip)
{
    char text[16];
    uint32_t ipaddr = ip;
    uint8_t refresh = 1;
    int8_t md_index = 0;
    int8_t rocker_val;
    int8_t key_val;
    
    /* 把原始IP地址转换为字符串 */
    sprintf(text, "%03d.%03d.%03d.%03d", \
        (uint8_t)(ipaddr>>24), (uint8_t)(ipaddr>>16), \
        (uint8_t)(ipaddr>>8), (uint8_t)(ipaddr));
    
    /* 填充背景色 */
	GUI_SetColor(BK_COLOR_0);
	GUI_FillRect(0, 0, 399, 239);
    
	/* 填充标题背景 */
	GUI_SetColor(BK_COLOR_1);
	GUI_FillRect(0, 0, 399, 0+GET_TEXT_HEIGHT(28));
    
    //字符串显示位置为78~321
    GUI_FillRoundedRect(58, 100-16, 341, 100+36+16, 5);
    
	/* 显示标题 */
	GUI_SetColor(FONT_COLOR_0);
    GUI_SetTextMode(GUI_TM_TRANS);
	GUI_SetFont(&GUI_FontArial28);
	GUI_DispStringHCenterAt("Edit IP Address", 199, 7);
    
    /* 显示IP地址 */
    GUI_SetFont(&GUI_FontArial36);
    GUI_DispStringHCenterAt(text, 199, 100);
    

    GUI_SetBkColor(BK_COLOR_1);
    GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
    while(1)
    {
        /* 刷新显示 */
        if(refresh)
        {
            refresh = 0;
            GUI_GotoXY(78, 100);
            
            for(int32_t i=0; i<15; i++)
            {
                if(i == md_index)
                {
                    GUI_SetTextMode(GUI_TEXTMODE_REV);
                    GUI_DispChar(text[md_index]);
                    GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
                }
                else GUI_DispChar(text[i]);
            }
        }
        
        /* 修改光标对应的数字 */
        key_val = GetKeyNum();
        if(key_val >= 0)
        {
            /* 百位不能大于2 */
            if(md_index == 0 || md_index == 4 || md_index == 8 || md_index == 12)
            {
                if(key_val > 2)
                    continue;
            }
            
            text[md_index] = '0' + key_val;
            if(++md_index > 14)
                md_index = 0;
            if(md_index == 3 || md_index == 7 || md_index == 11)
                ++md_index;
            
            refresh = 1;
        }
        
        /* 左右移动光标 */
        if(Rocker_GetPanVal(&rocker_val, 3))
        {
            if(rocker_val > 0)
            {
                if(++md_index > 14)
                    md_index = 0;
                if(md_index == 3 || md_index == 7 || md_index == 11)
                    ++md_index;
                refresh = 1;
            }
            else if(rocker_val < 0)
            {
                if(--md_index < 0)
                    md_index = 14;
                if(md_index == 3 || md_index == 7 || md_index == 11)
                    --md_index;
                refresh = 1;
            }
        }
        
        /* 退出编辑 */
        if(GetKey(KEY_BACK, 0))
        {
            LED_Ctrl(KEY_BACK, LED_1S);
            
            StringToIpAddr(text, &ipaddr);
            
            break;
        }
        
        vTaskDelay(5);
    }
    
    /* 返回编辑后的IP地址 */
    return ipaddr;
}

/****************************************** END OF FILE *******************************************/
