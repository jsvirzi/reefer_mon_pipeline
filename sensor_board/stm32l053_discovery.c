/**
  ******************************************************************************
  * File Name          : main.c
  * Date               : 26/12/2014 23:08:43
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2014 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal.h"
#include "i2c.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define JSV_I2C_ISR_BUSY 0x00008000
#define JSV_I2C_ISR_TC 0x00000040
#define JSV_I2C_ISR_TXE 0x00000001
#define JSV_I2C_ISR_TXIS 0x00000002
#define JSV_I2C_ISR_RNE 0x00000004
#define JSV_I2C_CR2_NACK 0x00008000
#define JSV_I2C_CR2_STOP 0x00004000
#define JSV_I2C_CR2_START 0x00002000
#define JSV_I2C_CR2_NBYTES_OFFSET 16
#define JSV_I2C_CR2_NBYTES_MASK 0x000F0000
#define JSV_I2C_CR2_RELOAD 0x01000000
#define JSV_I2C_CR2_AUTOEND 0x02000000
#define JSV_I2C_CR1_PE 0x00000001
#define JSV_I2C_CR1_ANFOFF 0x00001000
#define JSV_I2C_CR1_DNF_OFFSET 8
#define JSV_I2C_CR1_DNF_MASK 0x00000F00
#define JSV_I2C_CR2_SADD_MASK 0x000003FF
/* the following offset is +1 because we are in 7-bit mode */
#define JSV_I2C_CR2_SADD_OFFSET 1
#define JSV_I2C_CR2_RDWRN 0x00000400
#define JSV_I2C_ICR_NACKCF 0x00000010
#define JSV_I2C_ICR_STOPCF 0x00000020

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

#define UART_TXE 0x80
#define UART_RXNE 0x20

void uart_send_byte(char byte) {
		while((huart1.Instance->ISR & UART_TXE) == 0);
		huart1.Instance->TDR = byte;
}

void uart_send_string(char *s) {
	int len = strlen(s);
	char ch;
	while(len--) {
		ch = *s++;
		if(ch == '\n') ch = 13;
		while((huart1.Instance->ISR & UART_TXE) == 0);
		huart1.Instance->TDR = ch;
	}
}

void delay_ms(int delay) {
	int start = HAL_GetTick();
	int stop = start + delay;
	while(HAL_GetTick() < stop);
}

int read_i2c_register(I2C_HandleTypeDef *hi2c, int addr, int reg, int nbytes, unsigned char *buff) {
	
	int idx;
	volatile long unsigned int status;

	hi2c->Instance->ISR = JSV_I2C_ISR_TXE | JSV_I2C_ISR_TXIS; /* clear TX flags if they are there */
	hi2c->Instance->ICR = JSV_I2C_ICR_NACKCF | JSV_I2C_ICR_STOPCF;
	
	status = hi2c->Instance->CR2;
	status &= ~JSV_I2C_CR2_AUTOEND; /* AUTOEND = 0 */
	status &= ~JSV_I2C_CR2_RELOAD; /* RELOAD = 0 */
	status &= ~JSV_I2C_CR2_NBYTES_MASK;
	status |= (1 << JSV_I2C_CR2_NBYTES_OFFSET); /* single byte, namely reg address */
	status &= ~JSV_I2C_CR2_RDWRN; /* write */
	status &= ~JSV_I2C_CR2_SADD_MASK;
	status |= (addr << JSV_I2C_CR2_SADD_OFFSET);
	status |= JSV_I2C_CR2_START;
	hi2c->Instance->CR2 = status;
	
	while((hi2c->Instance->ISR & JSV_I2C_ISR_TXE) == 0);
	hi2c->Instance->TXDR = reg;

	delay_ms(2);
	
	// status = hi2c->Instance->CR2;
	status |= JSV_I2C_CR2_AUTOEND; /* AUTOEND = 1 */
	status &= ~JSV_I2C_CR2_RELOAD; /* RELOAD = 0 */
	status &= ~JSV_I2C_CR2_NBYTES_MASK;
	status |= (nbytes << JSV_I2C_CR2_NBYTES_OFFSET);
	status |= JSV_I2C_CR2_RDWRN; /* read */
	status |= JSV_I2C_CR2_START; /* restart */
	hi2c->Instance->CR2 = status;
	
	for(idx=0;idx<nbytes;++idx) {
		while((hi2c->Instance->ISR & JSV_I2C_ISR_RNE) == 0);
		buff[idx] = hi2c->Instance->RXDR;
	}

	return 0;
}

