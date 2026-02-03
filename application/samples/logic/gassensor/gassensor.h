#ifndef CO2SENSOR_H_
#define CO2SENSOR_H_
#include "../../driver/serial1/serial1.h"
#include <stdint.h>

typedef struct{
    uint16_t tvoc_ppb;//TVOC浓度 （ug/m3)
    uint16_t eco2_ppm;//eCO2浓度 (ppm)
    uint16_t formaldehyde_ppb;//甲醛浓度(ug/m3)
    uint16_t data_valid; //数据有效标志位
}gas_data_t;
// 初始化气体传感器
void GasSensor_Init(void);

// 读取并处理气体传感器数据
uint8_t Checksum_Verify(uint8_t* data, uint8_t length);
void GasSensor_ReadData(void);
gas_data_t* GasSensor_GetData(void);

//打包发送...
void GasSensor_SendData(void) ;

#endif