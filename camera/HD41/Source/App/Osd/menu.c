/***************************************************************************************************
* FILE: menu.c
*
* DESCRIPTION:  --
*
* CREATED: 2019/1/3, by kevin
***************************************************************************************************/
#include "menu_framework.h"
#include "menu.h"
#include "system.h"
#include "ccp.h"
#include "visca2camera.h"
#include "ir_remote.h"


#define EN_MENU


const uint8_t strOnOff[][3] = 
{
    "OFF",
    " ON",  
};

static int32_t _SerialPort(struct menu_item *pthis, int32_t todo);
static int32_t _Protocol(struct menu_item *pthis, int32_t todo);
static int32_t _Address(struct menu_item *pthis, int32_t todo);
static int32_t _BaudRate(struct menu_item *pthis, int32_t todo);
static int32_t _SpeedScale(struct menu_item *pthis, int32_t todo);
static int32_t _SpeedLimit(struct menu_item *pthis, int32_t todo);

static int32_t _Ptz(struct menu_item *pthis, int32_t todo);
static int32_t _Dzoom(struct menu_item *pthis, int32_t todo);
static int32_t _FocusMode(struct menu_item *pthis, int32_t todo);
//static int32_t _AfSens(struct menu_item *pthis, int32_t todo);
static int32_t _RemoteSpeed(struct menu_item *pthis, int32_t todo);
//static int32_t _RemoteSpeedLimit(struct menu_item *pthis, int32_t todo);
static int32_t _PanDir(struct menu_item *pthis, int32_t todo);
static int32_t _TiltDir(struct menu_item *pthis, int32_t todo);
static int32_t _PictureFlip(struct menu_item *pthis, int32_t todo);
static int32_t _PictureMirror(struct menu_item *pthis, int32_t todo);

static int32_t _Exposure(struct menu_item *pthis, int32_t todo);
static int32_t _AeMode(struct menu_item *pthis, int32_t todo);
static int32_t _Shutter(struct menu_item *pthis, int32_t todo);
static int32_t _Iris(struct menu_item *pthis, int32_t todo);
static int32_t _Bright(struct menu_item *pthis, int32_t todo);
static int32_t _Gain(struct menu_item *pthis, int32_t todo);
static int32_t _ExpCompesation(struct menu_item *pthis, int32_t todo);
static int32_t _Backlight(struct menu_item *pthis, int32_t todo);
//static int32_t _Highlight(struct menu_item *pthis, int32_t todo);

static int32_t _Wb(struct menu_item *pthis, int32_t todo);
static int32_t _WbMode(struct menu_item *pthis, int32_t todo);
static int32_t _RGain(struct menu_item *pthis, int32_t todo);
static int32_t _BGain(struct menu_item *pthis, int32_t todo);
static int32_t _WbTrigger(struct menu_item *pthis, int32_t todo);

static int32_t _Picture(struct menu_item *pthis, int32_t todo);
static int32_t _2DNr(struct menu_item *pthis, int32_t todo);
static int32_t _3DNr(struct menu_item *pthis, int32_t todo);
static int32_t _Contrast(struct menu_item *pthis, int32_t todo);
static int32_t _ColorGain(struct menu_item *pthis, int32_t todo);
static int32_t _Sharpness(struct menu_item *pthis, int32_t todo);
//static int32_t _Hue(struct menu_item *pthis, int32_t todo);
static int32_t _Gamma(struct menu_item *pthis, int32_t todo);
static int32_t _Wdr(struct menu_item *pthis, int32_t todo);
static int32_t _Defog(struct menu_item *pthis, int32_t todo);
static int32_t _DeFlicker(struct menu_item *pthis, int32_t todo);

#if defined(EN_NETWORK)
static int32_t _Network(struct menu_item *pthis, int32_t todo);
static int32_t _Dhcp(struct menu_item *pthis, int32_t todo);
static int32_t _Ip(struct menu_item *pthis, int32_t todo);
static int32_t _Mask(struct menu_item *pthis, int32_t todo);
static int32_t _Gateway(struct menu_item *pthis, int32_t todo);
#endif

static int32_t _System(struct menu_item *pthis, int32_t todo);
static int32_t _VideoFormat(struct menu_item *pthis, int32_t todo);
static int32_t _IrAddress(struct menu_item *pthis, int32_t todo);
static int32_t _ShowBrief(struct menu_item *pthis, int32_t todo);
static int32_t _ReloadPreset1(struct menu_item *pthis, int32_t todo);
static int32_t _Version(struct menu_item *pthis, int32_t todo);
static int32_t _Reset(struct menu_item *pthis, int32_t todo);

static int32_t _FactoryPattern(struct menu_item *pthis, int32_t todo);
static int32_t _PresetSavePic(struct menu_item *pthis, int32_t todo);
static int32_t _Rs485(struct menu_item *pthis, int32_t todo);

static int32_t _ResetSystem(struct menu_item *pthis, int32_t todo);
static int32_t _ResetPicture(struct menu_item *pthis, int32_t todo);
#if defined(EN_NETWORK)
static int32_t _ResetNetwork(struct menu_item *pthis, int32_t todo);
#endif
static int32_t _ResetAllSettings(struct menu_item *pthis, int32_t todo);

const struct menu_content menu_main[] = 
{
    {"SERIALPORT SETTINGS ", _SerialPort},
    {"PTZ SETTINGS        ", _Ptz},
    {"EXPOSURE SETTINGS   ", _Exposure},
    {"WB SETTINGS         ", _Wb},
    {"PICTURE SETTINGS    ", _Picture},
#if defined(EN_NETWORK)
    {"NETWORK SETTINGS    ", _Network},
#endif
    {"SYSTEM SETTINGS     ", _System},
};

const struct menu_content menu_serialport[] = 
{
    {"PROTOCOL:           ", _Protocol},
    {"ADDRESS:            ", _Address},
    {"BAUDRATE:           ", _BaudRate},
    {"SPEED SCALE:        ", _SpeedScale},
    {"SPEED LIMIT:        ", _SpeedLimit},
};

const struct menu_content menu_ptz[] = 
{
    {"DZOOM:              ", _Dzoom},
    {"FOCUS MODE:         ", _FocusMode},
    //    {"AF SENS:            ", _AfSens},
    {"REMOTE SPEED:       ", _RemoteSpeed},
    //    {"ADAPTIVE P/T:       ", _RemoteSpeedLimit},
    {"PAN DIR:            ", _PanDir},
    {"TILT DIR:           ", _TiltDir},
    {"FLIP:               ", _PictureFlip},
    {"MIRROR:             ", _PictureMirror},
};

const struct menu_content menu_exposure[] = 
{
    {"AE MODE:            ", _AeMode},
    {"SHUTTER:            ", _Shutter},
    {"IRIS:               ", _Iris},
    {"BRIGHT:             ", _Bright},
    {"GAIN:               ", _Gain},
    {"EXP COMPESATION:    ", _ExpCompesation},
    {"BACKLIGHT:          ", _Backlight},
    //    {"HIGHLIGHT:          ", _Highlight},
};

const struct menu_content menu_wb[] = 
{
    {"WB MODE:            ", _WbMode},
    {"RGAIN:              ", _RGain},
    {"BGAIN:              ", _BGain},
    {"TRIGGER             ", _WbTrigger},
};

