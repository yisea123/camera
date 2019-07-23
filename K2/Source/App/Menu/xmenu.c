/***************************************************************************************************
* FILE: menu.c
*
* DESCRIPTION:  --
*
* CREATED: 2018/8/21, by kevin
***************************************************************************************************/

/***************************************************************************************************
* INCLUDES
***************************************************************************************************/
#include "xmenu.h"
#include "system.h"
/***************************************************************************************************
* DEFINES
***************************************************************************************************/
#define UITEM_LENGTH(n)     (sizeof(n)/sizeof(struct uitem))
#define TEXT_LENGTH(n)      (sizeof(n)/sizeof(char [32]))

/***************************************************************************************************
* VARIABLES
***************************************************************************************************/
LIST_HEAD(MenuStack);
struct ulist_handle *MenuIndex;


/***************************************************************************************************
* FUNCTION DECLARATION
***************************************************************************************************/
/* 选择协议*/
int func_SelProtc(int arg);

/* 协议设置 */
int func_Protc(int arg);
int func_ProtcViscaIP(int arg);
int func_ViscaIpCompatible(int arg);
int func_ViscaIpManageAddresses(int arg);
int func_ViscaIpAddNewAddress(int arg);
int func_ProtcOnvif(int arg);
int func_ProtcVisca(int arg);
int func_ViscaCompatible(int arg);
int func_ProtcPelcoD(int arg);
int func_ProtcPelcoP(int arg);
int func_ProtcPanasonic(int arg);
int func_ProtcSamsung(int arg);

/* 网络设置 */
int func_Net(int arg);
int func_NetDhcp(int arg);
int func_NetIpAddr(int arg);
int func_NetGateway(int arg);
int func_NetMask(int arg);

/* 串口设置 */
int func_Serial(int arg);
int func_Rs232UsbBaudRate(int arg);
int func_Rs422Rs485BaudRate(int arg);
int func_CmdRepeater(int arg);

/* 摇杆设置 */
int func_Rocker(int arg);
int func_PanReverse(int arg);
int func_TiltReverse(int arg);
int func_RockerCorrect(int arg);

/* 蜂鸣器 */
int func_Buzzer(int arg);

/* 休眠时间 */
int func_SleepTime(int arg);

/* 语言选择 */
int func_Language(int arg);

/* 关于设备 */
int func_AboutDevice(int arg);

/* 重置设备 */
int func_ResetDevice(int arg);

/***************************************************************************************************
* Description:  
***************************************************************************************************/
void func_MenuGetin(const char *title, const struct uitem *item, int32_t length, int32_t (*call_back)(int32_t))
{
    UList_StackPush(&MenuStack, MenuIndex);
    UList_Init(&MenuIndex, title, ON);
    Ulist_AppendItem(MenuIndex, item, length);
    UList_Show(MenuIndex);
    UList_Handle(MenuIndex, call_back);
    UList_Destroy(MenuIndex);
    UList_StackPop(&MenuStack, &MenuIndex);
    UList_Show(MenuIndex);
}

void Menu_Getin2(const char (*text)[32], int32_t length, int32_t sel_index, int32_t (*call_back)(int32_t))
{
    struct uitem uitem_tmp;
    UList_StackPush(&MenuStack, MenuIndex);
    UList_Init(&MenuIndex, text[0], ON);
    for(int32_t i=1; i<length; i++)
    {
        memset(&uitem_tmp, 0, sizeof(struct uitem));
        uitem_tmp.type = uitem_sel;
        uitem_tmp.genius = ((i-1)==sel_index?1:0);
        strcat(uitem_tmp.text, text[i]);
        Ulist_AppendItem(MenuIndex, &uitem_tmp, 1);
    }
    MenuIndex->sel_index = sel_index;
    Ulist_FindSelectedItem(MenuIndex);
    UList_Show(MenuIndex);
    UList_Handle(MenuIndex, call_back);
    UList_Destroy(MenuIndex);
    UList_StackPop(&MenuStack, &MenuIndex);
    UList_Show(MenuIndex);
}

