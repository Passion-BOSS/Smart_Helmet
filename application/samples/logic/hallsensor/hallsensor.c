#include "hallsensor.h"
#include <stdbool.h>

// 消抖阈值配置
#define DEBOUNCE_THRESHOLD 3      // 连续3次检测相同状态才确认
#define CHECK_INTERVAL_MS 10      // 检测间隔(毫秒)
// 或者使用条件编译

#ifndef CONFIG_HALL_SENSOR_PIN
//#warning "CONFIG_HALL_SENSOR_PIN not defined, using default pin 5"
#define CONFIG_HALL_SENSOR_PIN 5
#endif

// 全局变量
static bool sensor_initialized = false;
static CapState last_stable_state = STATE_ERROR;
static uint32_t debounce_counter = 0;

/**
 * 霍尔传感器初始化
 */
void hall_sensor_init(void) {
    uapi_pin_set_mode(CONFIG_HALL_SENSOR_PIN, GPIO_DIRECTION_INPUT);
    sensor_initialized = true;
    osal_printk("霍尔传感器初始化完成，GPIO引脚: %d\n", CONFIG_HALL_SENSOR_PIN);
}

/**
 * 读取原始传感器状态（无消抖）
 */
static CapState read_raw_sensor_state(void) {
    if (!sensor_initialized) {
        osal_printk("错误: 传感器未初始化\n");
        return STATE_ERROR;
    }
    
    int pin_value = hal_gpio_get_inval(CONFIG_HALL_SENSOR_PIN);
    
    // 根据霍尔传感器逻辑判断状态
    // 假设: 传感器为常开型，有磁场时输出低电平(0)表示帽子扣上
    if (pin_value == 0) {
        return CAP_ON;  // 磁场检测到，帽子扣上
    } else if (pin_value == 1) {
        return CAP_OFF; // 无磁场，帽子打开
    } else {
        osal_printk("错误: 读取GPIO引脚值失败\n");
        return STATE_ERROR;
    }
}

/**
 * 带消抖的状态检测
 */
static CapState get_debounced_state(void) {
    static CapState previous_state = STATE_ERROR;
    static uint32_t same_state_count = 0;
    
    CapState current_state = read_raw_sensor_state();
    
    if (current_state == STATE_ERROR) {
        return STATE_ERROR;
    }
    
    // 首次读取或状态变化
    if (previous_state == STATE_ERROR) {
        previous_state = current_state;
        same_state_count = 1;
        return current_state;
    }
    
    // 状态相同，增加计数器
    if (current_state == previous_state) {
        same_state_count++;
    } else {
        // 状态变化，重置计数器
        same_state_count = 1;
        previous_state = current_state;
    }
    
    // 达到消抖阈值才确认状态变化
    if (same_state_count >= DEBOUNCE_THRESHOLD) {
        return current_state;
    }
    
    // 状态不稳定，返回上一次的稳定状态
    return last_stable_state;
}

/**
 * 获取帽子状态（主接口，带消抖处理）
 */
CapState get_cap_state(void) {
    CapState current_state = get_debounced_state();
    
    if (current_state == STATE_ERROR) {
        osal_printk("错误: 无法读取传感器状态\n");
        return STATE_ERROR;
    }
    
    // 更新稳定状态
    if (current_state != last_stable_state) {
        last_stable_state = current_state;
        debounce_counter = 0;
        
        // 状态变化时打印信息
        if (current_state == CAP_ON) {
            osal_printk("[状态确认] 帽子已扣上 ✓\n");
        } else {
            osal_printk("[状态确认] 帽子已打开 ✗\n");
        }
    }
    
    return current_state;
}

/**
 * 检查帽子是否刚被扣上（边缘检测）
 */
bool is_cap_just_put_on(void) {
    static CapState prev_state = STATE_ERROR;
    CapState current_state = get_cap_state();
    
    if (prev_state == STATE_ERROR) {
        prev_state = current_state;
        return false;
    }
    
    bool triggered = (prev_state == CAP_OFF && current_state == CAP_ON);
    prev_state = current_state;
    
    if (triggered) {
        osal_printk("[边缘检测] 检测到帽子刚被扣上！\n");
    }
    
    return triggered;
}

/**
 * 检查帽子是否刚被取下（边缘检测）
 */
bool is_cap_just_taken_off(void) {
    static CapState prev_state = STATE_ERROR;
    CapState current_state = get_cap_state();
    
    if (prev_state == STATE_ERROR) {
        prev_state = current_state;
        return false;
    }
    
    bool triggered = (prev_state == CAP_ON && current_state == CAP_OFF);
    prev_state = current_state;
    
    if (triggered) {
        osal_printk("[边缘检测] 检测到帽子刚被取下！\n");
    }
    
    return triggered;
}

/**
 * 等待帽子状态变化（阻塞式）
 */
CapState wait_for_cap_state_change(uint32_t timeout_ms) {
    CapState initial_state = get_cap_state();
    uint32_t start_time = osKernelGetSysTimerCount();
    
    if (initial_state == STATE_ERROR) {
        osal_printk("错误: 初始状态读取失败\n");
        return STATE_ERROR;
    }
    
    osal_printk("等待状态变化，当前状态: %s, 超时: %dms\n", 
                initial_state == CAP_ON ? "帽子扣上" : "帽子打开", 
                timeout_ms);
    
    while (1) {
        CapState current_state = get_cap_state();
        
        if (current_state == STATE_ERROR) {
            osal_printk("错误: 读取传感器状态失败\n");
            return STATE_ERROR;
        }
        
        if (current_state != initial_state) {
            osal_printk("检测到状态变化: %s -> %s\n",
                        initial_state == CAP_ON ? "帽子扣上" : "帽子打开",
                        current_state == CAP_ON ? "帽子扣上" : "帽子打开");
            return current_state;
        }
        
        // 检查超时
        if (osKernelGetSysTimerCount() - start_time > timeout_ms) {
            osal_printk("等待状态变化超时\n");
            return STATE_ERROR;
        }
        
        osal_msleep(CHECK_INTERVAL_MS);
    }
}

/**
 * 传感器诊断信息
 */
void hall_sensor_diagnostic(void) {
    if (!sensor_initialized) {
        osal_printk("诊断: 传感器未初始化\n");
        return;
    }
    
    int pin_value = hal_gpio_get_inval(CONFIG_HALL_SENSOR_PIN);
    CapState current_state = get_cap_state();
    CapState raw_state = read_raw_sensor_state();
    
    osal_printk("=== 霍尔传感器诊断信息 ===\n");
    osal_printk("GPIO引脚: %d\n", CONFIG_HALL_SENSOR_PIN);
    osal_printk("引脚电平: %d\n", pin_value);
    osal_printk("原始状态: %s\n", 
               raw_state == CAP_ON ? "帽子扣上" : 
               raw_state == CAP_OFF ? "帽子打开" : "错误");
    osal_printk("稳定状态: %s\n", 
               current_state == CAP_ON ? "帽子扣上" : 
               current_state == CAP_OFF ? "帽子打开" : "错误");
    osal_printk("消抖计数: %lu\n", (unsigned long)debounce_counter);
    osal_printk("初始化状态: %s\n", sensor_initialized ? "已初始化" : "未初始化");
    osal_printk("==========================\n");
}