const struct menu_content menu_picture[] =
{
    {"2D NR:              ", _2DNr},
    {"3D NR:              ", _3DNr},
    {"CONTRAST:           ", _Contrast},
    {"COLOR GAIN:         ", _ColorGain},
    {"SHARPNESS:          ", _Sharpness},
    //    {"HUE:                ", _Hue},
    {"GAMMA:              ", _Gamma},
    {"WDR:                ", _Wdr},
    {"DEFOG:              ", _Defog},
    {"DE-FLICKER:         ", _DeFlicker},
};
#if defined(EN_NETWORK)
const struct menu_content menu_network[] =
{
    {"DHCP:               ", _Dhcp},
    {"IP:                 ", _Ip},
    {"MASK:               ", _Mask},
    {"GATE:               ", _Gateway},
};
#endif
const struct menu_content menu_system[] =
{
    {"VIDEO FORMAT:       ", _VideoFormat},
    {"IR ADDRESS:         ", _IrAddress},
    {"SHOW BRIEF:         ", _ShowBrief},
    {"RELOAD PRESET 1:    ", _ReloadPreset1},
    {"VERSION: " MENU_VERSION, _Version},
    {"RESET SETTINGS      ", _Reset},
};

const struct menu_content menu_factory[] =
{
    {"FACTORY PATTERN:    ", _FactoryPattern},
    {"PRESET PICTURE:     ", _PresetSavePic},
    {"RS485:              ", _Rs485},
};

const struct menu_content menu_reset[] =
{
    {"RESET SYSTEM        ", _ResetSystem},
    {"RESET PICTURE       ", _ResetPicture},
#if defined(EN_NETWORK)
    {"RESET NETWORK       ", _ResetNetwork},
#endif
    {"RESET ALL SETTINGS  ", _ResetAllSettings},
};


#define DELAY_OSD   300
/***************************************************************************************************
* Description:  打开&退出菜单
***************************************************************************************************/
#ifdef EN_MENU
static uint32_t TimerOpen = 0;
#endif
void MENU_Open(void)
{
#ifdef EN_MENU
    uint8_t count;
    if(!CheckInterval(&TimerOpen, DELAY_OSD))
        return;
    
    if(stSysInfo.video_format_out1 == VIDEO_320x240p60)
        return;
    
    if(stRunInfo.menu != 0)
    {
        MENU_Back();
        return;
    }
    
    NewMenu();
    
    count = sizeof(menu_main) / sizeof(struct menu_content);
    for(int32_t i=0; i<count; i++)
    {
        if(AddMenuItem(&menu_main[i]))
            return;
    }
    
    stRunInfo.menu = MENU_ON;
    
    ShowCurMenu();
#endif
}

/***************************************************************************************************
* Description:  退出菜单
***************************************************************************************************/
void MENU_Back(void)
{
#ifdef EN_MENU
    if(!CheckInterval(&TimerOpen, DELAY_OSD))
        return;
    
    if(stRunInfo.menu == MENU_ON)
    {
        if(DeletMenu())
        {
            visca_title_cmd(0x0F, TITLE_CLOSE, 1);
            visca_title_cmd(0x0F, TITLE_CLEAR, 1);
            stRunInfo.menu = MENU_OFF;
            Ptc_Init();
            CalPtzSpeed();
            IR_AddrDetection();
            System_SavePara();
        }
        else
        {
            ShowCurMenu();
        }
    }
    else if(stRunInfo.menu == MENU_CAM)
    {
        cam_menu_switch();
        visca_title_cmd(0x0F, TITLE_OPEN, 1);
        stRunInfo.menu = MENU_ON;
    }
#endif
}

/***************************************************************************************************
* Description:  --
***************************************************************************************************/
void MENU_Up(void)
{
#ifdef EN_MENU
    static uint32_t TimerUp = 0;
    if(!CheckInterval(&TimerUp, DELAY_OSD))
        return;
    
    if(stRunInfo.menu == MENU_ON)
    {
        MenuRollUp();
    }
    else if(stRunInfo.menu == MENU_CAM)
    {
        cam_menu_up();
    }
#endif
}

/***************************************************************************************************
* Description:  --
***************************************************************************************************/
void MENU_Down(void)
{
#ifdef EN_MENU
    static uint32_t TimerDown = 0;
    if(!CheckInterval(&TimerDown, DELAY_OSD))
        return;
    
    if(stRunInfo.menu == MENU_ON)
    {
        MenuRollDown();
    }
    else if(stRunInfo.menu == MENU_CAM)
    {
        cam_menu_down();
    }
#endif
}

/***************************************************************************************************
* Description:  --
***************************************************************************************************/
void MENU_Left(void)
{
#ifdef EN_MENU
    static uint32_t TimerLeft = 0;
    if(!CheckInterval(&TimerLeft, DELAY_OSD))
        return;
    
    if(stRunInfo.menu == MENU_ON)
    {
        MenuExeFunc(MENU_LEFT);
    }
    else if(stRunInfo.menu == MENU_CAM)
    {
        cam_menu_left();
    }
#endif
}

/***************************************************************************************************
* Description:  --
***************************************************************************************************/
void MENU_Right(void)
{
#ifdef EN_MENU
    static uint32_t TimerRight = 0;
    if(!CheckInterval(&TimerRight, DELAY_OSD))
        return;
    
    if(stRunInfo.menu == MENU_ON)
    {
        MenuExeFunc(MENU_RIGHT);
    }
    else if(stRunInfo.menu == MENU_CAM)
    {
        cam_menu_right();
    }
#endif
}

/***************************************************************************************************
* Description:  确定
***************************************************************************************************/
void MENU_Enter(void)
{
#ifdef EN_MENU
    static uint32_t TimerEnter = 0;
    if(!CheckInterval(&TimerEnter, DELAY_OSD))
        return;
    
    if(stRunInfo.menu == MENU_ON)
    {
        MenuExeFunc(MENU_ENTER);
    }
#endif
}

/***************************************************************************************************
* Description:  数字功能
***************************************************************************************************/
void MENU_Num(uint8_t num)
{
#ifdef EN_MENU
    static uint32_t TimerNum = 0;
    if(!CheckInterval(&TimerNum, DELAY_OSD))
        return;
    
    if(stRunInfo.menu == MENU_ON)
    {
        MenuExeFunc(num);
    }
#endif
}

/***************************************************************************************************
* Description: 把小于1000的数字转换为字符，右对齐
***************************************************************************************************/
uint8_t * NumToString(uint16_t num)
{
    static uint8_t buffer[3] = {0};
    if(num < 10)
    {
        buffer[0] = ' ';
        buffer[1] = ' ';
        buffer[2] = num + '0';
    }
    else if(num < 100)
    {
        buffer[0] = ' ';
        buffer[1] = num / 10 + '0';
        buffer[2] = num % 10 + '0';
    }
    else
    {
        buffer[0] = num / 100 + '0';
        buffer[1] = num % 100 / 10 + '0';
        buffer[2] = num % 100 % 10 + '0';
    }
    return buffer;
}
/* 带正负号 */
uint8_t* NumToStringWithSymbol(int32_t num)
{
    static uint8_t buffer[4] = {0};
    uint8_t start_bit = 0;
    if(num != 0)
    {
        if(num > 0) buffer[0] = '-';
        else buffer[0] = '+';
        start_bit = 1;
        num = abs(num);
    }
    else buffer[3] = ' ';
    
    if(num < 10)
    {
        buffer[start_bit+0] = num + '0';
        buffer[start_bit+1] = ' ';
        buffer[start_bit+2] = ' ';
    }
    else if(num < 100)
    {
        buffer[start_bit+0] = num / 10 + '0';
        buffer[start_bit+1] = num % 10 + '0';
        buffer[start_bit+2] = ' ';
    }
    else
    {
        buffer[start_bit+0] = num / 100 + '0';
        buffer[start_bit+1] = num % 100 / 10+ '0';
        buffer[start_bit+2] = num % 100 % 10+ '0';
    }
    return buffer;
}

