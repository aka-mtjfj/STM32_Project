#include "TCS34725_Reg.h"
#include "stm32f4xx.h"
#include "My_I2C.h"
#include "Delay.h"
/**
 * @file       ：TCS34725颜色传感器
 * @brief        ：包含了初始化，读取ID,读取四个颜色参数的函数
 * @details    ：1.依赖于My_I2c,Delay文件，如果有其他文件调用I2C，可以使用外设或重
									新复制一个MyI2C_2文件,更改引脚使用。
									2.使用引脚：PB6-SCL,PB7-SDA	。
									3.需要包含TCS34725_Reg头文件来引用宏定义。
 * @date ：2025年-3月-13日
 */
 
 
 
/**
  * @brief ：函数描述:向颜色传感器里写入一个字节
  * @param ：参数描述:
											Addre：写入从机寄存器的地址，只需要输入源地址，无需移位置位操作
											Byte：要写入的字节
  * @return ：返回描述:无
  * @refer ：说明:从机要求写入从机寄存器地址时要求最高位为1
  */
void TCS34725_WriteReg(uint8_t Addre,uint8_t Byte)//从机地址，内部地址，内容
{
	MyI2C_Start();
	MyI2C_SendByte(I2C_TCS34725_ADDR<<1);//协议要先传从机地址,写数据
	
	MyI2C_ReadAck();//每一个字节都要应答一次
	MyI2C_SendByte(Addre|TCS34725_CMD_MASK);//设置写入从机寄存器地址，最高位1表示地址
	MyI2C_ReadAck();
	MyI2C_SendByte(Byte);//写入的字节
	MyI2C_ReadAck();
	MyI2C_Stop();
	
}

/**
  * @brief ：函数描述:读出颜色传感器一个字节
  * @param ：参数描述:
											Addre：从机寄存器地址，无需变化操作
  * @return ：返回描述:读取到的该地址下的八位无符号数据
  * @refer ：说明:从机要求写入从机寄存器地址时要求最高位为1
  */
uint8_t TCS34725_ReadReg(uint8_t Addre)
{
	uint8_t tmp;
	MyI2C_Start();
	MyI2C_SendByte(I2C_TCS34725_ADDR<<1);//传入从机地址写操作
	MyI2C_ReadAck();
	MyI2C_SendByte(Addre|TCS34725_CMD_MASK);//内部某一地址
	MyI2C_ReadAck();
	
	MyI2C_Start();//重新开始
	MyI2C_SendByte((I2C_TCS34725_ADDR<<1)|0x1);//从机地址读
	MyI2C_ReadAck();
	tmp=MyI2C_ReadByte();//读出数据
	MyI2C_SendAck(1);
	MyI2C_Stop();
	return tmp;

}

/**
  * @brief ：函数描述:对颜色传感器的初始化
  * @param ：参数描述:无
  * @return ：返回描述:无
  * @refer ：说明:需要先向使能寄存器里配置启动
  */
void TCS34725_Init()
{
	uint8_t tmp=0;
	MyI2C_Init();
	TCS34725_WriteReg(TCS34725_ENABLE_ADDR|TCS34725_CMD_MASK,TCS34725_ENABLE_PON);//使能寄存器写入开启电源和震荡源
	Delay_ms(5);//延时五毫秒（启用PON的2.4ms预热延迟）
	tmp=TCS34725_ReadReg(TCS34725_ENABLE_ADDR|TCS34725_CMD_MASK);//得到使能寄存器的值
	tmp|=TCS34725_ENABLE_AEN;//或操作使能AEN，激活ADC
	TCS34725_WriteReg(TCS34725_ENABLE_ADDR|TCS34725_CMD_MASK,tmp);//写入
	TCS34725_WriteReg(0x01,0xD6);//更改积分时间
}

/**
  * @brief ：函数描述:获取设备ID
  * @param ：参数描述:无
  * @return ：返回描述:设备ID,八位无符号整型
  * @refer ：说明:无
  */
uint8_t TCS34725_GetID()
{
	return TCS34725_ReadReg(TCS34725_ID_ADDR|TCS34725_CMD_MASK);
}

/**
  * @brief ：函数描述:读取颜色传感器四个颜色参数
  * @param ：参数描述:传入四个参数指针，分别对应clear,red,green,blue，传入参数
							须为16位无符号整型。
  * @return ：返回描述:
											 1.将读到的数据分别写入传进来的四个参数中。
											 2.返回读取状态，成功读取返回1，读取出错返回0。
  * @refer ：说明:调用时需要先对颜色传感器进行初始化。
  */
uint8_t TCS34725_Get_RGBData(uint16_t *ClearData,uint16_t *RedData,uint16_t*GreenData,uint16_t*BlueData)
{
	
uint16_t DataH, DataL;								//定义数据高8位和低8位的变量
	uint8_t Status;
	Status=TCS34725_ReadReg(TCS34725_STATUS_ADDR);
	if(Status & TCS34725__STATUS_AVALID)
	{
	DataH = TCS34725_ReadReg(TCS34725_CDATAH_ADDR);		//读取加速度计X轴的高8位数据
	DataL = TCS34725_ReadReg(TCS34725_CDATAL_ADDR);		//读取加速度计X轴的低8位数据
	*ClearData=(DataH << 8)|DataL;						//数据拼接，通过输出参数返回
	
	DataH = TCS34725_ReadReg(TCS34725_RDATAH_ADDR);	
	DataL = TCS34725_ReadReg(TCS34725_RDATAL_ADDR);	
	*RedData=(DataH << 8)|DataL;						

	DataH = TCS34725_ReadReg(TCS34725_GDATAH_ADDR);	
	DataL = TCS34725_ReadReg(TCS34725_GDATAL_ADDR);	
	*GreenData=(DataH << 8)|DataL;								
	
	DataH = TCS34725_ReadReg(TCS34725_BDATAH_ADDR);	
	DataL = TCS34725_ReadReg(TCS34725_BDATAL_ADDR);	
	*BlueData=(DataH << 8)|DataL;		
		return 1;
	}
	else
		return 0;
}
