// led_pwm.c
#include "led_pwm.h"
#define PWM_TOTAL_PERIOD 1000000 // 1秒钟的周期
/*pwm更新逻辑
while (cfg_repeat.low_time <= PWM_HIGH_TIME_CYC) {
    uapi_pwm_update_cfg(CONFIG_PWM_CHANNEL, &cfg_repeat);  // 这里更新PWM配置
    uapi_tcxo_delay_ms((uint32_t)TEST_TCXO_DELAY_500MS);
    osal_printk("pwm cyc_done_cnt:%u, duty_radio:[%u/%u]\r\n", g_pwm_cyc_done_cnt,
        cfg_repeat.high_time, cfg_repeat.high_time + cfg_repeat.low_time);
    cfg_repeat.low_time++;
    cfg_repeat.high_time--;
}
    更新函数：uapi_pwm_update_cfg()
*/
//#define LED_PWM_PIN  GPIO_04  // 根据实际引脚定义
static uint32_t g_pwm_cyc_done_cnt = 0;
static bool g_pwm_initialized = false;
static uint8_t g_brightness = 50;  // 默认亮度50%
// 在文件开头添加宏定义检查
#ifndef CONFIG_PWM_CHANNEL
    #ifdef CONFIG_PWM_CHANNEL_NUM
        #define CONFIG_PWM_CHANNEL CONFIG_PWM_CHANNEL_NUM
    #else
        // 如果没有定义，使用默认值
        #define CONFIG_PWM_CHANNEL 4
    #endif
#endif
static errcode_t led_pwm_callback(uint8_t channel)
{
    unused(channel);
    g_pwm_cyc_done_cnt++;
    return ERRCODE_SUCC;
}
void led_pwm_deinit(void)
{
    if (g_pwm_initialized) {
        uapi_pwm_close(CONFIG_PWM_CHANNEL);
        uapi_pwm_deinit();
        g_pwm_initialized = false;
    }
}
void led_pwm_init(void)
{
    
    pwm_config_t cfg_repeat = {
        PWM_LOW_TIME_CYC,
        PWM_HIGH_TIME_CYC,
        0,
        0xFF,
        true
    };
    uapi_gpio_set_dir(CONFIG_LED_PWM_PIN, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(CONFIG_LED_PWM_PIN, GPIO_LEVEL_HIGH);//高电平输出？

    uapi_pwm_init();
    uapi_pwm_open(CONFIG_PWM_CHANNEL, &cfg_repeat);
    uapi_pwm_register_interrupt(CONFIG_PWM_CHANNEL, led_pwm_callback);//pwm中断回调

    uapi_pwm_start(CONFIG_PWM_CHANNEL);
   osal_printk("LED PWM初始化完成");
}
// 如果PWM频率太高导致LED闪烁，增大这个值period
// 如果PWM频率太低导致亮度调节不细腻，减小这个值
// 如果需要不同的PWM行为，可以调整：
// .cycles = 0xFF,     // 持续输出
// .cycles = 10,       // 只输出10个周期后停止
// .repeat = true,     // 重复模式
// .repeat = false,    // 单次模式
void led_pwm_set_brightness(uint8_t brightness)
{
    if (brightness > 100) brightness = 100;
    //计算高电平低电平持续时间
    uint32_t high_time = (PWM_TOTAL_PERIOD * brightness) / 100;
    uint32_t low_time = PWM_TOTAL_PERIOD - high_time;

    // 创建PWM配置结构体
    pwm_config_t pwm_cfg = {
        .low_time = low_time,    // 低电平时间（与亮度互补）
        .high_time = high_time,         // 高电平时间（直接对应亮度）
        0,                               // 保留参数
        0xFF,                            // 周期参数
        true                             // 重复模式
    };
    
    // 更新PWM配置[1,2](@ref)
    uapi_pwm_update_cfg(CONFIG_PWM_CHANNEL, &pwm_cfg);
    
    g_brightness = brightness;
     // 可选：添加调试信息
    osal_printk("PWM亮度设置: %d%%, 高电平: %lu, 低电平: %lu, 总周期: %lu\r\n", 
                brightness, high_time, low_time, PWM_TOTAL_PERIOD);
}
// 获取当前亮度
uint8_t led_pwm_get_brightness(void)
{
    return
 g_brightness;
}

// 开启手电筒
void led_pwm_turn_on(void)
{
    led_pwm_set_brightness(g_brightness);
}

// 关闭手电筒  
void led_pwm_turn_off(void)
{
    led_pwm_set_brightness(0);
}