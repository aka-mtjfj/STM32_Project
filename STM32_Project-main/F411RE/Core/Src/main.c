/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include "OLED.h"
#include "key.h"
#include "DHT11.h"
#include "Delay.h"
#include "TCS34725.h"
#include "VL6180X.h"
#include "stdio.h"
#include "L610.h"
#define STATU_OK  1
#define STATU_FAULT  0
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
int fputc(int c,FILE *stream)
{
	uint8_t ch[1]={c};
	HAL_UART_Transmit(&huart1,ch,1,0xffff);
	return c;
}
int fgetc(FILE *stream)
{
	uint8_t ch[1];
	HAL_UART_Receive(&huart1,ch,1,0xffff);
	return ch[0];
}
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
extern char RxFlag;
extern uint8_t UART6_RxFlag;
extern char Rxbuf[1024];//引用外部数组
extern uint8_t UART6_Rxbuf[512];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
typedef struct Maintenance_parameter
{
	uint8_t Maintenance_State;//每个模块的状
	uint8_t upper_computer_state;
	uint8_t FPGA_state;
	uint8_t TCS34725_state;
	uint8_t DHT11_state;
	uint8_t VL6180X_state;
}Maintenance_parameter;
/**
  * @brief ：函数描:进入调试模式
  * @param ：参数描:结构体地
  * @return ：返回描:
  * @refer ：说:测个每个模块状，存入结构体内
  */
void Enter_MaintenanceMode(Maintenance_parameter* Maintenance)
{
	/*DHT11温湿度模*/
	uint8_t temp,humi;
	DHT11_ReadData(&temp,&humi);
	if(temp<51&&humi<=95&&humi>=20)
	{
		Maintenance->DHT11_state=STATU_OK;
		Maintenance->Maintenance_State|=0x10;
	}
	else
	{
		Maintenance->DHT11_state=STATU_FAULT;
		Maintenance->Maintenance_State&=(~0x10);
	}
	/*TCS34725颜色传感器模*/
	uint16_t tmp1,tmp2,tmp3,tmp4;
	TCS34725_Get_RGBData(&tmp1,&tmp2,&tmp3,&tmp4);
	if((tmp1<10000)&&(tmp2<10000)	&&(tmp3<10000)&&(tmp4<10000))
	{
		Maintenance->TCS34725_state=STATU_OK;
		Maintenance->Maintenance_State|=0x08;
	}
	else
	{
		Maintenance->TCS34725_state=STATU_FAULT;
		Maintenance->Maintenance_State&=(~0x08);
	}
	/*VL6180X距离传感器模*/
	uint8_t ID;
	ID=VL6180X_GetID();
	if(ID==0xB4)
	{
		Maintenance->VL6180X_state=STATU_OK;
		Maintenance->Maintenance_State|=0x04;
	}
	else
	{
		Maintenance->VL6180X_state=STATU_FAULT;
		Maintenance->Maintenance_State&=(~0x04);
	}
	/*与FPGA通信模块*/
	uint8_t Rx=0x05;
	
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET);
	__HAL_TIM_SET_COUNTER(&htim2,0);
	__HAL_TIM_SET_AUTORELOAD(&htim2,30000-1);
	HAL_TIM_Base_Start(&htim2);

	if(Rx==0x05)
	{
		Maintenance->FPGA_state=STATU_OK;
		Maintenance->Maintenance_State|=0x02;
	}
	else
	{
		Maintenance->FPGA_state=STATU_FAULT;
		Maintenance->Maintenance_State&=(~0x02);
	}
	/*与上位机通信模块*/
	printf("Test Meggage\r\n");
	Delay_ms(10);
	uint16_t len=strlen(Rxbuf);
	if(len!=0&&len<1024)
	{
		Maintenance->upper_computer_state=STATU_OK;
		Maintenance->Maintenance_State|=0x01;
	}
	else
	{
		Maintenance->upper_computer_state=STATU_FAULT;
		Maintenance->Maintenance_State&=(~0x01);
	}
}
/**
  * @brief ：函数描述:显示模块
  * @param ：参数描述:结构
  * @return ：返回描述:
* @refer ：说明:在OLED屏幕上显示信息,上位机存在时显示
  */
