/**
 ******************************************************************************
 * @file    main.c
 * @author  Central Labs
 * @version V1.0.0
 * @date    27-April-2017
 * @brief   Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics International N.V.
 * All rights reserved.</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted, provided that the following conditions are met:
 *
 * 1. Redistribution of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of STMicroelectronics nor the names of other
 *    contributors to this software may be used to endorse or promote products
 *    derived from this software without specific written permission.
 * 4. This software, including modifications and/or derivative works of this
 *    software, must execute solely and exclusively on microcontroller or
 *    microprocessor devices manufactured by or for STMicroelectronics.
 * 5. Redistribution and use of this software other than as permitted under
 *    this license is void and will automatically terminate your rights under
 *    this license.
 *
 * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
 * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
 * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "datalog_application.h"
#include "usbd_cdc_interface.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#define DATAQUEUE_SIZE     ((uint32_t)100)

#define ACQUISITION_PERIOD_MS (10000)

typedef enum {
	THREAD_1 = 0, THREAD_2
} Thread_TypeDef;

/* Private variables ---------------------------------------------------------*/

osThreadId GetDataThreadId, WriteDataThreadId;

osMessageQId dataQueue_id;
osMessageQDef(dataqueue, DATAQUEUE_SIZE, int);

osPoolId sensorPool_id;
osPoolDef(sensorPool, DATAQUEUE_SIZE, T_SensorsData);

osSemaphoreId readDataSem_id;
osSemaphoreDef(readDataSem);

osSemaphoreId stopReadDataSem_id;
osSemaphoreDef(stopReadDataSem);

/* LoggingInterface = USB_Datalog  --> Save sensors data on SDCard (enable with double click) */
/* LoggingInterface = SDCARD_Datalog  --> Send sensors data via USB */
LogInterface_TypeDef LoggingInterface = SDCARD_Datalog;

USBD_HandleTypeDef USBD_Device;
static volatile uint8_t BUTTONInterrupt = 0;
volatile uint8_t no_VL53L0X = 0;
volatile uint8_t BatteryLow = 0;

void *LSM6DSM_X_0_handle = NULL;
void *LSM6DSM_G_0_handle = NULL;
void *LSM303AGR_X_0_handle = NULL;
void *LSM303AGR_M_0_handle = NULL;
void *LPS22HB_P_0_handle = NULL;
void *LPS22HB_T_0_handle = NULL;
void *VL53L0X_0_handler = NULL;

void SystemClock_Config(void);

/* Private function prototypes -----------------------------------------------*/
static void GetData_Thread(void const *argument);
static void WriteData_Thread(void const *argument);
void Battery_Handler(void);

static void Error_Handler(void);
static void initializeAllSensors(void);
void enableAllSensors(void);
void disableAllSensors(void);
void setOdrAllSensors(void);

void dataTimer_Callback(void const *arg);
void dataTimerStart(void);
void dataTimerStop(void);
uint32_t t_coin = 0;

osTimerId sensorTimId;
osTimerDef(SensorTimer, dataTimer_Callback);
char data_s_prox[32];
uint32_t exec;

void stopAcquisitionTimer_Callback(void const *arg);
void stopAcquisitionTimerStart(void);
osTimerId stopAcquisitionTimId;
osTimerDef(StopAcquisitionTimer, stopAcquisitionTimer_Callback);
uint32_t myExec;



/**
 * @brief  Main program
 * @param  None
 * @retval None
 */
