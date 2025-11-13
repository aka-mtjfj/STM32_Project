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

#define INTERMEDIATE_BUFFER_SIZE (2*BUFFER_SIZE )
static uint16_t intermediate_audio_buffer[INTERMEDIATE_BUFFER_SIZE];

static volatile uint16_t A_ready = 0;
static volatile uint16_t B_ready = 0;
#define HALF_SIZE (INTERMEDIATE_BUFFER_SIZE / 2)

static uint8_t  silence_flag =0;
static volatile uint8_t writing_half = 0;      // 0 = 正在写 A 区, 1 = 正在写 B 区
static volatile uint16_t write_offset = 0;     // 当前半区内的写入偏移（0 ~ HALF_SIZE-1）
/**
  * @brief ：函数描述:
  * @param ：参数描述:
  * @return ：返回描述:1为正常解析包，2为缓冲区逸出
  * @refer ：说明:
  */
uint8_t parse_nrf_packet_into_intermediate_buffer(void) {
    // 检查是否两个缓冲区都满（无法写入）
    if (A_ready && B_ready) {
      
        return 2; // 缓冲区溢出
    }

    uint32_t base = writing_half * HALF_SIZE;

    // 检查是否会超出当前半区（防越界）
    if (write_offset + SAMPLES_PER_PACKET > HALF_SIZE) {
        // 理论上不应发生，除非包太大
        return 2;
    }

    // 写入当前半区
    for (int i = 0; i < SAMPLES_PER_PACKET; i++) {
        uint16_t sample = (((uint16_t)NRF24L01_RxPacket[i * 2]) << 8) |
                          ((uint16_t)NRF24L01_RxPacket[i * 2 + 1]);
        intermediate_audio_buffer[base + write_offset] = sample;
        write_offset++;
    }

    // 检查当前半区是否已满
    if (write_offset >= HALF_SIZE) {
        // 标记当前半区 ready
        if (writing_half == 0) {
            A_ready = 1;
        } else {
            B_ready = 1;
        }
        // 切换到另一个半区，并重置偏移
        writing_half = 1 - writing_half;
        write_offset = 0;
    }

   
    return 1;
}
uint8_t fill_dac_buffer_from_intermediate(uint16_t* target_buffer) {
		//__disable_irq(); // 关闭中断，保护共享变量
	    if (A_ready) {
        for (int i = 0; i < BUFFER_SIZE; i++) {
            target_buffer[i] = intermediate_audio_buffer[i];
        }
        A_ready = 0;
    }
    if (B_ready) {
        for (int i = 0; i < BUFFER_SIZE; i++) {
            target_buffer[i] = intermediate_audio_buffer[HALF_SIZE + i];
        }
        B_ready = 0;
    }
    return 1;
				//__enable_irq(); // 恢复中断,单字读写，不需要
        
    }
			
		//__enable_irq(); // 恢复中断,单字读写，不需要
    


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

void receiving_state_handler(void) {
	uint8_t flag=0;
	static uint16_t timeout=0;
    if (NRF24L01_Receive() == 1) {
			if(silence_flag==1)
			{
				silence_flag=0;
				HAL_DAC_Start(&hdac,DAC_CHANNEL_1);
			}
        flag=parse_nrf_packet_into_intermediate_buffer();
        
    } else {
			flag=1;
        timeout++;
		if(timeout>=5000)
		{
						silence_flag=1;//静音标志置一
			HAL_DAC_Stop(&hdac,DAC_CHANNEL_1);
			timeout=0;
		}
			
	}

    }
			


void Voice_Device_RX_Init(void) {
    NRF24L01_Init();

    // 初始化 DAC circular buffer 为静音
    for (int i = 0; i < CIRCULAR_BUFFER_SIZE; i++) {
        dac_buffer[i] = 2048;
    }

    // 初始化中间缓冲区

    // 启动 DAC 触发定时器
    HAL_TIM_Base_Start(&htim2);
HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (uint32_t*)dac_buffer, CIRCULAR_BUFFER_SIZE, DAC_ALIGN_12B_R);
    current_rx_state = STATE_RECEIVING;
    printf("接收端初始化完成。\r\n");
}