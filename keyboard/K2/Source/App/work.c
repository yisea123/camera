/***************************************************************************************************
* FILE: work.c
*
* DESCRIPTION:  --
*
* CREATED: 2018/8/25, by kevin
***************************************************************************************************/

/***************************************************************************************************
* INCLUDES
***************************************************************************************************/
#include "work.h"
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

/***************************************************************************************************
* Description:  ���������
***************************************************************************************************/
void KeyNumInput(void)
{
    int8_t num = GetKeyNum();
    
    if(num >= 0)
    {
        if(++stRunInfo.op_num_index < 4)
        {
            stRunInfo.op_num *= 10;
            stRunInfo.op_num += num;
            if(stRunInfo.op_num == 0)
            {
                stRunInfo.op_num_index--;
            }
        }
        else
        {
            stRunInfo.op_num_index = 1;
            stRunInfo.op_num = num;
        }
        
        UI_ShowOpNum();
    }
}

/***************************************************************************************************
* Description:  ɾ����Ϊ������
***************************************************************************************************/
void KeyNumBackspace(void)
{
    if(stRunInfo.op_num_index > 0)
    {
        stRunInfo.op_num_index --;
        stRunInfo.op_num /= 10;
    }
    else
    {
        stRunInfo.op_num_index = 0;
    }
    UI_ShowOpNum();
}