/***************************************************************************************************
* Description:  
***************************************************************************************************/
void func_MenuEnter(void)
{
    struct list_item *tmp = list_entry(MenuIndex->cur_item, struct list_item, list);
    if(tmp->genius != NULL)
    {
        ((int32_t (*)(int32_t))tmp->genius)(arg_getin);
    }
}

void Menu_Select(int32_t *reg)
{
    Ulist_SetSel(MenuIndex);
    struct list_item *tmp = list_entry(MenuIndex->cur_item, struct list_item, list);
    *reg = tmp->index;
    UList_ShowList(MenuIndex);
}




/* 协议选择 -----------------------------------------------------------*/
const char text_SelProtc[][32] = 
{
    "Select Protocol",
    "VISCA Over IP",
    //    "ONVIF",
    "VISCA",
    "PELCO D",
    "PELCO P",
    //    "Panasonic",
    //    "SAMSUNG",
};

int func_SelProtc(int arg)
{
    switch (arg)
	{
        case arg_getbrief:
        return (int)text_SelProtc[stSysPara.ptc.type+1];
		break;
        
        case arg_getin:
        Menu_Getin2(text_SelProtc, TEXT_LENGTH(text_SelProtc), stSysPara.ptc.type, func_SelProtc);
        break;
        
        case arg_enter:
        case arg_right:
        Menu_Select(&stSysPara.ptc.type);
        break;
        
        case arg_left:
        case arg_back:
        return 1;
        break; 
    }
	return 0;
}

/* 协议设置 -----------------------------------------------------------*/
const char title_ProtcSetting[] = "Protocol Setting";
const struct uitem uitem_ProtcSetting[] =
{
//	{
//        uitem_list,
//        (int32_t)func_ProtcViscaIP,
//		"VISCA Over IP",
//	},
    //	{
    //        uitem_list,
    //        (int32_t)func_ProtcOnvif,
    //		"ONVIF",
    //	},
	{
        uitem_list,
        (int32_t)func_ProtcVisca,
		"VISCA",
	},
    //	{
    //        uitem_list,
    //        (int32_t)func_ProtcPelcoD,
    //		"PELCO D",
    //	},
    //	{
    //        uitem_list,
    //        (int32_t)func_ProtcPelcoP,
    //		"PELCO P",
    //	},
    //	{
    //        uitem_list,
    //        (int32_t)func_ProtcPanasonic,
    //		"Panasonic",
    //	},
    //	{
    //        uitem_list,
    //        (int32_t)func_ProtcSamsung,
    //		"SAMSUNG",
    //	},
};
int func_Protc(int arg)
{
	switch (arg)
	{
        case arg_getbrief:
        
		break;
        
        case arg_getin:
        func_MenuGetin(title_ProtcSetting, uitem_ProtcSetting, UITEM_LENGTH(uitem_ProtcSetting), func_Protc);
		break;
        
        case arg_enter:
        case arg_right:
        func_MenuEnter();
        break;
        
        case arg_left:
        case arg_back:
        return 1;
		break;
	}
	return 0;
}

/* VISCA Over IP 设置 ----------------------------------------------------------------------------*/
const char title_ProtcViscaIp[] = "VISCA Over IP";
const struct uitem uitem_ProtcViscaIp[] =
{
	{
        uitem_func,
        (int32_t)func_ViscaIpCompatible,
		"Compatible Mode",
	},
    {
        uitem_func,
        (int32_t)func_ViscaIpManageAddresses,
		"Manage Custom Address",
	},
    {
        uitem_func,
        (int32_t)func_ViscaIpAddNewAddress,
		"Add New Custom Address",
	},
};
int func_ProtcViscaIP(int arg)
{
	switch (arg)
	{
        case arg_getbrief:
        break;
        
        case arg_getin:
        func_MenuGetin(title_ProtcViscaIp, uitem_ProtcViscaIp, UITEM_LENGTH(uitem_ProtcViscaIp), func_ProtcViscaIP);
        break;
        
        case arg_enter:
        case arg_right:
        func_MenuEnter();
		break;
        
        case arg_left:
        case arg_back:
        System_SavePara(SYS_SAVE_VISCA_IP);
        return 1;
		break;
	}
	return 0;
}