/***************************************************************************************************
* Description:  串口设置
***************************************************************************************************/
static int32_t _SerialPort(struct menu_item *pthis, int32_t todo)
{
    switch(todo)
    {
        case MENU_RIGHT:
        case MENU_ENTER:
        {
            uint8_t count;
            NewMenu();
            count = sizeof(menu_serialport) / sizeof(struct menu_content);
            for(int32_t i=0; i<count; i++)
            {
                if(AddMenuItem(&menu_serialport[i]))
                    return 1;
            }
            ShowCurMenu();
        }
        break;
        
        default:
        break;
    }
    return 0;
}
/* 协议 */
static int32_t _Protocol(struct menu_item *pthis, int32_t todo)
{
    const uint8_t strProtocol[][7] = 
    {
        "   AUTO",
        "  VISCA",
        "PELCO D",
        "PELCO P",
    };
    switch(todo)
    {
        case MENU_REFRESH:
        memcpy(&pthis->text[20-7], strProtocol[CUR_PTC_TYPE], 7);
        break;
        
        case MENU_LEFT:
        if(CUR_PTC_TYPE > CCP_AUTO) CUR_PTC_TYPE--;
        else CUR_PTC_TYPE = CCP_PELCO_P;
        break;
        
        case MENU_RIGHT:
        if(CUR_PTC_TYPE < CCP_PELCO_P) CUR_PTC_TYPE++;
        else CUR_PTC_TYPE = CCP_AUTO;
        break;
    }
    
    if(todo == MENU_LEFT || todo == MENU_RIGHT)
    {
        if((CUR_PTC_TYPE == CCP_AUTO) || (CUR_PTC_TYPE == CCP_VISCA))
        {
            if(CUR_PTC_ADDR > 7) CUR_PTC_ADDR = 1;
        }
        ShowCurLine();
    }
    return 0;
}
/* 地址 */
static int32_t _Address(struct menu_item *pthis, int32_t todo)
{
    switch(todo)
    {
        case MENU_REFRESH:
        memcpy(&pthis->text[20-3], NumToString(CUR_PTC_ADDR), 3);
        break;
        
        case MENU_LEFT:
        if((CUR_PTC_TYPE == CCP_AUTO) || (CUR_PTC_TYPE == CCP_VISCA))
        {
            if(CUR_PTC_ADDR > 1) CUR_PTC_ADDR--;
            else CUR_PTC_ADDR = 7;
        }
        else if(CUR_PTC_TYPE == CCP_PELCO_D || CUR_PTC_TYPE == CCP_PELCO_P)
        {
            if(CUR_PTC_ADDR > 0) CUR_PTC_ADDR--;
            else CUR_PTC_ADDR = 255;
        }
        ShowCurLine();
        break;
        
        case MENU_RIGHT:
        if((CUR_PTC_TYPE == CCP_AUTO) || (CUR_PTC_TYPE == CCP_VISCA))
        {
            if(CUR_PTC_ADDR < 7) CUR_PTC_ADDR++;
            else CUR_PTC_ADDR = 1;
        }
        else if(CUR_PTC_TYPE == CCP_PELCO_D || CUR_PTC_TYPE == CCP_PELCO_P)
        {
            if(CUR_PTC_ADDR < 255) CUR_PTC_ADDR++;
            else CUR_PTC_ADDR = 0;
        }
        ShowCurLine();
        break;
    }
    return 0;
}
/* 波特率 */
static int32_t _BaudRate(struct menu_item *pthis, int32_t todo)
{
    const uint8_t strBaudrate[][6] = 
    {
        "  2400",
        "  4800",
        "  9600",
        " 19200",
        " 38400",
        "115200",
    };
    switch(todo)
    {
        case MENU_REFRESH:
        memcpy(&pthis->text[20-6], strBaudrate[PTC_BAUDRATE], 6);
        break;
        
        case MENU_LEFT:
        if(PTC_BAUDRATE > BAUD_2400) PTC_BAUDRATE--;
        else PTC_BAUDRATE = BAUD_115200;
        ShowCurLine();
        break;
        
        case MENU_RIGHT:
        if(PTC_BAUDRATE < BAUD_115200) PTC_BAUDRATE++;
        else PTC_BAUDRATE = BAUD_2400;
        ShowCurLine();
        break;
    }
    return 0;
}
/* 速度模式 */
static int32_t _SpeedScale(struct menu_item *pthis, int32_t todo)
{
    const uint8_t strSpeedScale[][3] = 
    {
        "STD",
        "EXP",
    };
    switch(todo)
    {
        case MENU_REFRESH:
        memcpy(&pthis->text[20-3], strSpeedScale[CUR_SPEED_GRADE], 3);
        break;
        
        case MENU_LEFT:
        case MENU_RIGHT:
        if(CUR_SPEED_GRADE != 0) CUR_SPEED_GRADE = 0;
        else CUR_SPEED_GRADE = 1;
        ShowCurLine();
        break;
    }
    return 0;
}
/* 速度限制 */
static int32_t _SpeedLimit(struct menu_item *pthis, int32_t todo)
{
    switch(todo)
    {
        case MENU_REFRESH:
        memcpy(&pthis->text[20-3], strOnOff[PTC_SPEED_LIMIT], 3);
        break;
        
        case MENU_LEFT:
        case MENU_RIGHT:
        if(PTC_SPEED_LIMIT != ON) PTC_SPEED_LIMIT = ON;
        else PTC_SPEED_LIMIT = OFF;
        ShowCurLine();
        break;
    }
    return 0;
}

