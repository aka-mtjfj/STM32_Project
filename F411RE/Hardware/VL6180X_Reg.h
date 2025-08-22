#include "STM32F4xx.h" // Device header
#ifndef __VL6180X_H
#define __VL6180X_H
#define VL6180X_DEFAULT_ID         0xB4   //默认ID为0xB4
#define VL6180X_DEFAULT_I2C_ADDR   0x29   //I2C协议默认地址为0x29,可更改，但掉电丢失
#define VL6180X_REG_ID             0x000  //存储设备ID的寄存器地址(0xB4)

#define VL6180X_REG_SYSTERM_INTERRUPT_CLEAR 0x15  //清除中断寄存器
#define VL6180X_REG_SYSRANGE_START 0x018          //测量模式和起止寄存器
#define VL6180X_REG_RESULT_RANGE_STATUS   0x04D		//结果错误码和忙位寄存器
#define VL6180X_REG_RESULT_INTERRUPT_STATUS 0x04F	//中断状态寄存器，数据完成被置位
#define VL6180X_REG_RESULT_RANGE_VAL 0x062				//测距结果数据寄存器

#define VL6180X_REG_ADDRESS_SIZE   2							//内部地址为16位，寄存器地址字节长度

#define VL6180X_ERROR_NONE         0  //无错误
#define VL6180X_ERROR_SYSERR_1     1  //system error 1
#define VL6180X_ERROR_SYSERR_5     5  //system error 5
#define VL6180X_ERROR_ECEFAIL      6  //早期收敛估计失败
#define VL6180X_ERROR_NOCONVERGE   7  //未检测到目标或未收敛
#define VL6180X_ERROR_RANGEIGNORE  8  //忽略阈值检查失败
#define VL6180X_ERROR_SNR          11 //信噪比不足
#define VL6180X_ERROR_RAWFLOW      12 //原始数据过大过小
#define VL6180X_ERROR_RAEUFLOW     13 //
#define VL6180X_ERROR_RANGEFLOW    14 //测距值过小
#define VL6180X_ERROR_RANGEUFLOW   15 //测距值过大


#endif