const char textViscaIpCompatible[][32] = {
    "Compatible Mode",
    "Mode1",
    "Mode2",
};
int func_ViscaIpCompatible(int arg)
{
    switch (arg)
	{
        case arg_getbrief:
        return (int)textViscaIpCompatible[stSysPara.ptc.visca_ip.compatible_mode+1];
		break;
        
        case arg_getin:
        Menu_Getin2(textViscaIpCompatible, TEXT_LENGTH(textViscaIpCompatible), \
            stSysPara.ptc.visca_ip.compatible_mode, func_ViscaIpCompatible);
		break;
        
        case arg_enter:
        case arg_right:
        Menu_Select(&stSysPara.ptc.visca_ip.compatible_mode);
        break;
        
        case arg_left:
        case arg_back:
        return 1;
		break;
	}
	return 0;
}

/***************************************************************************************************
* Description:  修改IP地址
***************************************************************************************************/
extern struct address_arr ViscaIpAddr;
int32_t func_ModefyIpAddr(int32_t arg)
{
    if(arg == arg_getbrief)
        return 0;
    
    uint32_t ipaddr = ViscaIpCustomAddr.arr[arg];
    
    ipaddr = Edit_IpAddress(ipaddr);
    
    ViscaIpCustomAddr.arr[arg] = ipaddr;
    
    return 0;
}

/***************************************************************************************************
* Description:  删除IP地址
***************************************************************************************************/
int32_t func_DeleteIpAddr(int32_t arg)
{
    if(arg == arg_getbrief)
        return 0;
    
    for(int i=arg; i<ViscaIpCustomAddr.length; i++)
    {
        ViscaIpCustomAddr.arr[i] = ViscaIpCustomAddr.arr[i+1];
    }
    ViscaIpCustomAddr.length--;
    
    return 0;
}

/***************************************************************************************************
* Description:  管理自定义IP地址子菜单
***************************************************************************************************/
const char title_ManageCustomIp[] = "Manage Custom Address";
const struct uitem uitem_ManageCustomIp[] =
{
	{
        uitem_func,
        (int32_t)func_ModefyIpAddr,
		"Modify",
	},
    {
        uitem_func,
        (int32_t)func_DeleteIpAddr,
		"Delete",
	},
};

struct ulist_handle *ulist_ManageCustomIp;
int32_t func_ManageCustomIp(int32_t arg)
{
    switch (arg)
	{
        case arg_getbrief:
		break;
        
        case arg_getin:
        {
            struct uitem tmp_uitem; 
            UList_Init(&ulist_ManageCustomIp, title_ManageCustomIp, ON);
            Ulist_AppendItem(ulist_ManageCustomIp, uitem_ManageCustomIp, UITEM_LENGTH(uitem_ManageCustomIp));
            UList_Show(ulist_ManageCustomIp);
            UList_Handle(ulist_ManageCustomIp, func_ManageCustomIp);
            UList_Destroy(ulist_ManageCustomIp);
            UList_Destroy(MenuIndex);
            
            UList_Init(&MenuIndex, "Custom Address List", ON);
            
            for(int32_t i=0; i<ViscaIpCustomAddr.length; i++)
            {
                memset(&tmp_uitem, 0, sizeof(struct uitem));
                tmp_uitem.type = uitem_func;
                tmp_uitem.genius = (int32_t)func_ManageCustomIp;
                IpAddrToString(tmp_uitem.text, ViscaIpCustomAddr.arr[i]);
                Ulist_AppendItem(MenuIndex, &tmp_uitem, 1);
            }
            UList_Show(MenuIndex);
        }
        break;
        
        case arg_enter:
        case arg_right:
        {
            struct list_item *tmp = list_entry(ulist_ManageCustomIp->cur_item, struct list_item, list);
            if(tmp->genius != NULL)
            {
                struct list_item *tmp2 = list_entry(MenuIndex->cur_item, struct list_item, list);
                ((int32_t (*)(int32_t))tmp->genius)(tmp2->index);
            }
            return 1;
        }
        break;
        
        case arg_left:
        case arg_back:
        
        return 1;
        break; 
    }
    return 0;
}