static int32_t _Ptz(struct menu_item *pthis, int32_t todo)
{
    switch(todo)
    {
        case MENU_RIGHT:
        case MENU_ENTER:
        {
            uint8_t count;
            NewMenu();
            count = sizeof(menu_ptz) / sizeof(struct menu_content);
            for(int32_t i=0; i<count; i++)
            {
                if(AddMenuItem(&menu_ptz[i]))
                    return 1;
            }
            ShowCurMenu();
        }
        break;
        
        default:
        break;
    }
    return 0;
}
/* 数字变倍 */
static int32_t _Dzoom(struct menu_item *pthis, int32_t todo)
{
    switch(todo)
    {
        case MENU_REFRESH:
        memcpy(&pthis->text[20-3], strOnOff[stCamInfo.dzoom^3], 3);
        break;
        
        case MENU_LEFT:
        case MENU_RIGHT:
        if(stCamInfo.dzoom == VISCA_OFF)
            stCamInfo.dzoom = VISCA_ON;
        else
            stCamInfo.dzoom = VISCA_OFF;
        cam_dzoom(stCamInfo.dzoom);
        ShowCurLine();
        break;
    }
    return 0;
}
/* 聚焦模式 */
static int32_t _FocusMode(struct menu_item *pthis, int32_t todo)
{
    const uint8_t strFocusMode[][6] = 
    {
        "MANUAL",
        "  AUTO",
    };
    switch(todo)
    {
        case MENU_REFRESH:
        memcpy(&pthis->text[20-6], strFocusMode[stCamInfo.focus_mode^3], 6);
        break;
        
        case MENU_LEFT:
        case MENU_RIGHT:
        if(stCamInfo.focus_mode == VISCA_FOCUS_AUTO) stCamInfo.focus_mode = VISCA_FOCUS_MANUAL;
        else stCamInfo.focus_mode = VISCA_FOCUS_AUTO;
        cam_focus_mode(stCamInfo.focus_mode);
        ShowCurLine();
        break;
    }
    return 0;
}
/* 聚焦灵敏度 */
//static int32_t _AfSens(struct menu_item *pthis, int32_t todo);
/* 遥控速度 */
static int32_t _RemoteSpeed(struct menu_item *pthis, int32_t todo)
{
    switch(todo)
    {
        case MENU_REFRESH:
        memcpy(&pthis->text[20-3], NumToString(stSysInfo.remote_speed), 3);
        break;
        
        case MENU_LEFT:
        if(stSysInfo.remote_speed > 1) stSysInfo.remote_speed--;
        else stSysInfo.remote_speed = 16;
        ShowCurLine();
        break;
        
        case MENU_RIGHT:
        if(stSysInfo.remote_speed < 16) stSysInfo.remote_speed++;
        else stSysInfo.remote_speed = 1;
        ShowCurLine();
        break;
    }
    return 0;
}
//static int32_t _RemoteSpeedLimit(struct menu_item *pthis, int32_t todo);
const uint8_t strPanTiltDir[][7] = 
{
    " NORMAL",
    "INVERSE",
};
/* 水平反向 */
static int32_t _PanDir(struct menu_item *pthis, int32_t todo)
{
    switch(todo)
    {
        case MENU_REFRESH:
        memcpy(&pthis->text[20-7], strPanTiltDir[stSysInfo.pan_inversion], 7);
        break;
        
        case MENU_LEFT:
        case MENU_RIGHT:
        if(stSysInfo.pan_inversion != ON) stSysInfo.pan_inversion = ON;
        else stSysInfo.pan_inversion = OFF;
        ShowCurLine();
        break;
    }
    return 0;
}
/* 垂直反向 */
static int32_t _TiltDir(struct menu_item *pthis, int32_t todo)
{
    switch(todo)
    {
        case MENU_REFRESH:
        memcpy(&pthis->text[20-7], strPanTiltDir[stSysInfo.tilt_inversion], 7);
        break;
        
        case MENU_LEFT:
        case MENU_RIGHT:
        if(stSysInfo.tilt_inversion != ON) stSysInfo.tilt_inversion = ON;
        else stSysInfo.tilt_inversion = OFF;
        ShowCurLine();
        break;
    }
    return 0;
}
/* 图像翻转 */
static int32_t _PictureFlip(struct menu_item *pthis, int32_t todo)
{
    switch(todo)
    {
        case MENU_REFRESH:
        memcpy(&pthis->text[20-3], strOnOff[stCamInfo.flip^3], 3);
        break;
        
        case MENU_LEFT:
        case MENU_RIGHT:
        if(stCamInfo.flip == VISCA_ON) stCamInfo.flip = VISCA_OFF;
        else stCamInfo.flip = VISCA_ON;
        cam_flip(stCamInfo.flip);
        ShowCurLine();
        break;
    }
    return 0;
}
/* 图像镜像 */
static int32_t _PictureMirror(struct menu_item *pthis, int32_t todo)
{
    switch(todo)
    {
        case MENU_REFRESH:
        memcpy(&pthis->text[20-3], strOnOff[stCamInfo.mirror^3], 3);
        break;
        
        case MENU_LEFT:
        case MENU_RIGHT:
        if(stCamInfo.mirror == VISCA_ON) stCamInfo.mirror = VISCA_OFF;
        else stCamInfo.mirror = VISCA_ON;
        cam_mirror(stCamInfo.mirror);
        ShowCurLine();
        break;
    }
    return 0;
}

/***************************************************************************************************
* Description:  曝光
***************************************************************************************************/
static int32_t _Exposure(struct menu_item *pthis, int32_t todo)
{
    switch(todo)
    {
        case MENU_RIGHT:
        case MENU_ENTER:
        {
            NewMenu();
            
            AddMenuItem(&menu_exposure[0]);
            if(stCamInfo.ae_mode == VISCA_AE_MANUAL)
            {
                AddMenuItem(&menu_exposure[1]);
                AddMenuItem(&menu_exposure[2]);
                AddMenuItem(&menu_exposure[4]);
            }
            else if(stCamInfo.ae_mode == VISCA_AE_SHUTTER_PRIORITY)
            {
                AddMenuItem(&menu_exposure[1]);
                AddMenuItem(&menu_exposure[5]);
                AddMenuItem(&menu_exposure[6]);
            }
            else if(stCamInfo.ae_mode == VISCA_AE_IRIS_PRIORITY)
            {
                AddMenuItem(&menu_exposure[2]);
                AddMenuItem(&menu_exposure[5]);
                AddMenuItem(&menu_exposure[6]);
            }
            else if(stCamInfo.ae_mode == VISCA_AE_BRIGHT_PRIORITY)
            {
                AddMenuItem(&menu_exposure[3]);
            }
            else
            {
                AddMenuItem(&menu_exposure[5]);
                AddMenuItem(&menu_exposure[6]);
            }
            ShowCurMenu();
        }
        break;
        
        default:
        break;
    }
    return 0;
}
/* 曝光模式 */
uint8_t TransAeMode()
{
    uint8_t ret;
    switch(stCamInfo.ae_mode)
    {
        case VISCA_AE_FULL_AUTO:        ret = 0; break;
        case VISCA_AE_MANUAL:           ret = 1; break;
        case VISCA_AE_SHUTTER_PRIORITY: ret = 2; break;
        case VISCA_AE_IRIS_PRIORITY:    ret = 3; break;
        case VISCA_AE_BRIGHT_PRIORITY:  ret = 4; break;
        default: ret = 0; break;
    }
    return ret;
}
static int32_t _AeMode(struct menu_item *pthis, int32_t todo)
{
    const uint8_t strAeMode[][11] = 
    {
        "       AUTO",
        "     MANUAL",
        "SHUTTER PRI",
        "   IRIS PRI",
        " BRIGHT PRI",
    };
    switch(todo)
    {
        case MENU_REFRESH:
        memcpy(&pthis->text[20-11], strAeMode[TransAeMode()], 11);
        break;
        
        case MENU_LEFT:
        if(stCamInfo.ae_mode == VISCA_AE_FULL_AUTO) stCamInfo.ae_mode = VISCA_AE_MANUAL;
        else if(stCamInfo.ae_mode == VISCA_AE_MANUAL) stCamInfo.ae_mode = VISCA_AE_BRIGHT_PRIORITY;
        else if(stCamInfo.ae_mode == VISCA_AE_BRIGHT_PRIORITY) stCamInfo.ae_mode = VISCA_AE_IRIS_PRIORITY;
        else if(stCamInfo.ae_mode == VISCA_AE_IRIS_PRIORITY) stCamInfo.ae_mode = VISCA_AE_SHUTTER_PRIORITY;
        else stCamInfo.ae_mode = VISCA_AE_FULL_AUTO;
        cam_ae_mode(stCamInfo.ae_mode);
        DeletMenu();
        MENU_Enter();
        break;
        
        case MENU_RIGHT:
        if(stCamInfo.ae_mode == VISCA_AE_FULL_AUTO) stCamInfo.ae_mode = VISCA_AE_SHUTTER_PRIORITY;
        else if(stCamInfo.ae_mode == VISCA_AE_SHUTTER_PRIORITY) stCamInfo.ae_mode = VISCA_AE_IRIS_PRIORITY;
        else if(stCamInfo.ae_mode == VISCA_AE_IRIS_PRIORITY) stCamInfo.ae_mode = VISCA_AE_BRIGHT_PRIORITY;
        else if(stCamInfo.ae_mode == VISCA_AE_BRIGHT_PRIORITY) stCamInfo.ae_mode = VISCA_AE_MANUAL;
        else stCamInfo.ae_mode = VISCA_AE_FULL_AUTO;
        cam_ae_mode(stCamInfo.ae_mode);
        DeletMenu();
        MENU_Enter();
        break;
    }
    return 0;
}
static int32_t _Shutter(struct menu_item *pthis, int32_t todo)
{
    const uint8_t strShutterSpeed50[][6] = 
    {
        "   1/1", "   1/2", "   1/3", "   1/6",
        "  1/12", "  1/25", "  1/50", "  1/75",
        " 1/100", " 1/120", " 1/150", " 1/215",
        " 1/300", " 1/425", " 1/600", "1/1000",
        "1/1250", "1/1750", "1/2500", "1/3500",
        "1/6000", " 1/10K",
    };
    const uint8_t strShutterSpeed60[][7] = 
    {
        "   1/1", "   1/2", "   1/4", "   1/8",
        "  1/15", "  1/30", "  1/60", "  1/90",
        " 1/100", " 1/125", " 1/180", " 1/250",
        " 1/350", " 1/500", " 1/725", "1/1000",
        "1/1500", "1/2000", "1/3000", "1/4000",
        "1/6000", " 1/10K",
    };
    switch(todo)
    {
        case MENU_REFRESH:
        if(stSysInfo.video_format_out1 == VIDEO_1920x1080p25 || \
            stSysInfo.video_format_out1 == VIDEO_1920x1080p50 || \
                stSysInfo.video_format_out1 == VIDEO_1920x1080i50)
            memcpy(&pthis->text[20-6], strShutterSpeed50[stCamInfo.shutter], 6);
        else
            memcpy(&pthis->text[20-6], strShutterSpeed60[stCamInfo.shutter], 6);
        break;
        
        case MENU_LEFT:
        cam_shutter(VISCA_DOWN);
        ShowCurLine();
        break;
        
        case MENU_RIGHT:
        cam_shutter(VISCA_UP);
        ShowCurLine();
        break;
    }
    return 0;
}
static int32_t _Iris(struct menu_item *pthis, int32_t todo)
{
    const uint8_t strIris[][5] = {
        "CLOSE", "  F14", "  F11", " F9.6",
        "   F8", " F6.8", " F5.6", " F4.8",
        "   F4", " F3.4", " F2.8", " F2.4",
        "   F2", " F1.8",
    };
    switch(todo)
    {
        case MENU_REFRESH:
        memcpy(&pthis->text[20-5], strIris[stCamInfo.iris], 5);
        break;
        
        case MENU_LEFT:
        cam_iris(VISCA_DOWN);
        ShowCurLine();
        break;
        
        case MENU_RIGHT:
        cam_iris(VISCA_UP);
        ShowCurLine();
        break;
    }
    return 0;
}
static int32_t _Bright(struct menu_item *pthis, int32_t todo)
{
    switch(todo)
    {
        case MENU_REFRESH:
        memcpy(&pthis->text[20-3], NumToString(stCamInfo.bright), 3);
        break;
        
        case MENU_LEFT:
        cam_bright(VISCA_DOWN);
        ShowCurLine();
        break;
        
        case MENU_RIGHT:
        cam_bright(VISCA_UP);
        ShowCurLine();
        break;
    }
    return 0;
}
static int32_t _Gain(struct menu_item *pthis, int32_t todo)
{
    switch(todo)
    {
        case MENU_REFRESH:
        memcpy(&pthis->text[20-3], NumToString(stCamInfo.exp_gain), 3);
        break;
        
        case MENU_LEFT:
        cam_gain(VISCA_DOWN);
        ShowCurLine();
        break;
        
        case MENU_RIGHT:
        cam_gain(VISCA_UP);
        ShowCurLine();
        break;
    }
    return 0;
}
static int32_t _ExpCompesation(struct menu_item *pthis, int32_t todo)
{
    switch(todo)
    {
        case MENU_REFRESH:
        memcpy(&pthis->text[20-3], NumToString(stCamInfo.exp_comp), 3);
        break;
        
        case MENU_LEFT:
        if(stCamInfo.exp_comp > MIN_EXP_COMP) stCamInfo.exp_comp--;
        else stCamInfo.exp_comp = MAX_EXP_COMP;
        cam_expcomp_direct(stCamInfo.exp_comp);
        ShowCurLine();
        break;
        case MENU_RIGHT:
        if(stCamInfo.exp_comp < MAX_EXP_COMP) stCamInfo.exp_comp++;
        else stCamInfo.exp_comp = MIN_EXP_COMP;
        cam_expcomp_direct(stCamInfo.exp_comp);
        ShowCurLine();
        break;
    }
    return 0;
}

