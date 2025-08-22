#include "STM32F4xx.h" // Device header
#include "Delay.h"
/**
 * @file       ：温湿度计传感器
 * @brief        ：用于测试环境温度和湿度，温度范围在0到50摄氏度内使用，信号引脚
 //是PA15,不同设备使用时注意时钟，hal_delay不适用。
 * @details    ：读取数据最好有间隔
 * @date ：2025年-3月-12日
 */
 /**
  * @brief ：函数描述:将信号线写入和读出，如有需要修改只需更改此处和初始化代码
  * @param ：参数描述:传入需要设置的电平，0或1
  * @return ：返回描述：返回线上电平
  * @refer ：说明:使用开漏输出
  */
void DHT11_W_SDA(uint8_t Pinstate)
{
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,(GPIO_PinState)Pinstate);
}

uint8_t DHT11_R_SDA()
{
	return HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_15);
}

void DHT11_Rst(void);
uint8_t DHT11_Ack(void);

/**
* @brief ：函数描述:DHT11的初始化，包括引脚初始化，复位，返回应答
  * @param ：参数描述:无
* @return ：返回描述:返回模块应答值
* @refer ：说明:对于应答值，0为有应答，1为无应答
  */
uint8_t DHT11_Init()
{
	  __HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.Mode=GPIO_MODE_OUTPUT_OD;//开漏输出
	GPIO_InitStructure.Speed=GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Pin=GPIO_PIN_15;
 	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	DHT11_Rst();
	return DHT11_Ack();

}
/**
  * @brief ：函数描述:模块复位，在初始化和读取数据前都需要先复位告知从机
  * @param ：参数描述:无
  * @return ：返回描述:无
  * @refer ：说明:此处第一个延时至少大于18ms,第二个在20-40间
  */
void DHT11_Rst()
{
		DHT11_W_SDA(0);
	Delay_ms(20);
	DHT11_W_SDA(1);
	Delay_us(30);
}
/**
  * @brief ：函数描述:接收模块应答值
  * @param ：参数描述:无
* @return ：返回描述:应答值，0为有应答，1为无应答
* @refer ：说明:应答信号为80us低电平,80us高电平，超时返回1
  */
uint8_t DHT11_Ack()
{
	uint8_t retry=0;
	while(DHT11_R_SDA()==1&&retry<100)//等待检测低电平，因为主机已经释放信号线
	{
		retry++;
		Delay_us(1);
	}
	if(retry>=100)
		return 1;
	else
		retry=0;
	while(DHT11_R_SDA()==0&&retry<100)//等待从机释放信号线
	{
		retry++;
		Delay_us(1);
	}
	if(retry>=100)
		return 1;
	else
		return 0;
	
}
/**
  * @brief ：函数描述:读取一个比特
  * @param ：参数描述:无
  * @return ：返回描述:比特
* @refer ：说明:0或1，从机释放20-30us为0，80us为1
  */
uint8_t DHT11_ReadBit()
{
	uint8_t retry=0;
	while(DHT11_R_SDA()==1&&retry<100)//等待从机拉低
	{
		retry++;
		Delay_us(1);
	}
	retry=0;
		while(DHT11_R_SDA()==0&&retry<100)//等待从机拉高
	{
		retry++;
		Delay_us(1);
	}
	Delay_us(40);//相隔40us后查看电平，如果为比特0，则高电平已结束，比特1反之
	if(DHT11_R_SDA()==1)
		return 1;
	else
		return 0;
}
/**
  * @brief ：函数描述:读取一个字节的数据
  * @param ：参数描述:无
  * @return ：返回描述:一个字节
  * @refer ：说明:高位先行
  */
uint8_t DHT11_ReadByte()
{
	uint8_t i=0,Data=0;
	for(i=0;i<8;i++)
	{
		Data<<=1;
		Data|=DHT11_ReadBit();
	}
	return Data;
}
/**
  * @brief ：函数描述:读取温度和湿度，返回整数，小数部分读取buf[1],buf[3]即可
  * @param ：参数描述:温度
  * @return ：返回描述:是否读取成功
  * @refer ：说明:buf前两个分别是温度整数小数部分，后面两个分别是湿度
  */
uint8_t DHT11_ReadData(uint8_t* temp,uint8_t* humi)
{
	uint8_t buf[5];
	uint8_t i=0;
	DHT11_Rst();
	if(DHT11_Ack()==0)
	{
		for(i=0;i<5;i++)
		{
			buf[i]=DHT11_ReadByte();
		}
		if((buf[0]+buf[2]+buf[1]+buf[3])==buf[4])
		{
			*humi=buf[0];
			*temp=buf[2];
		}
		
	}
	else return 1;
	return 0;
	
}

