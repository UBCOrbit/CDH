/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
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
#include "main.h"
#include "stm32f4xx_hal.h"
#include <stdlib.h>
#include <string.h>

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

//Definition for boolean variables used in conditional statements
#define TRUE 1
#define FALSE 0

//Standard definition for data buffer
#define PACKET_SIZE 256 // bytes
#define BYTE_SIZE 8 // number of bits in a byte

//Timeout used in Serial communication transmitting and receiving
#define timeOut 0x0FFF

//Structure declaring board settings, allows each board to keep track of other boards.
struct board {
	uint8_t data[PACKET_SIZE];
	char letter;
}STM_A, STM_B, STM_C;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))
void printStringToConsole(uint8_t message[]);
void clearArray(uint8_t *buf);
void print_buffer(uint8_t *buf, int size);
uint8_t receive_packet(uint8_t *buf);
//uint8_t mram_packet(uint8_t *pointer, uint8_t *mram_pointer);
uint8_t check_start_protocol(uint8_t *buf);
void get_address(uint8_t *buf, uint8_t *adr);
void check_flag(uint8_t *buf, uint8_t *flg);
void get_lengthCommand(uint8_t *buf, uint8_t *len_command);
void save_data (uint8_t *buf, uint8_t *data, uint8_t *length_command);
void save_command (uint8_t * buf, uint8_t *data, uint8_t *len_command);
uint8_t parse_packet(uint8_t *buf, uint8_t *adr, uint8_t *flg, uint8_t *len_command, uint8_t *data, int packet_legnth);
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

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
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();

  /* USER CODE BEGIN 2 */
  //Allocating 256 bytes of memory for received packets.
  uint8_t check;
  // COMMs_Sync Pointers
  uint8_t * address;
//  address = (uint8_t)1;
  uint8_t * c_d_flag;
//  c_d_flag = address + 1;
  uint8_t * length_command;
//  length_command = c_d_flag + 1;
  uint8_t * buffer;
//  buffer = length_command + 1;
  buffer = (uint8_t*) malloc (PACKET_SIZE);
  uint8_t * data;
//  data = 1000;
  data = (uint8_t*) malloc (PACKET_SIZE);
  // Start the program with the light turned off.
//  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, RESET);   /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  /* USER CODE END WHILE */
  /* USER CODE BEGIN 3 */
    check = receive_packet(buffer);
    // Turns on light if the receive packet function signals that it received a packet.
    if (check == 1) {
    	parse_packet(buffer, address, c_d_flag, length_command, data, PACKET_SIZE);
    	print_buffer(*address, 1);

    	break;
//      HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, SET); // Note: had to manually configure the LD2 pin in the configuration part of this file.
    }

   // Template for main functionality and example for how this code will function.


  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Configure the main internal regulator output voltage
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

    /**Initializes the CPU, AHB and APB busses clocks
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USART1 init function */
static void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* USART2 init function */
static void MX_USART2_UART_Init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA1 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA5 */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

// Description: Transmit a string over huart2. If solder bridges SB13 and SB14 are not removed,
//				this will transmit a message to the STLink chip and can be printed on a serial monitor
//				directly (such as the Arduino serial monitor). Otherwise, need to connect the huart2 pins to
// 				an Ardunio an receive the message from that end.
// Input: message to be transmitted
void print_string_to_console(char *message[]) {
	HAL_UART_Transmit(&huart2, (uint8_t*)message, strlen(message), timeOut);
}


// Description: This function writes null bytes to the buffer array passed to it
// Input: pointer to buffer that needs to be cleared
void clear_array(uint8_t *buf) {
	for (int i = 0; i < PACKET_SIZE; i++) {
		buf[i] = '\0';
	}
}

// Description: Uses the print_string_to_console function to print the contents
//        of the buffer to the serial monitor.
// Input: Pointer the to buffer to be printed and the length of the buffer
//      (number of char elements in the buffer)
void print_buffer(uint8_t *buf, int size) {
  char temp;
  for (uint8_t i = 0; i < size; i++) {
	temp = (char)*(buf + i);
    print_string_to_console(temp);
  }
}

