#ifndef _KEY_H_
#define _KEY_H_
#include <stdint.h>
#include <stdio.h>
#include "app_init.h"
#include "pinctrl.h"
#include "gpio.h"
#include "common_def.h"
#include "osal_debug.h"
#include "cmsis_os2.h"

// #define ROLL_UP_PIN GPIO_00
// #define ROLL_DOWN_PIN GPIO_01
// #define PUSH_BUTTON_PIN GPIO_02
void led_key_init(void);
int8_t menu_Enter_event(void);
int8_t menu_Roll_event(void);
bool key_get_push_state(void);
bool key_get_roll_up_state(void);
bool key_get_roll_down_state(void);

#endif /* _KEY_H_ */