/***************************************************************************************************
* FILE: menu.h
*
* DESCRIPTION: --
*
* CREATED: 2019/1/3, by kevin
***************************************************************************************************/
#ifndef __MENU_H__
#define __MENU_H__

#ifdef __cplusplus
extern "C" {
#endif
    



#define MENU_OFF        (0)
#define MENU_ON         (1)
#define MENU_CAM        (2)
#define MENU_DEBUG      (3)


void MENU_Open(void);
void MENU_Back(void);
void MENU_Enter(void);
void MENU_Up(void);
void MENU_Down(void);
void MENU_Left(void);
void MENU_Right(void);
void MENU_Num(uint8_t num);

void ShowBrief(void);

#ifdef __cplusplus
}
#endif

#endif
/****************************************** END OF FILE *******************************************/