static int32_t _Backlight(struct menu_item *pthis, int32_t todo)
{
    switch(todo)
    {
        case MENU_REFRESH:
        memcpy(&pthis->text[20-3], strOnOff[stCamInfo.backlight^3], 3);
        case MENU_ENTER:
        break;
        case MENU_LEFT:
        case MENU_RIGHT:
        if(stCamInfo.backlight != VISCA_ON) stCamInfo.backlight = VISCA_ON;
        else stCamInfo.backlight = VISCA_OFF;
        cam_backlight(stCamInfo.backlight);
        ShowCurLine();
        break;
    }
    return 0;
}
//static int32_t _Highlight(struct menu_item *pthis, int32_t todo);

/***************************************************************************************************
* Description:  白平衡
***************************************************************************************************/
static int32_t _Wb(struct menu_item *pthis, int32_t todo)
{
    switch(todo)
    {
        case MENU_RIGHT:
        case MENU_ENTER:
        {
            NewMenu();
            
            AddMenuItem(&menu_wb[0]);
            if(stCamInfo.wb_mode == VISCA_WB_MANUAL)
            {
                AddMenuItem(&menu_wb[1]);
                AddMenuItem(&menu_wb[2]);
            }
            else if(stCamInfo.wb_mode == VISCA_WB_ONE_PUSH_WB)
            {
                AddMenuItem(&menu_wb[3]);
            }
            ShowCurMenu();
        }
        break;
        
        default:
        break;
    }
    return 0;
}
/* 白平衡模式 */
static int32_t _WbMode(struct menu_item *pthis, int32_t todo)
{
    const uint8_t strWbMode[][8] = 
    {
        "    AUTO",
        "  INDOOR",
        " OUTDOOR",
        "ONE PUSH",
        "     ATW",
        "  MANUAL",
    };
    switch(todo)
    {
        case MENU_REFRESH:
        memcpy(&pthis->text[20-8], strWbMode[stCamInfo.wb_mode], 8);
        break;
        
        case MENU_LEFT:
        if(stCamInfo.wb_mode > VISCA_WB_AUTO) stCamInfo.wb_mode--;
        else stCamInfo.wb_mode = VISCA_WB_MANUAL;
        cam_wb_mode(stCamInfo.wb_mode);
        DeletMenu();
        MENU_Enter();
        break;
        case MENU_RIGHT:
        if(stCamInfo.wb_mode < VISCA_WB_MANUAL) stCamInfo.wb_mode++;
        else stCamInfo.wb_mode = VISCA_WB_AUTO;
        cam_wb_mode(stCamInfo.wb_mode);
        DeletMenu();
        MENU_Enter();
        break;
    }
    return 0;
}
/* 红增益 */
static int32_t _RGain(struct menu_item *pthis, int32_t todo)
{
    switch(todo)
    {
        case MENU_REFRESH:
        memcpy(&pthis->text[20-3], NumToString(stCamInfo.rgain), 3);
        break;
        
        case MENU_LEFT:
        if(stCamInfo.wb_mode != VISCA_WB_MANUAL) return 0;
        cam_rgain(VISCA_DOWN);
        ShowCurLine();
        break;
        case MENU_RIGHT:
        if(stCamInfo.wb_mode != VISCA_WB_MANUAL) return 0;
        cam_rgain(VISCA_UP);
        ShowCurLine();
        break;
    }
    return 0;
}


