#include "stm32f4xx.h" // Device header
#include "Delay.h"
#include "OLED.h"
#include "stdio.h"
#include "string.h"
extern uint8_t RxFlag;
extern uint16_t Rxbuf[1024];//引用外部数组
 void Clear_buffer(void);
void Clear_Buffer()
{
		for(uint16_t i=0;i<1024;i++)
{
	Rxbuf[i]=0;//清空接收数组
}
}
/******注意：使用此函数会出现无法返回的错误，暂时未知原因，主函数直接使用该函数内部代码*******/
uint8_t L610_Init()
{

	//查询版本信息
		char *strx;
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
					//printf("查询信息失败");
					Delay_s(1);
					printf("ATI\r\n");
					Delay_s(1);
				}
				else
					break;
			}
		}
		Clear_Buffer();
		printf("version信息正确");
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
					//printf("还未获取到IP");
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
		
		//连接华为云
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
					//printf("连接失败");
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
		//上报属性
		
		printf("AT+HMPUB=1,\"$oc/devices/67d96ae7375e694aa693dd81_zhihuinongye/sys/properties/report\",76,\"{\\\"services\\\":[{\\\"service_id\\\":\\\"Sprayswitchcontrol\\\",\\\"properties\\\":{\\\"Switch\\\":1}}]}\"\r\n");
		Delay_s(1);
		strx=strstr((const char*)Rxbuf,(const char*)"+HMPUB OK");
		while(strx==NULL)
		{
			Clear_Buffer();	
			printf("1上报失败");
			Delay_s(1);
			printf("AT+HMPUB=1,\"$oc/devices/67d96ae7375e694aa693dd81_zhihuinongye/sys/properties/report\",76,\"{\\\"services\\\":[{\\\"service_id\\\":\\\"Sprayswitchcontrol\\\",\\\"properties\\\":{\\\"Switch\\\":0}}]}\"\r\n");
			Delay_s(1);
			strx=strstr((const char*)Rxbuf,(const char*)"+HMPUB OK");
		}
		Clear_Buffer();
		printf("1上报成功");
		OLED_ShowString(1,1,"test4 OK.");
		
		return 0;
}
