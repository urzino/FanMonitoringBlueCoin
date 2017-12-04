/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
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
#include "stm32xxx_hal.h"
#include <string.h>
#include "BlueCoin.h"
#include "BlueCoin_prox.h"
#include "tof_gestures.h"
#include "tof_gestures_TAP_1.h"
#include "tof_gestures_SWIPE_1.h"
#include "tof_gestures_DIRSWIPE_1.h"
#include <limits.h>

#define START_M 0
#define WAIT_M 1
#define GET_M 2


/** leaky factor for filtered range
 *
 * r(n) = averaged_r(n-1)*leaky +r(n)(1-leaky)
 *
 * */
int LeakyFactorFix8 = (int)( 0.0 * 256); //(int)( 0.6 *256);
/** How many device detect set by @a DetectSensors()*/
int nDevPresent = 0;
/** bit is index in VL53L0XDevs that is not necessary the dev id of the BSP */
int nDevMask;



/********************************************************************************/
/* GESTURES : Various demo states that can be selected by a short or long press */
/*            on blue button                                                    */
/********************************************************************************/

uint32_t tof_gestures_enableDebugModuleMask;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

static void *VL53L0X_0_handler = NULL;
static void *VL53L0X_1_handler = NULL;

int BUTTONInterrupt = 0;
uint32_t t_coin = 0;

uint8_t demo = 0;
uint8_t complete_Measurement =0;
uint8_t readPROX=START_M;