void Show_MaintenanceInformation(Maintenance_parameter* Maintenance)
{
	OLED_Clear();
	OLED_ShowString(1,1,"Maintenance:");
	if(Maintenance->upper_computer_state==STATU_OK)
	{
		
		if(Maintenance->DHT11_state==STATU_OK)
		{	
			OLED_ShowString(2,1,"DHT:");
			OLED_ShowNum(2,5,1,1);
			}
		else
		{	
			OLED_ShowString(2,1,"DHT:");
			OLED_ShowNum(2,5,0,1);
			}
		if(Maintenance->FPGA_state==STATU_OK)
				{	
			OLED_ShowString(2,8,"FPGA:");
			OLED_ShowNum(2,13,1,1);
			}
		else
					{	
			OLED_ShowString(2,8,"FPGA:");
			OLED_ShowNum(2,13,0,1);
			}
		if(Maintenance->TCS34725_state==STATU_OK)
					{	
			OLED_ShowString(3,1,"TCS:");
			OLED_ShowNum(3,5,1,1);
			}
		else
					{	
			OLED_ShowString(3,1,"TCS:");
			OLED_ShowNum(3,5,0,1);
			}
		if(Maintenance->VL6180X_state==STATU_OK)
					{	
			OLED_ShowString(3,8,"VL618:");
			OLED_ShowNum(3,14,1,1);
			}
		else
					{	
			OLED_ShowString(3,8,"VL618:");
			OLED_ShowNum(3,14,0,1);
			}
	}
	else
	{
		OLED_Clear();
		OLED_ShowString(1,1,"Maintenance:");
		OLED_ShowString(2,1,"  Error:There is");
		OLED_ShowString(3,1,"no Host Computer!"); 
	}
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C3_Init();
  MX_TIM1_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_TIM2_Init();
  MX_SPI1_Init();
  MX_USART6_UART_Init();
  /* USER CODE BEGIN 2 */
	//对基本模块进行初始化
	OLED_Init();
	Key_Init();
	DHT11_Init();
	TCS34725_Init();
	VL6180X_Init();
 HAL_UART_Receive_DMA(&huart1,(uint8_t*)Rxbuf,1024);//启DMA接收
  HAL_UART_Receive_DMA(&huart6,(uint8_t*)UART6_Rxbuf,6);//启DMA接收
			HAL_TIM_Base_Start_IT(&htim1);
			HAL_TIM_Base_Stop(&htim2);
			__HAL_TIM_SET_COUNTER(&htim2,0);
			HAL_TIM_Base_Start_IT(&htim2);
 __HAL_UART_ENABLE_IT(&huart1,UART_IT_IDLE);//启空闲时中断
 __HAL_UART_ENABLE_IT(&huart6,UART_IT_IDLE);//启空闲时中断
	Maintenance_parameter Maintenance;
	OLED_ShowString(1,1,"Machine Init...");
/******************实际测试封装成函数无法返回，故主函数直接使用。Begin******************************/

char *strx;//接收数组测指

		
		Delay_s(1);
		printf("ATI\r\n");
		while(1)
		{
			if(RxFlag==1)
			{
				RxFlag=0;
				strx=strstr((const char*)Rxbuf,(const char*)"Fibocom");
				if(strx==NULL)
				{
					Delay_s(1);
					printf("ATI\r\n");
					Delay_s(1);
				}
				else
					break;
			}
		}
		Clear_Buffer();
		printf("version信息正确\r\n");
		OLED_Clear();
		OLED_ShowString(1,1,"test1 OK.");
		Delay_s(1);
		
		//请求IP
		printf("AT+MIPCALL?\r\n");
		Delay_s(1);
		while(1)
		{
			if(RxFlag==1)
			{
				RxFlag=0;
				strx=strstr((const char*)Rxbuf,(const char*)"+MIPCALL=1");
				if(strx==NULL)
				{
					Clear_Buffer();
					Delay_s(1);
					printf("AT+MIPCALL=1\r\n");
					Delay_s(1);
				}
				else
					break;
			}
		}
		Clear_Buffer();
		printf("获取IP成功");
		OLED_ShowString(1,1,"test2 OK.");
		Delay_s(1);
		
		//连接华为
		printf("AT+HMCON=0,60,\"314deb3694.st1.iotda-device.cn-north-4.myhuaweicloud.com\",\"1883\",\"67e7b3485367f573f77d90d1_Agriculture\",\"123123lxy\",0\r\n");
		Delay_s(2);
		while(1)
		{
			if(RxFlag==1)
			{
				RxFlag=0;
				strx=strstr((const char*)Rxbuf,(const char*)"+HMCON OK");
				if(strx==NULL)
				{
					Clear_Buffer();
					Delay_s(1);
		printf("AT+HMCON=0,60,\"314deb3694.st1.iotda-device.cn-north-4.myhuaweicloud.com\",\"1883\",\"67e7b3485367f573f77d90d1_Agriculture\",\"123123lxy\",0\r\n");
					Delay_s(1);
				}
				else
					break;
			}
		}
		Clear_Buffer();
		printf("连接成功");
		OLED_ShowString(1,1,"test3 OK.");
		Delay_s(1);
		


	
/******************实际测试封装成函数无法返回，故主函数直接使用。End******************************/

		
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	//变量的初值
	uint8_t keynum,temp,humi;
	uint16_t ClearData,RedData,GreenData,BlueData;
	uint8_t code_order=0;
	uint8_t TimeOutCnt=0;
	int password=1234;//使用者需要输入密
	int inputword=0;
		OLED_Clear();
OLED_ShowString(1,1,"Please enter the");
OLED_ShowString(2,1,"password:");
/*************************模块的使用Begin**********************************/
	while(0)
	{
		uint8_t tmp=VL6180X_GetRange();
		OLED_ShowNum(1,7,tmp,3);
		//Delay_ms(40);
	}
	while(0)
	{
	TCS34725_Get_RGBData(&ClearData,&RedData,&GreenData,&BlueData);
	if(RedData>GreenData&&RedData>BlueData)
	{
		OLED_ShowString(2,7,"Red   ");
	}
		else if(GreenData>RedData&&GreenData>BlueData)
		{
			OLED_ShowString(2,7,"Green  ");
		}
		else	if(BlueData>GreenData&&BlueData>RedData)
		{
		OLED_ShowString(2,7,"Blue  ");
		}
		Delay_ms(50);
	}
	while(0)
	{
		DHT11_ReadData(&temp,&humi);
		OLED_ShowNum(2,6,temp,2);
		OLED_ShowNum(3,6,humi,2);
		Delay_ms(20);
	}
/*************************模块的使用End**********************************/
	//密码模块
	while(1)
	{
	keynum=Key_GetNum();
		if(code_order!=3&&keynum!=0)
		{
			code_order++;
			if(keynum==5)
			{
				if(code_order!=0)
					code_order--;
				inputword/=10;
				OLED_ShowString(3,code_order," ");
				if(code_order!=0)
				code_order--;
			}
			else
			{
				inputword+=keynum;
			inputword*=10;
			OLED_ShowNum(3,code_order,keynum,1);
			}
		}
		else if(code_order==3&&keynum!=0)
		{
			code_order++;
			if(keynum==5)
			{code_order--;
				inputword/=10;
				OLED_ShowString(3,code_order," ");
				code_order--;
			}
			else{
				inputword+=keynum;
			
			OLED_ShowNum(3,code_order,keynum,1);
			code_order=0;
			if(inputword==password)
			{
				OLED_ShowString(4,1,"      ");
				OLED_ShowString(4,1,"OK!");
				Delay_ms(200);
				OLED_Clear();
				OLED_ShowString(1,1,"***");
				OLED_ShowString(2,1,"Wait Cmd ...");
				break;
			}
			else
			{
				inputword=0;
				OLED_ShowString(4,1,"      ");
				OLED_ShowString(4,1,"Error!");
				Delay_s(1);
				OLED_ShowString(3,1,"    ");
				OLED_ShowString(4,1,"      ");
			}
			}
		}
	}
/************************命令接收和执行模***************************/
  while (1)
  {

		if(RxFlag==1)
		{
			RxFlag=0;
/********指令：电机小幅度启并上报********/			
			if(strstr((char*)Rxbuf,"Motor_Small")!=NULL)
			{
					HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET);
				
				//if询问FPGA是否已启
				OLED_Clear();
				OLED_ShowString(1,1,"Cmd:Mtr_Small");
				OLED_ShowString(2,1,"State:OK");				
				printf("AT+HMPUB=1,\"$oc/devices/67d96ae7375e694aa693dd81_zhihuinongye/sys/properties/report\",75,\"{\\\"services\\\":[{\\\"service_id\\\":\\\"Sprayswitchcontrol\\\",\\\"properties\\\":{\\\"Motor\\\":1}}]}\"\r\n");
				//如果没有启动，再做处
				__HAL_TIM_SET_COUNTER(&htim2,0);
				__HAL_TIM_SET_AUTORELOAD(&htim2,20000-1);
				HAL_TIM_Base_Start(&htim2);
				HAL_UART_Transmit(&huart1,(uint8_t*)"\r\n",2,0xffff);
				HAL_UART_Transmit(&huart1,(uint8_t*)"FPGA Success!",13,0xffff);
			}
/*********指令：电机中幅度启并上报***********/	
				else	if(strstr((char*)Rxbuf,"Motor_Middle")!=NULL)
			{
					HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET);
				//if询问FPGA是否已启
				OLED_ShowString(1,1,"Cmd:Mtr_Middle");
				OLED_ShowString(2,1,"State:OK");	
				printf("AT+HMPUB=1,\"$oc/devices/67d96ae7375e694aa693dd81_zhihuinongye/sys/properties/report\",75,\"{\\\"services\\\":[{\\\"service_id\\\":\\\"Sprayswitchcontrol\\\",\\\"properties\\\":{\\\"Motor\\\":3}}]}\"\r\n");
				//如果没有启动，再做处
				__HAL_TIM_SET_COUNTER(&htim2,0);
				__HAL_TIM_SET_AUTORELOAD(&htim2,50000-1);
				HAL_TIM_Base_Start(&htim2);
				HAL_UART_Transmit(&huart1,(uint8_t*)"\r\n",2,0xffff);
				HAL_UART_Transmit(&huart1,(uint8_t*)"FPGA Success!",13,0xffff);
			}
/*********指令：电机大幅度启并上报**********/	
				else	if(strstr((char*)Rxbuf,"Motor_Numerous")!=NULL)
			{

				OLED_ShowString(1,1,"Cmd:Mtr_Numerous");
				OLED_ShowString(2,1,"State:OK");					
					HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET);
				printf("AT+HMPUB=1,\"$oc/devices/67d96ae7375e694aa693dd81_zhihuinongye/sys/properties/report\",75,\"{\\\"services\\\":[{\\\"service_id\\\":\\\"Sprayswitchcontrol\\\",\\\"properties\\\":{\\\"Motor\\\":5}}]}\"\r\n");
				__HAL_TIM_SET_COUNTER(&htim2,0);
				__HAL_TIM_SET_AUTORELOAD(&htim2,100000-1);
				HAL_TIM_Base_Start(&htim2);
				HAL_UART_Transmit(&huart1,(uint8_t*)"\r\n",2,0xffff);
				HAL_UART_Transmit(&huart1,(uint8_t*)"FPGA Success!",13,0xffff);
			}
