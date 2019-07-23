/***************************************************************************************************
* FILE: ir_cmd.h

* DESCRIPTION: 红外码值定义，一条指令由一个32位无符号数实现，高八位表示红外地址，低24位表示代码

* CREATED: 2018/11/22, by kevin
***************************************************************************************************/
#ifndef __IR_CMD_DEF_H__
#define __IR_CMD_DEF_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* 红外指令码值定义 */
#define IR_CMD_POWER                (0x01)
#define IR_CMD_PTZ_RESET            (0x02)
#define IR_CMD_MENU_OPEN            (0x03)
#define IR_CMD_MENU_CLOSE           (0x04)
#define IR_CMD_S_OUT                (0x05)
#define IR_CMD_DATASCREEN           (0x06)
#define IR_CMD_HOME                 (0x07)
#define IR_CMD_UP                   (0x08)
#define IR_CMD_DOWN                 (0x09)
#define IR_CMD_LEFT                 (0x0A)
#define IR_CMD_RIGHT                (0x0B)
#define IR_CMD_FOCUS_AUTO           (0x0C)
#define IR_CMD_FOCUS_MANUAL         (0x0D)
#define IR_CMD_FOCUS_FAR            (0x0E)
#define IR_CMD_FOCUS_NEAR           (0x0F)
#define IR_CMD_ZOOM_TELE_SLOW       (0x10)      
#define IR_CMD_ZOOM_WIDE_SLOW       (0x11)
#define IR_CMD_ZOOM_TELE_FAST       (0x12)
#define IR_CMD_ZOOM_WIDE_FAST       (0x13)
#define IR_CMD_BACKLIGHT            (0x14)
#define IR_CMD_BRIGHT_UP            (0x15)
#define IR_CMD_BRIGHT_DOWN          (0x16)
#define IR_CMD_F1                   (0x20)
#define IR_CMD_F2                   (0x21)
#define IR_CMD_F3                   (0x22)
#define IR_CMD_F4                   (0x23)
#define IR_CMD_F5                   (0x24)
#define IR_CMD_F6                   (0x25)
#define IR_CMD_F7                   (0x26)
#define IR_CMD_F8                   (0x27)

#define IR_CMD_NUM_CALL             (0x0100)
#define IR_CMD_NUM_SET              (0x0200)
#define IR_CMD_NUM_RESET            (0x0300)

#define IR_CMD_NUM_MODE(n)          (((n)&0x0300)>>8)



#ifdef __cplusplus
 }
#endif
     
#endif
/****************************************** END OF FILE *******************************************/
