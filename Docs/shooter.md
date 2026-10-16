# Shooter 发射机构设计

分支 `shooter_test`，从 `basic_version` 拉出。参考工程为 `E:\rm\Code\27_3SE_hero\hero_up`，发射逻辑在 `PrivateApplications\MainControl\shootControl.c`。本文先整理逻辑和分层，控制算法暂不部署。

## 1. 原工程逻辑（hero_up）

### 1.1 电机
- 实际有 6 个摩擦轮，都是 3508 + C620，接在 FDCAN2 上，分前后两级，每级 3 个轮。
- 控制帧 `0x200` 依次放 LEFT、RIGHT、UP（电调 ID 1-3），控制帧 `0x1FF` 依次放 LEFT1、RIGHT1、UP1（电调 ID 5-7）。两帧的第 4 个槽位都空着。
- 代码里没有 inverse 配置，旋转方向直接写在目标转速的正负号里。RIGHT 和 RIGHT1 取负，其余四个取正。
- 变量名和实际位置对不上。按注释，LEFT 装在"后右"，UP 装在"前下"。另外 `front_fric_speed` 实际分给的是后级的三个轮。

### 1.2 状态
`fric_mode` 是一个 1 bit 位域，切换写成 `fric_mode++`，靠溢出实现翻转。

| 状态 | 进入条件 | 目标转速 (rpm) |
|---|---|---|
| STOP | `ctrl_terminal == CONTROL_STOP`，或双拨杆打上触发保护 | 全部为 0 |
| FRIC_OFF | 上电后的初始状态；在 ON 状态下按 F 或拨左拨杆再切一次 | ±200 低速反转，防止弹丸卡在轮间 |
| FRIC_ON 常规 | PC 模式按 F 键的上升沿，或遥控左拨杆拨到上的上升沿 | ±3110 |
| FRIC_ON 吊射 | ON 状态下 `sniper == ON` | 一级 ±4300，另一级 ±4320 |

发送层还有一道保护：如果约 40 ms 内没收到下板的 B2B 帧，0x200 和 0x1FF 都改发 0。

### 1.3 控制
- 目标转速由 DecisionTask 生成，周期 10 ms。速度环跑在 ControlTask 里，周期约 2 ms。
- 6 路共用同一组 PD 参数：kp = 17.5，kd = 12.5，ki = 0，输出限幅 ±8000。反馈直接用 rpm 原始值，不滤波。
- 没有前馈，启停没有斜坡，也没有到速判定。

### 1.4 不要照搬的问题
1. 上下板各自根据键位翻转 `fric_mode`，两边没有同步。可能出现下板已经开始拨弹、上板摩擦轮却还在反转的情况。
2. 没有 fric ready 信号。摩擦轮离线标志 `fricMotorError` 已经算出来了，但没有任何地方用它。
3. 反馈 ID 0x204 会被映射到 idx 2，和 0x203 冲突。
4. 裁判弹速统计、卡尔曼滤波、最小二乘弹速自适应这几块都是死代码，从来没有参与控制。
5. D 项不滤波，而目标每 10 ms 阶跃一次，每次阶跃都会在 D 项上打出尖峰。

## 2. 本工程设计

### 2.1 电机注册
需求是 3 个摩擦轮，对应原工程的单级结构，也就是 `SHOOT_SINGLE_STAGE_MODE`。

- 电机类型为 `RMMotor_t`，型号 `RM_C620`，模式 `RM_CURRENT`。
- 3 个电机放在同一个 `RMMotorGroup_t` 里，发送 ID 为 0x200，电调 ID 为 1、2、3。
- 旋转方向用电机自己的 `inverse` 字段配置，目标转速一律写正值。
- 3 个电机都要填进 `canXMotorBindings` 绑定表，反馈 ID 为 0x201 到 0x203。
- 模块内部用 `Motor_t *motor[SHOOTER_FRIC_COUNT]` 引用电机，不直接依赖具体型号。