int write_i2c_register(I2C_HandleTypeDef *hi2c, int addr, int reg, int nbytes, unsigned char *buff) {
	
	volatile long unsigned int status;

	hi2c->Instance->ISR = JSV_I2C_ISR_TXE | JSV_I2C_ISR_TXIS; /* clear TX flags if they are there */
	hi2c->Instance->ICR = JSV_I2C_ICR_NACKCF | JSV_I2C_ICR_STOPCF;
	
	status = hi2c->Instance->CR2;
	status |= JSV_I2C_CR2_AUTOEND; /* AUTOEND = 0 */
	status &= ~JSV_I2C_CR2_RELOAD; /* RELOAD = 0 */
	status &= ~JSV_I2C_CR2_NBYTES_MASK;
	status |= ((1 + nbytes) << JSV_I2C_CR2_NBYTES_OFFSET); /* total bytes to write. 1 for register address */
	status &= ~JSV_I2C_CR2_RDWRN; /* write */
	status &= ~JSV_I2C_CR2_SADD_MASK;
	status |= (addr << JSV_I2C_CR2_SADD_OFFSET);
	status |= JSV_I2C_CR2_START;
	hi2c->Instance->CR2 = status;
	
	while((hi2c->Instance->ISR & JSV_I2C_ISR_TXE) == 0);
	hi2c->Instance->TXDR = reg;
	while(nbytes--) {
		while((hi2c->Instance->ISR & JSV_I2C_ISR_TXE) == 0);
		hi2c->Instance->TXDR = *buff++;
	}
	
	return 0;
}

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */

	unsigned char buffer[256];
	char gp_str[256];
	
  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_LPUART1_UART_Init();
  MX_USART2_UART_Init();
//  MX_USB_DEVICE_Init();

  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* USER CODE BEGIN 3 */
  /* Infinite loop */
	
#if 0
/* proximity sensor initialize */

	int proximity_sensor_addr = 0x20;
	buffer[0] = 0xfe;
	write_i2c_register(proximity_sensor_addr, 0x3, 1, buffer);
	buffer[0] = 1;
	write_i2c_register(proximity_sensor_addr, 0x1, 1, buffer);
	buffer[0] = 0xaa;
	read_i2c_register(proximity_sensor_addr, 0x1, 1, buffer);
#endif

#if 0
	int accel_sensor_addr = 0x1d;
	read_i2c_register(accel_sensor_addr, 0, 2, buffer);
#endif

//	I2C_HandleTypeDef *hi2c;
	I2C_HandleTypeDef *hi2c_parallax_gyro = &hi2c1;
	I2C_HandleTypeDef *hi2c_parallax_compass = &hi2c1;
	
	buffer[0] = 0;
	int parallax_gyro_sensor_addr = 0x68;
	read_i2c_register(hi2c_parallax_gyro, parallax_gyro_sensor_addr, 0xf, 1, buffer);
	
	uart_send_byte(0x11);
		
/* continuous mode */	
	int parallax_compass_sensor_addr = 0x1e;
	buffer[0] = 0;
	write_i2c_register(hi2c_parallax_compass, parallax_compass_sensor_addr, 2, 1, buffer);
	
	int reg, nbytes, start = HAL_GetTick(), gyro[3];
	int stop = start + 2000;
  while (1)
  {
//		sprintf(buffer, "ticks = %d\n\r", stop);
//		uart_send_string(buffer);
		stop += 1000;
		while(HAL_GetTick() < stop);
		
		uart_send_byte(12); /* clear screen */

#if 0
#if 0
	/* this does not work. jsv. it should work */	
		reg = 0;
		nbytes = 13;
		read_i2c_register(parallax_compass_sensor_addr, reg, nbytes, buffer);
#else		
	/* this does work */
		for(reg=0;reg<13;++reg) read_i2c_register(parallax_compass_sensor_addr, reg, 1, &buffer[reg]);
#endif

		sprintf(gp_str, "1: %2.2x %2.2x %2.2x\n", buffer[0x4], buffer[0x6], buffer[0x8]);
		uart_send_string(gp_str);
		
		sprintf(gp_str, "2: %2.2x %2.2x %2.2x %2.2x", buffer[0x2], buffer[0xa], buffer[0xb], buffer[0xc]);
		uart_send_string(gp_str);
#endif

		buffer[0] = 0;
		reg = 9; /* status register on the parallax compass sensor */
		while((buffer[0]&1) == 0) {
			read_i2c_register(hi2c_parallax_compass, parallax_compass_sensor_addr, reg, 1, buffer);
		}
		
#if 0
		reg = 3;
		nbytes = 6;
		read_i2c_register(parallax_compass_sensor_addr, reg, nbytes, buffer);
#else
		int idx = 0;
		for(reg=3;reg<9;++reg,++idx) 
			read_i2c_register(hi2c_parallax_compass, parallax_compass_sensor_addr, reg, 1, &buffer[idx]);
#endif		

/* format of data is X, Z, Y. We want X, Y, Z */
		gyro[0] = (buffer[0] << 8) | buffer[1];
		gyro[1] = (buffer[4] << 8) | buffer[5];
		gyro[2] = (buffer[2] << 8) | buffer[3];
		
#if 0		
		read_i2c_register(proximity_sensor_addr, reg, 1, buffer);
		int proximity = buffer[0] & 2;
#endif

		sprintf(gp_str, "X=%5d Y=%5d\nZ=%5d", gyro[0], gyro[1], gyro[2]);		
		uart_send_string(gp_str);
		
  }
	
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

  __PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSI48;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_6;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_3;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1
                              |RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_LPUART1
                              |RCC_PERIPHCLK_I2C1|RCC_PERIPHCLK_USB;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_HSI;
  PeriphClkInit.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_HSI;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

  __SYSCFG_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
