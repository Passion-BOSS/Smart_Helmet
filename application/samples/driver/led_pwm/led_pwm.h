#ifndef _LED_PWM_H_
#define _LED_PWM_H_
#include "pinctrl.h"
#include "gpio.h"
#include "soc_osal.h"
#include "app_init.h"
#include "common_def.h"
#include "pwm.h"
#include "tcxo.h"

#define CONFIG_PWM_CHANNEL 4  // 默认值

/*// 核心职责：
1. 手电筒状态机管理（开/关/调光）
2. 亮度级别保存和恢复
3. 用户操作处理逻辑
4. 与其他模块的协调
// 不应该包含：硬件直接操作、PWM底层配置*/

#define TEST_TCXO_DELAY_500MS      500
#define PWM_LOW_TIME_CYC           0
#define PWM_HIGH_TIME_CYC          20

//static void led_pwm_task(void *arg);
void led_pwm_init(void);
void led_pwm_deinit(void);
void led_pwm_set_brightness(uint8_t brightness);
uint8_t led_pwm_get_brightness(void);
void led_pwm_turn_on(void);
void led_pwm_turn_off(void);
#endif /* _LED_PWM_H_ */