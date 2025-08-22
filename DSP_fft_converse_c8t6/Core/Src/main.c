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
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "oled.h"
#include "arm_math.h"
#include "math.h"
#include "delay.h"
#include "string.h"
#include "stdio.h"
/**
 * @file      ：离散傅里叶变换
 * @brief     ：1.使用相关函数对输入的信号进行傅里叶变换，采样频率20kHz，有采样定理，输出的频率范围为0-10kHz
									2.输入信号为了满足基四FFT，所以需要将信号长度设定为4的n次方，不足补零，因此每个输出频率分辨率
									为20k/256=78Hz
									3.因为环境中以及信号采集时会有低频分量，所以结果中会有部分低频分量。
									4.因为采集时的幅度没有进行对应转化，同时我们只关注于频率占比，所以输出幅值表示该频率占比多少。
									5.由于输出为双边频率谱，所以我们只需要关注正频率即可，即输出的前一半数据。
 * @details   ：1.可以通过调节咪头的增益引脚调整相关参数，这里使用默认值。
									2.使用引脚：OLED:SCL->PB12
																	 SDA->PB13
															MAX9814:OUT->PA0(ADC输入)。
 * @date ：2025年4月30日
 */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
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
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
	#define FFT_LENGTH		256 		//傅里叶变换长
	float fft_inputbuf[FFT_LENGTH*2];//时域的复数输入
	uint32_t ADC_DataNum=0;			//adc采样次数
	uint32_t ADCValue=0;				//adc采样值
	extern uint8_t ADC_TimeOutFlag;			//定时器到时间标志
	float fft_outputbuf[FFT_LENGTH];		//频域的幅值输（频率占比程度）
	
	
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
  MX_ADC1_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	OLED_Init();
	HAL_TIM_Base_Start_IT(&htim2);
	HAL_ADC_Start_DMA(&hadc1,&ADCValue,1); //开启ADC的DMA传输
	
	arm_cfft_radix4_instance_f32 scfft;//
	arm_cfft_radix4_init_f32(&scfft, FFT_LENGTH, 0, 1);//初始化傅里叶变换函数
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		if(ADC_TimeOutFlag)
		{
			ADC_TimeOutFlag = 0;//到达时间后进行采
			if(ADC_DataNum < FFT_LENGTH)
			{			
				fft_inputbuf[2*ADC_DataNum]=ADCValue;
				fft_inputbuf[2*ADC_DataNum+1]=0;
				ADC_DataNum++;			
			}
			else//到达fft长度后转换到频域，输出幅值的数组
			{

				HAL_TIM_Base_Stop(&htim2);
				ADC_DataNum = 0;
				arm_cfft_radix4_f32(&scfft, fft_inputbuf);//原地转化到频域
				arm_cmplx_mag_f32(fft_inputbuf,fft_outputbuf,FFT_LENGTH);		//幅度输出
				OLED_Clear();
				for(uint16_t i=0;i<128;i++)//显示正频率频谱
				{
				float	Amplitude=(fft_outputbuf[i]-2000)*64.0/4096.0;
				OLED_DrawLine(i,64,i,64-(int16_t)Amplitude);
				}

				OLED_Update();
				HAL_TIM_Base_Start(&htim2);

			}		
		}	
		
	//	printf("%d\r\n",ADCValue);
		
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
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
