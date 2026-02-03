 #include "key.h"
 /*滚轮逻辑*/
 // 按键事件标志（硬件层面）
static volatile bool g_key_pressed = false;
static volatile bool g_roll_up_pressed = false;
static volatile bool g_roll_down_pressed = false;
// 按键事件状态机变量
static struct {
    bool last_key_state;
    uint32_t press_start_time;
    uint8_t debounce_counter;
} g_enter_state = {0};
static struct {
    bool last_up_state;
    bool last_down_state;
} g_roll_state = {0};

void gpio_callback_func(pin_t pin,uintptr_t param)
 {
    (void)param;
    osDelay(10);
    uint32_t pin_state = hal_gpio_get_inval(pin);
    if (pin == CONFIG_PUSH_BUTTON_PIN) {
       g_key_pressed = (pin_state==0);
 }
    else if (pin == CONFIG_ROLL_UP_PIN) {
        g_roll_up_pressed = (pin_state==0);
    }
    else if (pin == CONFIG_ROLL_DOWN_PIN) {
        g_roll_down_pressed = (pin_state==0);
    }
    uapi_gpio_clear_interrupt(pin);
    return ;
}
// 新增：获取原始按键状态（供中断使用）
bool key_get_push_state(void) { return g_key_pressed; }
bool key_get_roll_up_state(void) { return g_roll_up_pressed; }
bool key_get_roll_down_state(void) { return g_roll_down_pressed; }
 void led_key_init(void)
 {
    //引脚复用模式GPIO
    uapi_pin_set_mode(CONFIG_ROLL_UP_PIN, HAL_PIO_FUNC_GPIO); 
    uapi_pin_set_mode(CONFIG_ROLL_DOWN_PIN, HAL_PIO_FUNC_GPIO); 
    uapi_pin_set_mode(CONFIG_PUSH_BUTTON_PIN, HAL_PIO_FUNC_GPIO); 
    //输入方向
    uapi_gpio_set_dir(CONFIG_ROLL_UP_PIN, GPIO_DIRECTION_INPUT);
    uapi_gpio_set_dir(CONFIG_ROLL_DOWN_PIN, GPIO_DIRECTION_INPUT);
    uapi_gpio_set_dir(CONFIG_PUSH_BUTTON_PIN, GPIO_DIRECTION_INPUT);
    //上拉输入
    uapi_pin_set_pull(CONFIG_ROLL_UP_PIN, PIN_PULL_TYPE_UP);
    uapi_pin_set_pull(CONFIG_ROLL_DOWN_PIN, PIN_PULL_TYPE_UP);
    uapi_pin_set_pull(CONFIG_PUSH_BUTTON_PIN, PIN_PULL_TYPE_UP);

    uapi_gpio_register_isr_func(CONFIG_ROLL_UP_PIN, GPIO_INTERRUPT_DEDGE, gpio_callback_func);//注册GPIO中断回调  
    uapi_gpio_register_isr_func(CONFIG_ROLL_DOWN_PIN, GPIO_INTERRUPT_DEDGE,gpio_callback_func);//注册GPIO中断回调  
    uapi_gpio_register_isr_func(CONFIG_PUSH_BUTTON_PIN, GPIO_INTERRUPT_DEDGE, gpio_callback_func);//注册GPIO中断回调 
    
    g_enter_state.last_key_state = false;
    g_enter_state.press_start_time = 0;
    g_enter_state.debounce_counter = 0;
    
    g_roll_state.last_up_state = false;
    g_roll_state.last_down_state = false; 

 }
 
 int8_t menu_Enter_event(void)
{
    static bool processing_event = false;
    
    if (processing_event || !g_key_pressed) {
        return 0;
    }
    
    processing_event = true;
    
    // 检测按键持续时间
    osDelay(20); // 等待稳定
    uint32_t start_time = osKernelGetTickCount();
    
    // 等待按键释放或超时
    while (g_key_pressed) {
        if (osKernelGetTickCount() - start_time > 1000) { // 长按1秒
            processing_event = false;
            g_key_pressed = false; // 清除标志
            return 2; // 长按事件
        }
        osDelay(10);
    }
    
    processing_event = false;
    return 1; // 短按事件
}

int8_t menu_Roll_event(void)
{
    int8_t result = 0;
    
    if (g_roll_up_pressed) {
        result = -1; // 向上滚动
        g_roll_up_pressed = false; // 清除标志
    } else if (g_roll_down_pressed) {
        result = 1;  // 向下滚动  
        g_roll_down_pressed = false; // 清除标志
    }
    
    return result;
}



