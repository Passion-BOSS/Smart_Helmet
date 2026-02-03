#include "serial1.h"
// serial1.c 文件开头添加配置
#ifndef CONFIG_UART1_BUS_ID
#define CONFIG_UART1_BUS_ID UART_BUS_1  // 默认使用UART1
#endif

#ifndef CONFIG_UART1_TXD_PIN
#define CONFIG_UART1_TXD_PIN 17  // 根据硬件设置
#endif

#ifndef CONFIG_UART1_RXD_PIN  
#define CONFIG_UART1_RXD_PIN 18 // 根据硬件设置
#endif

#ifndef CONFIG_UART1_TXD_PIN_MODE
#define CONFIG_UART1_TXD_PIN_MODE HAL_PIO_FUNC_GPIO // 复用模式
#endif

#ifndef CONFIG_UART1_RXD_PIN_MODE
#define CONFIG_UART1_RXD_PIN_MODE HAL_PIO_FUNC_GPIO // 复用模式
#endif
#define UART_GETC_DELAY_TIME 2
// 1. 定义环形缓冲区（FIFO）用于中断接收
#define SERIAL_RX_BUFFER_SIZE 256
static uint8_t s_uart_rx_fifo[SERIAL_RX_BUFFER_SIZE];
static volatile uint16_t s_uart_rx_write_pos = 0;
static volatile uint16_t s_uart_rx_read_pos = 0;
static uart_bus_t g_uart_bus = CONFIG_UART1_BUS_ID; // 根据你的实际UART总线号修改，例如 UART_BUS_1

// // 2. 环形缓冲区辅助函数
// static uint16_t Serial_GetRxFifoCount(void)
// {
//     return (s_uart_rx_write_pos >= s_uart_rx_read_pos) ?
//            (s_uart_rx_write_pos - s_uart_rx_read_pos) :
//            (SERIAL_RX_BUFFER_SIZE - s_uart_rx_read_pos + s_uart_rx_write_pos);
// }

static void Serial_RxFifoWrite(uint8_t data)
{
    s_uart_rx_fifo[s_uart_rx_write_pos] = data;
    s_uart_rx_write_pos = (s_uart_rx_write_pos + 1) % SERIAL_RX_BUFFER_SIZE;
    // 简单的溢出处理：如果缓冲区满了，丢弃最旧的数据（移动读指针）
    if (s_uart_rx_write_pos == s_uart_rx_read_pos) {
        s_uart_rx_read_pos = (s_uart_rx_read_pos + 1) % SERIAL_RX_BUFFER_SIZE;
    }
}

static uint8_t Serial_RxFifoRead(uint8_t* data)
{
    if (s_uart_rx_read_pos == s_uart_rx_write_pos) {
        return 0; // 缓冲区空
    }
    *data = s_uart_rx_fifo[s_uart_rx_read_pos];
    s_uart_rx_read_pos = (s_uart_rx_read_pos + 1) % SERIAL_RX_BUFFER_SIZE;
    return 1;
}
// 3. 海思UART中断回调函数
// 这个函数由海思HAL层在收到UART数据时自动调用
/*
硬件驱动层。负责响应最底层的硬件中断。它的任务只有一个：尽快把UART接收寄存器里的数据读出来，存到一个安全的缓冲区（如上面的环形缓冲区s_uart_rx_fifo）中，然后立刻退出中断。它不应该执行复杂的数据解析逻辑。
GasSensor_ReadData函数：
职责：应用逻辑层。它不知道数据是怎么来的（可能是中断，也可能是轮询）。它只关心从缓冲区里读取原始数据，并按照TVOC-301的协议（9字节一帧，包头0x2C，校验和）进行解析，最终得到有意义的浓度数值。
*/
static errcode_t Uart_EvtCallback(uart_bus_t bus, hal_uart_evt_id_t evt, uintptr_t param)
{
    (void)param;
    if (bus != g_uart_bus) {
        return ERRCODE_SUCC;
    }

    switch (evt) {
        case UART_EVT_RX_ISR: // 接收中断
            {
                uint8_t data;
                int32_t ret;
                // 循环读取，直到硬件FIFO为空
                do {
                    ret = hal_uart_read(bus, &data, 1);
                    if (ret > 0) {
                        // 将收到的字节写入软件环形缓冲区
                        Serial_RxFifoWrite(data);
                    }
                } while (ret > 0);
            }
            break;
        // 其他事件（如发送中断、错误中断）可以根据需要处理
        default:
            break;
    }
    return ERRCODE_SUCC;
}
void Serial_Init(void){
    #if defined(CONFIG_PINCTRL_SUPPORT_IE)
    uapi_pin_set_ie(CONFIG_UART1_RXD_PIN, PIN_IE_1);
#endif /* CONFIG_PINCTRL_SUPPORT_IE */
    uapi_pin_set_mode(CONFIG_UART1_TXD_PIN, CONFIG_UART1_TXD_PIN_MODE);
    uapi_pin_set_mode(CONFIG_UART1_RXD_PIN, CONFIG_UART1_RXD_PIN_MODE);
    uart_pin_config_t pin_config = {
        .tx_pin = CONFIG_UART1_TXD_PIN,
        .rx_pin = CONFIG_UART1_RXD_PIN,
        .cts_pin = PIN_NONE,
        .rts_pin = PIN_NONE
    };

    uart_attr_t attr = {
        .baud_rate = 9600, // 传感器文档要求的波特率
        .data_bits = UART_DATA_BIT_8,
        .stop_bits = UART_STOP_BIT_1,
        .parity = UART_PARITY_NONE
    };
    hal_uart_init(g_uart_bus, Uart_EvtCallback, &pin_config, &attr, UART_FLOW_CTRL_NONE, NULL);
    }

uint8_t Serial_ReceiveByte(uint8_t* data) // 尝试接收一个字节，成功返回1，失败返回0
{
    if(data==NULL){
        return 0;
    }
    return Serial_RxFifoRead(data);
}
void Serial_SendByte(uint8_t data)       // 发送一个字
{
    uapi_uart_write(g_uart_bus, &data, 1,UART_GETC_DELAY_TIME);
}
void Serial_SendArray(uint8_t* array, uint16_t len) // 发送一个数组
{
    if(array==NULL||len==0){
        return;
    }
    uapi_uart_write(g_uart_bus, array, len,UART_GETC_DELAY_TIME);
}




