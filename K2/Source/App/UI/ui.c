/***************************************************************************************************
* FILE: ui.c

* DESCRIPTION:  --

* CREATED: 2018/8/9, by kevin






***************************************************************************************************/

/***************************************************************************************************
* INCLUDES
***************************************************************************************************/
#include "bsp.h"
#include "ui.h"
#include "os.h"
#include "menu.h"
#include "system.h"
#include "icon.h"

/***************************************************************************************************
* DEFINES
***************************************************************************************************/


/***************************************************************************************************
* VARIABLES
***************************************************************************************************/


/***************************************************************************************************
* FUNCTION DECLARATION
***************************************************************************************************/


/***************************************************************************************************
* Description:  显示状态图标
***************************************************************************************************/
void UI_ShowStatusIcon(int type, int arg)
{
    
    
    
}

/***************************************************************************************************
* Description:  显示当前选择的相机地址
***************************************************************************************************/
void UI_ShowCamAddr(void)
{
    char addr_buf[15] = {0};
    uint32_t addr;
    
    //设置字体
	GUI_SetFont(&GUI_FontArial36);
    GUI_SetColor(FONT_COLOR_0);
    
    addr = GetCurAddr();
    
    //IP地址
    if(stSysPara.ptc.type == VISCA_IP || stSysPara.ptc.type == ONVIF)
    {
        IpAddrToString(addr_buf, addr);
    }
    //1~255地址
    else
    {
        sprintf(addr_buf, "Address: %d", addr);
    }
    GUI_DispStringAt(addr_buf, 20, 75);
    GUI_SetColor(BK_COLOR_1);
    GUI_FillRect(GUI_GetDispPosX(), 75, 275, 75+36);
}

/***************************************************************************************************
* Description:  
***************************************************************************************************/
void UI_ShowAction(const char *str, int clear)
{
    GUI_RECT rect = {20, 150, 379, 229};
    
    GUI_SetFont(&GUI_FontArial28);
    GUI_SetColor(BK_COLOR_1);
    GUI_FillRectEx(&rect);
    
    GUI_SetColor(FONT_COLOR_0);
    GUI_DispStringInRectWrap(str, &rect, GUI_TA_LEFT, GUI_WRAPMODE_WORD);
}

/***************************************************************************************************
* Description:  
***************************************************************************************************/
void UI_ShowNotSupport(void)
{
    const char textNotSuppurt[] = "Unsupported operations";
    UI_ShowAction(textNotSuppurt, 0);
}

/***************************************************************************************************
* Description:  
***************************************************************************************************/
void UI_ShowOpNum(void)
{
    char num_buf[4] = " ";
    
    GUI_SetColor(BK_COLOR_1);
    GUI_FillRoundedRect(285, 70, 389, 120, 4);
    
    GUI_SetColor(FONT_COLOR_0);
	GUI_SetFont(&GUI_FontArial36);
    sprintf(num_buf, "%d", stRunInfo.op_num);
    GUI_DispStringHCenterAt(num_buf, 337, 77);
}

/***************************************************************************************************
* Description:  显示网络
***************************************************************************************************/
void UI_ShowNetState(void)
{
    if(stRunInfo.menu == 0)
    {
        if(gnetif.flags & NETIF_FLAG_LINK_UP)
        {
            GUI_PNG_Draw(_acnet_ready, sizeof(_acnet_ready), 295, 25);
        }
        else
        {
            GUI_PNG_Draw(_acnet_notready, sizeof(_acnet_notready), 295, 25);
        }
    }
}

/***************************************************************************************************
* Description:  
***************************************************************************************************/
#define ICON_SHOW_TIME      500
static uint8_t SentIconShow = 0;
static uint8_t RecvIconShow = 0;
void UI_ShowSentPacket(void)
{
    SentIconShow = 2;
}
void UI_ShowRecvPacket(void)
{
    RecvIconShow = 2;
}
void UI_StatusIcon_Handler(void)
{
    if(SentIconShow > 1)
    {
        SentIconShow = 1;
        stRunInfo.sent_timer = HAL_GetTick();
        GUI_PNG_Draw(_acsent, sizeof(_acsent), 330, 25);
    }
    
    if(RecvIconShow > 1)
    {
        RecvIconShow = 1;
        stRunInfo.recv_timer = HAL_GetTick();
        GUI_PNG_Draw(_acrecv, sizeof(_acrecv), 355, 25);
    }
    
    if((SentIconShow==1) && CheckTimeDiff(stRunInfo.sent_timer, ICON_SHOW_TIME))
    {
        SentIconShow = 0;
        GUI_SetColor(BK_COLOR_1);
        GUI_FillRect(330, 25, 330+22, 25+26);
    }
    if((RecvIconShow==1) && CheckTimeDiff(stRunInfo.recv_timer, ICON_SHOW_TIME))
    {
        RecvIconShow = 0;
        GUI_SetColor(BK_COLOR_1);
        GUI_FillRect(355, 25, 355+22, 25+26);
    }
}

/***************************************************************************************************
* Description:  
***************************************************************************************************/
extern const struct uitem uitem_ProtcSetting[];

const char textProtocol[][32] = 
{
  "VISCA Over IP",
  "VISCA",
  "PELCO D",
  "PELCO P",
};

void UI_DrawMainFrame(void)
{
    stRunInfo.menu = 0;
    
    GUI_SetTextMode(GUI_TM_NORMAL);
    GUI_SetBkColor(BK_COLOR_1);
    
	/* 填充背景色 */
	GUI_SetColor(BK_COLOR_0);
	GUI_FillRect(0, 0, 399, 239);
    
    /* 填充第二层背景圆角矩形 */
    GUI_SetColor(BK_COLOR_1);
	GUI_FillRoundedRect(10, 10, 275, 120, 4);
    GUI_FillRoundedRect(285, 10, 389, 60, 4);
    GUI_FillRoundedRect(285, 70, 389, 120, 4);
    GUI_FillRoundedRect(10, 130, 389, 229, 4);
    
    /* 显示协议 */
    GUI_SetColor(FONT_COLOR_0);
	GUI_SetFont(&GUI_FontArial36);
    GUI_DispStringAt(textProtocol[stSysPara.ptc.type], 20, 25);
    
    /* 显示相机地址 */
    UI_ShowCamAddr();
    
    /* 显示操作数 */
    UI_ShowOpNum();
    
    /* 显示欢迎 */
    UI_ShowAction("Welcome!", 1);
    
    /* 显示状态图标 */
    UI_ShowNetState();
}






/****************************************** END OF FILE *******************************************/
