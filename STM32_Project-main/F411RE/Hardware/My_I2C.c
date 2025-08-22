 #include "stm32f4xx.h"                  // Device header
#include "Delay.h"
/**
  * @brief ：协议初始化，打开时钟，初始化GPIO,设置初始电平
  * @param ：无
  * @return ：无
  * @retval ：无
  */

void MyI2C_Init()//读写两条线需要GPIO实施
{
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.Mode=GPIO_MODE_OUTPUT_OD;//主机控制的开漏输出
	GPIO_InitStructure.Speed=GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Pin=GPIO_PIN_7;//两个控制线都配置为开漏输出
 	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.Pin=GPIO_PIN_6;
 	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**
  * @brief ：写入CLK
  * @param ：要写入的电平状态
  * @return ：无
  * @retval ：无
  */

void MyI2C_W_SCL(uint8_t BitVal)//B6为SCL
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, (GPIO_PinState)BitVal);//强转后不为0就是1
	Delay_us(10);//适应从机通信速率
}

/**
  * @brief ：写入SDA
  * @param ：要写入的电平状态
  * @return ：无
  * @retval ：无
  */

void MyI2C_W_SDA(uint8_t BitVal)//B7为SDA
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, (GPIO_PinState)BitVal);
	Delay_us(10);
}

/**
* @brief ：读出SDA线上的电平
  * @param ：无
  * @return ：线上电平
  * @retval ：八位无符号数0或1
  */
uint8_t MyI2C_R_SDA()
{
	uint8_t tmp;
	tmp=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_7);
	Delay_us(10);
	return tmp;//时钟由主机控制，延时前后读取都可以，函数完后再改变SCL
}

/**
  * @brief ：开始时序函数
  * @param ：无
  * @return ：无
  * @retval ：无
  */

void MyI2C_Start()
{
	MyI2C_W_SDA(1);//考虑到再次开始的情况，在SCL低电平时主机可以改变SDA的值
	MyI2C_W_SCL(1);
	MyI2C_W_SDA(0);
	MyI2C_W_SCL(0);

}

/**
  * @brief ：结束阶段时序函数
  * @param ：无
  * @return ：无
  * @retval ：无
  */

void MyI2C_Stop()
{
	MyI2C_W_SDA(0);//停止时SDA线上电平不确定（最后一个bit），先拉低到结束条件
	MyI2C_W_SCL(1);
	MyI2C_W_SDA(1);
}

/**
  * @brief ：发送一个字节
  * @param ：要发送的八位无符号字节
  * @return ：无
  * @retval ：无
  */
void MyI2C_SendByte(uint8_t Byte )
{
	int8_t i;
	for(i=0;i<8;i++)//高位先行
	{
		MyI2C_W_SDA(Byte&(0x80>>i));//起始两者电平都为低，先写SDA，后SCL脉冲被读出，
		MyI2C_W_SCL(1);													//与操作验证每一位
		MyI2C_W_SCL(0);
		}//此时SCL为0，SDA可能为1也可能为0
	
}
/**
  * @brief ：发送应答
  * @param ：要发送的应答状态0或1，八位无符号
  * @return ：无
  * @retval ：无
  */
void MyI2C_SendAck( uint8_t Ack )//回应
{	
		MyI2C_W_SDA( Ack);
		MyI2C_W_SCL(1);
		MyI2C_W_SCL(0);
	
}

/**
  * @brief ：读取接收一个字节
  * @param ：无
  * @return ：接收到的字节
  * @retval ：八位无符号数
  */
uint8_t MyI2C_ReadByte()
{
	int8_t i;
	uint8_t Recieve=0x00;//初始化为0
	MyI2C_W_SDA(1);//释放数据线，权限交给从机，如果之前由读位，现在从机输入数据
	for(i=0;i<8;i++)
	{
		MyI2C_W_SCL(1);
		if(MyI2C_R_SDA() == 1){Recieve |= (0x80 >> i);}//只有读到1的时候才或操作，简化
		
		MyI2C_W_SCL(0);
		
	}
	return Recieve;
}

/**
  * @brief ：读取从机应答
  * @param ：无
  * @return ：从机应答值
  * @retval ：八位无符号整形
  */
uint8_t MyI2C_ReadAck()
{
	uint8_t Recieve;
	MyI2C_W_SDA(1);//SDA线的控制权交给从机
	MyI2C_W_SCL(1);
		Recieve=(uint8_t)MyI2C_R_SDA();
		MyI2C_W_SCL(0);
	return Recieve;
}



