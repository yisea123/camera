/***************************************************************************************************
* FILE: ptz.h
*
* DESCRIPTION: --
*
* CREATED: 2017/11/15, by kevin xu
***************************************************************************************************/
#ifndef PTZ_H
#define PTZ_H
#include "bsp.h"
#include "ms41909.h"

#ifdef __cplusplus
extern "C" {
#endif

/****************************************** 云台位置参数 ******************************************/
#define MOTOR_DRIVER_FREQUENCY  50

/* 驱动细分数 */
#define DRIVER_SUB_RATE         8   
/* VISCA协议一个单位对应的角度 */
#define VISCA_DEGREE_UNIT       (0.075f)    

#define PAN_MOTOR_STEP_ANG      (0.9f)  /* 电机步距角 */
#define PAN_REDUCTION_RATIO     (8.0f)  /* 结构减速比 */
#define PAN_ONE_STEP_ANG        (PAN_MOTOR_STEP_ANG / PAN_REDUCTION_RATIO / DRIVER_SUB_RATE)   /* 转动一微步云台实际转动的角度 */
#define PAN_ONE_DEGREE_STEPS    (1.0f / PAN_ONE_STEP_ANG)  /* 云台转一度的微步数 */
#define PAN_ONE_DEGREE_SPEED    (PAN_ONE_DEGREE_STEPS / 50)
#define PAN_VISCA_STEPS         (VISCA_DEGREE_UNIT / PAN_ONE_STEP_ANG)  /* 云台转动VISCA一个的微步数 */

#define PAN_DIR_RIGHT           0
#define PAN_DIR_LEFT            1
#define PAN_CK_POS              (-10750)
#define PAN_HOME                0
#define PAN_MAX_POS             (int32_t)(PAN_ONE_DEGREE_STEPS * 165)
#define PAN_MINI_POS            (int32_t)(PAN_ONE_DEGREE_STEPS * -165)

#define TILT_MOTOR_STEP_ANG     (1.8f)
#define TILT_REDUCTION_RATIO    (4.0f)
#define TILT_ONE_STEP_ANG       (TILT_MOTOR_STEP_ANG / TILT_REDUCTION_RATIO / DRIVER_SUB_RATE)
#define TILT_ONE_DEGREE_STEPS   (1.0f / TILT_ONE_STEP_ANG)  /* 云台转一度的微步数 */
#define TILT_ONE_DEGREE_SPEED   (TILT_ONE_DEGREE_STEPS / 50)
#define TILT_VISCA_STEPS        (VISCA_DEGREE_UNIT / TILT_ONE_STEP_ANG)
    
#define TILT_DIR_UP             0
#define TILT_DIR_DOWN           1
#define TILT_CK_POS             (-520)
#define TILT_HOME               0
#define TILT_MAX_POS            (int32_t)(TILT_ONE_DEGREE_STEPS * 90) 
#define TILT_MINI_POS           (int32_t)(TILT_ONE_DEGREE_STEPS * -30)

/**************************************** 云台预设速度参数 ****************************************/
#define PAN_BRAKE_DEC           5
#define PAN_GOHOME_SPEED        60
#define PAN_GOHOME_ACC          3
#define PAN_GOHOME_DEC          3
#define PAN_PRESET_SPEED        60
#define PAN_PRESET_ACC          3
#define PAN_PRESET_DEC          3
#define PAN_CMD_DEC             3
#define PAN_REMOTE_DEC          3

#define PAN_AGING_SPEED         60

#define TILT_BRAKE_DEC          2
#define TILT_GOHOME_SPEED       60
#define TILT_GOHOME_ACC         1
#define TILT_GOHOME_DEC         1
#define TILT_PRESET_SPEED       60
#define TILT_PRESET_ACC         1
#define TILT_PRESET_DEC         1
#define TILT_CMD_DEC            1
#define TILT_REMOTE_DEC         1

#define TILT_AGING_SPEED        40

#define PAN_ACC     2
#define PAN_DEC     5
#define TILT_ACC    1
#define TILT_DEC    1


#define PAN     MS41909_MOTOR1
#define TILT    MS41909_MOTOR2

#define PAN_RESET_DIR       (-1)
#define PAN_RESET_DISTANCE  25600
#define PAN_RESET_SPEED     50
#define PAN_RESET_ACC       1
#define PAN_RESET_DEC       5

#define TILT_RESET_DIR      (-1)
#define TILT_RESET_DISTANCE 25600
#define TILT_RESET_SPEED    5
#define TILT_RESET_ACC      1
#define TILT_RESET_DEC      5

#define PT_RESET_TIMEOUT    30000

/* 速度计算 */
void CalPtzSpeed(void);
uint8_t TransPanSpeed(uint8_t speed);
uint8_t TransTiltSpeed(uint8_t speed);
uint8_t GetPresetSpeed(int32_t motor, int32_t cur_pos, int32_t tar_pos);

/* 初始化 */
void PanTilt_Init(void);
void PanTilt_Reset(int32_t panPos, int32_t tiltPos);
void Ptz_Init(void);
void PTZ_CheckPos(void);

#ifdef __cplusplus
}
#endif
#endif
/****************************************** END OF FILE *******************************************/