int main(void) {
	HAL_Init();

	SystemClock_Config();

	BSP_LED_Init(LED1);
	BSP_LED_Off(LED1);
	BSP_LED_Init(LED2);
	BSP_LED_Off(LED2);
	BSP_LED_Init(LED3);
	BSP_LED_Off(LED3);
	BSP_LED_Init(LED4);
	BSP_LED_Off(LED4);
	BSP_LED_Init(LED5);
	BSP_LED_Off(LED5);
	BSP_LED_Init(LED6);
	BSP_LED_Off(LED6);
	BSP_LED_Init(LED7);
	BSP_LED_Off(LED7);
	BSP_LED_Init(LED8);
	BSP_LED_Off(LED8);

	BSP_PB_Init(BUTTON_2, BUTTON_MODE_EXTI);

	BSP_ChrgPin_Init();

	BSP_PB_Init(BUTTON_1, BUTTON_MODE_EXTI);

	/* Shutdown pin initialization */
	BSP_ShutDown_Init();

	t_coin = HAL_GetTick();

	HAL_Delay(900);

	BSP_SD_Detect_Init();

	while (!BSP_SD_IsDetected()) {
		/* LED On */
		BSP_LED_On(LED1);
	}
	BSP_LED_Off(LED1);
	HAL_Delay(200);
	DATALOG_SD_Init();

	/* Thread 1 definition */
	osThreadDef(THREAD_1, GetData_Thread, osPriorityAboveNormal, 0,
			configMINIMAL_STACK_SIZE*4);

	/* Thread 2 definition */
	osThreadDef(THREAD_2, WriteData_Thread, osPriorityNormal, 0,
			configMINIMAL_STACK_SIZE*4);

	/* Start thread 1 */
	GetDataThreadId = osThreadCreate(osThread(THREAD_1), NULL);

	/* Start thread 2 */
	WriteDataThreadId = osThreadCreate(osThread(THREAD_2), NULL);

	/* Start scheduler */
	osKernelStart();

	/* We should never get here as control is now taken by the scheduler */
	BSP_LED_On(LED1);
	for (;;)
		;

}

void blink(int LED){
	BSP_LED_On(LED);
	HAL_Delay(10);
	BSP_LED_Off(LED);
	HAL_Delay(10);
	BSP_LED_On(LED);
	HAL_Delay(10);
	BSP_LED_Off(LED);
}

/**
 * @brief  Get data raw from sensors to queue
 * @param  thread not used
 * @retval None
 */

static void GetData_Thread(void const *argument) {
	(void) argument;
	T_SensorsData *mptr;

	sensorPool_id = osPoolCreate(osPool(sensorPool));
	dataQueue_id = osMessageCreate(osMessageQ(dataqueue), NULL);

	readDataSem_id = osSemaphoreCreate(osSemaphore(readDataSem), 1);
	osSemaphoreWait(readDataSem_id, osWaitForever);

	stopReadDataSem_id = osSemaphoreCreate(osSemaphore(stopReadDataSem), 1);
	osSemaphoreWait(stopReadDataSem_id, osWaitForever);

	/* Initialize and Enable the available sensors */
	initializeAllSensors();
	enableAllSensors();

	for (;;) {
		osSemaphoreWait(readDataSem_id, osWaitForever);
		if (BUTTONInterrupt && LoggingInterface == SDCARD_Datalog) {
			if (SD_Log_Enabled) {
				dataTimerStop();
				osMessagePut(dataQueue_id, 0x00000007, osWaitForever);
				osSemaphoreWait(stopReadDataSem_id, osWaitForever);
			} else {
				blink(LED4);
				osMessagePut(dataQueue_id, 0x00000007, osWaitForever);
			}
			BUTTONInterrupt = 0;
		} else {
			/* Try to allocate a memory block and check if is not NULL */
			mptr = osPoolAlloc(sensorPool_id);
			if (mptr != NULL) {
				/* Get Data from Sensors */
				if (getSensorsData(mptr) == COMPONENT_OK) {
					/* Push the new memory Block in the Data Queue */
					if (osMessagePut(dataQueue_id, (uint32_t) mptr,
					osWaitForever) != osOK) {
						BSP_LED_Off(LED1);
						BSP_LED_Off(LED4);
						BSP_LED_Off(LED6);
						BSP_LED_Off(LED5);
						BSP_LED_On(LED2);
						Error_Handler();
					}
				} else {
					BSP_LED_Off(LED1);
					BSP_LED_Off(LED4);
					BSP_LED_Off(LED6);
					BSP_LED_Off(LED5);
					BSP_LED_On(LED2);
					BSP_LED_On(LED3);
					Error_Handler();
				}
			} else {
				BSP_LED_Off(LED1);
				BSP_LED_Off(LED4);
				BSP_LED_Off(LED6);
				BSP_LED_Off(LED5);
				BSP_LED_On(LED1);
				BSP_LED_On(LED2);
				BSP_LED_On(LED3);
				Error_Handler();
			}
		}
	}
}

/**
 * @brief  Write data in the queue on file or streaming via USB
 * @param  argument not used
 * @retval None
 */