/***********指令：测量距离并上报**************/				
			else if(strstr((char*)Rxbuf,"Range_distance")!=NULL)
			{
				
				uint16_t tmp=0;
					tmp=VL6180X_GetRange();
				OLED_Clear();
				OLED_ShowString(1,1,"Cmd:Rge_distance");
				OLED_ShowString(2,1,"Distance:");
				OLED_ShowNum(2,10,tmp,3);
				if(tmp>99)
				printf("AT+HMPUB=1,\"$oc/devices/67d96ae7375e694aa693dd81_zhihuinongye/sys/properties/report\",80,\"{\\\"services\\\":[{\\\"service_id\\\":\\\"Sprayswitchcontrol\\\",\\\"properties\\\":{\\\"Distance\\\":%d}}]}\"\r\n",tmp);
				else
				printf("AT+HMPUB=1,\"$oc/devices/67d96ae7375e694aa693dd81_zhihuinongye/sys/properties/report\",79,\"{\\\"services\\\":[{\\\"service_id\\\":\\\"Sprayswitchcontrol\\\",\\\"properties\\\":{\\\"Distance\\\":%d}}]}\"\r\n",tmp);
				Delay_s(1);
		strx=strstr((const char*)Rxbuf,(const char*)"+HMPUB OK");
				TimeOutCnt=0;
		while(strx==NULL)
		{
			TimeOutCnt++;
			if(TimeOutCnt==10)
				break;
			Clear_Buffer();	
			printf("距离上报失败");
			Delay_s(1);
				if(tmp>99)
				printf("AT+HMPUB=1,\"$oc/devices/67d96ae7375e694aa693dd81_zhihuinongye/sys/properties/report\",80,\"{\\\"services\\\":[{\\\"service_id\\\":\\\"Sprayswitchcontrol\\\",\\\"properties\\\":{\\\"Distance\\\":%d}}]}\"\r\n",tmp);
				else
				printf("AT+HMPUB=1,\"$oc/devices/67d96ae7375e694aa693dd81_zhihuinongye/sys/properties/report\",79,\"{\\\"services\\\":[{\\\"service_id\\\":\\\"Sprayswitchcontrol\\\",\\\"properties\\\":{\\\"Distance\\\":%d}}]}\"\r\n",tmp);
			Delay_s(1);
			strx=strstr((const char*)Rxbuf,(const char*)"+HMPUB OK");
		}
		Clear_Buffer();
		printf("距离上报成功");
				HAL_UART_Transmit(&huart1,(uint8_t*)"\r\n",2,0xffff);
				HAL_UART_Transmit(&huart1,(uint8_t*)"Success!",8,0xffff);
			}
