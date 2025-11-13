#include "stm32f3xx.h"
#ifndef Voice_Send_h
#define Voice_Send_h
// 缓冲区配置
#define BUFFER_SIZE      256     // 每个缓冲区大小,样本
#define SAMPLES_PER_PACKET 16    // 每个NRF数据包包含的样本数
#define PACKETS_PER_BUFFER (BUFFER_SIZE / SAMPLES_PER_PACKET)  // =16
#define PACKET_SIZE      32      // 16样本 × 2字节

// 设备状态
typedef enum {
    STATE_IDLE = 0,
    STATE_TRANSMITTING,
    STATE_RECEIVING
} device_state_t;

extern volatile device_state_t current_state;
extern volatile uint16_t arr1[BUFFER_SIZE], arr2[BUFFER_SIZE];  // ADC DMA双缓冲          
extern volatile uint8_t buffer_ready;
extern volatile uint16_t sequence_number;

void prepare_nrf_packet(void);
void Voice_Device_Tx_Init();
void transmitting_state_handler(void);
#endif