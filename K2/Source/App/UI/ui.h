/***************************************************************************************************
* FILE: ui.h
*
* DESCRIPTION:  --
*
* CREATED: 2018/8/10, by kevin
***************************************************************************************************/
#ifndef __UI_H_
#define __UI_H_

#include "ui_framework.h"

/***************************************************************************************************
* DEFINES
***************************************************************************************************/

/***************************************************************************************************
* VARIABLES DECLARATION
***************************************************************************************************/

/* Î»Í¼ */
extern GUI_CONST_STORAGE GUI_BITMAP bmlogo;
extern GUI_CONST_STORAGE GUI_BITMAP bmnet_ready;
extern GUI_CONST_STORAGE GUI_BITMAP bmkatov_mini;


/* ×ÖÌå */
extern GUI_CONST_STORAGE GUI_FONT GUI_FontArial24;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontArial28;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontArial36;



/***************************************************************************************************
* FUNCTION DECLARATION
***************************************************************************************************/
void func_MainTask(void * argument);
void UI_DrawMainFrame(void);



void UI_ShowNotSupport(void);
void UI_ShowOpNum(void);
void UI_ShowAction(const char *str, int clear);
void UI_ShowCamAddr(void);


void UI_ShowNetState(void);

void UI_ShowSentPacket(void);
void UI_ShowRecvPacket(void);
void UI_StatusIcon_Handler(void);

#endif
/****************************************** END OF FILE *******************************************/
