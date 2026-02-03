#include"../../driver/serial1/serial1.h"
#include "../../logic/gassensor/gassensor.h"
#define GASSENSOR_TASK_STACK_SIZE     0x1000
#define GASSENSOR_TASK_PRIORITY       17
#define GASSENSOR_TASK_DELAY_MS       20
#include "app_init.h"
/**
 * @brief 气体传感器主任务函数
 * @param arg 任务参数（未使用）
 */
static void *gassensor_task(void *arg)
{
    (void)arg;
osal_printk("gassensor task started\r\n");
gas_data_t * p_gas_data={0};
Serial_Init();                    // 初始化串口，波特率设为9600
while(1){
    GasSensor_ReadData();
    p_gas_data=GasSensor_GetData();
    if(p_gas_data->data_valid){
        printf("TVOC: %d ug/m3, Formaldehyde: %d ug/m3, eCO2: %d ppm\n",
                   p_gas_data->tvoc_ppb,
                   p_gas_data->formaldehyde_ppb,
                   p_gas_data->eco2_ppm);
        p_gas_data->data_valid=0;//清除标志位
    }
    osal_msleep(10);
}
return NULL;
}
/**
 * @brief 应用入口函数 - 创建手电筒任务
 */
static void gassensor_app_entry(void)
{
    osal_task *task_handle = NULL;
    
    osal_printk("Initializing gassensor application...\r\n");
    
    // 创建气体传感器任务
    osal_kthread_lock();
    task_handle = osal_kthread_create(
        (osal_kthread_handler)gassensor_task, 
        0, 
        "Gassensor_Task", 
        GASSENSOR_TASK_STACK_SIZE
    );
    
    if (task_handle != NULL) {
        // 设置任务优先级
        osal_kthread_set_priority(task_handle, GASSENSOR_TASK_PRIORITY);
        osal_printk("Gassensor task created successfully, priority: %d\r\n", GASSENSOR_TASK_PRIORITY);
    } else {
        osal_printk("Failed to create gassensor task!\r\n");
    }
    osal_kthread_unlock();
}

/* 运行气体传感器应用 */
app_run(gassensor_app_entry);