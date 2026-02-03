#include"../../driver/serial1/serial1.h"
#include"../../logic/hallsensor/hallsensor.h"
#define HALLSENSOR_TASK_STACK_SIZE     0x1000
#define HALLSENSOR_TASK_PRIORITY       17
#define HALLSENSOR_TASK_DELAY_MS       20
#include "app_init.h"
/**
 * @brief 气体传感器主任务函数
 * @param arg 任务参数（未使用）
 */
static void *hallsensor_task(void *arg)
{
    (void)arg;
osal_printk("hallsensor task started\r\n");
hall_sensor_init();
hall_sensor_diagnostic();
while(1){
    CapState state = get_cap_state();

    if(is_cap_just_put_on()){
        osal_printk(" 检测到帽子被扣上！执行相关操作...\n");
        // 这里可以添加帽子扣上时的处理逻辑
            // 例如：点亮LED、播放声音、发送通知等
    }
    // 检测帽子刚被取下
        if (is_cap_just_taken_off()) {
            osal_printk(" 检测到帽子被取下！执行相关操作...\n");
            // 这里可以添加帽子取下时的处理逻辑
        }
        
        // 根据当前状态执行不同操作
        switch (state) {
            case CAP_ON:
                // 帽子扣上状态下的持续操作
                break;
            case CAP_OFF:
                // 帽子打开状态下的持续操作
                break;
            case STATE_ERROR:
                osal_printk("传感器错误，请检查连接\n");
                break;
        }
    osal_msleep(10);
}
return NULL;
}
/**
 * @brief 应用入口函数 - 创建手电筒任务
 */
static void hallsensor_app_entry(void)
{
    osal_task *task_handle = NULL;
    
    osal_printk("Initializing hallsensor application...\r\n");
    
    // 创建气体传感器任务
    osal_kthread_lock();
    task_handle = osal_kthread_create(
        (osal_kthread_handler)hallsensor_task, 
        0, 
        "hallsensor_Task", 
        HALLSENSOR_TASK_STACK_SIZE
    );
    
    if (task_handle != NULL) {
        // 设置任务优先级
        osal_kthread_set_priority(task_handle, HALLSENSOR_TASK_PRIORITY);
        osal_printk("Hallsensor task created successfully, priority: %d\r\n", HALLSENSOR_TASK_PRIORITY);
    } else {
        osal_printk("Failed to create hallsensor task!\r\n");
    }
    osal_kthread_unlock();
}

/* 运行气体传感器应用 */
app_run(hallsensor_app_entry);