int func_ViscaIpManageAddresses(int arg)
{
    switch (arg)
	{
        case arg_getbrief:
		break;
        
        case arg_getin:
        {
            struct uitem tmp_uitem; 
            UList_StackPush(&MenuStack, MenuIndex);
            UList_Init(&MenuIndex, "Custom Address List", ON);
            
            for(int32_t i=0; i<ViscaIpCustomAddr.length; i++)
            {
                memset(&tmp_uitem, 0, sizeof(struct uitem));
                tmp_uitem.type = uitem_func;
                tmp_uitem.genius = (int32_t)func_ManageCustomIp;
                IpAddrToString(tmp_uitem.text, ViscaIpCustomAddr.arr[i]);
                Ulist_AppendItem(MenuIndex, &tmp_uitem, 1);
            }
            UList_Show(MenuIndex);
            UList_Handle(MenuIndex, func_ViscaIpManageAddresses);
            UList_Destroy(MenuIndex);
            UList_StackPop(&MenuStack, &MenuIndex);
            UList_Show(MenuIndex);
        }
        break;
        
        case arg_enter:
        case arg_right:
        {
            struct list_item *tmp = list_entry(MenuIndex->cur_item, struct list_item, list);
            if(tmp->genius != NULL)
            {
                ((int32_t (*)(int32_t))tmp->genius)(arg_getin);
            }
        }
        return 0;
        
        break;
        
        case arg_left:
        case arg_back:
        
        return 1;
        break; 
    }
    return 0;
}

int func_ViscaIpAddNewAddress(int arg)
{
    uint32_t ipaddr;
    switch (arg)
	{
        case arg_getbrief:
		break;
        
        case arg_getin:
        if(ViscaIpCustomAddr.length < 50)
        {
            ipaddr = Edit_IpAddress(0);
            UList_Show(MenuIndex);
            if(ipaddr != 0)
            {
                ViscaIpCustomAddr.arr[ViscaIpCustomAddr.length] = ipaddr;
                ViscaIpCustomAddr.length++;
            }
        }
        else
        {
            
        }
		break;
        
        default:
		break;
	}
    
    return 0;
}

int func_ProtcOnvif(int arg)
{
	switch (arg)
	{
        case arg_getbrief:
        
		break;
        
        
		break;
        case arg_left:
        
		break;
        case arg_right:
        
		break;
        case arg_enter:
        
		break;
        case arg_back:
        
		break;
        default:
		break;
	}
	return 0;
}

const char title_ProtcVisca[] = "Visca";
const struct uitem uitem_ProtcVisca[] =
{
	{
        uitem_func,
        (int32_t)func_ViscaCompatible,
		"Compatible Mode",
	},
};
int func_ProtcVisca(int arg)
{
	switch (arg)
	{
        case arg_getbrief:
        break;
        
        case arg_getin:
        func_MenuGetin(title_ProtcVisca, uitem_ProtcVisca, UITEM_LENGTH(uitem_ProtcVisca), func_ProtcVisca);
        break;
        
        case arg_enter:
        case arg_right:
        func_MenuEnter();
		break;
        
        case arg_left:
        case arg_back:
        return 1;
		break;
	}
	return 0;
}

const char textViscaCompatible[][32] = {
    "Compatible Mode",
    "SONY",
    "JJTS/KATOV",
    "Other1",
    "Other2",
};
int func_ViscaCompatible(int arg)
{    
    switch (arg)
	{
        case arg_getbrief:
        return (int)textViscaCompatible[stSysPara.ptc.visca.compatible_mode+1];
		break;
        
        case arg_getin:
        Menu_Getin2(textViscaCompatible, TEXT_LENGTH(textViscaCompatible), \
            stSysPara.ptc.visca.compatible_mode, func_ViscaCompatible);
		break;
        
        case arg_enter:
        case arg_right:
        Menu_Select(&stSysPara.ptc.visca.compatible_mode);
        break;
        
        case arg_left:
        case arg_back:
        return 1;
		break;
	}
	return 0;
}