### 2.2 状态机
| 状态 | 含义 | 输出 |
|---|---|---|
| `SHOOTER_PROTECTED` | 保护：遥控丢失、双上拨、电机离线 | 电流全部为 0，PID 清零，`fric_ready = 0` |
| `SHOOTER_ISFRIC` | 摩擦轮工作 | 速度闭环到目标转速 |

状态切换：
- `PROTECTED → ISFRIC`：上层请求开启摩擦轮，并且 3 个电机都在线。
- `ISFRIC → PROTECTED`：上层请求关闭摩擦轮，或任意一个电机离线。
- state 由上层直接给出，模块内部不做 toggle，按键边沿检测放在 Task 层（和 gimbal 一致）。

原工程 FRIC_OFF 下的低速反转（防卡弹）不单独设一个状态。它作为 ISFRIC 的一个目标档位，`SHOOTER_SPEED_REVERSE`，是否需要等调试时再决定。

### 2.3 分层
调用顺序和 gimbal 一样，每个周期依次调用 `Shooter_Input → Shooter_Estimate → Shooter_Control`。

**Input 输入层**
- 输入：`state`，加上转速档位 `speed_level`。档位取值为常规、吊射、反转，后面可能还会加弹速自适应修正。
- 处理：
  - 把档位查表换成 3 个电机各自的目标转速。档位参数放在 config 里。
  - 换档时目标做斜坡，防止阶跃引起 D 项尖峰和电流冲击。
  - PROTECTED 状态下忽略输入，目标清 0。

**Estimate 估计层**
- 读取每个电机的 `feedback`：转速（rad/s 或 rpm，单位全模块统一）、在线状态（`status` 加上 timestamp 是否在更新）。
- 计算 `fric_ready`：3 个轮的 |目标 − 反馈| 都小于容差，并且保持 N 个周期，才置位。
- 把 `fric_ready` 通过接口函数交给 Task 层，Task 层再转给 stir 作为开火许可。不允许 stir 直接读 shooter 的结构体。

**Control 控制层**
- 3 路速度环，结果写入 `motorcmd.current`。之后调用 `motorops.sendcommand`，同组只需要发一帧。
- 暂时只留接口：PID 参数结构体和前馈预留项 `feedforward`。PID 参数和前馈量等调参时再填。

### 2.4 结构体草案
```c
typedef enum
{
    SHOOTER_PROTECTED,
    SHOOTER_ISFRIC
} ShooterState_t;

typedef enum
{
    SHOOTER_SPEED_NORMAL,
    SHOOTER_SPEED_SNIPER,
    SHOOTER_SPEED_REVERSE,
    SHOOTER_SPEED_COUNT
} ShooterSpeedLevel_t;

typedef struct
{
    ShooterState_t state;
    ShooterSpeedLevel_t speed_level;
} ShooterInput_t;

typedef struct
{
    float speed_rpm[SHOOTER_FRIC_COUNT];
    uint8_t online[SHOOTER_FRIC_COUNT];
    uint8_t fric_ready;
} ShooterObserver_t;

typedef struct
{
    Motor_t *motor[SHOOTER_FRIC_COUNT];
    float speed_table_rpm[SHOOTER_SPEED_COUNT];
    float ramp_rpm_per_tick;
    float ready_tolerance_rpm;
    uint16_t ready_hold_tick;
    ShooterPIDParam_t speed_pid;
} ShooterConfig_t;
```

## 3. 依赖与待定事项
- `basic_version` 里还没有 gimbal_test 上做的改动：Motor 基类的 `can`、`Motor_BaseInit`，以及 `RateDivider`。要么先把这几项挑到本分支，要么等 gimbal_test 合入 master 后再 rebase。
- 待确认：
  - 3 个轮的旋转方向，以及电调 ID 怎么分配。
  - 目标转速档位的数值。原工程常规档是 3110 rpm，吊射档是 4300 rpm，这是按双级结构调出来的值，单级需要重新标定。
  - 是否保留低速反转。
- 和 stir 的接口：shooter 只输出 `fric_ready`。热量计算和开火判断都放在 stir 一侧，见 `stir_test` 分支的 `Docs/stir.md`。
