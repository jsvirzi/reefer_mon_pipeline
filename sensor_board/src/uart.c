#include "stm32l0xx_hal.h"

UART_HandleTypeDef huart1;
static void MX_USART1_UART_Init(void);

#define UART_TXE 0x80
#define UART_RXNE 0x20

char uart_lut[0x10] = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

int uart_init() {
  MX_USART1_UART_Init();
}

int uart_send_string(char *s) {
	int len = strlen(s);
	char ch;
	while(len--) {
		ch = *s++;
		while((huart1.Instance->ISR & UART_TXE) == 0);
		huart1.Instance->TDR = ch;
	}
	return 0;
}

int uart_send(int val) {
	char ch;
	ch = (val >> 28) & 0xf;
	while((huart1.Instance->ISR & UART_TXE) == 0);
	huart1.Instance->TDR = uart_lut[ch];
	ch = (val >> 24) & 0xf;
	while((huart1.Instance->ISR & UART_TXE) == 0);
	huart1.Instance->TDR = uart_lut[ch];
	ch = (val >> 20) & 0xf;
	while((huart1.Instance->ISR & UART_TXE) == 0);
	huart1.Instance->TDR = uart_lut[ch];
	ch = (val >> 16) & 0xf;
	while((huart1.Instance->ISR & UART_TXE) == 0);
	huart1.Instance->TDR = uart_lut[ch];
	ch = (val >> 12) & 0xf;
	while((huart1.Instance->ISR & UART_TXE) == 0);
	huart1.Instance->TDR = uart_lut[ch];
	ch = (val >>  8) & 0xf;
	while((huart1.Instance->ISR & UART_TXE) == 0);
	huart1.Instance->TDR = uart_lut[ch];
	ch = (val >>  4) & 0xf;
	while((huart1.Instance->ISR & UART_TXE) == 0);
	huart1.Instance->TDR = uart_lut[ch];
	ch = (val >>  0) & 0xf;
	while((huart1.Instance->ISR & UART_TXE) == 0);
	huart1.Instance->TDR = uart_lut[ch];
	while((huart1.Instance->ISR & UART_TXE) == 0);
	huart1.Instance->TDR = 0x0d;
	while((huart1.Instance->ISR & UART_TXE) == 0);
	huart1.Instance->TDR = 0x0a;
}

void MX_USART1_UART_Init(void) { /* USART1 init function */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONEBIT_SAMPLING_DISABLED;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  HAL_UART_Init(&huart1);
}