int func_ProtcPelcoD(int arg)
{
	switch (arg)
	{
        case arg_getbrief:
        
		break;
        
        
		break;
        case arg_left:
        
		break;
        case arg_right:
        
		break;
        case arg_enter:
        
		break;
        case arg_back:
        
		break;
        default:
		break;
	}
	return 0;
}
int func_ProtcPelcoP(int arg)
{
	switch (arg)
	{
        case arg_getbrief:
        
		break;
        
        
		break;
        case arg_left:
        
		break;
        case arg_right:
        
		break;
        case arg_enter:
        
		break;
        case arg_back:
        
		break;
        default:
		break;
	}
	return 0;
}
int func_ProtcPanasonic(int arg)
{
	switch (arg)
	{
        case arg_getbrief:
        
		break;
        
        
		break;
        case arg_left:
        
		break;
        case arg_right:
        
		break;
        case arg_enter:
        
		break;
        case arg_back:
        
		break;
        default:
		break;
	}
	return 0;
}
int func_ProtcSamsung(int arg)
{
	switch (arg)
	{
        case arg_getbrief:
        
		break;
        
		break;
        case arg_left:
        
		break;
        case arg_right:
        
		break;
        case arg_enter:
        
		break;
        case arg_back:
        
		break;
        default:
		break;
	}
	return 0;
}

/* 网络设置 -----------------------------------------------------------*/
const char title_Net[] = "Ethernet";
const struct uitem uitem_Net[] =
{
	{
        uitem_func,
        (int32_t)func_NetDhcp,
		"DHCP",
	},
    {
        uitem_func,
        (int32_t)func_NetIpAddr,
		"IP",
	},
    {
        uitem_func,
        (int32_t)func_NetMask,
		"Mask",
	},
    {
        uitem_func,
        (int32_t)func_NetGateway,
		"Gateway",
	},
    
};
int func_Net(int arg)
{
	switch (arg)
    {
        case arg_getbrief:
        break;
        
        case arg_getin:
        if(stSysPara.net.dhcp)
        {
            stSysPara.net.ipaddr = SwapIp(gnetif.ip_addr.addr);
            stSysPara.net.mask = SwapIp(gnetif.netmask.addr);
            stSysPara.net.gateway = SwapIp(gnetif.gw.addr);
        }
        func_MenuGetin(title_Net, uitem_Net, UITEM_LENGTH(uitem_Net), func_Net);
        break;
        
        case arg_enter:
        case arg_right:
        func_MenuEnter();
        break;
        
        case arg_left:
        case arg_back:
        return 1;
        break;
    }
	return 0;
}
const char textDhcp[][32] = {
    "DHCP",
    "OFF",
    "ON",
};
int func_NetDhcp(int arg)
{
    switch (arg)
	{
        case arg_getbrief:
        return (int)textDhcp[stSysPara.net.dhcp+1];
		break;
        
        case arg_getin:
        Menu_Getin2(textDhcp, TEXT_LENGTH(textDhcp), \
            stSysPara.net.dhcp, func_NetDhcp);
		break;
        
        case arg_enter:
        case arg_right:
        Menu_Select(&stSysPara.net.dhcp);
        break;
        
        case arg_left:
        case arg_back:
        return 1;
		break;
	}
    return 0;
}
static char str_ipaddr[16] = "";
int func_NetIpAddr(int arg)
{
	switch (arg)
	{
        case arg_getbrief:
        IpAddrToString(str_ipaddr, stSysPara.net.ipaddr);
        return (int)str_ipaddr;
		break;
        
        case arg_getin:
        {
            if(stSysPara.net.dhcp)
            {
                ShowHint("Error", "Not Editable!");
                vTaskDelay(1000);
            }
            else 
                stSysPara.net.ipaddr = Edit_IpAddress(stSysPara.net.ipaddr);
            UList_Show(MenuIndex);
        }
		break;
	}
	return 0;
}
int func_NetGateway(int arg)
{
	switch (arg)
	{
        case arg_getbrief:
        IpAddrToString(str_ipaddr, stSysPara.net.gateway);
        return (int)str_ipaddr;
		break;
        
        case arg_getin:
        {
            if(stSysPara.net.dhcp)
            {
                ShowHint("Error", "Not Editable!");
                vTaskDelay(1000);
            }
            else 
                stSysPara.net.gateway = Edit_IpAddress(stSysPara.net.gateway);
            UList_Show(MenuIndex);
        }
		break;
	}
	return 0;
}
int func_NetMask(int arg)
{
	switch (arg)
	{
        case arg_getbrief:
        IpAddrToString(str_ipaddr, stSysPara.net.mask);
        return (int)str_ipaddr;
		break;
        
        case arg_getin:
        {
            if(stSysPara.net.dhcp) 
            {
                ShowHint("Error", "Not Editable!");
                vTaskDelay(1000);
            }
            else 
                stSysPara.net.mask = Edit_IpAddress(stSysPara.net.mask);
            UList_Show(MenuIndex);
        }
		break;
	}
	return 0;
}

