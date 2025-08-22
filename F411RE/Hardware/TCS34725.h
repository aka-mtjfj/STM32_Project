#include "stm32f4xx.h"
#ifndef __TCS34725_H
#define __TCS34725_H
void TCS34725_Init(void);
uint8_t TCS34725_GetID(void);
uint8_t TCS34725_ReadReg(uint8_t Addre);
void TCS34725_WriteReg(uint8_t Addre,uint8_t Byte);
uint8_t TCS34725_Get_RGBData(uint16_t *ClearData,uint16_t *RedData,uint16_t*GreenData,uint16_t*BlueData);
#endif