/***************************************************************************************************
* Description:  ����ɨ�貢�����¼�
***************************************************************************************************/
void KeyEvent(void)
{
    /* �򿪲˵� */
    if(GetKey(KEY_MENU, 0))
    {
        LED_Ctrl(KEY_MENU, LED_ON);
        MenuOpen(arg_getin);
    }
    
    /* ɾ�������ַ� */
    if(GetKey(KEY_BACK, 0))
    {
        LED_Ctrl(KEY_BACK, LED_1S);
        KeyNumBackspace();
    }
    
    /* �������� */
    if(GetKey(KEY_LOCK, 0))
    {
        LED_Ctrl(KEY_LOCK, LED_ON);
        stRunInfo.lock = 1;
        UI_ShowAction("Locked", 1);
        vTaskDelay(500);
    }
    
    /* ��̨�ٶ�+ */
    if(GetKey(KEY_SP_ADD, 0))
    {
        LED_Ctrl(KEY_SP_ADD, LED_1S);
        if(stSysPara.speed_grade < 8)
        {
            stSysPara.speed_grade++;
            System_SavePara(SYS_SAVE_SPEED_GRADE);
        }
        char str[30];
        sprintf(str, "PTZ Speed Set To %d", stSysPara.speed_grade);
        UI_ShowAction(str, 0);
    }
    
    /* ��̨�ٶ�- */
    if(GetKey(KEY_SP_SUB, 0))
    {
        LED_Ctrl(KEY_SP_SUB, LED_1S);
        if(stSysPara.speed_grade > 1)
        {
            stSysPara.speed_grade--;
            System_SavePara(SYS_SAVE_SPEED_GRADE);
        }
        char str[30];
        sprintf(str, "PTZ Speed Set To %d", stSysPara.speed_grade);
        UI_ShowAction(str, 0);
    }
    
    /* ���ְ��� */
    KeyNumInput();
    
    /* �жϲ�������ָ��ʱ����Ч */
    if(pevent_handler == NULL)
    {
        return;
    }
    
    /* �����豸 */
    if(GetKey(KEY_SEARCH, 0))
    {
        LED_Ctrl(KEY_SEARCH, LED_1S);
        if(pevent_handler->key_search_handler != NULL)
        {
            pevent_handler->key_search_handler(0);
        }
        else UI_ShowNotSupport();
        
    }
    
    /* �����ַ */
    if(GetKey(KEY_CAMERA, 0))
    {
        LED_Ctrl(KEY_CAMERA, LED_1S);
        if(pevent_handler->key_camera_handler != NULL)
        {
            pevent_handler->key_camera_handler(stRunInfo.op_num);
            stRunInfo.op_num = 0;
            UI_ShowOpNum();
        }
        else UI_ShowNotSupport();
    }
    
    /* ����Ԥ��λ */
    if(GetKey(KEY_PRESET_CALL, 0))
    {
        LED_Ctrl(KEY_PRESET_CALL, LED_1S);
        if(pevent_handler->key_preset_call_handler != NULL)
        {
            pevent_handler->key_preset_call_handler(stRunInfo.op_num);
            stRunInfo.op_num = 0;
            UI_ShowOpNum();
        }
        else UI_ShowNotSupport();
    }
    
    /* ����Ԥ��λ */
    if(GetKey(KEY_PRESET_SET, 0))
    {
        LED_Ctrl(KEY_PRESET_SET, LED_1S);
        if(pevent_handler->key_preset_set_handler != NULL)
        {
            pevent_handler->key_preset_set_handler(stRunInfo.op_num);
            stRunInfo.op_num = 0;
            UI_ShowOpNum();
        }
        else UI_ShowNotSupport();
    }
    
    /* ɾ��Ԥ��λ */
    if(GetKey(KEY_PRESET_DELETE, 0))
    {
        LED_Ctrl(KEY_PRESET_DELETE, LED_1S);
        if(pevent_handler->key_preset_delete_handler != NULL)
        {
            pevent_handler->key_preset_delete_handler(stRunInfo.op_num);
            stRunInfo.op_num = 0;
            UI_ShowOpNum();
        }
        else UI_ShowNotSupport();
    }
    
    /* ���OSD */
    if(GetKey(KEY_OSD, 0))
    {
        LED_Ctrl(KEY_OSD, LED_1S);
        if(pevent_handler->key_osd_handler != NULL)
        {
            pevent_handler->key_osd_handler(0);
        }
        else UI_ShowNotSupport();
    }
    
    /* ��������л� */
    if(GetKey(KEY_BACKLIGHT, 0))
    {
        LED_Ctrl(KEY_BACKLIGHT, LED_1S);
        if(pevent_handler->key_backlight_handler != NULL)
        {
            pevent_handler->key_backlight_handler(0);
        }
        else UI_ShowNotSupport();
    }
    
    /* ��ƽ��ģʽ */
    if(GetKey(KEY_WB_MODE, 0))
    {
        LED_Ctrl(KEY_WB_MODE, LED_1S);
        if(pevent_handler->key_wb_mode_handler != NULL)
        {
            pevent_handler->key_wb_mode_handler(0);
        }
        else UI_ShowNotSupport();
    }
    
    /* RGAIN+ */
    if(GetKey(KEY_RGAIN_ADD, 1))
    {
        LED_Ctrl(KEY_RGAIN_ADD, LED_1S);
        if(pevent_handler->key_rgain_add_handler != NULL)
        {
            pevent_handler->key_rgain_add_handler(0);
        }
        else UI_ShowNotSupport();
    }
    
    /* RGAIN- */
    if(GetKey(KEY_RGAIN_SUB, 1))
    {
        LED_Ctrl(KEY_RGAIN_SUB, LED_1S);
        if(pevent_handler->key_rgain_sub_handler != NULL)
        {
            pevent_handler->key_rgain_sub_handler(0);
        }
        else UI_ShowNotSupport();
    }
    
    /* BGAIN+ */
    if(GetKey(KEY_BGAIN_ADD, 1))
    {
        LED_Ctrl(KEY_BGAIN_ADD, LED_1S);
        if(pevent_handler->key_bgain_add_handler != NULL)
        {
            pevent_handler->key_bgain_add_handler(0);
        }
        else UI_ShowNotSupport();
    }
    
    /* BGAIN- */
    if(GetKey(KEY_BGAIN_SUB, 1))
    {
        LED_Ctrl(KEY_BGAIN_SUB, LED_1S);
        if(pevent_handler->key_bgain_sub_handler != NULL)
        {
            pevent_handler->key_bgain_sub_handler(0);
        }
        else UI_ShowNotSupport();
    }
    
    /* FOCUS AUTO */
    if(GetKey(KEY_FOCUS_AUTO, 0))
    {
        LED_Ctrl(KEY_FOCUS_AUTO, LED_1S);
        if(pevent_handler->key_focus_auto_handler != NULL)
        {
            pevent_handler->key_focus_auto_handler(0);
        }
        else UI_ShowNotSupport();
    }
    
    /* FOCUS FAR */
    if(GetKey(KEY_FOCUS_FAR, 1))
    {
        LED_Ctrl(KEY_FOCUS_FAR, LED_1S);
        if(pevent_handler->key_focus_far_handler != NULL)
        {
            pevent_handler->key_focus_far_handler(0);
        }
        else UI_ShowNotSupport();
    }
    
    /* FOCUS NEAR */
    if(GetKey(KEY_FOCUS_NEAR, 1))
    {
        LED_Ctrl(KEY_FOCUS_NEAR, LED_1S);
        if(pevent_handler->key_focus_near_handler != NULL)
        {
            pevent_handler->key_focus_near_handler(0);
        }
        else UI_ShowNotSupport();
    }
    
    /* �ع�ģʽ */
    if(GetKey(KEY_AE_MODE, 0))
    {
        LED_Ctrl(KEY_AE_MODE, LED_1S);
        if(pevent_handler->key_ae_mode_handler != NULL)
        {
            pevent_handler->key_ae_mode_handler(0);
        }
        else UI_ShowNotSupport();
    }
    if(GetKey(KEY_AE_ADD, 1))
    {
        LED_Ctrl(KEY_AE_ADD, LED_1S);
        if(pevent_handler->key_ae_add_handler != NULL)
        {
            pevent_handler->key_ae_add_handler(0);
        }
        else UI_ShowNotSupport();
    }
    if(GetKey(KEY_AE_SUB, 1))
    {
        LED_Ctrl(KEY_AE_SUB, LED_1S);
        if(pevent_handler->key_ae_sub_handler != NULL)
        {
            pevent_handler->key_ae_sub_handler(0);
        }
        else UI_ShowNotSupport();
    }
    
    
}