/* 串口设置 -----------------------------------------------------------*/
const char title_Serial[] = "Serial Port";
const struct uitem uitem_Serial[] =
{
	{
        uitem_func,
        (int32_t)func_Rs232UsbBaudRate,
		"RS232/USB Baud Rate",
	},
    {
        uitem_func,
        (int32_t)func_Rs422Rs485BaudRate,
		"RS422/RS485 Baud Rate",
	},
    {
        uitem_func,
        (int32_t)func_CmdRepeater,
		"Instruction Repeater",
	},
};
int func_Serial(int arg)
{
	switch (arg)
	{
        case arg_getbrief:
        break;
        
        case arg_getin:
        func_MenuGetin(title_Serial, uitem_Serial, UITEM_LENGTH(uitem_Serial), func_Serial);
        break;
        
        case arg_enter:
        case arg_right:
        func_MenuEnter();
		break;
        
        case arg_left:
        case arg_back:
        return 1;
		break;
	}
	return 0;
}
const char textRs232BaudRate[][32] =
{
    "RS232/USB Baud Rate",
    "2400",
    "4800",
    "9600",
    "19200",
    "38400",
    "115200",
};
int func_Rs232UsbBaudRate(int arg)
{
    switch (arg)
	{
        case arg_getbrief:
        return (int)textRs232BaudRate[stSysPara.serial.rs232_baudrate+1];
		break;
        
        case arg_getin:
        Menu_Getin2(textRs232BaudRate, TEXT_LENGTH(textRs232BaudRate), \
            stSysPara.serial.rs232_baudrate, func_Rs232UsbBaudRate);
		break;
        
        case arg_enter:
        case arg_right:
        Menu_Select(&stSysPara.serial.rs232_baudrate);
        break;
        
        case arg_left:
        case arg_back:
        return 1;
		break;
	}
    return 0;
}
const char textRs422BaudRate[][32] =
{
    "RS422/RS485 Baud Rate",
    "2400",
    "4800",
    "9600",
    "19200",
    "38400",
    "115200",
};
int func_Rs422Rs485BaudRate(int arg)
{
    switch (arg)
	{
        case arg_getbrief:
        return (int)textRs422BaudRate[stSysPara.serial.rs422_baudrate+1];
		break;
        
        case arg_getin:
        Menu_Getin2(textRs422BaudRate, TEXT_LENGTH(textRs422BaudRate), \
            stSysPara.serial.rs422_baudrate, func_Rs422Rs485BaudRate);
		break;
        
        case arg_enter:
        case arg_right:
        Menu_Select(&stSysPara.serial.rs422_baudrate);
        break;
        
        case arg_left:
        case arg_back:
        return 1;
		break;
	}
    return 0;
}
const char textCmdRepeater[][32] =
{
    "Instruction Repeater",
    "OFF",
    "RS232/USB As Input",
    "RS422/RS485 As Input",
};
int func_CmdRepeater(int arg)
{
    switch (arg)
	{
        case arg_getbrief:
        return (int)(stSysPara.serial.mode == 0 ? textCmdRepeater[1] : "ON");
		break;
        
        case arg_getin:
        Menu_Getin2(textCmdRepeater, TEXT_LENGTH(textCmdRepeater), \
            stSysPara.serial.mode, func_CmdRepeater);
		break;
        
        case arg_enter:
        case arg_right:
        Menu_Select(&stSysPara.serial.mode);
        break;
        
        case arg_left:
        case arg_back:
        return 1;
		break;
	}
    return 0;
}

