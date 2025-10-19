#include "voice_recieve.h"
#include "NRF24L01.h"
#include <string.h>
#include <stdio.h>
#include "stm32f3xx.h"

extern DAC_HandleTypeDef hdac;
extern TIM_HandleTypeDef htim2; // 如果使用定时器触发
extern DMA_HandleTypeDef hdma_dac_ch1;  

volatile device_state_t current_rx_state = STATE_IDLE;

#define CIRCULAR_BUFFER_SIZE (2*BUFFER_SIZE)
uint16_t dac_buffer[CIRCULAR_BUFFER_SIZE];

#define INTERMEDIATE_BUFFER_SIZE (BUFFER_SIZE * 4)
static uint16_t intermediate_audio_buffer[INTERMEDIATE_BUFFER_SIZE];
static volatile uint16_t ib_write_index = 0;
static volatile uint16_t ib_read_index = 0;
static volatile uint16_t ib_count = 0;
static uint32_t no_data_count = 0;
static uint32_t ib_overflow_count = 0;
/**
  * @brief ：函数描述:
  * @param ：参数描述:
  * @return ：返回描述:1为正常解析包，2为缓冲区逸出
  * @refer ：说明:
  */
uint8_t parse_nrf_packet_into_intermediate_buffer(void) {
	
	uint8_t need_print = 0; // 打印标记位
    for (int i = 0; i < SAMPLES_PER_PACKET; i++) 
	{
        uint16_t sample = (((uint16_t)NRF24L01_RxPacket[i * 2]) << 8) |((uint16_t)NRF24L01_RxPacket[i * 2 + 1]);
				//__disable_irq(); // 关闭中断，保护共享变量
        if (ib_count < INTERMEDIATE_BUFFER_SIZE) {
            intermediate_audio_buffer[ib_write_index] = sample;
            ib_write_index = (ib_write_index + 1) % INTERMEDIATE_BUFFER_SIZE;
            ib_count++;
        } else {
            ib_overflow_count++;
            if (ib_overflow_count % 100 == 0) {
                need_print=1;
							//printf("警告: 中间缓冲区溢出 %lu 次\n", (unsigned long)ib_overflow_count);
							
            }
            ib_read_index = (ib_read_index + 1) % INTERMEDIATE_BUFFER_SIZE;
            intermediate_audio_buffer[ib_write_index] = sample;
            ib_write_index = (ib_write_index + 1) % INTERMEDIATE_BUFFER_SIZE; // ib_count 保持为 INTERMEDIATE_BUFFER_SIZE
        }
    }
	//__enable_irq(); // 恢复中断
	return 1;
		if (need_print)
		{
			return 2;
		}
}
uint8_t fill_dac_buffer_from_intermediate(uint16_t* target_buffer) {
		//__disable_irq(); // 关闭中断，保护共享变量
    if (ib_count >= BUFFER_SIZE) {
        for (int i = 0; i < BUFFER_SIZE; i++) {
            target_buffer[i] = intermediate_audio_buffer[ib_read_index];
            ib_read_index = (ib_read_index + 1) % INTERMEDIATE_BUFFER_SIZE;
            ib_count--;
        }
				//__enable_irq(); // 恢复中断,单字读写，不需要
        return 1;
    }
		//__enable_irq(); // 恢复中断,单字读写，不需要
    return 0;
}

void HAL_DAC_ConvHalfCpltCallbackCh1(DAC_HandleTypeDef *hdac) {

        // 前半区 (0 ~ BUFFER_SIZE-1) 刚播放完，现在正在播放后半区 → 安全填充前半区
        fill_dac_buffer_from_intermediate(&dac_buffer[0]);
}

void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef *hdac) {

        // 后半区 (BUFFER_SIZE ~ end) 刚播放完，现在正在播放前半区 → 安全填充后半区
	fill_dac_buffer_from_intermediate(&dac_buffer[BUFFER_SIZE]);
  
}
void HAL_DAC_DMAUnderrunCallbackCh1(DAC_HandleTypeDef *hdac)
{
    static uint32_t underrun_count = 0;
    underrun_count++;
    if (underrun_count % 10 == 0) {
        printf("DAC 欠载 %lu 次！\n", (unsigned long)underrun_count);
    }

    hdac->State = HAL_DAC_STATE_READY;

    // 重新启动 DMA
    HAL_DAC_Start_DMA(hdac, DAC_CHANNEL_1, (uint32_t*)dac_buffer, CIRCULAR_BUFFER_SIZE, DAC_ALIGN_12B_R);
}
/**
  * @brief ：函数描述:
  * @param ：参数描述:
  * @return ：返回描述:1为正常解析包，2为缓冲区逸出
  * @refer ：说明:
  */
uint8_t receiving_state_handler(void) {
	uint8_t flag=0;
    if (NRF24L01_Receive() == 1) {
        flag=parse_nrf_packet_into_intermediate_buffer();
        no_data_count = 0;
    } else {
			flag=1;
        no_data_count++;
    }
			return flag;
		
}


void Voice_Device_RX_Init(void) {
    NRF24L01_Init();

    // 初始化 DAC circular buffer 为静音
    for (int i = 0; i < CIRCULAR_BUFFER_SIZE; i++) {
        dac_buffer[i] = 2048;
    }

    // 初始化中间缓冲区
    ib_write_index = 0;
    ib_read_index = 0;
    ib_count = 0;
    no_data_count = 0;
    // 启动 DAC 触发定时器
    HAL_TIM_Base_Start(&htim2);
HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (uint32_t*)dac_buffer, CIRCULAR_BUFFER_SIZE, DAC_ALIGN_12B_R);
    current_rx_state = STATE_RECEIVING;
    printf("接收端初始化完成。\r\n");
}