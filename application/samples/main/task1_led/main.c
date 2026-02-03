
#include "../../logic/flashlight/flashlight.h"

#define FLASHLIGHT_TASK_STACK_SIZE     0x1000
#define FLASHLIGHT_TASK_PRIORITY       24
#define FLASHLIGHT_TASK_DELAY_MS       20

/**
 * @brief 手电筒主任务函数
 * @param arg 任务参数（未使用）
 */
static void *flashlight_main_task(const char *arg)
{
    unused(arg);
    
    osal_printk("Flashlight task started\r\n");
    
    // 初始化硬件模块
    led_key_init();          // 初始化按键
    led_pwm_init();          // 初始化PWM
      void *dummy_param = NULL;
    flashlight_task(dummy_param);
    return NULL;}
   
/**
 * @brief 应用入口函数 - 创建手电筒任务
 */
static void flashlight_app_entry(void)
{
    osal_task *task_handle = NULL;
    
    osal_printk("Initializing flashlight application...\r\n");
    
    // 创建手电筒任务
    osal_kthread_lock();
    task_handle = osal_kthread_create(
        (osal_kthread_handler)flashlight_main_task, 
        0, 
        "Flashlight_Task", 
        FLASHLIGHT_TASK_STACK_SIZE
    );
    
    if (task_handle != NULL) {
        // 设置任务优先级
        osal_kthread_set_priority(task_handle, FLASHLIGHT_TASK_PRIORITY);
        osal_printk("Flashlight task created successfully, priority: %d\r\n", FLASHLIGHT_TASK_PRIORITY);
    } else {
        osal_printk("Failed to create flashlight task!\r\n");
    }
    osal_kthread_unlock();
}

/* 运行手电筒应用 */
app_run(flashlight_app_entry);