/* 摇杆设置 -----------------------------------------------------------*/
const char title_Rocker[] = "Rocker";
const struct uitem uitem_Rocker[] =
{
	{
        uitem_func,
        (int32_t)func_PanReverse,
		"PAN Reverse",
	},
    {
        uitem_func,
        (int32_t)func_TiltReverse,
		"TILT Reverse",
	},
//    {
//        uitem_func,
//        (int32_t)func_RockerCorrect,
//		"Correct",
//	},
};
int func_Rocker(int arg)
{
	switch (arg)
	{
        case arg_getbrief:
        break;
        
        case arg_getin:
        func_MenuGetin(title_Rocker, uitem_Rocker, UITEM_LENGTH(uitem_Rocker), func_Rocker);
        break;
        
        case arg_enter:
        case arg_right:
        func_MenuEnter();
		break;
        
        case arg_left:
        case arg_back:
        return 1;
		break;
	}
	return 0;
}
const char textPanReverse[][32] =
{
    "PAN Reverse",
    "OFF",
    "ON",
};
int func_PanReverse(int arg)
{
    switch (arg)
	{
        case arg_getbrief:
        return (int)textPanReverse[stSysPara.rocker.pan_reverse+1];
		break;
        
        case arg_getin:
        Menu_Getin2(textPanReverse, TEXT_LENGTH(textPanReverse), \
            stSysPara.rocker.pan_reverse, func_PanReverse);
		break;
        
        case arg_enter:
        case arg_right:
        Menu_Select(&stSysPara.rocker.pan_reverse);
        break;
        
        case arg_left:
        case arg_back:
        return 1;
		break;
	}
    return 0;
}
const char textTiltReverse[][32] =
{
    "TILT Reverse",
    "OFF",
    "ON",
};
int func_TiltReverse(int arg)
{
    switch (arg)
	{
        case arg_getbrief:
        return (int)textTiltReverse[stSysPara.rocker.tilt_reverse+1];
		break;
        
        case arg_getin:
        Menu_Getin2(textTiltReverse, TEXT_LENGTH(textTiltReverse), \
            stSysPara.rocker.tilt_reverse, func_TiltReverse);
		break;
        
        case arg_enter:
        case arg_right:
        Menu_Select(&stSysPara.rocker.tilt_reverse);
        break;
        
        case arg_left:
        case arg_back:
        return 1;
		break;
	}
    return 0;
}
int func_RockerCorrect(int arg)
{
	return 0;
}

/* 蜂鸣器设置 -----------------------------------------------------------*/
const char textBuzzer[][32] =
{
    "Buzzer",
    "OFF",
    "ON",
};
int func_Buzzer(int arg)
{    
    switch (arg)
	{
        case arg_getbrief:
        return (int)textBuzzer[stSysPara.buzzer+1];
		break;
        
        case arg_getin:
        Menu_Getin2(textBuzzer, TEXT_LENGTH(textBuzzer), \
            stSysPara.buzzer, func_Buzzer);
		break;
        
        case arg_enter:
        case arg_right:
        Menu_Select(&stSysPara.buzzer);
        break;
        
        case arg_left:
        case arg_back:
        return 1;
		break;
	}
    return 0;
}

/* 休眠时间 -----------------------------------------------------------*/
const char textSleepTime[][32] =
{
    "Sleep Time",
    "1 Minute",
    "5 Minutes",
    "30 Minutes",
    "No Sleep",
};
int func_SleepTime(int arg)
{
    switch (arg)
	{
        case arg_getbrief:
        return (int)textSleepTime[stSysPara.sleep_time+1];
		break;
        
        case arg_getin:
        Menu_Getin2(textSleepTime, TEXT_LENGTH(textSleepTime), \
            stSysPara.sleep_time, func_SleepTime);
		break;
        
        case arg_enter:
        case arg_right:
        Menu_Select(&stSysPara.sleep_time);
        break;
        
        case arg_left:
        case arg_back:
        return 1;
		break;
	}
    return 0;
}

