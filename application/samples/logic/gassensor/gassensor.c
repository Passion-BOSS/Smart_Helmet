#include "gassensor.h"

//存储读取的数据
gas_data_t gas_data={0};

// 数据解析状态机状态
typedef enum {
    STATE_WAIT_FOR_HEADER,
    STATE_RECEIVING_DATA
} parse_state_t;

static parse_state_t
 s_parse_state = STATE_WAIT_FOR_HEADER;
static uint8_t s_rx_frame_buffer[9]; // 用于存放一帧数据
static uint8_t s_rx_frame_index = 0;

// 校验和验证函数
uint8_t Checksum_Verify(uint8_t* data, uint8_t length)
{
    uint8_t sum = 0;
    for (int i = 0; i < (length - 1); i++) {
        sum += data[i];
    }
    return (sum == data[length - 1]);
}

void GasSensor_Init(void)
{
    Serial_Init();
    // 初始化状态机
    s_parse_state = STATE_WAIT_FOR_HEADER;
    s_rx_frame_index = 0;
}

void GasSensor_ReadData(void)
{
    uint8_t byte_received;

    // 循环读取环形缓冲区中的所有可用字节
    while (Serial_ReceiveByte(&byte_received)) {
        switch (s_parse_state) {
            case STATE_WAIT_FOR_HEADER:
                // 寻找帧头 0x2C
                if (byte_received == 0x2C) {
                    s_rx_frame_buffer[0] = byte_received;
                    s_rx_frame_index = 1;
                    s_parse_state = STATE_RECEIVING_DATA;
                }
                break;

            case STATE_RECEIVING_DATA:
                // 正在接收一帧数据
                s_rx_frame_buffer[s_rx_frame_index] = byte_received;
                s_rx_frame_index++;

                // 如果收满了9个字节
                if (s_rx_frame_index >= 9) {
                    // 进行校验
                    if (Checksum_Verify(s_rx_frame_buffer, 9)) {
                        // 校验成功，解析数据
                        gas_data.tvoc_ppb = (s_rx_frame_buffer[2] << 8) | s_rx_frame_buffer[3];
                        gas_data.formaldehyde_ppb = (s_rx_frame_buffer[4] << 8) | s_rx_frame_buffer[5];
                        gas_data.eco2_ppm = (s_rx_frame_buffer[6] << 8) | s_rx_frame_buffer[7];
                        gas_data.data_valid = 1;
                    } else {
                        gas_data.data_valid = 0;
                        // 校验失败，可以打印错误日志
                    }
                    // 无论本帧成功与否，都重置状态机，准备接收下一帧
                    s_parse_state = STATE_WAIT_FOR_HEADER;
                    s_rx_frame_index = 0;
                }
                break;

            default:
                s_parse_state = STATE_WAIT_FOR_HEADER;
                s_rx_frame_index = 0;
                break;
        }
    }
    // 如果状态是 STATE_RECEIVING_DATA 但 s_rx_frame_index < 9，说明一帧数据还没收完，下次进入函数继续收
}

// 获取解析好的数据
gas_data_t* GasSensor_GetData(void)
{
    return &gas_data;
}