// Description: Tries to receive a packet. If a packet is received, the function
//				saves the packet at the input pointer. Additionally, it returns 1 once
//        the packet has been saved. The function returns 0 if it is unable to
//				receive.
// Input: pointer to where the packet is to be saved
// Returns: Confirmation on successful reception of packet.
uint8_t receive_packet(uint8_t *buf) {
	// Creates temporary buffer for receiving the packet.
	uint8_t tempBuffer[PACKET_SIZE];
	clear_array(tempBuffer);
	// Saves the packet in the temporary buffer.
	if (HAL_UART_Receive(&huart1, tempBuffer, PACKET_SIZE, timeOut) == HAL_OK) {
		for (int i = 0; i < PACKET_SIZE; i += 1){
			*(buf + i) = tempBuffer[i];
		}
		return TRUE;
	}
	return FALSE;
}

//// Pulls a packet from the
//uint8_t mram_packet(uint8_t *pointer, uint8_t *mram_pointer) {
//
//}

// Description: Checks whether the packet follows the correct start bit
//        protocol.
// Input: Pointer to where the packet is stored.
uint8_t check_start_protocol(uint8_t *buf) {

	uint8_t holder[BYTE_SIZE];
  char buffer = *buf;
  for (int i = 7; 0 <= i; i --) {
    holder[7 - i] = ((buffer >> i) & 0x01);
  }
  if ((holder[0] == 0) && (holder[1] == 1) && (holder[2] == 1) &&
        (holder[3] == 0)) {
    return 1;
  }
  else {
    return 0;
  }
}

// Description: updates the address pointer with the value specified
// by the address field of the packet
//
// Input: pointer to where the packet is stored
void get_address(uint8_t *buf, uint8_t *adr) {
  uint8_t holder[BYTE_SIZE];
  uint8_t temp1 = *buf;
  for (int i = 7; 0 <= i; i --) {
    holder[7 - i] = ((temp1 >> i) & 0x01);
  }
  *adr = ((holder[4] * (4)) +
          (holder[5] * (2)) +
          (holder[6] * (1)));
}

// Description: updates the flag pointer with the value specified
// by the command/data flag field of the packet
//
// Input: pointer to where the packet is stored
void check_flag(uint8_t *buf, uint8_t *flg) {
  uint8_t holder[BYTE_SIZE];
  uint8_t temp1 = *buf;
  for (int i = 7; 0 <= i; i --) {
    holder[7 - i] = ((temp1 >> i) & 0x01);
  }
  *flg = holder[7];
}


// Description: updates the length_command pointer with the value specified
// by the length/command field of the packet
//
// Input: pointer to where the packet is stored
void get_lengthCommand(uint8_t *buf, uint8_t *len_command) {
  uint8_t holder[BYTE_SIZE];
  uint8_t temp = *(buf + 1);
  for (int i = 7; 0 <= i; i --) {
    holder[7 - i] = ((temp >> i) & 0x01);
  }
  *len_command = ((holder[0] * (128)) +
                  (holder[1] * (64)) +
                  (holder[2] * (32)) +
                  (holder[3] * (16)) +
                  (holder[4] * (8)) +
                  (holder[5] * (4)) +
                  (holder[6] * (2)) +
                  (holder[7] * (1)));
}

// Description: Saves what is in the payload field of the saved packet into the data
// pointer.
void save_data (uint8_t *buf, uint8_t *data, uint8_t *len_command) {
	for (int x = 0; x < *len_command; x++) {
		  *(data + x) = *(buf + 2 + x);
	  }
}

void save_command (uint8_t * buf, uint8_t *data, uint8_t *len_command) {
	
}


// Description: This function parses packets received from COMMs. Checks each of the overhead
// fields and updates each of the relevant pointers. Additionally, it updates the payload
// field.
//
// Input: pointer to where the packet is saved, and the length of the packet
// Output: returns 1 if the packets was properly parsed and returns 0 if there was an issue
uint8_t parse_packet(uint8_t *buf, uint8_t *adr, uint8_t *flg, uint8_t *len_command, uint8_t *data, int packet_legnth) {
	if (check_start_protocol(buf) == 0) {
		print_string_to_console("Incorrect start protocol.");
		return 0;
	}
	get_address(buf, adr);
	check_flag(buf, flg);
	get_lengthCommand(buf, len_command);
	if (*flg == 0) {
		save_command(buf, data, len_command);
	} else {
		save_data(buf, data, len_command);
	}
	return 1;
}




/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
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

/**
  * @}
  */

/**
  * @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
