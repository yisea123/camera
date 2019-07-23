/***************************************************************************************************
* FILE: cy3014.c

* DESCRIPTION:  --

* CREATED: 2017/11/22, by kevin xu
****************************************************************************************************
* MODIFY:  --

* DESCRIPTION: --

* DATE: --
***************************************************************************************************/

/***************************************************************************************************
* INCLUDE FILES
***************************************************************************************************/
#include "cy3014.h"
#include "queue.h"
#include "system.h"
#include "ccp.h"
#include "visca2camera.h"


/***************************************************************************************************
* LOCAL DEFINES
***************************************************************************************************/
//第一个字节
#define USB_PTZ             0x11
#define	USB_PICTURE			0x21
#define USB_RESOLUTION      0x30
#define USB_FPS             0x40
#define USB_USB2            0x50
//第二个字节
#define USB_ZOOM_ABS        0x0B
#define USB_ZOOM_REL        0x0C
#define USB_PT_ABS          0x0D
#define USB_PT_REL          0x0E
#define USB_BRIGHT			0x02
#define USB_CONSTRAST		0x03
#define USB_SATURATION      0x07
#define USB_ACUITY			0x08
#define USB_FLICK           0x05

/***************************************************************************************************
* LOCAL GLOBAL VARIABLES
***************************************************************************************************/
ByteQueue Queue_CY3014_RX;
/***************************************************************************************************
* FUNCTION DECLARATION
***************************************************************************************************/

/***************************************************************************************************
* Description:  3014重启
* Arguments:    mode：0-USB启动,下载模式 1-SPI启动,运行模式
***************************************************************************************************/
void CYUSB3014_ReBoot(uint8_t mode)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USB3014_RESET = 0; 
    delay_ms(50);
    if(mode == BOOT_FROM_USB)
    {//USB启动 PMODE1 = 1 PMODE2浮空
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        
        USB3014_PMODE1 = 1;
    }
    else
    {//SPI启动 PMODE1浮空 PMODE2 = 0 
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        
        USB3014_PMODE2 = 0;
    }
    USB3014_RESET = 1; 
}

/***************************************************************************************************
* Description:  硬件I2C初始化
***************************************************************************************************/
void I2C1_Init(void)
{
    I2C_InitTypeDef I2C1_InitStruct;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    I2C_DeInit(I2C1);
    
    I2C1_InitStruct.I2C_Ack = I2C_Ack_Enable;
    I2C1_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C1_InitStruct.I2C_ClockSpeed = 400000;
    I2C1_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;//高低电平时间平分
    I2C1_InitStruct.I2C_Mode = I2C_Mode_I2C;        //I2C模式
    I2C1_InitStruct.I2C_OwnAddress1 = 0xC2;         //从模式地址
    I2C_Init(I2C1, &I2C1_InitStruct);
    
    NVIC_InitStructure.NVIC_IRQChannel = I2C1_EV_IRQn;//事件中断
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_Init(&NVIC_InitStructure);
    NVIC_InitStructure.NVIC_IRQChannel = I2C1_ER_IRQn;//错误中断
    NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_Init(&NVIC_InitStructure);
    
    I2C_ITConfig(I2C1, I2C_IT_BUF | I2C_IT_EVT, ENABLE);//启用中断
}


/***************************************************************************************************
* Description: CYUSB3014 I2C从设备中断
***************************************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
    void I2C1_EV_IRQHandler(void)
    {
        static uint32_t I2C1_SR = 0;
        
        /* 读取状态寄存器 */
        I2C1_SR = I2C1->SR1;
        I2C1_SR |= (I2C1->SR2) << 16;
        
        if(I2C1_SR & 0x0002)
        {//Start
            ByteQueue_EnQueue(&Queue_CY3014_RX, 0xA0);
            ByteQueue_EnQueue(&Queue_CY3014_RX, 0x5F);
        }
        
        if(I2C1_SR & 0x0040)
        {
            uint8_t data = I2C1->DR;
            ByteQueue_EnQueue(&Queue_CY3014_RX, data);
        }
        else if(I2C1_SR & 0x0084)
        {
            I2C1->DR = 0; 
        }
        
        if(I2C1_SR & 0x0010)
        {//Stop
            I2C1->CR1 |= 0x0001;
            ByteQueue_EnQueue(&Queue_CY3014_RX, 0xA1);
            ByteQueue_EnQueue(&Queue_CY3014_RX, 0x5E);
        }
    }
    
    void I2C1_ER_IRQHandler(void)
    {
        
    }
