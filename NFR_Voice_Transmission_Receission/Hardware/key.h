#ifndef __key_H_
#define __key_H_
#include "stm32f3xx.h"
void Key_Init(void);
unsigned char Key_GetNum(void);
uint8_t Key_GetState(void);
void Key_Kick(void);
#endif

