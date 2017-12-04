/**
******************************************************************************
* @file    Audio/Audio_playback_and_record/Src/main.c
* @author  MCD Application Team
* @version V1.1.0
* @date    01-July-2015
* @brief   Audio playback and record main file.
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
*
* Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
* You may not use this file except in compliance with the License.
* You may obtain a copy of the License at:
*
*        http://www.st.com/software_license_agreement_liberty_v2
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private defines -----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

static void *PCM1774_X_0_handle = NULL;
USBD_HandleTypeDef hUSBDDevice;
volatile int16_t temp_buffer[48 * 2];
uint16_t PCM_Buffer[AUDIO_CHANNELS * AUDIO_SAMPLING_FREQUENCY / 1000];
uint16_t PDM_Buffer[AUDIO_CHANNELS * AUDIO_SAMPLING_FREQUENCY / 1000 * 64 / 8];
int16_t Audio_OUT_Buff[2 * AUDIO_SAMPLING_FREQUENCY / 1000 * 4];

extern USBD_AUDIO_ItfTypeDef  USBD_AUDIO_fops;
extern uint16_t PDM_Buffer[];

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void SystemClock_Config(void);
void User_Process(void);

/* Private function prototypes -----------------------------------------------*/


int main(void)
{
   /* STM32F4xx HAL library initialization:
  - Configure the Flash prefetch, instruction and Data caches
  - Configure the Systick to generate an interrupt each 1 msec
  - Set NVIC Group Priority to 4
  - Global MSP (MCU Support Package) initialization
  */
  HAL_Init();
  
  /* Configure the system clock to 168 MHz */
  SystemClock_Config();
  
  /* Initialize LED */
  BSP_LED_Init(LED1);
  
  /* Initialize USB descriptor basing on channels number and sampling frequency */
  USBD_AUDIO_Init_Microphone_Descriptor(&hUSBDDevice, AUDIO_SAMPLING_FREQUENCY, AUDIO_CHANNELS);
  /* Init Device Library */
  USBD_Init(&hUSBDDevice, &AUDIO_Desc, 0);
  /* Add Supported Class */
  USBD_RegisterClass(&hUSBDDevice, &USBD_AUDIO);
  /* Add Interface callbacks for AUDIO Class */  
  USBD_AUDIO_RegisterInterface(&hUSBDDevice, &USBD_AUDIO_fops);
  /* Start Device Process */
  USBD_Start(&hUSBDDevice);
  
  BSP_AUDIO_IN_Init(AUDIO_SAMPLING_FREQUENCY, 16, AUDIO_CHANNELS);
  
  BSP_AUDIO_IN_Record(PDM_Buffer, 0);
  BSP_AUDIO_OUT_Init(PCM1774_0, &PCM1774_X_0_handle, NULL, 20, AUDIO_SAMPLING_FREQUENCY);
  BSP_AUDIO_OUT_SetVolume(PCM1774_X_0_handle, 40);

  while (1)
  {
    BSP_LED_Off(LED1);
    HAL_Delay(950);
    BSP_LED_On(LED1);
    HAL_Delay(50);
  }
  
}

/**
* @brief  AudioProcess function. Called during Audio acquisition interrupts.
* @param  None
* @retval None
*/
void AudioProcess(void)
{
  uint16_t index = 0;
  static uint16_t OUT_Buff_lvl = 0;
  
  BSP_AUDIO_IN_PDMToPCM((uint16_t * )PDM_Buffer, PCM_Buffer);  
  
  for (index = 0; index < AUDIO_SAMPLING_FREQUENCY/1000 ; index++)
  {
    Audio_OUT_Buff[OUT_Buff_lvl + 1] = PCM_Buffer[index * AUDIO_CHANNELS + 0];
    Audio_OUT_Buff[OUT_Buff_lvl + 0] = PCM_Buffer[index * AUDIO_CHANNELS + 1];
    OUT_Buff_lvl = (OUT_Buff_lvl + 2)%(sizeof(Audio_OUT_Buff)/sizeof(Audio_OUT_Buff[0]));
  }
  
  if (OUT_Buff_lvl >= (sizeof(Audio_OUT_Buff)/sizeof(Audio_OUT_Buff[0]))/2)
  {
    BSP_AUDIO_OUT_Play(PCM1774_X_0_handle, (uint16_t*)Audio_OUT_Buff, sizeof(Audio_OUT_Buff)/sizeof(Audio_OUT_Buff[0]));      
  }
  
  Send_Audio_to_USB((int16_t *)PCM_Buffer, AUDIO_SAMPLING_FREQUENCY / 1000 * AUDIO_CHANNELS); 
}


/**
* @brief  Half Transfer user callback, called by BSP functions.
* @param  None
* @retval None
*/
void BSP_AUDIO_IN_HalfTransfer_CallBack(void)
{
  AudioProcess();
}

/**
* @brief  Transfer Complete user callback, called by BSP functions.
* @param  None
* @retval None
*/
void BSP_AUDIO_IN_TransferComplete_CallBack(void)
{
  AudioProcess();
}


/**
* @brief  System Clock Configuration
* @param  None
* @retval None
*/
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;
  
  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();
  
  /* The voltage scaling allows optimizing the power consumption when the device is
  clocked below the maximum system frequency, to update the voltage scaling value
  regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  RCC_OscInitStruct.PLL.PLLR = 2;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);
  
  /*Select Main PLL output as USB clock source */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_CK48;
  PeriphClkInitStruct.Clk48ClockSelection = RCC_CK48CLKSOURCE_PLLQ;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
  clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
}

#ifdef  USE_FULL_ASSERT
/**
* @brief  Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param  file: pointer to the source file name
* @param  line: assert_param error line source number
* @retval None
*/
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  
  /* Infinite loop */
  while (1)
  {
  }
}
#endif

void Error_Handler(void)
{
//  PRINTF("Error_Handler\n");
  /* Blink LED2 */
  while(1)
  {
    BSP_LED_Toggle(LED2);
    HAL_Delay(100);
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