/*************指令：测量当前颜色并上报***************/				
			else if(strstr((char*)Rxbuf,"Get_Color")!=NULL)
			{
				OLED_Clear();
				OLED_ShowString(1,1,"Cmd:Get_Color");
				OLED_ShowString(2,1,"Color:");
		TCS34725_Get_RGBData(&ClearData,&RedData,&GreenData,&BlueData);
	if(RedData>GreenData&&RedData>BlueData)
	{
		OLED_ShowString(2,7,"Red   ");
		printf("AT+HMPUB=1,\"$oc/devices/67d96ae7375e694aa693dd81_zhihuinongye/sys/properties/report\",75,\"{\\\"services\\\":[{\\\"service_id\\\":\\\"Sprayswitchcontrol\\\",\\\"properties\\\":{\\\"Color\\\":1}}]}\"\r\n");

	}
		else if(GreenData>RedData&&GreenData>BlueData)
		{
			OLED_ShowString(2,7,"Green  ");
		printf("AT+HMPUB=1,\"$oc/devices/67d96ae7375e694aa693dd81_zhihuinongye/sys/properties/report\",75,\"{\\\"services\\\":[{\\\"service_id\\\":\\\"Sprayswitchcontrol\\\",\\\"properties\\\":{\\\"Color\\\":2}}]}\"\r\n");

		}
		else	if(BlueData>GreenData&&BlueData>RedData)
		{
		OLED_ShowString(2,7,"Blue  ");
		printf("AT+HMPUB=1,\"$oc/devices/67d96ae7375e694aa693dd81_zhihuinongye/sys/properties/report\",75,\"{\\\"services\\\":[{\\\"service_id\\\":\\\"Sprayswitchcontrol\\\",\\\"properties\\\":{\\\"Color\\\":3}}]}\"\r\n");

		}
		Delay_s(1);
		strx=strstr((const char*)Rxbuf,(const char*)"+HMPUB OK");
		TimeOutCnt=0;	
		while(strx==NULL)//上报失败重新发送
		{
			TimeOutCnt++;
			if(TimeOutCnt==10)
				break;
			Clear_Buffer();	
			printf("颜色上报失败");
			Delay_s(1);
	if(RedData>GreenData&&RedData>BlueData)
	{
		OLED_ShowString(2,7,"Red   ");
		printf("AT+HMPUB=1,\"$oc/devices/67d96ae7375e694aa693dd81_zhihuinongye/sys/properties/report\",75,\"{\\\"services\\\":[{\\\"service_id\\\":\\\"Sprayswitchcontrol\\\",\\\"properties\\\":{\\\"Color\\\":1}}]}\"\r\n");

	}
		else if(GreenData>RedData&&GreenData>BlueData)
		{
			OLED_ShowString(2,7,"Green  ");
		printf("AT+HMPUB=1,\"$oc/devices/67d96ae7375e694aa693dd81_zhihuinongye/sys/properties/report\",75,\"{\\\"services\\\":[{\\\"service_id\\\":\\\"Sprayswitchcontrol\\\",\\\"properties\\\":{\\\"Color\\\":2}}]}\"\r\n");

		}
		else	if(BlueData>GreenData&&BlueData>RedData)
		{
		OLED_ShowString(2,7,"Blue  ");
		printf("AT+HMPUB=1,\"$oc/devices/67d96ae7375e694aa693dd81_zhihuinongye/sys/properties/report\",75,\"{\\\"services\\\":[{\\\"service_id\\\":\\\"Sprayswitchcontrol\\\",\\\"properties\\\":{\\\"Color\\\":3}}]}\"\r\n");

		}
		Delay_s(1);
			strx=strstr((const char*)Rxbuf,(const char*)"+HMPUB OK");
		}
		Clear_Buffer();
		printf("颜色上报成功");
				HAL_UART_Transmit(&huart1,(uint8_t*)"\r\n",2,0xffff);
				HAL_UART_Transmit(&huart1,(uint8_t*)"Success!",8,0xffff);
			}
			
