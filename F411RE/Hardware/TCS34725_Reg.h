#include "stm32f4xx.h"
#ifndef __TCS34725_Reg_H
#define __TCS34725_Reg_H
#define I2C_READ 0x01								//读写位，读为1，写为0
#define TCS34725_ENABLE_PON  0x01  //写入值，PON使能，开启电源，使能振荡器振动
#define TCS34725_ENABLE_AEN  0x02  //RGBC使能，激活ADC，写入1激活，写入0禁用。
#define TCS34725__STATUS_AVALID  0x01 //判断时与操作，该位为1表示完成一次有效采样
#define TCS34725_CMD_MASK    0x80 //写入寄存器地址时最高位必须为1

#define I2C_TCS34725_ADDR    0x29  //从机地址

#define TCS34725_ENABLE_ADDR 0x00  //使能寄存器
#define TCS34725_ID_ADDR     0x12  //从机ID寄存器地址
#define TCS34725_STATUS_ADDR 0x13  //状态寄存器
#define TCS34725_CDATAL_ADDR  0x14  //清除寄存器低8位
#define TCS34725_CDATAH_ADDR 0x15  //高八位
#define TCS34725_RDATAL_ADDR  0x16  //红色
#define TCS34725_RDATAH_ADDR 0x17  //
#define TCS34725_GDATAL_ADDR  0x18  //绿色
#define TCS34725_GDATAH_ADDR 0x19  //
#define TCS34725_BDATAL_ADDR  0x1A  //蓝色
#define TCS34725_BDATAH_ADDR 0x1B  //

#endif
