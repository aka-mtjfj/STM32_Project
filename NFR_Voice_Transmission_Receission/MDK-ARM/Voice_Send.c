#include "stm32f3xx.h"
#include "Voice_Send.h"
#include "NRF24L01.h"  // NRF驱动头文件
#include "stdio.h"
volatile device_state_t current_state = STATE_IDLE;
volatile uint16_t arr1[BUFFER_SIZE], arr2[BUFFER_SIZE];  // DMA双缓冲
volatile uint8_t buffer_ready = 0;

volatile uint16_t* current_dma_buffer = arr1;  // 当前满的DMA缓冲区：0=arr1, 1=arr2
volatile uint16_t* ready_buffer = NULL;         // 已填满、待处理的缓冲区

volatile uint16_t sample_read_index = 0;  // 当前读取位置

volatile uint8_t packets_remaining = 0;  // 当前缓冲区剩余要发送的包数
extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim2;
extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_adc1; 

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

void Voice_Device_Tx_Init()
{
	NRF24L01_Init();
	    // 填充初始静音值
    for(int i = 0; i < BUFFER_SIZE; i++)
	{
        arr1[i] = 2048;
        arr2[i] = 2048;
    }
    
    // 启动第一次 DMA 传输（播放 buffer1）
		HAL_ADC_Start_DMA(&hadc1, (uint32_t*)arr1, BUFFER_SIZE);
		HAL_TIM_Base_Start(&htim2);
	
		
		
		printf("发送端初始化完成。\r\n");
		
}

 

/**
 * @brief ADC DMA传输完成中断回调函数  
 * @note 当DMA填充完整个缓冲区时自动调用
 *       对于双缓冲，这表示arr2已满，可以开始处理arr2的数据
 *HAL_ADC_ConvCpltCallback
*/

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    // 只有在发送模式下才处理ADC数据
	    if (hadc->Instance != ADC1) return; // 安全检查
			 //__disable_irq();
        // 设置当前可用的缓冲区
			ready_buffer=current_dma_buffer;
            if (current_dma_buffer == arr1) {
        current_dma_buffer = arr2;
    } else {
        current_dma_buffer = arr1;
    }
						packets_remaining = PACKETS_PER_BUFFER;  // 需要发送16个包
        // 设置缓冲区就绪标志，通知主循环可以发送数据
        buffer_ready = 1;
        // 重置读取索引，从arr2的开头开始读取  
        sample_read_index = 0;
		//__enable_irq();
		//__HAL_TIM_DISABLE(&htim2);          // 暂停触发
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)current_dma_buffer, BUFFER_SIZE);
		//__HAL_TIM_ENABLE(&htim2);           // 恢复触发

}

// 直接从DMA缓冲区读取数据并打包到NRF发送缓冲区
void prepare_nrf_packet(void) {
    volatile uint16_t* source_buffer = ready_buffer;//确定现在可发的是哪个数组
    static uint32_t overflow_count = 0;
		static uint16_t packet_cnt=0;
    // 音频数据（16位样本拆分为2个字节）
	    if (ready_buffer == NULL) {
        // 异常情况，填充静音
        for (int i = 0; i < SAMPLES_PER_PACKET * 2; i++) {
            NRF24L01_TxPacket[i] = (i % 2 == 0) ? 0x08 : 0x00;
        }
        return;
    }
    for(int i = 0; i < SAMPLES_PER_PACKET; i++)
	{
        uint16_t sample;
        
        // 从当前DMA缓冲区读取样本
        if(sample_read_index < BUFFER_SIZE) {
            sample = source_buffer[sample_read_index++];
        } else {
					overflow_count++;
            // 缓冲区越界保护，使用静音
					if ((overflow_count % 100) == 0) { // 每累积100次溢出才打印一次
						//printf("警告: 缓冲区逸出出现 %d 次。\n", overflow_count);
					}
            sample = 2048;
        }
        // 16位样本拆分为2个字节存入发送缓冲区
        NRF24L01_TxPacket[i*2] = (sample >> 8) & 0xFF;     // 高字节
        NRF24L01_TxPacket[i*2 + 1] = sample & 0xFF;        // 低字节
    }
	packet_cnt++;
		if(packet_cnt%4000==0)
		{
			packet_cnt=0;
			//printf("累计4000个数据包已准备好。\n");
		}
}

// 全局或静态变量，用于跟踪发送状态
volatile static uint8_t is_packet_prepared = 0; 
volatile static uint8_t buffer_sent_cnt=0;

void transmitting_state_handler(void) 
{
   volatile uint8_t sendflag;
		
    // 如果没有准备好发送的数据包，并且有数据需要发送
    if (!is_packet_prepared && buffer_ready && packets_remaining > 0) {
        prepare_nrf_packet(); // 准备下一个数据包
        is_packet_prepared = 1; // 标记已准备好
    }
    // 如果数据包已准备好，尝试发送
    if (is_packet_prepared) {
        sendflag = NRF24L01_Send();
        if(sendflag == 1) 
					{
            packets_remaining--;  // 减少剩余包数
            is_packet_prepared = 0; // 标记发送完成，可以准备下一个包
					buffer_sent_cnt++;//缓冲区发送个数
            // 如果当前缓冲区的所有包都发送完了
            if(packets_remaining == 0) {
                buffer_ready = 0;  // 停止发送，等待下一个缓冲区
							
							if(buffer_sent_cnt%100==0)
							{
								//printf("%d个缓冲区所有数据包发送完成\n",buffer_sent_cnt);
								buffer_sent_cnt=0;
							}
            }
        } 
				else {
            // 发送失败处理
            printf("发送数据包出错，错误码是%d\n", sendflag);
            is_packet_prepared = 0; // 丢弃当前失败的包
        }
    }
}
	
	void prepare_silence_packet(void) {
    // 静音数据（2048 = 0x0800）
    for(int i = 0; i < SAMPLES_PER_PACKET; i++) {
        NRF24L01_TxPacket[i*2] = 0x08;     // 高字节
        NRF24L01_TxPacket[i*2 + 1] = 0x00; // 低字节
    }
		printf("静音数据包已准备好。\n");
}
	