/* 蓝增益 */
static int32_t _BGain(struct menu_item *pthis, int32_t todo)
{
    switch(todo)
    {
        case MENU_REFRESH:
        memcpy(&pthis->text[20-3], NumToString(stCamInfo.bgain), 3);
        break;
        
        case MENU_LEFT:
        if(stCamInfo.wb_mode != VISCA_WB_MANUAL) return 0;
        cam_bgain(VISCA_DOWN);
        ShowCurLine();
        break;
        case MENU_RIGHT:
        if(stCamInfo.wb_mode != VISCA_WB_MANUAL) return 0;
        cam_bgain(VISCA_UP);
        ShowCurLine();
        break;
    }
    return 0;
}
/* 触发 */
static int32_t _WbTrigger(struct menu_item *pthis, int32_t todo)
{
    switch(todo)
    {
        case MENU_ENTER:
        case MENU_RIGHT:
        cam_wb_onepush_trigger();
        break;
    }
    return 0;
}
/***************************************************************************************************
* Description:  图像
***************************************************************************************************/
static int32_t _Picture(struct menu_item *pthis, int32_t todo)
{
    switch(todo)
    {
        case MENU_RIGHT:
        case MENU_ENTER:
        {
            uint8_t count;
            NewMenu();
            count = sizeof(menu_picture) / sizeof(struct menu_content);
            for(int32_t i=0; i<count; i++)
            {
                if(AddMenuItem(&menu_picture[i]))
                    return 1;
            }
            ShowCurMenu();
        }
        break;
        
        default:
        break;
    }
    return 0;
}
/* 2D降噪 */
static int32_t _2DNr(struct menu_item *pthis, int32_t todo)
{
    switch(todo)
    {
        case MENU_REFRESH:
        memcpy(&pthis->text[20-3], strOnOff[stCamInfo.nr2d^3], 3);
        case MENU_ENTER:
        break;
        case MENU_LEFT:
        case MENU_RIGHT:
        if(stCamInfo.nr2d == VISCA_ON) stCamInfo.nr2d = VISCA_OFF;
        else stCamInfo.nr2d = VISCA_ON;
        cam_2dnr(stCamInfo.nr2d);
        ShowCurLine();
        break;
    }
    return 0;
}
/* 3D降噪 */
static int32_t _3DNr(struct menu_item *pthis, int32_t todo)
{
    const uint8_t strNR[][4] = 
    {
        "   0",
        "   1",
        "   2",
        "   3",
        "   4",
        "   5",
    };
    switch(todo)
    {
        case MENU_REFRESH:
        memcpy(&pthis->text[20-4], strNR[stCamInfo.nr3d], 4);
        break;
        case MENU_LEFT:
        if(stCamInfo.nr3d > MIN_NR) stCamInfo.nr3d--;
        else stCamInfo.nr3d = MAX_NR;
        cam_3dnr(stCamInfo.nr3d);
        ShowCurLine();
        break;
        case MENU_RIGHT:
        if(stCamInfo.nr3d < MAX_NR) stCamInfo.nr3d++;
        else stCamInfo.nr3d = MIN_NR;
        cam_3dnr(stCamInfo.nr3d);
        ShowCurLine();
        break;
    }
    return 0;
}
/* 对比度 */
static int32_t _Contrast(struct menu_item *pthis, int32_t todo)
{
    switch(todo)
    {
        case MENU_REFRESH:
        memcpy(&pthis->text[20-3], NumToString(stCamInfo.constrast), 3);
        break;
        case MENU_LEFT:
        if(stCamInfo.constrast > MIN_CONSTRAST) stCamInfo.constrast--;
        else stCamInfo.constrast = MAX_CONSTRAST;
        cam_constrast_direct(stCamInfo.constrast);
        ShowCurLine();
        break;
        case MENU_RIGHT:
        if(stCamInfo.constrast < MAX_CONSTRAST) stCamInfo.constrast++;
        else stCamInfo.constrast = MIN_CONSTRAST;
        cam_constrast_direct(stCamInfo.constrast);
        ShowCurLine();
        break;
    }
    return 0;
}
/* 饱和度 */
static int32_t _ColorGain(struct menu_item *pthis, int32_t todo)
{
    switch(todo)
    {
        case MENU_REFRESH:
        memcpy(&pthis->text[20-3], NumToString(stCamInfo.saturation), 3);
        break;
        case MENU_LEFT:
        if(stCamInfo.saturation > MIN_SATURATION) stCamInfo.saturation--;
        else stCamInfo.saturation = MAX_SATURATION;
        cam_saturation(stCamInfo.saturation);
        ShowCurLine();
        break;
        case MENU_RIGHT:
        if(stCamInfo.saturation < MAX_SATURATION) stCamInfo.saturation++;
        else stCamInfo.saturation = MIN_SATURATION;
        cam_saturation(stCamInfo.saturation);
        ShowCurLine();
        break;
    }
    return 0;
}
/* 锐度 */
static int32_t _Sharpness(struct menu_item *pthis, int32_t todo)
{
    switch(todo)
    {
        case MENU_REFRESH:
        memcpy(&pthis->text[20-3], NumToString(stCamInfo.sharpness), 3);
        break;
        case MENU_LEFT:
        cam_sharpness(VISCA_DOWN);
        ShowCurLine();
        break;
        case MENU_RIGHT:
        cam_sharpness(VISCA_UP);
        ShowCurLine();
        break;
    }
    return 0;
}
//static int32_t _Hue(struct menu_item *pthis, int32_t todo);
static int32_t _Gamma(struct menu_item *pthis, int32_t todo)
{
    switch(todo)
    {
        case MENU_REFRESH:
        memcpy(&pthis->text[20-3], NumToString(stCamInfo.gamma), 3);
        break;
        
        case MENU_LEFT:
        if(stCamInfo.gamma > MIN_GAMMA) stCamInfo.gamma--;
        else stCamInfo.gamma = MAX_GAMMA;
        cam_gamma(stCamInfo.gamma);
        ShowCurLine();
        break;
        case MENU_RIGHT:
        if(stCamInfo.gamma < MAX_GAMMA) stCamInfo.gamma++;
        else stCamInfo.gamma = MIN_GAMMA;
        cam_gamma(stCamInfo.gamma);
        ShowCurLine();
        break;
    }
    return 0;
}
static int32_t _Wdr(struct menu_item *pthis, int32_t todo)
{
    switch(todo)
    {
        case MENU_REFRESH:
        if(stCamInfo.wdr == VISCA_OFF)
            memcpy(&pthis->text[20-3], NumToString(0), 3);
        else
            memcpy(&pthis->text[20-3], NumToString(stCamInfo.wdr_level), 3);
        break;
        
        case MENU_LEFT:
        if(stCamInfo.wdr == VISCA_ON)
        {
            if(stCamInfo.wdr_level > MIN_WDR)
            {
                stCamInfo.wdr_level--;
                cam_wdr(stCamInfo.wdr_level);
            }
            else
            {
                stCamInfo.wdr = VISCA_OFF;
                cam_wdr(0);
            }
        }
        else
        {
            stCamInfo.wdr = VISCA_ON;
            stCamInfo.wdr_level = MAX_WDR;
            cam_wdr(stCamInfo.wdr_level);
        }
        ShowCurLine();
        break;
        case MENU_RIGHT:
        if(stCamInfo.wdr == VISCA_ON)
        {
            if(stCamInfo.wdr_level < MAX_WDR)
            {
                stCamInfo.wdr_level++;
                cam_wdr(stCamInfo.wdr_level);
            }
            else
            {
                stCamInfo.wdr = VISCA_OFF;
                cam_wdr(0);
            }
        }
        else
        {
            stCamInfo.wdr = VISCA_ON;
            stCamInfo.wdr_level = MIN_WDR;
            cam_wdr(stCamInfo.wdr_level);
        }
        ShowCurLine();
        break;
    }
    return 0;
}
static int32_t _Defog(struct menu_item *pthis, int32_t todo)
{
    switch(todo)
    {
        case MENU_REFRESH:
        memcpy(&pthis->text[20-3], NumToString(stCamInfo.defog), 3);
        break;
        
        case MENU_LEFT:
        if(stCamInfo.defog > MIN_DEFOG) stCamInfo.defog--;
        else stCamInfo.defog = MAX_DEFOG;
        cam_defog(VISCA_ON, stCamInfo.defog);
        ShowCurLine();
        break;
        case MENU_RIGHT:
        if(stCamInfo.defog < MAX_DEFOG) stCamInfo.defog++;
        else stCamInfo.defog = MIN_DEFOG;
        cam_defog(VISCA_ON, stCamInfo.defog);
        ShowCurLine();
        break;
    }
    return 0;
}
static int32_t _DeFlicker(struct menu_item *pthis, int32_t todo)
{
    const uint8_t strFlick[][4] = 
    {
        " OFF",
        "50HZ",
        "60HZ",
    };
    switch(todo)
    {
        case MENU_REFRESH:
        memcpy(&pthis->text[20-4], strFlick[stCamInfo.flick], 4);
        break;
        
        case MENU_LEFT:
        if(stCamInfo.flick > 0) stCamInfo.flick--;
        else stCamInfo.flick = 2;
        cam_flick(stCamInfo.flick);
        ShowCurLine();
        break;
        case MENU_RIGHT:
        if(stCamInfo.flick < 2) stCamInfo.flick++;
        else stCamInfo.flick = 0;
        cam_flick(stCamInfo.flick);
        ShowCurLine();
        break;
    }
    return 0;
}

