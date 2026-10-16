#ifndef SHOOTER_H
#define SHOOTER_H

#include <stdint.h>

#include "Motor.h"
#include "algorism.h"
#include "pid.h"

/* 摩擦轮转速统一 rpm，电流指令为电机协议原始量
 * 每个控制周期依次调用：Shooter_Input -> Shooter_Estimate -> Shooter_Control
 */

#define SHOOTER_FRIC_COUNT      3U

/* 参考 hero_up 的取值，仅作为填写 ShooterConfig_t 的默认参考 */
#define SHOOTER_REF_SPEED_KP        17.5f
#define SHOOTER_REF_SPEED_KD        12.5f
#define SHOOTER_REF_OUTPUT_MAX      8000.0f
#define SHOOTER_REF_FRIC_RPM        3110.0f
#define SHOOTER_REF_BACK_RPM        200.0f

/*--------------------------------------状态机--------------------------------------*/
typedef enum {
    SHOOTER_PROTECTED,  ///< 保护：输出清零，PID 清零
    SHOOTER_ISBACK,     ///< 回转：低速反转，防止弹丸卡在轮间
    SHOOTER_ISFRIC      ///< 摩擦轮工作：闭环到目标转速
} ShooterState_t;

/*--------------------------------------输入层--------------------------------------*/
typedef struct {
    ShooterState_t state;
    float fric_speed_rpm;   ///< ISFRIC 下的目标转速（正值，方向由电机 inverse 决定）
} ShooterInput_t;

typedef struct {
    float speed_rpm[SHOOTER_FRIC_COUNT];
} ShooterTarget_t;

/*--------------------------------------估计层--------------------------------------*/
typedef struct {
    float speed_rpm[SHOOTER_FRIC_COUNT];    ///< 已按 inverse 折算，与目标同号
    uint8_t online[SHOOTER_FRIC_COUNT];
    uint16_t ready_count;                   ///< 连续到速的周期数
    uint8_t fric_ready;                     ///< 三个轮都到速并保持
    uint32_t last_timestamp[SHOOTER_FRIC_COUNT];
    uint16_t offline_count[SHOOTER_FRIC_COUNT];
} ShooterObserver_t;

/*--------------------------------------控制层--------------------------------------*/
typedef struct {
    float kp;
    float ki;
    float kd;
    float sum_error_max;
    float output_max;
} ShooterPIDParam_t;

typedef struct {
    PIDStruct speed_pid[SHOOTER_FRIC_COUNT];
    float output[SHOOTER_FRIC_COUNT];       ///< 写入电机的电流指令
} ShooterController_t;

/*--------------------------------------对象与构造参数--------------------------------------*/
typedef struct {
    Motor_t *motor[SHOOTER_FRIC_COUNT];
    ShooterPIDParam_t speed_param;
    float back_speed_rpm;           ///< ISBACK 的反转转速（正值）
    float ready_tolerance_rpm;      ///< 到速判定的转速误差
    uint16_t ready_hold_tick;       ///< 到速需连续保持的周期数
    uint16_t offline_tick;          ///< 反馈帧计数连续不变多少周期判定离线
    ShooterState_t init_state;
} ShooterConfig_t;

typedef struct {
    ShooterState_t state;
    Motor_t *motor[SHOOTER_FRIC_COUNT];
    ShooterTarget_t target;
    ShooterObserver_t observer;
    ShooterController_t controller;
    ShooterPIDParam_t speed_param;
    float back_speed_rpm;
    float fric_speed_rpm;
    float ready_tolerance_rpm;
    uint16_t ready_hold_tick;
    uint16_t offline_tick;
} Shooter_t;

/* 构造：把三个电机切到电流模式 */
void Shooter_Init(Shooter_t *shooter, const ShooterConfig_t *config);
/* 输入层：状态切换与目标转速；任一电机离线时强制进入保护 */
void Shooter_Input(Shooter_t *shooter, const ShooterInput_t *input);
/* 估计层：读取电机转速，判断在线与到速 */
void Shooter_Estimate(Shooter_t *shooter);
/* 控制层：三路速度环，结果写入 motorcmd 并发送 */
void Shooter_Control(Shooter_t *shooter);
/* 摩擦轮是否已到速，供上层作为开火许可 */
uint8_t Shooter_IsFricReady(const Shooter_t *shooter);

#endif
