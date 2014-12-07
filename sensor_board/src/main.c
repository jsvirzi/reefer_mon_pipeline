#include "stm32l0xx_hal.h"
#include "i2c.h"
#include "temperature_humidity.h"

#include <stdio.h>

/* declared globally so that the debugger can see them.
  in production, they do not have to be global */
int status, byte;

/* private function prototypes */
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);

/* for use anywhere */
char gp_str[1024];

int main(void) {

  HAL_Init(); /* reset all peripherals, init flash interface and systick. */

  SystemClock_Config(); /* configure system clock */

  /* initialize all configured peripherals */
  MX_GPIO_Init(); /* i2c is being bit-banged */
	uart_init();
	
	while(1) {
		int temperature, humidity;
		read_temperature_and_humidity(&temperature, &humidity);
//		uart_send(temperature);
//		uart_send(humidity);
		float t, h;
		t = 175.72 * temperature / 65536.0 - 46.85;
		h = 125.0 * humidity / 65536.0 - 6.0;
		sprintf(gp_str, "TEMP=%.2f. HUMIDITY=%.2f\n", t, h);
		uart_send_string(gp_str);
	}
	
	read_temperature_user_register();
//	read_temperature_sensor_firmware();
//	read_temperature_sensor_serial_number();

}

void SystemClock_Config(void) { /* system clock configuration */

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

  __PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_5;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

  __SYSCFG_CLK_ENABLE();

}

void MX_GPIO_Init(void) { /* gpio configuration */
	GPIO_InitTypeDef GPIO_Init;
	
/* GPIO Ports Clock Enable */
  __GPIOA_CLK_ENABLE();
  __GPIOB_CLK_ENABLE();

	GPIO_Init.Pin = GPIO_PIN_6;
	GPIO_Init.Speed = GPIO_SPEED_HIGH;
	GPIO_Init.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_Init.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_Init);
	
	GPIO_Init.Pin = GPIO_PIN_7;
	GPIO_Init.Speed = GPIO_SPEED_HIGH;
	GPIO_Init.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_Init.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_Init);
	
	status = GPIOB->IDR;
}

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