/***************************************************************************************************
* Description:  网络
***************************************************************************************************/
#if defined(EN_NETWORK)
static int32_t _Network(struct menu_item *pthis, int32_t todo)
{
    switch(todo)
    {
        case MENU_RIGHT:
        case MENU_ENTER:
        {
            uint8_t count;
            NewMenu();
            count = sizeof(menu_network) / sizeof(struct menu_content);
            for(int32_t i=0; i<count; i++)
            {
                if(AddMenuItem(&menu_network[i]))
                    return 1;
            }
            ShowCurMenu();
        }
        break;
        
        default:
        break;
    }
    return 0;
}
/* DHCP */
static int32_t _Dhcp(struct menu_item *pthis, int32_t todo)
{
    switch(todo)
    {
        case MENU_REFRESH:
        memcpy(&pthis->text[20-3], strOnOff[stCamInfo.dhcp^3], 3);
        break;
        
        case MENU_LEFT:
        case MENU_RIGHT:
        if(stCamInfo.dhcp == VISCA_ON) stCamInfo.dhcp = VISCA_OFF;
        else stCamInfo.dhcp = VISCA_ON;
        cam_dhcp(stCamInfo.dhcp);
        ShowCurLine();
        break;
    }
    return 0;
}
static uint8_t str_buffer[5] = {0};
uint8_t NumToString2(uint16_t num)
{
    uint8_t count = 0;
    uint8_t index = 4;
    uint8_t temp;
    
    do{
        temp = num % 10;
        str_buffer[index--] = temp + '0';
        num /= 10;
        count++;
    }while(num != 0);
    return count;
}
static uint8_t ip_str_buffer[15] = {0};
uint8_t IpToStr(uint32_t val)
{
    uint8_t len = 0;
    uint8_t temp;
    uint8_t count = 0;
    
    memset(ip_str_buffer, ' ', sizeof(ip_str_buffer));
    
    temp = (uint8_t)((val>>24)&0xFF);
    len = NumToString2(temp);
    memcpy(&ip_str_buffer[count], &str_buffer[5-len], len);
    count += len;
    ip_str_buffer[count] = '.';
    count++;
    
    temp = (uint8_t)((val>>16)&0xFF);
    len = NumToString2(temp);
    memcpy(&ip_str_buffer[count], &str_buffer[5-len], len);
    count += len;
    ip_str_buffer[count] = '.';
    count++;
    
    temp = (uint8_t)((val>>8)&0xFF);
    len = NumToString2(temp);
    memcpy(&ip_str_buffer[count], &str_buffer[5-len], len);
    count += len;
    ip_str_buffer[count] = '.';
    count++;
    
    temp = (uint8_t)((val>>0)&0xFF);
    len = NumToString2(temp);
    memcpy(&ip_str_buffer[count], &str_buffer[5-len], len);
    count += len;
    
    return count;
}

/* IP */
static int32_t _Ip(struct menu_item *pthis, int32_t todo)
{
    if(todo == MENU_REFRESH)
    {
        uint8_t len = IpToStr(stCamInfo.ip);
        memcpy(&pthis->text[20-len], ip_str_buffer, len);
    }
    return 0;
}
/* 掩码 */
static int32_t _Mask(struct menu_item *pthis, int32_t todo)
{
    if(todo == MENU_REFRESH)
    {
        uint8_t len = IpToStr(stCamInfo.mask);
        memcpy(&pthis->text[20-len], ip_str_buffer, len);
    }
    return 0;
}
/* 网关 */
static int32_t _Gateway(struct menu_item *pthis, int32_t todo)
{
    if(todo == MENU_REFRESH)
    {
        uint8_t len = IpToStr(stCamInfo.gateway);
        memcpy(&pthis->text[20-len], ip_str_buffer, len);
    }
    return 0;
}
#endif

