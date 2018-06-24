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
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* USER CODE BEGIN Includes */
//Definition for boolean variables used in conditional statements
#define TRUE 1
#define FALSE 0

//Standard definition for data buffer
#define BUFFER_SIZE 64

//Timeout used in Serial communication transmitting and receiving
#define timeOut 0x0FFF

//Structure declaring board settings, allows each board to keep track of other boards.
struct board {
	uint8_t data[BUFFER_SIZE];
	char letter;
}STM_A, STM_B, STM_C;
/* USER CODE END PV */
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart6;

int reset = 1;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_SPI2_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART6_UART_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
void STM_BOARD_Init(void);
void printStringToConsole(char message[]);
void processData(uint8_t tempBuffer[], int baseIndex, int numBytes);
void copyData(uint8_t tempBuffer[], int baseIndex, int numBytes);
void compareData(int baseIndex, int numBytes);
void clearArray(uint8_t *buffer);
void get_reInit();
void reInit_someone();	
/* USER CODE END PFP */

int main(void)
{

  /* USER CODE BEGIN 1 */
  

  if (reset == 1){
    get_reInit;
  }
  else{
    reInit_someone;
  }

  Start:
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
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART6_UART_Init();

  /* USER CODE BEGIN 2 */
	char *testString = "Hello!\n";
	for(int i = 0; i < strlen(testString); i++){
		STM_A.data[i] = testString[i];
	}
	printStringToConsole("STM A Initialized!\n");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
  /* USER CODE END WHILE */
		compareData(0, strlen(testString));
  /* USER CODE BEGIN 3 */

	}
  /* USER CODE END 3 */

}

/* USER CODE BEGIN 4 */
void STM_BOARD_Init(void){
	//Declare letter to identify boards for output messages.
	STM_A.letter = 'A';
	STM_B.letter = 'B';
	STM_C.letter = 'C';
	//Clear data buffers.
	clearArray(STM_A.data);
	clearArray(STM_B.data);
	clearArray(STM_C.data);
}

// Description: Transmit a string over huart2. If solder bridges SB13 and SB14 are not removed,
//				this will transmit a message to the STLink chip and can be printed on a serial monitor
//				directly (such as the Arduino serial monitor). Otherwise, need to connect the huart2 pins to
// 				an Ardunio an receive the message from that end.
// Input: message to be transmitted
void printStringToConsole(char message[]) {
	HAL_UART_Transmit(&huart1,(uint8_t*)message, strlen(message),timeOut);
}

// Description: Query STM_B for a portion of data. Compare received STM_B data with STM_A's own data.
//				Send result of comparison over to C for reset if needed.
// Input: base index (starting index) of the data in memory, number of bytes to be compared
// Side-Effects: Request data from B. Receive and store B's data. Send result to C. Get power cycled.
// Assumptions: A, B, and C have the exact same data[] array
void compareData(int baseIndex, int numBytes){
	printStringToConsole("A: Comparison begun.\n");

	// Generate query request for STMB data ---------------------------------------------
	char addressString[8] = "";
	itoa(baseIndex, addressString, 10);

	char sizeString[24] = "";
	itoa(numBytes, sizeString, 10);

	char fullString[32] = "";
	strcpy(fullString, addressString);
	strcat(fullString, sizeString);

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);

	// Send query to B ------------------------------------------------------------------
	HAL_Delay(500);
	printStringToConsole(fullString);

	if (HAL_SPI_Transmit(&hspi2, (uint8_t*)fullString, strlen(fullString), timeOut) == HAL_OK)
		printStringToConsole("A: Sent Request\n");

	// Wait for data from B and store data in temporary buffer when it comes in ---------
	int received = 0;
	uint8_t tempBuffer[BUFFER_SIZE];
	clearArray(tempBuffer);

	while(received == 0){
		printStringToConsole("Waiting..");
		if(HAL_SPI_Receive(&hspi1, tempBuffer, numBytes, timeOut) == HAL_OK)
			received = 1;
	}

	printStringToConsole("A: Received B data\n");
	processData(tempBuffer, baseIndex, numBytes);

	// Compare received B data with A data -----------------------------------------------
	int i = 0;
	int comp_result = 1;
	for(i=0; i<numBytes; i++){
		if(STM_A.data[baseIndex + i] != STM_B.data[baseIndex + i])
			comp_result = 0;
	}

	// General result message for C -------------------------------------------------------
	char result[2];
	itoa(comp_result, result, 10);
	result[1] = '\0';
	char *preMsg = "A: result found is ";
	printStringToConsole(preMsg);
	printStringToConsole(result);

	// Send result string to C ------------------------------------------------------------
	HAL_SPI_Transmit(&hspi2, (uint8_t*)result, strlen(result), timeOut);
}

