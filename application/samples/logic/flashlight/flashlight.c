#include "flashlight.h"
#include "sfc.h"
static int8_t g_brightness = 50; // 手电筒默认亮度
static bool g_flashlight_active = false;

// 手电筒初始化
void flashlight_init(void)
{
    // 从存储加载保存的亮度设置
    flashlight_load_settings();
    
    // 初始状态为关闭
    g_flashlight_active = false;
    led_pwm_turn_off();
}


// 手电筒亮度调节状态机
void flashlight_brightness_control(void)
{
    int8_t roll_event = menu_Roll_event();
    
    if (roll_event != 0) {
        g_brightness += roll_event;
        
        // 限制亮度范围
        if (g_brightness > 100) g_brightness = 100;
        if (g_brightness < 0) g_brightness = 0;
        
        led_pwm_set_brightness(g_brightness);
    }
}

// 手电筒开关控制
void flashlight_switch_control(void)
{
    int8_t enter_event = menu_Enter_event();
    
    if (enter_event == 1) {
        // 短按 - 切换开关状态
        g_flashlight_active = !g_flashlight_active;
        if (g_flashlight_active) {
            led_pwm_set_brightness(g_brightness);
        } else {
            led_pwm_set_brightness(0);
        }
    } else if (enter_event == 2) {
        // 长按 - 保存设置并关闭
        g_flashlight_active = false;
        led_pwm_set_brightness(0);
        flashlight_save_settings();
    }
}
// 定义Flash配置结构体（根据文档中的示例）
const sfc_flash_config_t sfc_cfg = {
    .read_type = FAST_READ_QUAD_OUTPUT,    // 快速四线输出读取
    .write_type = PAGE_PROGRAM,            // 页编程写入
    .mapping_addr = 0x200000,              // Flash映射地址
    .mapping_size = 0x800000,              // Flash映射大小
};

// 定义存储地址（确保不与其他数据冲突）
#define FLASHLIGHT_CONFIG_ADDR 0x210000

// 保存手电筒设置到Flash
void flashlight_save_settings(void)
{
    uint16_t temp = (0xAE << 8) | g_brightness;
    errcode_t ret;
    
    // 初始化Flash控制器
    ret = uapi_sfc_init((sfc_flash_config_t *)&sfc_cfg);
    if (ret != ERRCODE_SUCC) {
        osal_printk("Flash init failed: %d\n", ret);
        return;
    }
    
    // 注意：Flash写入前通常需要先擦除对应扇区
    // 如果需要擦除（可选）
    uapi_sfc_reg_erase(FLASHLIGHT_CONFIG_ADDR, sizeof(uint16_t));
    // 写入数据到Flash指定地址
    ret = uapi_sfc_reg_write(FLASHLIGHT_CONFIG_ADDR, (uint8_t*)&temp, sizeof(temp));
    if (ret != ERRCODE_SUCC) {
        osal_printk("Flash write failed: %d\n", ret);
    } else {
        osal_printk("Brightness %d saved to Flash\n", g_brightness);
    }
}

// 从Flash加载手电筒设置
void flashlight_load_settings(void)
{
    uint16_t saved_value;
    errcode_t ret;
    
    // 初始化Flash控制器
    ret = uapi_sfc_init((sfc_flash_config_t *)&sfc_cfg);
    if (ret != ERRCODE_SUCC) {
        osal_printk("Flash init failed: %d\n", ret);
        return;
    }
    
    // 从Flash读取保存的数据
    ret = uapi_sfc_reg_read(FLASHLIGHT_CONFIG_ADDR, (uint8_t*)&saved_value, sizeof(saved_value));
    if (ret != ERRCODE_SUCC) {
        osal_printk("Flash read failed: %d\n", ret);
        return;
    }
    
    // 验证魔数并恢复亮度设置
    if ((saved_value >> 8) == 0xAE) {
        g_brightness = saved_value & 0xFF;
        if (g_brightness > 100) {
            g_brightness = 50;  // 安全限制
        }
        osal_printk("Brightness %d loaded from Flash\n", g_brightness);
    } else {
        osal_printk("Invalid Flash data, using default brightness\n");
        g_brightness = 50;  // 默认亮度
    }
}

// 手电筒主任务函数
void flashlight_task(void *parameter)
{
    (void)parameter;
    
    flashlight_init();
    
    while (1) {
        if (g_flashlight_active) {
            // 开启状态：处理亮度和开关控制
            flashlight_brightness_control();
            flashlight_switch_control();
        } else {
            // 关闭状态：只检测开启事件
            int8_t enter_event = menu_Enter_event();
            if (enter_event == 1) {
                g_flashlight_active = true;
                led_pwm_set_brightness(g_brightness);
            }
        }
        
        osDelay(20);
    }
}