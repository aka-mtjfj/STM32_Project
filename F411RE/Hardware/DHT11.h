#ifndef __DHT11_H
#define __DHT11_H
uint8_t DHT11_Init(void);
uint8_t DHT11_ReadData(uint8_t* temp,uint8_t* humi);
#endif