// Description: Take received STM_B data from temporary buffer and store in the data array for STM_B on this board.
// Input: temporary buffer, base index (starting index) of the data in memory, number of bytes to be compared
void processData(uint8_t tempBuffer[], int baseIndex, int numBytes) {
	int stmCount = 0;

	// Copy temporary buffer to STM_B.data ------------------------------------------------
	while (stmCount <= numBytes + 1) {
		STM_B.data[baseIndex + stmCount] = tempBuffer[stmCount];
		stmCount++;
	}

	// Append null char -------------------------------------------------------------------
	STM_B.data[baseIndex + stmCount] = '\0';
	printStringToConsole("A: Finished comparison.\n");
}

void copyData(uint8_t tempBuffer[], int baseIndex, int numBytes) {
	int stmCount = 0;

	// Copy temporary buffer to STM_B.data ------------------------------------------------
	while (stmCount <= numBytes + 1) {
		STM_A.data[baseIndex + stmCount] = tempBuffer[stmCount];
		stmCount++;
	}

	// Append null char -------------------------------------------------------------------
	STM_A.data[baseIndex + stmCount] = '\0';
}

//Description: This function writes null bytes to the buffer array passed to it
//Input: pointer to buffer that needs to be cleared
void clearArray(uint8_t *buffer){
	for (int i = 0; i < BUFFER_SIZE; i++) {
		buffer[i] = '\0';
	}
}

//Description: This function restarts the current STM
void get_reInit(){
  int received = 0;
	uint8_t tempBuffer[BUFFER_SIZE];
	clearArray(tempBuffer);

	while(received == 0){
		printStringToConsole("Waiting..");
		if(HAL_SPI_Receive(&hspi6, tempBuffer, numBytes, timeOut) == HAL_OK)
			received = 1;
	}

	printStringToConsole("A: Received C data\n");
	copyData(tempBuffer, baseIndex, numBytes);

  goto Start;
}

//Description: This function resends all current variable values to STM's that were reset
void reInit_someone(){
  int reqBytes = 2;
	int baseIndex, numBytes;

	char baseIndex_s[2];
	char numBytes_s[2];

	uint8_t tempBuffer[BUFFER_SIZE];
	clearArray(tempBuffer);

	// Parse data request string --------------------------------------
	baseIndex_s[0] = tempBuffer[0];
	baseIndex_s[1] = '\0';

	// Store numBytes -------------------------------------------------
	numBytes_s[0] = tempBuffer[1];
	numBytes_s[1] = '\0';

	// Convert from bytes to int --------------------------------------
	baseIndex = atoi(baseIndex_s);
	numBytes = atoi(numBytes_s);

	// Generate data array --------------------------------------------
	uint8_t reqData[numBytes];
	clearArray(reqData);

	// Transfer data from internal storage to msg buffer --------------
	for(int count = 0; count < numBytes; count++){
	  reqData[count] = STM_A.data[baseIndex+count];
	}

	// Send data to STM_A and STM_B -------------------------------------------------
	HAL_Delay(500);
	printStringToConsole("\n A:Beginning transmission\n");

	HAL_SPI_Transmit(&hspi6, (uint8_t*)reqData, strlen(reqData), timeOut);
}
/* USER CODE END 4 */

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

/* SPI1 init function */
static void MX_SPI1_Init(void)
{

  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* SPI2 init function */
static void MX_SPI2_Init(void)
{

  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

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

/* USART6 init function */
static void MX_USART6_UART_Init(void)
{

  huart6.Instance = USART6;
  huart6.Init.BaudRate = 115200;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/** Pinout Configuration
*/
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */

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
