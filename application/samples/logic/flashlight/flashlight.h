// flashlight.h - 移除不必要的include
#ifndef __FLASHLIGHT_H__
#define __FLASHLIGHT_H__
#include "../../driver/button/key.h"
#include "../../driver/led_pwm/led_pwm.h"
#include "common_def.h"
#include "osal_debug.h"
#include "cmsis_os2.h"

#include "soc_osal.h"

#ifdef __cplusplus
extern "C" {
#endif

void flashlight_init(void);
void flashlight_task(void *parameter);
void flashlight_brightness_control(void);
void flashlight_switch_control(void);
void flashlight_save_settings(void);
void flashlight_load_settings(void);
void flashlight_task(void *parameter);

#ifdef __cplusplus
}
#endif

#endif