/***************************************************************************************************
* Description:  系统设置
***************************************************************************************************/
static uint8_t res_temp;
static int32_t _System(struct menu_item *pthis, int32_t todo)
{
    switch(todo)
    {
        case MENU_RIGHT:
        case MENU_ENTER:
        {
            uint8_t count;
            
            res_temp = stSysInfo.video_format_out1;
            
            NewMenu();
            count = sizeof(menu_system) / sizeof(struct menu_content);
            for(int32_t i=0; i<count; i++)
            {
                if(AddMenuItem(&menu_system[i]))
                    return 1;
            }
            ShowCurMenu();
        }
        break;
        
        default:
        break;
    }
    return 0;
}
static int32_t _VideoFormat(struct menu_item *pthis, int32_t todo)
{
    const uint8_t strVideoFormat[][7] = 
    {
        "  ERROR", " 240P60", " 480P60", " 600P30", " 600P60",
        " 720P15", " 720P25", " 720P29", " 720P30", " 720P50", " 720P59", " 720P60",
        "1080P25", "1080P29", "1080P30", "1080P50", "1080P59", "1080P60", "1080I50", "1080I59", "1080I60",
        "2160P25", "2160P29", "2160P30", "2160P50", "2160P59", "2160P60"
    };
    switch(todo)
    {
        case MENU_REFRESH:
        memcpy(&pthis->text[20-7], strVideoFormat[res_temp], 7);
        break;
        
        case MENU_LEFT:
        if(res_temp > VIDEO_1280x720p50) res_temp--;
        else res_temp = VIDEO_1920x1080i60;
        if(res_temp == VIDEO_1280x720p59 || \
            res_temp == VIDEO_1920x1080p29 || \
                res_temp == VIDEO_1920x1080p59 || \
                    res_temp == VIDEO_1920x1080i59)
        {
            res_temp--;
        }
        ShowCurLine();
        break;
        case MENU_RIGHT:
        if(res_temp < VIDEO_1920x1080i60) res_temp++;
        else res_temp = VIDEO_1280x720p50;
        if(res_temp == VIDEO_1280x720p59 || \
            res_temp == VIDEO_1920x1080p29 || \
                res_temp == VIDEO_1920x1080p59 || \
                    res_temp == VIDEO_1920x1080i59)
        {
            res_temp++;
        }
        ShowCurLine();
        break;
        
        case MENU_ENTER:
        if(res_temp != stSysInfo.video_format_out1)
        {
            SetMainVideoFormat(res_temp);
            stSysInfo.video_format_out1 = res_temp;
            ShowCurMenu();
        }
        break;
    }
    return 0;
}
static int32_t _IrAddress(struct menu_item *pthis, int32_t todo)
{
    switch(todo)
    {
        case MENU_REFRESH:
        if(stSysInfo.ir_addr)
            memcpy(&pthis->text[20-3], NumToString(stSysInfo.ir_addr), 3);
        else
            memcpy(&pthis->text[20-3], strOnOff[0], 3);
        break;
        
        case MENU_LEFT:
        if(stSysInfo.ir_addr > 0) stSysInfo.ir_addr--;
        else stSysInfo.ir_addr = 3;
        ShowCurLine();
        break;
        
        case MENU_RIGHT:
        if(stSysInfo.ir_addr < 3) stSysInfo.ir_addr++;
        else stSysInfo.ir_addr = 0;
        ShowCurLine();
        break;
    }
    return 0;
}
static int32_t _ShowBrief(struct menu_item *pthis, int32_t todo)
{
    switch(todo)
    {
        case MENU_REFRESH:
        memcpy(&pthis->text[20-3], strOnOff[stSysInfo.show_brief], 3);
        break;
        
        case MENU_LEFT:
        case MENU_RIGHT:
        if(stSysInfo.show_brief == ON) stSysInfo.show_brief = OFF;
        else stSysInfo.show_brief = ON;
        ShowCurLine();
        break;
    }
    return 0;
}
static int32_t _ReloadPreset1(struct menu_item *pthis, int32_t todo)
{
    switch(todo)
    {
        case MENU_REFRESH:
        memcpy(&pthis->text[20-3], strOnOff[stSysInfo.call_preset1], 3);
        break;
        
        case MENU_LEFT:
        case MENU_RIGHT:
        if(stSysInfo.call_preset1 == ON) stSysInfo.call_preset1 = OFF;
        else stSysInfo.call_preset1 = ON;
        ShowCurLine();
        break;
    }
    return 0;
}
static int32_t _Version(struct menu_item *pthis, int32_t todo)
{
    if(todo == 7)
    {
        NewMenu();
        AddMenuItem(&menu_factory[0]);
        AddMenuItem(&menu_factory[1]);
        AddMenuItem(&menu_factory[2]);
        ShowCurMenu();
    }
    else if(todo == 8)
    {
        visca_title_cmd(0x0F, TITLE_CLOSE, 1);
        cam_menu_switch();
        stRunInfo.menu = MENU_CAM;
    }
    return 0;
}
static int32_t _FactoryPattern(struct menu_item *pthis, int32_t todo)
{
    switch(todo)
    {
        case MENU_REFRESH:
        memcpy(&pthis->text[20-3], strOnOff[stRunInfo.factory_pattern], 3);
        break;
        
        case MENU_LEFT:
        case MENU_RIGHT:
        if(stRunInfo.factory_pattern == ON) stRunInfo.factory_pattern = OFF;
        else stRunInfo.factory_pattern = ON;
        LinkCam2In1(stRunInfo.factory_pattern);
        ShowCurLine();
        break;
    }
    return 0;
}
static int32_t _PresetSavePic(struct menu_item *pthis, int32_t todo)
{
        switch(todo)
    {
        case MENU_REFRESH:
        memcpy(&pthis->text[20-3], strOnOff[PRESET_SAVE_PIC], 3);
        break;
        
        case MENU_LEFT:
        case MENU_RIGHT:
        if(PRESET_SAVE_PIC == ON) PRESET_SAVE_PIC = OFF;
        else PRESET_SAVE_PIC = ON;
        ShowCurLine();
        break;
    }
    return 0;
}
static int32_t _Rs485(struct menu_item *pthis, int32_t todo)
{
    switch(todo)
    {
        case MENU_REFRESH:
        memcpy(&pthis->text[20-3], strOnOff[IS_RS485_ENABLE], 3);
        break;
        
        case MENU_LEFT:
        case MENU_RIGHT:
        if(IS_RS485_ENABLE == ON) IS_RS485_ENABLE = OFF;
        else IS_RS485_ENABLE = ON;
        ShowCurLine();
        break;
    }
    return 0;
}
/***************************************************************************************************
* Description:  恢复默认设置
***************************************************************************************************/
static int32_t _Reset(struct menu_item *pthis, int32_t todo)
{
    switch(todo)
    {
        case MENU_RIGHT:
        case MENU_ENTER:
        {
            uint8_t count;
            NewMenu();
            count = sizeof(menu_reset) / sizeof(struct menu_content);
            for(int32_t i=0; i<count; i++)
            {
                if(AddMenuItem(&menu_reset[i]))
                    return 1;
            }
            ShowCurMenu();
        }
        break;
        
        default:
        break;
    }
    return 0;
}


/***************************************************************************************************
* Description: 恢复默认设置
***************************************************************************************************/
static int32_t _ResetSystem(struct menu_item *pthis, int32_t todo)
{
    if(todo == MENU_RIGHT || todo == MENU_ENTER)
    {
        visca_title_cmd(0x0F, TITLE_CLEAR, 1);
        visca_title_cmd(0x0F, TITLE_OPEN, 1);
        visca_title_show(5, "    RESETTING...    ", 1);
        System_ResetPara();
        delay_ms(2000);
        MENU_Back();
    }
    return 0;
}
static int32_t _ResetPicture(struct menu_item *pthis, int32_t todo)
{
    if(todo == MENU_RIGHT || todo == MENU_ENTER)
    {
        visca_title_cmd(0x0F, TITLE_CLEAR, 1);
        visca_title_cmd(0x0F, TITLE_OPEN, 1);
        visca_title_show(5, "    RESETTING...    ", 1);
        cam_reset_picture();
        delay_ms(2000);
        MENU_Back();
    }
    return 0;
}
#if defined(EN_NETWORK)
static int32_t _ResetNetwork(struct menu_item *pthis, int32_t todo)
{
    if(todo == MENU_RIGHT || todo == MENU_ENTER)
    {
        visca_title_cmd(0x0F, TITLE_CLEAR, 1);
        visca_title_cmd(0x0F, TITLE_OPEN, 1);
        visca_title_show(5, "    RESETTING...    ", 1);
        cam_reset_net();
        delay_ms(2000);
        MENU_Back();
    }
    return 0;
}
#endif
static int32_t _ResetAllSettings(struct menu_item *pthis, int32_t todo)
{
    if(todo == MENU_RIGHT || todo == MENU_ENTER)
    {
        visca_title_cmd(0x0F, TITLE_CLEAR, 1);
        visca_title_cmd(0x0F, TITLE_OPEN, 1);
        visca_title_show(5, "    RESETTING...    ", 1);
        cam_reset_picture();
        cam_reset_net();
        System_ResetPara();
        delay_ms(2000);
        MENU_Back();
    }
    return 0;
}

/***************************************************************************************************
* Description:  显示简略信息
***************************************************************************************************/
void ShowBrief(void)
{
    if(stSysInfo.show_brief == OFF)
        return;
    
    NewMenu();
    AddMenuItem(&menu_serialport[0]);
    AddMenuItem(&menu_serialport[1]);
    AddMenuItem(&menu_serialport[2]);
#if defined(EN_NETWORK)
    AddMenuItem(&menu_network[0]);
    AddMenuItem(&menu_network[1]);
#endif
    AddMenuItem(&menu_system[0]);
    AddMenuItem(&menu_system[1]);
    AddMenuItem(&menu_system[2]);
    AddMenuItem(&menu_system[4]);
    
    ShowCurMenu();
    delay_ms(5000);
    
    DeletMenu();
    visca_title_cmd(0x0F, TITLE_CLOSE, 1);
    visca_title_cmd(0x0F, TITLE_CLEAR, 1);
}


/****************************************** END OF FILE *******************************************/