/***************************************************************************************************
* Description:  ҡ��ɨ�貢�����¼�
***************************************************************************************************/
void RockerEvent(void)
{
    static uint8_t flagPanTiltStop = 1, flagZoomStop = 1;
    
    static int8_t pan_val, tilt_val, zoom_val;
    int ret;
    
    /* �жϲ��������Ƿ��Ѿ�ע�� */
    if(pevent_handler == NULL)
    {
        return;
    }
    
    /* Rocker Key */
    if(GetKey(KEY_ROCKER, 0))
    {
        if(pevent_handler->key_rocker_handler != NULL)
        {
            pevent_handler->key_rocker_handler(0);
        }
        else UI_ShowNotSupport();
    }

    /* PAN TILT*/
    ret = Rocker_GetPanTiltVal(&pan_val, &tilt_val);
    if(ret)
    {
        flagPanTiltStop = 0;
        if(ret == 1)
            if(pevent_handler->rocker_pan_tilt_handler != NULL)
                pevent_handler->rocker_pan_tilt_handler(pan_val, tilt_val, stSysPara.speed_grade);
            else UI_ShowNotSupport();
    }
    else if(flagPanTiltStop == 0)
    {
        flagPanTiltStop = 1;
        if(pevent_handler->rocker_pan_tilt_handler != NULL)
            pevent_handler->rocker_pan_tilt_handler(0, 0, 0);
        else UI_ShowNotSupport();
    }

    /* ZOOM */
    ret = Rocker_GetZoomVal(&zoom_val, 1);
    if(ret)
    {
        flagZoomStop = 0;
        if(ret == 1)
            if(pevent_handler->rocker_zoom_handler != NULL)
                pevent_handler->rocker_zoom_handler(zoom_val, stSysPara.speed_grade);
            else UI_ShowNotSupport();
    }
    else if(flagZoomStop == 0)
    {
        flagZoomStop = 1;
        if(pevent_handler->rocker_zoom_handler != NULL) 
            pevent_handler->rocker_zoom_handler(0, 0);
        else UI_ShowNotSupport();
    }
}

/***************************************************************************************************
* Description:  ���ô���
***************************************************************************************************/
void IDLE_Handler(void)
{
    static uint8_t idle_flag = 0;
    if(System_CheckIdle())
    {
        if(!idle_flag)
        {
            LCD_SetBacklight(170);
            idle_flag = 1;
        }
    }
    else
    {
        if(idle_flag)
        {
            LCD_SetBacklight(255);
            idle_flag = 0;
        }
    }
}

/***************************************************************************************************
* Description:  ��ѭ��
***************************************************************************************************/
void func_MasterTask(void * argument)
{
    uint32_t counter;

    UI_DrawMainFrame();
    
    System_UpdateIdleTimer();
    
	while (1)
	{
        /* ������ */
        if(stRunInfo.lock)
        {
            if(GetKey(KEY_LOCK, 0))
            {
                LED_Ctrl(KEY_LOCK, LED_OFF);
                stRunInfo.lock = 0;
                UI_ShowAction("Unlocked", 1);
            }
            continue;
        }

        /* �������� */
        KeyEvent();
        
        /* ҡ���¼� */
        RockerEvent();
        
        /* ��ʱ���� */
        if(++counter > 100)
        {
            counter = 0;
            if(pevent_handler->timing_task_handler != NULL) 
                pevent_handler->timing_task_handler();
        }
        
        /* ״̬ͼ�� */
        UI_StatusIcon_Handler();

        /* �ͷ� */
        vTaskDelay(10);
	}
}

/****************************************** END OF FILE *******************************************/
