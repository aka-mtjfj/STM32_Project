/**
 ****************************************************************************************************
 * @file        demo.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-06-21
 * @brief       ATK-MW8266D模块TCP透传实验
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32F103开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "demo.h"
#include "./SYSTEM/sys/sys.h"
#include "./BSP/ATK_MW8266D/atk_mw8266d.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/TIMER/gtim.h"
#include<stdio.h>
#include<math.h>
#include<string.h>


#define DEMO_WIFI_SSID          "aniy"
#define DEMO_WIFI_PWD           "12345678"
#define DEMO_TCP_SERVER_IP      "192.168.3.15"	// 主机IP地址
#define DEMO_TCP_SERVER_PORT    "8080"				// 主机IP端口

extern TIM_HandleTypeDef g_timx_pwm_chy_handle; 

int k = 0;
char receive[18] = "LOCATION:";
int16_t x1 = 25;
int16_t y1 = 0;

float Angle1 = 90;
float Angle2 = 90;
float rcc1;
float rcc2;
double pi=3.14159265;
double x0=25.0, y0=0.0, h=50;
double x2, y2, result1, result2;
char *p1;
char *p2;



/**
 * @brief       显示IP地址
 * @param       无
 * @retval      无
 */
static void demo_show_ip(char *buf)
{
    printf("IP: %s\r\n", buf);
    lcd_show_string(60, 151, 128, 16, 16, buf, BLUE);

}

/**
 * @brief       按键0功能，功能测试
 * @param       is_unvarnished: 0，未进入透传
 *                              1，已进入透传
 * @retval      无
 */
static void demo_key0_fun(uint8_t is_unvarnished)
{
    uint8_t ret;
    
    if (is_unvarnished == 0)
    {
        /* 进行AT指令测试 */
        ret = atk_mw8266d_at_test();
        if (ret == 0)
        {
            printf("AT test success!\r\n");
        }
        else
        {
            printf("AT test failed!\r\n");
        }
    }
    else
    {
        /* 通过透传，发送信息至TCP Server */
        atk_mw8266d_uart_printf("This ATK-MW8266D TCP Connect Test.\r\n");
    }
}

/**
 * @brief       按键1功能，切换透传模式
 * @param       is_unvarnished: 0，未进入透传
 *                              1，已进入透传
 * @retval      无
 */
static void demo_key1_fun(uint8_t *is_unvarnished)
{
    uint8_t ret;
    
    if (*is_unvarnished == 0)
    {
        /* 进入透传 */
        ret = atk_mw8266d_enter_unvarnished();
        if (ret == 0)
        {
            *is_unvarnished = 1;
            printf("Enter unvarnished!\r\n");
        }
        else
        {
            printf("Enter unvarnished failed!\r\n");
        }
    }
    else
    {
        /* 退出透传 */
        atk_mw8266d_exit_unvarnished();
        *is_unvarnished = 0;
        printf("Exit unvarnished!\r\n");
    }
}

/**
 * @brief       进入透传时，将接收自TCP Server的数据发送到串口调试助手
 * @param       is_unvarnished: 0，未进入透传
 *                              1，已进入透传
 * @retval      无
 */
static void demo_upload_data(uint8_t is_unvarnished)
{
    uint8_t *buf;
    
    if (is_unvarnished == 1)
    {
        /* 接收来自ATK-MW8266D UART的一帧数据 */
        buf = atk_mw8266d_uart_rx_get_frame();
        if (buf != NULL)
        {
					receive[9] = buf[0];  //（
					receive[10] = buf[1];  
					receive[11] = buf[2];  
					receive[12] = buf[3];
					receive[13] = buf[4];  
					receive[14] = buf[5];
					receive[15] = buf[6];
					receive[16] = buf[7];
					receive[17] = buf[8];  //）

			
			if(receive[10] == '-')
			{
				if(receive[13] == '-' || receive[14] == '-')
				{
					if(receive[12] == ',')  //- -
					{
						if(receive[15] == ')')
						{
							x1 = -(receive[11] - 48);
							y1 = -(receive[14] - 48);
						}
						else
						{
							x1 = -(receive[11] - 48);
							y1 = -(receive[14] - 48)*10-(receive[15]-48);
						}
					}
					else
					{
						if(receive[16] == ')')
						{
							x1 = -(receive[11] - 48)*10-(receive[12]-48);
							y1 = -(receive[15] - 48);
						}
						else
						{
							x1 = -(receive[11] - 48)*10-(receive[12]-48);
							y1 = -(receive[15] - 48)*10-(receive[16]-48);
						}
					}
				}
				else // - +
				{
					if(receive[12] == ',')  
					{
						if(receive[14] == ')')
						{
							x1 = -(receive[11] - 48);
							y1 = receive[13] - 48;
						}
						else
						{
							x1 = -(receive[11] - 48);
							y1 = (receive[13] - 48)*10 + (receive[14]-48);
						}
					}
					else
					{
						if(receive[15] == ')')
						{
							x1 = -(receive[11] - 48)*10-(receive[12]-48);
							y1 = receive[14] - 48;
						}
						else
						{
							x1 = -(receive[11] - 48)*10-(receive[12]-48);
							y1 = (receive[14] - 48)*10 + (receive[15]-48);
						}
					}
				}
			}
			else //+ -
			{
				if(receive[12] == '-' || receive[13] == '-')
				{
					if(receive[11] == ',')  //
					{
						if(receive[14] == ')')
						{
							x1 = (receive[10] - 48);
							y1 = -(receive[13] - 48);
						}
						else
						{
							x1 = (receive[10] - 48);
							y1 = -(receive[13] - 48)*10-(receive[14]-48);
						}
					}
					else
					{
						if(receive[15] == ')')
						{
							x1 = (receive[10] - 48)*10+(receive[11]-48);
							y1 = -(receive[14] - 48);
						}
						else
						{
							x1 = (receive[10] - 48)*10+(receive[11]-48);
							y1 = -(receive[14] - 48)*10-(receive[15]-48);
						}
					}
				}
				else // + +
				{   
					if(receive[11] == ',')  
					{
						if(receive[13] == ')')
						{
							
							
							x1 = (buf[1] - 48);
							y1 = (buf[3] - 48);
							
						}
						else
						{
							x1 = (buf[1] - 48);
							y1 = (buf[3] - 48)*10 + (receive[13]-48);
						}
					}
					else
					{
						if(receive[14] == ')')
						{
							x1 = (receive[10] - 48)*10+(receive[11]-48);
							y1 = receive[13] - 48;
						}
						else
						{
							x1 = (receive[10] - 48)*10 + (receive[11]-48);
							y1 = (receive[13] - 48)*10 + (receive[14]-48);
						}
					}
				}
			}
			
			
			
           // printf("%s\n", buf);
			printf("(%d,\t", x1);
			printf("%d)\t", y1);

		
			
            /* 重开开始接收来自ATK-MW8266D UART的数据 */
            atk_mw8266d_uart_rx_restart();
        }
    }
}

