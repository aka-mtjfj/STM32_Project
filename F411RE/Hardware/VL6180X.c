#include "STM32F4xx.h" // Device header
#include "VL6180X_Reg.h"
#include "I2C.h"
/**
 * @file       ：10厘米内测距感应。
 * @brief        ：提供测距的初始化和获取ID,获取测距距离函数。
 * @details    ：1.使用hal库提供的I2C3外设，需要提前设置外设。
								 2.初始化开启了中断，如果有需要可以配置外部中断读取数据。
								 3.从机地址为0x29，上电后可不掉电修改保存。
								 4.从机寄存器寻址使用16位
 * @date ：2025年-3月-15日

 */
 
 
 /**
  * @brief ：函数描述:读取一个字节
  * @param ：参数描述:1.寄存器地址(16位)
											2.需要传输数据的指针，对应调用hal库I2C读取一个字节
  * @return ：返回描述:状态，HAL_OK=0x00U,其余全为非零
  * @refer ：说明:使用I2C2外设
  */
HAL_StatusTypeDef VL6180X_ReadByte(uint16_t regAddress, uint8_t* data)
{
	uint16_t DevAddress=0;
	DevAddress = (VL6180X_DEFAULT_I2C_ADDR<<1)|1;
	return 	HAL_I2C_Mem_Read(&hi2c3, DevAddress, regAddress, VL6180X_REG_ADDRESS_SIZE, data,1,30);
}

/**
  * @brief ：函数描述:写入一个字节
  * @param ：参数描述:1.寄存器地址(16位)
											2.形参，需要写入的字节
  * @return ：返回描述:状态，HAL_OK=0x00U,其余全为非零
  * @refer ：说明:使用I2C2外设
  */
HAL_StatusTypeDef VL6180X_WriteByte(uint16_t regAddress, uint8_t data)
{
	uint16_t DevAddress=0;
	DevAddress = (VL6180X_DEFAULT_I2C_ADDR<<1)&0xFE;//使最后一位为0
	return HAL_I2C_Mem_Write(&hi2c3, DevAddress, regAddress, VL6180X_REG_ADDRESS_SIZE, &data,1,30);
}

/**
  * @brief ：函数描述:VL6180X初始化
  * @param ：参数描述:无
  * @return ：返回描述:无
  * @refer ：说明:测距和感应光强初始化，开启中断
  */
void VL6180X_Init(void)
{
	uint8_t data=0;
	VL6180X_ReadByte(0x016,&data);//上电复位成功为1
	if(data ==1)
	{
		VL6180X_WriteByte(0x0207, 0x01); 
		VL6180X_WriteByte(0x0208, 0x01);
		VL6180X_WriteByte(0x0096, 0x00); 
		VL6180X_WriteByte(0x0097, 0xfd); 
		VL6180X_WriteByte(0x00e3, 0x01); 
		VL6180X_WriteByte(0x00e4, 0x03);
		VL6180X_WriteByte(0x00e5, 0x02); 
		VL6180X_WriteByte(0x00e6, 0x01); 
		VL6180X_WriteByte(0x00e7, 0x03); 
		VL6180X_WriteByte(0x00f5, 0x02); 
		VL6180X_WriteByte(0x00d9, 0x05); 
		VL6180X_WriteByte(0x00db, 0xce); 
		VL6180X_WriteByte(0x00dc, 0x03); 
		VL6180X_WriteByte(0x00dd, 0xf8); 
		VL6180X_WriteByte(0x009f, 0x00); 
		VL6180X_WriteByte(0x00a3, 0x3c); 
		VL6180X_WriteByte(0x00b7, 0x00); 
		VL6180X_WriteByte(0x00bb, 0x3c); 
		VL6180X_WriteByte(0x00b2, 0x09);
		VL6180X_WriteByte(0x00ca, 0x09); 
		VL6180X_WriteByte(0x0198, 0x01);
		VL6180X_WriteByte(0x01b0, 0x17); 
		VL6180X_WriteByte(0x01ad, 0x00);
		VL6180X_WriteByte(0x00ff, 0x05);
		VL6180X_WriteByte(0x0100, 0x05);
		VL6180X_WriteByte(0x0199, 0x05);
		VL6180X_WriteByte(0x01a6, 0x1b);
		VL6180X_WriteByte(0x01ac, 0x3e); 
		VL6180X_WriteByte(0x01a7, 0x1f);
		VL6180X_WriteByte(0x0030, 0x00);
		
		VL6180X_WriteByte(0x0011, 0x10); // 在测量完成时启用对新样品的轮询准备就绪
		VL6180X_WriteByte(0x010a, 0x30); // 设置平均采样周期
		VL6180X_WriteByte(0x003f, 0x46); // 设置明暗增益。不应更改暗增益。
		VL6180X_WriteByte(0x0031, 0xFF); // 设置多少次测量后进行自准 
		VL6180X_WriteByte(0x0040, 0x63); // 将ALS集成时间设置为100ms
		VL6180X_WriteByte(0x002e, 0x01); // 对测距传感器进行单次温度校准
		VL6180X_WriteByte(0x001b, 0x09); // 将默认测距间隔设置为 100ms
		VL6180X_WriteByte(0x003e, 0x31); // 将默认 ALS 测量周期设置为 500ms
		VL6180X_WriteByte(0x0014, 0x24); // 在新的 Sample Ready 阈值事件上配置中断
		
		VL6180X_WriteByte(0x0016, 0x00);
}
}

/**
  * @brief ：函数描述:获取ID
  * @param ：参数描述:无
* @return ：返回描述:返回ID，应为0xB4，读取错误时返回0
  * @refer ：说明:无
  */
uint8_t VL6180X_GetID(void)
{
	uint8_t ID;
	if(VL6180X_ReadByte(VL6180X_REG_ID,&ID))//返回HAL_OK=0x00U,其余全为非零
		ID =0;//get fail, return 0
	return ID;		
}

/**
  * @brief ：函数描述:获取测距距离
  * @param ：参数描述:无
  * @return ：返回描述:返回测到的距离
* @refer ：说明:单位为mm，单次测量
  */
uint8_t VL6180X_GetRange(void)
{
	uint8_t status,range;
	VL6180X_ReadByte( VL6180X_REG_RESULT_RANGE_STATUS, &status);//获取状态
	
	while(!(status&0x01))//状态寄存器最低位为1时准备好（非忙）
	{
		VL6180X_ReadByte( VL6180X_REG_RESULT_RANGE_STATUS, &status);
		HAL_Delay(10);
	}
	
	range=1;
	VL6180X_WriteByte(VL6180X_REG_SYSRANGE_START,range);//开始单次测量
	//读取中断标志位，为1表示已完成测量可以读取
	VL6180X_ReadByte( VL6180X_REG_RESULT_INTERRUPT_STATUS, &status);
	while(!(status&0x04))//4是距离测量完成标志(第三位置一)
	{
		VL6180X_ReadByte( VL6180X_REG_RESULT_INTERRUPT_STATUS, &status);
		HAL_Delay(100);
	}
	VL6180X_ReadByte(VL6180X_REG_RESULT_RANGE_VAL,&range);//读取距离
	//需要手动清除标志位，三种中断标志位全部清除
	VL6180X_WriteByte(VL6180X_REG_SYSTERM_INTERRUPT_CLEAR,0x07);
	return range;	
}