#ifdef __cplusplus
}
#endif

/***************************************************************************************************
* Description:  CYUSB3014初始化
***************************************************************************************************/
void CYUSB3014_Init(void)
{
    static uint8_t bIsInited = 0;
    
    I2C1_Init();
    
    if(bIsInited == 0)
    {
        bIsInited = 1;
        ByteQueue_Init(&Queue_CY3014_RX, (uint8_t *)malloc(100), 100);
    }
}

/***************************************************************************************************
* Description:  检测3014升级拨码是否使能
***************************************************************************************************/
void CYUSB3014_CheckUpdate(void)
{
    if(CYUSB3014_DOWNLOAD_EN)
    {
        CYUSB3014_ReBoot(BOOT_FROM_USB);
        while(CYUSB3014_DOWNLOAD_EN)
        {
            LED_POWER_TOGGLE;
            LED_STANDBY_TOGGLE;
            delay_ms(500);
        }
    }
}

/***************************************************************************************************
* Description:  从3014接收缓存中读取指令
***************************************************************************************************/
static uint8_t cy3014_index = 0;
static uint8_t cy3014_buf[10] = {0};
uint8_t CY3014_ReadCmd(void)
{
    uint8_t temp;
    if(ByteQueue_DeQueue(&Queue_CY3014_RX, &temp))
    {
        /* 指令头 */
        if((temp == 0xA0) && (ByteQueue_DeQueue(&Queue_CY3014_RX, &temp)) && (temp == 0x5F))
        {
            cy3014_index = 0;
            memset(cy3014_buf, 0, 10);
        }
        /* 指令结尾 */
        else if((temp == 0xA1) && (ByteQueue_DeQueue(&Queue_CY3014_RX, &temp)) && (temp == 0x5E))
        {
            //            uint8_t i;
            //            printf("\r\n");
            //            for(i=0; i<cy3014_index; i++)
            //            {
            //                printf("%02x ", cy3014_buf[i]);
            //            }
            //            printf("\r\n");
            return 1;
        }
        /* 中间数据放入buffer */
        else
        {
            cy3014_buf[cy3014_index] = temp;
            if(++cy3014_index >= 10)
            {
                cy3014_index = 0;
                memset(cy3014_buf, 0, 10);
            }
        }
    }
    return 0;
}