/*************指令：测量当前温度并上报******************/				
			else if(strstr((char*)Rxbuf,"Get_temperature")!=NULL)
			{
				DHT11_ReadData(&temp,&humi);
				OLED_Clear();
				OLED_ShowString(1,1,"Cmd:Get_temp");
				OLED_ShowString(2,1,"Temp:");
				OLED_ShowNum(2,6,temp,2);
				if(temp>9)
					printf("AT+HMPUB=1,\"$oc/devices/67d96ae7375e694aa693dd81_zhihuinongye/sys/properties/report\",82,\"{\\\"services\\\":[{\\\"service_id\\\":\\\"Sprayswitchcontrol\\\",\\\"properties\\\":{\\\"Temperature\\\":%d}}]}\"\r\n",temp);
				else
					printf("AT+HMPUB=1,\"$oc/devices/67d96ae7375e694aa693dd81_zhihuinongye/sys/properties/report\",81,\"{\\\"services\\\":[{\\\"service_id\\\":\\\"Sprayswitchcontrol\\\",\\\"properties\\\":{\\\"Temperature\\\":%d}}]}\"\r\n",temp);					
				Delay_s(1);
		strx=strstr((const char*)Rxbuf,(const char*)"+HMPUB OK");
		TimeOutCnt=0;
				while(strx==NULL)//上报失败重新上报
		{
			TimeOutCnt++;
			if(TimeOutCnt==10)
				break;
			Clear_Buffer();	
			printf("温度上报失败");
			Delay_s(1);
				if(temp>9)
					printf("AT+HMPUB=1,\"$oc/devices/67d96ae7375e694aa693dd81_zhihuinongye/sys/properties/report\",82,\"{\\\"services\\\":[{\\\"service_id\\\":\\\"Sprayswitchcontrol\\\",\\\"properties\\\":{\\\"Temperature\\\":%d}}]}\"\r\n",temp);
				else
					printf("AT+HMPUB=1,\"$oc/devices/67d96ae7375e694aa693dd81_zhihuinongye/sys/properties/report\",81,\"{\\\"services\\\":[{\\\"service_id\\\":\\\"Sprayswitchcontrol\\\",\\\"properties\\\":{\\\"Temperature\\\":%d}}]}\"\r\n",temp);					
			Delay_s(1);
			strx=strstr((const char*)Rxbuf,(const char*)"+HMPUB OK");
		}
		Clear_Buffer();
		printf("温度上报成功");
		HAL_UART_Transmit(&huart1,(uint8_t*)"\r\n",2,0xffff);
		HAL_UART_Transmit(&huart1,(uint8_t*)"Success!",8,0xffff);
	}