static void WriteData_Thread(void const *argument) {
	(void) argument;
	osEvent evt;
	T_SensorsData *rptr;
	int size;
	char data_s[256];

	for (;;) {
		evt = osMessageGet(dataQueue_id, osWaitForever);  // wait for message
		if (evt.status == osEventMessage) {
			if (evt.value.v == 0x00000007)  // Start/Stop message
					{
				if (SD_Log_Enabled) {
					blink(LED6);
					DATALOG_SD_Log_Disable();
					SD_Log_Enabled = 0;
					BSP_LED_Off(LED5);
					osTimerStop(stopAcquisitionTimId);
					osSemaphoreRelease(stopReadDataSem_id);
				} else {
					while (SD_Log_Enabled != 1) {
						if (DATALOG_SD_Log_Enable()) {
							SD_Log_Enabled = 1;
							BSP_LED_On(LED5);
							osDelay(100);
							dataTimerStart();
							stopAcquisitionTimerStart();
						} else {
							DATALOG_SD_Log_Disable();
						}
					}
				}
			} else  // Data message
			{
				rptr = evt.value.p;

				size = sprintf(data_s, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n",
						(int) rptr->ms_counter, (int) rptr->acc.AXIS_X,
						(int) rptr->acc.AXIS_Y, (int) rptr->acc.AXIS_Z,
						(int) rptr->gyro.AXIS_X, (int) rptr->gyro.AXIS_Y,
						(int) rptr->gyro.AXIS_Z, (int) rptr->mag.AXIS_X,
						(int) rptr->mag.AXIS_Y, (int) rptr->mag.AXIS_Z);
				osPoolFree(sensorPool_id, rptr); // free memory allocated for message
				DATALOG_SD_writeBuf(data_s, size);
			}
		}
	}
}

/* Idle task */
void vApplicationIdleHook(void) {
	static uint32_t LastLedTick;
	uint32_t Tick = HAL_GetTick();
	static uint32_t Threshold = 950;

	if (LoggingInterface == SDCARD_Datalog) {
		if (Tick - LastLedTick > Threshold) {
			if (Threshold == 950) {
				BSP_LED_On(LED1);
				Threshold = 50;
			} else {
				BSP_LED_Off(LED1);
				Threshold = 950;
			}
			LastLedTick = Tick;
		}
	}
}

void dataTimer_Callback(void const *arg) {
	osSemaphoreRelease(readDataSem_id);
}

void dataTimerStart(void) {
	osStatus status;

	/* Create periodic timer */
	exec = 1;
	sensorTimId = osTimerCreate(osTimer(SensorTimer), osTimerPeriodic, &exec);
	if (sensorTimId) {
		status = osTimerStart(sensorTimId, DATA_PERIOD_MS);
		if (status != osOK) {
			/* Timer could not be started */
		}
	}
}


void dataTimerStop(void) {
	osTimerStop(sensorTimId);
}

void stopAcquisitionTimer_Callback(void const *args){
	BUTTONInterrupt=1;
}

void stopAcquisitionTimerStart(void){
	osStatus status;

	myExec = 1;
	stopAcquisitionTimId = osTimerCreate(osTimer(StopAcquisitionTimer), osTimerOnce, &myExec);
	if (stopAcquisitionTimId) {
			status = osTimerStart(stopAcquisitionTimId, ACQUISITION_PERIOD_MS);
			if (status != osOK) {
				/* Timer could not be started */
			}
		}
}

/**
 * @brief  PWR PVD interrupt callback
 * @param  None
 * @retval None
 */
void HAL_PWR_PVDCallback(void) {
	BatteryLow = 1;
}

/**
 * @brief  Initialize all sensors
 * @param  None
 * @retval None
 */
static void initializeAllSensors(void) {
	if (BSP_ACCELERO_Init(LSM6DSM_X_0, &LSM6DSM_X_0_handle) != COMPONENT_OK) {
		while (1)
			;
	}

	if (BSP_GYRO_Init(LSM6DSM_G_0, &LSM6DSM_G_0_handle) != COMPONENT_OK) {
		while (1)
			;
	}

	if (BSP_ACCELERO_Init(LSM303AGR_X_0, &LSM303AGR_X_0_handle)
			!= COMPONENT_OK) {
		while (1)
			;
	}

	if (BSP_MAGNETO_Init(LSM303AGR_M_0, &LSM303AGR_M_0_handle)
			!= COMPONENT_OK) {
		while (1)
			;
	}
}

/**
 * @brief  Enable all sensors
 * @param  None
 * @retval None
 */
