#ifndef __VOICE_RECEIVE_H
#define __VOICE_RECEIVE_H
#include "stm32f3xx_hal.h"
#include "Voice_Send.h"
// 缓冲区配置
#define BUFFER_SIZE      256     // 每个缓冲区大小
#define SAMPLES_PER_PACKET 16    // 每个NRF数据包包含的样本数
#define PACKETS_PER_BUFFER (BUFFER_SIZE / SAMPLES_PER_PACKET)  // = 16
#define PACKET_SIZE      32      // 16样本 × 2字节

// 全局状态变量
extern volatile device_state_t current_rx_state;

// 缓冲区就绪标志 (由ISR设置，主循环清除)
extern volatile uint8_t rx_buffer_full_flag;
extern volatile uint8_t current_dac_buffer; // 0 表示 dac_buffer1 就绪, 1 表示 dac_buffer2 就绪

// Circular DAC 缓冲区（2 × BUFFER_SIZE）
extern uint16_t dac_buffer[2* BUFFER_SIZE];

// 初始化函数
void Voice_Device_RX_Init(void);

// 状态处理函数




uint8_t parse_nrf_packet_into_intermediate_buffer(void);
uint8_t fill_dac_buffer_from_intermediate(uint16_t* target_buffer);
uint8_t receiving_state_handler(void);
void Voice_Device_RX_Init(void);

#endif