int main(void)
{
  uint32_t msTick, msTickPrev = 0;

  Gesture_TAP_1_Data_t gestureTapData1;
  Gesture_DIRSWIPE_1_Data_t gestureDirSwipeData;
  int gesture_code, gesture_codeTAPR;
  int leakyR = 0;
  int leakyL = 0;
  int32_t leftRange, rightRange;
  uint16_t range_1 = 0.0;
  uint16_t range = 0.0;
  uint8_t range_statusL = 0;
  uint8_t range_statusR = 0;
  uint8_t NewDataReady_0=0;
  uint8_t NewDataReady_1=0;
  
  HAL_Init();
  
  /* Configure the system clock */
  SystemClock_Config();
  
  BSP_LED_Init(LED1);
  BSP_LED_Init(LED2);
  BSP_LED_Init(LED3);
  BSP_LED_Init(LED4);
  BSP_LED_Init(LED5);
  BSP_LED_Init(LED6);
  BSP_LED_Init(LED7);
  BSP_LED_Init(LED8);
  
  BSP_PB_Init(BUTTON_2, BUTTON_MODE_EXTI);
  BSP_PB_Init(BUTTON_1, BUTTON_MODE_EXTI);
  
  BSP_ShutDown_Init();
  t_coin = HAL_GetTick();
  BSP_LED_On(LED2);
  HAL_Delay(800);
  BSP_LED_Off(LED2);
  
  
  if(BSP_PROX_Init( VL53L0X_0, &VL53L0X_0_handler) != COMPONENT_OK)
    while(1);
    
    
  if(BSP_PROX_Init( VL53L0X_1, &VL53L0X_1_handler) != COMPONENT_OK)
    while(1);
    
  /* Initialize Tap gesture recognition */
  tof_gestures_initTAP_1(250, &gestureTapData1);
  /* Initialize directional swipes recognition : swipe detected below 250 mm, no max speed, min duration is 0.5 sec for a swipe and hand must cover both devices */
  tof_gestures_initDIRSWIPE_1(150, 0, 500, true, &gestureDirSwipeData);
  
  
  BSP_PROX_Set_DeviceMode(VL53L0X_0_handler, CONTINUOUS);
  BSP_PROX_Set_DeviceMode(VL53L0X_1_handler, CONTINUOUS);

  while(1)
  {    
    //read data
    switch(readPROX)
    {
      case START_M:
        BSP_PROX_Start_Measurement(VL53L0X_0_handler);
        BSP_PROX_Start_Measurement(VL53L0X_1_handler);
        readPROX= WAIT_M;
        break;
      case WAIT_M:
        if(NewDataReady_0 ==0)
        {
          BSP_PROX_Get_Measurement_DataReady( VL53L0X_0_handler, &NewDataReady_0);
        }
        if(NewDataReady_1 ==0)
        {
          BSP_PROX_Get_Measurement_DataReady( VL53L0X_1_handler, &NewDataReady_1);
        }
        
        if(NewDataReady_0 !=0 && NewDataReady_1 !=0)
        {
          readPROX= GET_M;
        }
          break;

      case GET_M:
        BSP_PROX_Get_Range( VL53L0X_0_handler, &range );
        BSP_PROX_Get_Range( VL53L0X_1_handler, &range_1 );
        readPROX=START_M;
        
        complete_Measurement=1;
        NewDataReady_0=0;
        NewDataReady_1=0;
        break;
      default:
        while(1);
    }
    
    //run demo over data read
    if(complete_Measurement)
    {
      switch(demo)
      {
        /* range demo */
        case 0:     
          if(range > 150 / 3)
            BSP_LED_On(LED1);
          else
            BSP_LED_Off(LED1);
          if(range > 300 / 3)
            BSP_LED_On(LED2);
          else
            BSP_LED_Off(LED2);
          if(range > 450 / 3)
            BSP_LED_On(LED3);
          else
            BSP_LED_Off(LED3);
          if(range > 600 / 3)
            BSP_LED_On(LED4);
          else
            BSP_LED_Off(LED4);
          if(range > 750 / 3)
            BSP_LED_On(LED5);
          else
            BSP_LED_Off(LED5);
          if(range > 900 / 3)
            BSP_LED_On(LED6);
          else
            BSP_LED_Off(LED6);
          if(range > 1050 / 3)
            BSP_LED_On(LED7);
          else
            BSP_LED_Off(LED7);
          if(range >= 1200 / 3)
            BSP_LED_On(LED8);
          else
            BSP_LED_Off(LED8);
          
          if(BUTTONInterrupt)
          {
            demo = 1;  
            BSP_LED_Off(LED1);
            BSP_LED_Off(LED2);
            BSP_LED_Off(LED4);
            BSP_LED_Off(LED6);
            BSP_LED_Off(LED3);
            BSP_LED_Off(LED7);
            BSP_LED_Off(LED8);
            BSP_LED_Off(LED5);
            BUTTONInterrupt = 0;
          }
          
          break;
         
        /* gesture detect demo */  
        case 1:
          
          msTick = HAL_GetTick();
          if(msTick % 5 == 0 && msTickPrev != msTick)
          {
            msTickPrev = msTick;

            BSP_LED_Toggle(LED1);
          }
          

          BSP_PROX_Get_RangeStatus(VL53L0X_0_handler, &range_statusL );
          BSP_PROX_Get_LeakyRange( VL53L0X_0_handler, &leakyL );
          
          BSP_PROX_Get_RangeStatus(VL53L0X_1_handler, &range_statusR );
          BSP_PROX_Get_LeakyRange( VL53L0X_1_handler, &leakyR );
       
          leftRange = (range_statusL == 0) ? leakyL : 1200;
          rightRange = (range_statusR == 0) ? leakyR : 1200;
          
          
          /* directional swipe detection */
          gesture_code = tof_gestures_detectDIRSWIPE_1(leftRange, rightRange, &gestureDirSwipeData);
          /* tap detection */
          if ( range_statusR == 0 )
          {
            /* Object (hand) detected */
            gesture_codeTAPR = tof_gestures_detectTAP_1(leakyR, &gestureTapData1);
          }else
          {
            /* No object (hand) detected : call the ToF Gesture module with 1200 mm as distance */
            gesture_codeTAPR = tof_gestures_detectTAP_1(1200, &gestureTapData1);
          }
       
          if(gesture_code == GESTURES_SWIPE_LEFT_RIGHT)
          {
            BSP_LED_On(LED6);
            BSP_LED_Off(LED4);
          }
          else if(gesture_code == GESTURES_SWIPE_RIGHT_LEFT)
          {
            BSP_LED_On(LED4);
            BSP_LED_Off(LED6);
          }
          else if(gesture_codeTAPR == GESTURES_SINGLE_TAP ){
            BSP_LED_On(LED1);
            BSP_LED_On(LED2);
            BSP_LED_On(LED4);
            BSP_LED_On(LED6);
            BSP_LED_On(LED3);
            BSP_LED_On(LED7);
            BSP_LED_On(LED8);
            BSP_LED_On(LED5);
            
            HAL_Delay(1000);
            
            BSP_LED_Off(LED1);
            BSP_LED_Off(LED2);
            BSP_LED_Off(LED4);
            BSP_LED_Off(LED6);
            BSP_LED_Off(LED3);
            BSP_LED_Off(LED7);
            BSP_LED_Off(LED8);
            BSP_LED_Off(LED5);
            
          }

          
          if(BUTTONInterrupt)
          {
            demo = 0;
            BUTTONInterrupt = 0;
          }
          break;
          
      default: 
        while(1);
          
      }
      
      complete_Measurement=0;
    }//IF
    
  }//while
}


/**
  * @brief  I2C error callback.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval None
  */
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
  I2C_ErrorCallback_PROX(hi2c);
}

/**
* @brief  EXTI line detection callbacks
* @param  GPIO_Pin: Specifies the pins connected EXTI line
* @retval None
*/

int temp = 0;
void HAL_GPIO_EXTI_Callback( uint16_t GPIO_Pin )
{

  if(GPIO_Pin == BUTTON_2_PIN)
  {
    if(HAL_GetTick() - temp > 4000)
      BUTTONInterrupt = 1;
  }
  else if(GPIO_Pin == BUTTON_1_PIN)
  {
    if(HAL_GetTick() - t_coin > 4000)
      BSP_ShutDown();
  }
  temp = HAL_GetTick();
  
}
/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 180000000
  *            HCLK(Hz)                       = 180000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 8
  *            PLL_N                          = 360
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            PLL_R                          = 2
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
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
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  RCC_OscInitStruct.PLL.PLLR = 2;
  
  if(  HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    while(1)
    {
      ;
    }
  }

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
