#ifndef HALLSENSOR_H
#define HALLSENSOR_H
#include "pinctrl.h"
#include "common_def.h"
#include "soc_osal.h"
#include "hal_gpio.h"
#include "cmsis_os2.h" 

// 状态定义
typedef enum {
    CAP_OFF = 0,    // 帽子未扣上
    CAP_ON = 1,     // 帽子已扣上
    STATE_ERROR = -1
} CapState;
/**
 * 霍尔传感器初始化
 * 配置GPIO引脚为输入模式
 */
void hall_sensor_init(void);

/**
 * 获取当前帽子状态（带消抖）
 * @return CapState 帽子状态
 */
CapState get_cap_state(void);

/**
 * 检查帽子是否刚被扣上（上升沿检测）
 * @return bool true-刚扣上, false-未变化或已扣上
 */
bool is_cap_just_put_on(void);

/**
 * 检查帽子是否刚被取下（下降沿检测）
 * @return bool true-刚取下, false-未变化或已取下
 */
bool is_cap_just_taken_off(void);

/**
 * 等待帽子状态变化
 * @param timeout_ms 超时时间(毫秒)
 * @return CapState 新的状态，超时返回STATE_ERROR
 */
CapState wait_for_cap_state_change(uint32_t timeout_ms);

/**
 * 传感器诊断信息打印
 */
void hall_sensor_diagnostic(void);

#endif /* HALLSENSOR_H */