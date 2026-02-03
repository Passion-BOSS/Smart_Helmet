#ifndef SERIAL1_H
#define SERIAL1_H
#include "pinctrl.h"
#include "uart.h"
#include "watchdog.h"
#include "soc_osal.h"
#include "app_init.h"
#include <stdint.h> // 用于使用 uint8_t 等类型
#include "common_def.h"

#define UART_BAUDRATE                      115200
#define CONFIG_UART_INT_WAIT_MS            5

// // 函数声明
//  uint16_t Serial_GetRxFifoCount(void);
//  void Serial_RxFifoWrite(uint8_t data);
//  uint8_t Serial_RxFifoRead(uint8_t* data);

void Serial_Init(void);
uint8_t Serial_ReceiveByte(uint8_t* data); // 尝试接收一个字节，成功返回1，失败返回0
void Serial_SendByte(uint8_t data);        // 发送一个字节
void Serial_SendArray(uint8_t* array, uint16_t len); // 发送一个数组

#endif