/**
 * @brief       例程演示入口函数
 * @param       无
 * @retval      无
 */
void demo_run(void)
{
    uint8_t ret;
    char ip_buf[16];
    uint8_t key;
	uint8_t key1;
    uint8_t is_unvarnished = 0;
    
    /* 初始化ATK-MW8266D */
    ret = atk_mw8266d_init(115200);
    if (ret != 0)
    {
        printf("ATK-MW8266D init failed!\r\n");
        while (1)
        {
            LED0_TOGGLE();
            delay_ms(200);
        }
    }
		else
        printf("ATK-MW8266D init Success!\r\n");
		
    
    printf("Joining to AP...\r\n");
    ret  = atk_mw8266d_restore();                               /* 恢复出厂设置 */
    ret += atk_mw8266d_at_test();                               /* AT测试 */
    ret += atk_mw8266d_set_mode(1);                             /* Station模式 */
    ret += atk_mw8266d_sw_reset();                              /* 软件复位 */
    ret += atk_mw8266d_ate_config(0);                           /* 关闭回显功能 */
    ret += atk_mw8266d_join_ap(DEMO_WIFI_SSID, DEMO_WIFI_PWD);  /* 连接WIFI */
    ret += atk_mw8266d_get_ip(ip_buf);                          /* 获取IP地址 */
    if (ret != 0)
    {
        printf("Error to join ap!\r\n");
        while (1)
        {
            LED0_TOGGLE();
            delay_ms(200);
        }
    }
    demo_show_ip(ip_buf);
    
    /* 连接TCP服务器 */
    ret = atk_mw8266d_connect_tcp_server(DEMO_TCP_SERVER_IP, DEMO_TCP_SERVER_PORT);
    if (ret != 0)
    {
        printf("Error to connect tcp server!\r\n");
        while (1)
        {
            LED0_TOGGLE();
            delay_ms(200);
        }
    }
    
    /* 重新开始接收新的一帧数据 */
    atk_mw8266d_uart_rx_restart();
    
    while (1)
    {
        key = key_scan(0);
        
        switch (key)
        {
            case KEY0_PRES:
            {
                /* 功能测试 */
                demo_key0_fun(is_unvarnished);
                break;
            }
            case KEY1_PRES:
            {
                /* 透传模式切换 */
                demo_key1_fun(&is_unvarnished);
                break;
            }
            default:
            {
                break;
            }
        }
        
        /* 发送透传接收自TCP Server的数据到串口调试助手 */
        demo_upload_data(is_unvarnished);
        
        delay_ms(10);
		
		lcd_clear(WHITE);
		int len, x = 40, i = 0 ,m=0 ;
		double t1=0 ,t2=0;
		len = strlen(receive);
		while(len && k==1)
		{
			lcd_show_char(x, 140, receive[i], 16, 0, RED);
			if(receive[i] == ')')
			break;
			i++;
			x = x + 10;
		}
		
		if(receive[9] == '(')
			k = 1;
		
		//lcd_show_num(80, 200,x1, 2, 16, BLUE);
		//lcd_show_num(100, 200, y1, 2, 16, BLUE);
		
//		key1 = key_scan(0);

	 switch (key)   //切换位置
        {
//            case KEY1_PRES:
//            {
//                x0=-x0;
//                break;
//            }
            case WKUP_PRES:
            {
                x0=-x0;
                break;
            }
            default:
            {
                break;
            }
        }
		
		x2 = x1 - x0;
		y2 = y1 - y0;
		m=sqrt(h*h+x2*x2);
		result1 = atan(x2/h);
		result2 = atan(y2/m);
		Angle1 = 90 - result1*180/pi;
		Angle2 = 90 - result2*180/pi;
		
		
    rcc1=20000-(Angle1/180*2000+500);
		rcc2=20000-(Angle2/180*2000+500);
	
    t1=Angle1*8;
		t2=Angle2*8;
		
		if(t1>t2)
		{
			lcd_show_num(80, 200,t1, 2, 16, BLUE);
		}
		else
		{
			lcd_show_num(80, 200,t2, 2, 16, BLUE);
		}
	

        /* 修改比较值控制占空比 */
        __HAL_TIM_SET_COMPARE(&g_timx_pwm_chy_handle, GTIM_TIMX_PWM_CHY1, rcc1);
	    __HAL_TIM_SET_COMPARE(&g_timx_pwm_chy_handle, GTIM_TIMX_PWM_CHY2, rcc2);
		
		
		
		
		  }
		
}