/***************************************************************************************************
* Description:  3014发送的指令处理函数
***************************************************************************************************/
static uint32_t u32Timer_USB_ZOOM = 0;
static uint8_t u8USB_ZOOM_RCV = 0;
static uint16_t u16USB_ZOOM_POS = 0;
void CY3014_CmdHandler(void)
{
    static uint8_t ResTemp = 0;
    
    if(u8USB_ZOOM_RCV)
    {
        if(CheckInterval(&u32Timer_USB_ZOOM, 100))
        {
            u8USB_ZOOM_RCV = 0;
            cam_zoom_direct(u16USB_ZOOM_POS);
        }
    }
    
    if(CY3014_ReadCmd())
    {
        switch(cy3014_buf[0])
        {
            case USB_RESOLUTION:
            {
                if(cy3014_buf[1] == 0x00)
                {
                    static uint32_t timer_usb_res = 0;
                    if(!CheckInterval(&timer_usb_res, 200)) return;
                    
                    switch(cy3014_buf[2])
                    {
                        case USB_RES_1920x1080:
                        case USB_RES_1280x720:
                        case USB_RES_800x600:
                        case USB_RES_640x480:
                        case USB_RES_320x240:
                        ResTemp = cy3014_buf[2];
                        break;
                        default:
                        ResTemp = 0;
                        break;
                    }
                }
            } break;
            
            case USB_FPS:
            {
                if(ResTemp == 0) return;
                EN_VIDEO_FORMAT eVideoFormat = VIDEO_NOP;
                switch(ResTemp)
                {
                    case USB_RES_320x240:
                    eVideoFormat = VIDEO_320x240p60;
                    break;
                    case USB_RES_640x480:
                    eVideoFormat = VIDEO_640x480p60;
                    break;
                    case USB_RES_800x600:
                    if(cy3014_buf[2] == USB_RATE60)
                        eVideoFormat = VIDEO_800x600p60;
                    else
                        eVideoFormat = VIDEO_800x600p30;
                    break;
                    case USB_RES_1280x720:
                    if(cy3014_buf[2] == USB_RATE15)
                        eVideoFormat = VIDEO_1280x720p15;
                    else
                        eVideoFormat = VIDEO_1280x720p60;
                    break;
                    case USB_RES_1920x1080:
                    eVideoFormat = VIDEO_1920x1080p30;
                    break;
                    default:
                    return;
                    break;
                }
                stSysInfo.video_format_out1 = eVideoFormat;
                while(stRunInfo.menu != MENU_OFF)
                {
                    MENU_Back();
                }
                SetCameraVideoFormat(eVideoFormat);
            } break;
            
            case USB_PICTURE:
            {
                switch(cy3014_buf[1])
                {
                    case USB_BRIGHT:
                    cam_expcomp_direct(cy3014_buf[2]);
                    break;
                    
                    case USB_CONSTRAST:
                    cam_constrast_direct(cy3014_buf[2]);
                    break;
                    
                    case USB_SATURATION:
                    cam_saturation(cy3014_buf[2]);
                    break;
                    
                    case USB_ACUITY:
                    cam_aperture_direct(cy3014_buf[2]);
                    break;
                    
                    case USB_FLICK:
                    if(cy3014_buf[2] == 0x3C) cam_flick(YHW_FLICK_50HZ);
                    else cam_flick(YHW_FLICK_60HZ);
                    break;
                    
                    default:
                    break;
                }
            } break;
            
            case USB_PTZ:
            {
                switch(cy3014_buf[1])
                { 
                    case USB_ZOOM_ABS:
                    {
                        u16USB_ZOOM_POS = cy3014_buf[3];
                        u16USB_ZOOM_POS <<= 8;
                        u16USB_ZOOM_POS |= cy3014_buf[2];
                        u8USB_ZOOM_RCV = 1;
                        u32Timer_USB_ZOOM = stRunInfo.time;
                    } break;
                    
                    case USB_ZOOM_REL:
                    {
                        uint8_t cmd_tmp;
                        if(cy3014_buf[2] == 0x01)
                            cmd_tmp = VISCA_ZOOM_TELE_STD;  //放大
                        else if(cy3014_buf[2] == 0xFF)
                            cmd_tmp = VISCA_ZOOM_WIDE_STD;  //缩小
                        else
                            cmd_tmp = VISCA_ZOOM_STOP;  //停止
                        cam_zoom(cmd_tmp);
                    } break;
                    
                    case USB_PT_ABS:
                    {
                        int16_t pan, tilt;
                        pan = cy3014_buf[3];
                        pan <<= 8;
                        pan |= cy3014_buf[2];
                        pan *= 13.3;
                        
                        tilt = cy3014_buf[5];
                        tilt <<= 8;
                        tilt |= cy3014_buf[4];
                        tilt *= 13.3;
                        
                        pt_abs_run(30, 25, pan, tilt);
                    } break;
                    
                    case USB_PT_REL:
                    {
                        uint8_t pan_dir, tilt_dir, pan_speed, tilt_speed;
                        if(cy3014_buf[3])
                        {
                            pan_speed = cy3014_buf[3] * 3;
                            if(cy3014_buf[2] == 0xFF) pan_dir = 1;
                            else if(cy3014_buf[2] == 0x01) pan_dir = 2;
                            else pan_dir = 3; 
                        }
                        else pan_dir = 3;

                        if(cy3014_buf[5])
                        {
                            tilt_speed = cy3014_buf[5] * 3;
                            if(cy3014_buf[4] == 0x01) tilt_dir = 1;
                            else if(cy3014_buf[4] == 0xFF) tilt_dir = 2;
                            else tilt_dir = 3;
                        }
                        else tilt_dir = 3;
                        pt_run(pan_speed, tilt_speed, pan_dir, tilt_dir);                        
                    } break;
                    
                    case USB_USB2:
                    
                    break;
                    
                    default:
                    break;
                }
            } break;
            
            default:
            break;
        }
    }
}

/****************************************** END OF FILE *******************************************/