/*指令：测量当前湿度并上报*/	
				else if(strstr((char*)Rxbuf,"Get_humidity")!=NULL)
			{
				DHT11_ReadData(&temp,&humi);
				OLED_Clear();
				OLED_ShowString(1,1,"Cmd:Get_humidity");
				OLED_ShowString(2,1,"Humi:");
				OLED_ShowNum(2,6,humi,2);
				uint8_t humi1=humi;
					printf("AT+HMPUB=1,\"$oc/devices/67d96ae7375e694aa693dd81_zhihuinongye/sys/properties/report\",79,\"{\\\"services\\\":[{\\\"service_id\\\":\\\"Sprayswitchcontrol\\\",\\\"properties\\\":{\\\"Humidity\\\":%d}}]}\"\r\n",humi1);
				
				Delay_s(1);
		strx=strstr((const char*)Rxbuf,(const char*)"+HMPUB OK");
				TimeOutCnt=0;
		while(strx==NULL)
		{
			TimeOutCnt++;
			if(TimeOutCnt==10)
				break;
			Clear_Buffer();	
			printf("湿度上报失败");
			Delay_s(1);
					printf("AT+HMPUB=1,\"$oc/devices/67d96ae7375e694aa693dd81_zhihuinongye/sys/properties/report\",79,\"{\\\"services\\\":[{\\\"service_id\\\":\\\"Sprayswitchcontrol\\\",\\\"properties\\\":{\\\"Humidity\\\":%d}}]}\"\r\n",humi1);
			Delay_s(1);
			strx=strstr((const char*)Rxbuf,(const char*)"+HMPUB OK");
		}
		Clear_Buffer();
		printf("湿度上报成功");
		HAL_UART_Transmit(&huart1,(uint8_t*)"\r\n",2,0xffff);
		HAL_UART_Transmit(&huart1,(uint8_t*)"Success!",8,0xffff);
	}
/*指令：测量当前各个模块参数并上报*/	
			else if(strstr((char*)Rxbuf,"Maintenance_Parameter")!=NULL)
			{
				Enter_MaintenanceMode(&Maintenance);
				Show_MaintenanceInformation(&Maintenance);
				if(Maintenance.Maintenance_State>9)
					printf("AT+HMPUB=1,\"$oc/devices/67d96ae7375e694aa693dd81_zhihuinongye/sys/properties/report\",88,\"{\\\"services\\\":[{\\\"service_id\\\":\\\"Sprayswitchcontrol\\\",\\\"properties\\\":{\\\"Maintenance_State\\\":%d}}]}\"\r\n",Maintenance.Maintenance_State);
				else
					printf("AT+HMPUB=1,\"$oc/devices/67d96ae7375e694aa693dd81_zhihuinongye/sys/properties/report\",87,\"{\\\"services\\\":[{\\\"service_id\\\":\\\"Sprayswitchcontrol\\\",\\\"properties\\\":{\\\"Maintenance_State\\\":%d}}]}\"\r\n",Maintenance.Maintenance_State);					
			Delay_s(1);
				strx=strstr((const char*)Rxbuf,(const char*)"+HMPUB OK");
			TimeOutCnt=0;			
		while(strx==NULL)
		{
			TimeOutCnt++;
			if(TimeOutCnt==10)
				break;
			Clear_Buffer();	
			printf("模块参数上报失败\r\n");
			Delay_s(1);
				if(Maintenance.Maintenance_State>9)
					printf("AT+HMPUB=1,\"$oc/devices/67d96ae7375e694aa693dd81_zhihuinongye/sys/properties/report\",88,\"{\\\"services\\\":[{\\\"service_id\\\":\\\"Sprayswitchcontrol\\\",\\\"properties\\\":{\\\"Maintenance_State\\\":%d}}]}\"\r\n",Maintenance.Maintenance_State);
				else
					printf("AT+HMPUB=1,\"$oc/devices/67d96ae7375e694aa693dd81_zhihuinongye/sys/properties/report\",87,\"{\\\"services\\\":[{\\\"service_id\\\":\\\"Sprayswitchcontrol\\\",\\\"properties\\\":{\\\"Maintenance_State\\\":%d}}]}\"\r\n",Maintenance.Maintenance_State);					
			Delay_s(1);
			strx=strstr((const char*)Rxbuf,(const char*)"+HMPUB OK");
		}
		Clear_Buffer();
		//printf("模块参数上报成功\r\n");
				HAL_UART_Transmit(&huart1,(uint8_t*)"Success!",8,0xffff);
			}