void enableAllSensors(void) {
	BSP_ACCELERO_Sensor_Enable(LSM6DSM_X_0_handle);
	BSP_GYRO_Sensor_Enable(LSM6DSM_G_0_handle);
	BSP_ACCELERO_Sensor_Enable(LSM303AGR_X_0_handle);
	BSP_MAGNETO_Sensor_Enable(LSM303AGR_M_0_handle);
}
/**
 * @brief  Set ODR all sensors
 * @param  None
 * @retval None
 */
void setOdrAllSensors(void) {
	BSP_ACCELERO_Set_ODR_Value(LSM303AGR_X_0_handle, ACCELERO_ODR);
	BSP_MAGNETO_Set_ODR_Value(LSM303AGR_M_0_handle, MAGNETO_ODR);
	BSP_GYRO_Set_ODR_Value(LSM6DSM_G_0_handle, GYRO_ODR);
}

/**
 * @brief  Disable all sensors
 * @param  None
 * @retval None
 */
void disableAllSensors(void) {
	BSP_ACCELERO_Sensor_Disable(LSM6DSM_X_0_handle);
	BSP_ACCELERO_Sensor_Disable(LSM303AGR_X_0_handle);
	BSP_GYRO_Sensor_Disable(LSM6DSM_G_0_handle);
	BSP_MAGNETO_Sensor_Disable(LSM303AGR_M_0_handle);
}

void Battery_Handler(void) {
	ChrgStatus_t status;
	uint16_t Voltage;
	char data_s[256];

	status = BSP_GetChrgStatus();
	BSP_GetVoltage(&Voltage);
}

/**
 * @brief  This function is executed in case of error occurrence
 * @param  None
 * @retval None
 */
static void Error_Handler(void) {
	while (1) {
		BSP_LED_Toggle(LED1);
		BSP_LED_Toggle(LED2);
		BSP_LED_Toggle(LED3);
		BSP_LED_Toggle(LED4);
		BSP_LED_Toggle(LED5);
		BSP_LED_Toggle(LED6);
		BSP_LED_Toggle(LED7);
		BSP_LED_Toggle(LED8);
		HAL_Delay(1000);
	}
}

/**
 * @brief  System Clock Configuration
 * @param  None
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;
	RCC_PeriphCLKInitTypeDef RCC_PeriphClkInitStruct;

	/* Enable Power Control clock */
	__HAL_RCC_PWR_CLK_ENABLE()
	;

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

	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		while (1) {
			;
		}
	}

	/* generate 48Mhz for SD card clock */
	RCC_PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SDIO
			| RCC_PERIPHCLK_CK48;
	RCC_PeriphClkInitStruct.SdioClockSelection = RCC_SDIOCLKSOURCE_CK48;
	RCC_PeriphClkInitStruct.Clk48ClockSelection = RCC_CK48CLKSOURCE_PLLSAIP;
	RCC_PeriphClkInitStruct.PLLSAI.PLLSAIM = 16;
	RCC_PeriphClkInitStruct.PLLSAI.PLLSAIN = 384;
	RCC_PeriphClkInitStruct.PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV8;
	HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphClkInitStruct);

	/*Select Main PLL output as USB clock source */
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_CK48;
	PeriphClkInitStruct.Clk48ClockSelection = RCC_CK48CLKSOURCE_PLLQ;
	HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

	/* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
	 clocks dividers */
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
}

/**
 * @brief  EXTI line detection callbacks
 * @param  GPIO_Pin: Specifies the pins connected EXTI line
 * @retval None
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == BUTTON_2_PIN) {
		BUTTONInterrupt = 1;
		osSemaphoreRelease(readDataSem_id);
	} else if (GPIO_Pin == SD_DETECT_GPIO_PIN) {
		BSP_SD_IsDetected();
	} else if (GPIO_Pin == CHRG_PIN) {
		BSP_SetLastChrgTick(HAL_GetTick());
	} else if (GPIO_Pin == BUTTON_1_PIN) {
		if (HAL_GetTick() - t_coin > 4000) {
			BSP_ShutDown();
		}
	}
}

/**
 * @brief  I2C error callback.
 * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
 *                the configuration information for the specified I2C.
 * @retval None
 */
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c) {
	I2C_ErrorCallback_PROX(hi2c);
}

#ifdef  USE_FULL_ASSERT

/**
 * @brief  Reports the name of the source file and the source line number
 *   where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1)
	{}
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