/* 语言设置 -----------------------------------------------------------*/
const char textLanguage[][32] =
{
    "Language",
    "English",
//    "Chinese",
};
int func_Language(int arg)
{    
    switch (arg)
	{
        case arg_getbrief:
        return (int)textLanguage[stSysPara.language+1];
		break;
        
        case arg_getin:
        Menu_Getin2(textLanguage, TEXT_LENGTH(textLanguage), \
            stSysPara.language, func_Language);
		break;
        
        case arg_enter:
        case arg_right:
        Menu_Select(&stSysPara.language);
        break;
        
        case arg_left:
        case arg_back:
        return 1;
		break;
	}
    return 0;
}

/* 设备信息 -----------------------------------------------------------*/
int func_AboutDevice(int arg)
{
    int8_t rocker_val;
    if(arg == arg_getbrief)
        return 0;
    
    ShowHint("Device Information", SOFTWARE_VERSION "\r\n" BUILD_TIME);
    
    while(1)
    {
        if(Rocker_GetPanVal(&rocker_val, 2))
        {
            if(rocker_val == -1)
                break;
        }
        
        if(GetKey(KEY_ROCKER, 0) || GetKey(KEY_BACK, 0))
        {
            break;
        }
    }
    UList_Show(MenuIndex);
    
    return 0;
}

/* 重置设备 -----------------------------------------------------------*/
const char textResetDevice[][32] =
{
    "Reset Device",
    "No",
    "Yes",
};
int func_ResetDevice(int arg)
{
    switch (arg)
	{
        case arg_getbrief:
		break;
        
        case arg_getin:
        Menu_Getin2(textResetDevice, TEXT_LENGTH(textResetDevice), 0, func_ResetDevice);
		break;
        
        case arg_enter:
        case arg_right:
        {
            int32_t val;
            Menu_Select(&val);
            if(val)
            {
                ShowHint("Warning", "Resetting...");
                System_ResetPara();
                vTaskDelay(2000);
            }
            return 1;
        }
        break;
        
        case arg_left:
        case arg_back:
        return 1;
		break;
	}
    return 0;
}

/***************************************************************************************************
* Description:  打开菜单
***************************************************************************************************/
const char title_MenuTop[32] = "Setting";
const struct uitem uitem_MenuTop[] =
{
	{
        uitem_func,
        (int32_t)func_SelProtc,
		"Select Protocol",
	},
	{
        uitem_list,
        (int32_t)func_Protc,
		"Protocol Setting",
	},
    {
        uitem_list,
        (int32_t)func_Net,
		"Ethernet",
	},
    {
        uitem_list,
        (int32_t)func_Serial,
		"Serial Port",
	},
    {
        uitem_list,
        (int32_t)func_Rocker,
		"Rocker",
	},
    {
        uitem_func,
        (int32_t)func_Buzzer,
		"Buzzer",
	},
    {
        uitem_func,
        (int32_t)func_SleepTime,
		"Sleep Time",
	},
    {
        uitem_func,
        (int32_t)func_Language,
		"Language",
	},
    {
        uitem_func,
        (int32_t)func_AboutDevice,
		"About Device",
	},
    {
        uitem_func,
        (int32_t)func_ResetDevice,
		"Reset Device",
	},
};


int32_t MenuOpen(int32_t arg)
{
    switch (arg)
	{
        case arg_getbrief:
		break;
        
        case arg_getin:
        {
            stRunInfo.menu = 1;
            UList_Init(&MenuIndex, title_MenuTop, ON);
            Ulist_AppendItem(MenuIndex, uitem_MenuTop, UITEM_LENGTH(uitem_MenuTop));
            UList_Show(MenuIndex);
            UList_Handle(MenuIndex, MenuOpen);
            UList_Destroy(MenuIndex);
            
            LED_Ctrl(KEY_MENU, LED_OFF);
            UART_Init();
            Ptc_Init();
            UI_DrawMainFrame();
            System_SavePara(SYS_SAVE_ALL);
        }
        break;
        
        case arg_enter:
        case arg_right:
        {
            struct list_item *tmp = list_entry(MenuIndex->cur_item, struct list_item, list);
            if(tmp->genius != NULL)
            {
                ((int32_t (*)(int32_t))tmp->genius)(arg_getin);
            }
        }
        break;
        
        case arg_left:
        case arg_back:
        
        return 1;
        break; 
    }
    return 0;
}
/****************************************** END OF FILE *******************************************/
