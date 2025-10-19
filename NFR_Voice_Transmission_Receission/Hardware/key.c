#include "stm32f3xx.h"                  // Device header

#include "Delay.h"

/*
brief:Initializate buttons(B1,B111)
param:none
back:none
*/
void Key_Init(){
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.Mode=GPIO_MODE_INPUT  ;
	GPIO_InitStructure.Pull=GPIO_PULLDOWN;
	GPIO_InitStructure.Speed=GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Pin=GPIO_PIN_5;
 	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	
}
uint8_t keynum=0;
uint8_t Key_GetNum()
{
	uint8_t tmp=0;
	if(keynum)
	{
		tmp=keynum;
	keynum=0;
	return tmp;
	}
	return 0;
}

uint8_t Key_GetState()
{
	if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_5)==1)
		return 1;
	else 
		return 0;
}





void Key_Kick()
{
	static uint8_t PreviousState=0;
	static uint8_t CurrentState=0;
	static uint8_t Count;
	Count ++;
	if (Count >= 20)
	{
		Count = 0;
	PreviousState=CurrentState;
	CurrentState=Key_GetState();
	
	if(CurrentState!=0&&PreviousState==0)
		keynum=CurrentState;
	}
	
	
}