/************指令输入错误*********/	
//			else
//			{
//				OLED_Clear();
//				OLED_ShowString(1,1,"Cmd:ErrorCmd");
//				OLED_ShowString(2,1,"State:Error1");
//				HAL_UART_Transmit(&huart1,(uint8_t*)"\r\n",2,0xffff);
//				HAL_UART_Transmit(&huart1,(uint8_t*)"Command Error!\r\n",16,0xffff);
//			}
		Clear_Buffer();
		}
/************************上位机过串口输入指令********************************************/
			if(UART6_RxFlag==1)
			{
				char str[30];
				UART6_RxFlag=0;
/********测距指令************/
				if(UART6_Rxbuf[0]=='d')
				{
					uint16_t tmp=0,tmp1,tmp2;
					tmp1=VL6180X_GetRange();
					Delay_ms(100);
					tmp2=VL6180X_GetRange();
					Delay_ms(100);
					tmp=(tmp1+tmp2)/2;//测距2次，求其平均数
					OLED_Clear();
					OLED_ShowString(1,1,"distance:");
					OLED_ShowString(2,1,"         ");
					OLED_ShowNum(2,1,tmp,3);
					OLED_ShowString(2,4,"mm");
					sprintf(str,"距离为:%d\r\n",tmp);
					HAL_UART_Transmit(&huart6,(uint8_t*)str,sizeof("距离为:%d\r\n"),0xffff);
					HAL_UART_Transmit(&huart6,(uint8_t*)"\r\n",2,0xffff);
					HAL_UART_Transmit(&huart6,(uint8_t*)"Success Receive\r\n",17,0xffff);
				}
/********舵机维护状态指令************/
					else	if(UART6_Rxbuf[0]=='e')
				{
					OLED_Clear();
					OLED_ShowString(1,1,"Motor_State:");
					char str[20];
						uint8_t Rx=0x05;
					HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET);					
					__HAL_TIM_SET_COUNTER(&htim2,0);
					__HAL_TIM_SET_AUTORELOAD(&htim2,30000-1);
						HAL_TIM_Base_Start(&htim2);
					if(Rx==0x05)
				{
					sprintf(str,"舵机状态正常\r\n");//正常则返回
					HAL_UART_Transmit(&huart6,(uint8_t*)"Success Receive\r\n",17,0xffff);
					
					OLED_ShowString(2,1,"OK");
					HAL_UART_Transmit(&huart6,(uint8_t*)str,20,0xffff);
					HAL_UART_Transmit(&huart6,(uint8_t*)"\r\n",2,0xffff);
				}
				}
/********颜色指令************/
			else if(UART6_Rxbuf[0]=='c')
		{
			
			OLED_Clear();
			OLED_ShowString(1,1,"Color:");
			TCS34725_Get_RGBData(&ClearData,&RedData,&GreenData,&BlueData);
			if(RedData>GreenData&&RedData>BlueData)//显示并回显
			{
				OLED_ShowString(2,1,"         ");
				OLED_ShowString(2,1,"Red   ");
			
				sprintf(str,"颜色为:红色\r\n");
				HAL_UART_Transmit(&huart6,(uint8_t*)str,sizeof("颜色为:红色\r\n"),0xffff);
			
			HAL_UART_Transmit(&huart6,(uint8_t*)"Success Receive\r\n",17,0xffff);
			}
				else if(GreenData>RedData&&GreenData>BlueData)
				{
					OLED_ShowString(2,1,"         ");
					OLED_ShowString(2,1,"Green  ");
			
					sprintf(str,"颜色为:绿色\r\n");
					HAL_UART_Transmit(&huart6,(uint8_t*)str,sizeof("颜色为:绿色\r\n"),0xffff);
			
			HAL_UART_Transmit(&huart6,(uint8_t*)"Success Receive\r\n",17,0xffff);
				}
				else	if(BlueData>GreenData&&BlueData>RedData)
				{
					OLED_ShowString(2,1,"         ");
					
					sprintf(str,"颜色为:蓝色\r\n");
			OLED_ShowString(2,1,"Blue     ");
					HAL_UART_Transmit(&huart6,(uint8_t*)str,sizeof("颜色为:蓝色\r\n"),0xffff);
			
			HAL_UART_Transmit(&huart6,(uint8_t*)"Success Receive\r\n",17,0xffff);
				}
		}
/********温湿度指令************/
				else if(UART6_Rxbuf[0]=='w')
				{
					DHT11_ReadData(&temp,&humi);
					OLED_Clear();
					OLED_ShowString(1,1,"temp:");
					OLED_ShowNum(1,6,temp,2);
					OLED_ShowString(2,1,"humi:");
					OLED_ShowNum(2,6,humi,2);
					sprintf(str,"温度为:%d,湿度为:%d",temp,humi);
			HAL_UART_Transmit(&huart6,(uint8_t*)str,30,0xffff);
			HAL_UART_Transmit(&huart6,(uint8_t*)"\r\n",2,0xffff);
			HAL_UART_Transmit(&huart6,(uint8_t*)"Success Receive\r\n",17,0xffff);
				}
/********运行小舵机指令************/
								else if(UART6_Rxbuf[0]=='B')
				{

					HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET);
				__HAL_TIM_SET_COUNTER(&htim2,0);
					__HAL_TIM_SET_AUTORELOAD(&htim2,20000-1);//更改定时寄存器的值，定不同时间
				HAL_TIM_Base_Start(&htim2);
					OLED_Clear();
					OLED_ShowString(1,1,"Motor:");
					OLED_ShowString(2,1,"Small Mode");
					sprintf(str,"舵机小模式已启动\r\n");
			HAL_UART_Transmit(&huart6,(uint8_t*)str,sizeof("舵机小模式已启动\r\n"),0xffff);
			
			HAL_UART_Transmit(&huart6,(uint8_t*)"Success Receive\r\n",17,0xffff);
				}
/********运行中舵机指令************/
								else if(UART6_Rxbuf[0]=='C')
				{

					HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET);
				__HAL_TIM_SET_COUNTER(&htim2,0);
				__HAL_TIM_SET_AUTORELOAD(&htim2,50000-1);
				HAL_TIM_Base_Start(&htim2);
					OLED_Clear();
					OLED_ShowString(1,1,"Motor:");
					OLED_ShowString(2,1,"Medium Mode");
					sprintf(str,"舵机中模式已启动\r\n");
			HAL_UART_Transmit(&huart6,(uint8_t*)str,30,0xffff);
			
			HAL_UART_Transmit(&huart6,(uint8_t*)"Success Receive\r\n",17,0xffff);
				}
/********运行大舵机指令************/
								else if(UART6_Rxbuf[0]=='D')
				{
					HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET);
				__HAL_TIM_SET_COUNTER(&htim2,0);
				__HAL_TIM_SET_AUTORELOAD(&htim2,100000-1);
				HAL_TIM_Base_Start(&htim2);
					OLED_Clear();
					OLED_ShowString(1,1,"Motor:");
					OLED_ShowString(2,1,"Numerous Mode");
					sprintf(str,"舵机大模式已开启\r\n");
			HAL_UART_Transmit(&huart6,(uint8_t*)str,30,0xffff);
			
			HAL_UART_Transmit(&huart6,(uint8_t*)"Success Receive\r\n",17,0xffff);
				}
				
			else if(UART6_Rxbuf[0]=='N')
		{
			uint8_t tmp=0x0;
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_RESET);
		HAL_SPI_Transmit(&hspi1,&tmp,1,0xff);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_SET);
		__HAL_TIM_SET_COUNTER(&htim2,0);
		HAL_UART_Transmit(&huart6,(uint8_t*)"舵机已停止\r\n",sizeof("舵机已停止\r\n"),0xffff);
			OLED_Clear();
			OLED_ShowString(1,1,"Motor:");
			OLED_ShowString(2,1,"Stop Mode");
	HAL_UART_Transmit(&huart6,(uint8_t*)"Success Receive\r\n",17,0xffff);
		}
/********错误指令处理************/
				else
				{
					OLED_Clear();
					OLED_ShowString(1,1,"Cmd:");
					OLED_ShowString(2,1,"Error2");
					HAL_UART_Transmit(&huart6,(uint8_t*)"Command Error\r\n",sizeof("Command Error\r\n"),0xffff);
				}
				
				
//清除缓存
			for(uint16_t i=0;i<512;i++)
			{
				UART6_Rxbuf[i]=0;//清空接收数组
			}
		}



    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 400;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
