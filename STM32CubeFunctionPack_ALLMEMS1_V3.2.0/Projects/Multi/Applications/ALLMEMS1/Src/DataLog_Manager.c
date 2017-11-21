/**
 ******************************************************************************
 * @file    DataLog_Manager.c
 * @author  Central LAB
 * @version V3.2.0
 * @date    30-Sep-2017
 * @brief   This file includes Source location interface functions
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
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
#include "TargetFeatures.h"
#include "sensor_service.h"
#include "DataLog_Manager.h"
#include "datalog_application.h"

#ifdef ALLMEMS1_ENABLE_SD_MEMS_RECORDING

/* Exported Variables -------------------------------------------------------------*/
volatile int  index_buff=0;
volatile uint8_t writeAudio_flag=0;

uint16_t Audio_OUT_Buff[AUDIO_BUFF_SIZE];


/* Imported Variables -------------------------------------------------------------*/
extern volatile uint8_t SD_Log_Enabled;
extern volatile uint8_t SD_LogAudio_Enabled;

extern volatile uint8_t SD_CardNotPresent;

extern uint16_t PCM_Buffer[];

/* Private variables -------------------------------------------------------------*/
static char myBuffer[ARRAYSIZE];

/* Private function prototypes ---------------------------------------------------*/
static SensorAxes_t Accelero_Sensor_Handler_Light(void *handle);
static SensorAxes_t Gyro_Sensor_Handler_Light( void *handle );
static SensorAxes_t Magneto_Sensor_Handler_Light( void *handle);
static float Temperature_Sensor_Handler_Light( void *handle );
static float Pressure_Sensor_Handler_Light( void *handle );
static float Humidity_Sensor_Handler_Light( void *handle );


/**
  * @brief  .
  * @param  None
  * @retval None
  */
void AudioProcess_SD_Recording(void)
{
  uint16_t index = 0;
  static uint16_t OUT_Buff_lvl = 0;
  
  for (index = 0; index < AUDIO_SAMPLING_FREQUENCY/1000 ; index++)
  {
    Audio_OUT_Buff[OUT_Buff_lvl] = PCM_Buffer[index];
    OUT_Buff_lvl = (OUT_Buff_lvl + 1)%AUDIO_BUFF_SIZE;
  }
  
  //first half
  if(OUT_Buff_lvl == AUDIO_BUFF_SIZE/2)
  {
    index_buff=0;
    writeAudio_flag=1; 
  }    
  //second half
  else if (OUT_Buff_lvl == 0)
  {
    index_buff= AUDIO_BUFF_SIZE;
    writeAudio_flag=1; 
  }    
}

/**
  * @brief  .
  * @param  None
  * @retval None
  */
void openFileAudio(void)
{  
  while( (SD_LogAudio_Enabled != 1) &&
         (SD_CardNotPresent != 1) )
  {
    if(DATALOG_SD_LogAudio_Enable())
    {
      SD_LogAudio_Enabled=1;
    }
    else
    {
      DATALOG_SD_LogAudio_Disable();
      //DATALOG_SD_Log_Disable();
      DATALOG_SD_DeInit();
      DATALOG_SD_Init();
    }
    HAL_Delay(100);
  }
}

/**
  * @brief  .
  * @param  None
  * @retval None
  */
void closeFileAudio(void)
{
  DATALOG_SD_LogAudio_Disable();
  SD_LogAudio_Enabled=0;
}

/**
  * @brief  .
  * @param  None
  * @retval None
  */
void StuffBuffer(void)
{
  SensorAxes_t acceleration;
  SensorAxes_t angular_velocity;
  SensorAxes_t magnetometer;
  
  float temperature;
  float pressure;
  float humidity;
  
  uint32_t msTick;
  
  msTick              = HAL_GetTick();
  acceleration        = Accelero_Sensor_Handler_Light(TargetBoardFeatures.HandleAccSensor);
  angular_velocity    = Gyro_Sensor_Handler_Light(TargetBoardFeatures.HandleGyroSensor);
  magnetometer        = Magneto_Sensor_Handler_Light(TargetBoardFeatures.HandleMagSensor);
  pressure            = Pressure_Sensor_Handler_Light(TargetBoardFeatures.HandlePressSensor);
  temperature         = Temperature_Sensor_Handler_Light(TargetBoardFeatures.HandleTempSensors[0]);
  humidity            = Humidity_Sensor_Handler_Light(TargetBoardFeatures.HandleHumSensor);
  
  saveData(myBuffer, acceleration, angular_velocity, magnetometer, pressure, temperature, humidity, msTick, FREQUENCY);
}

/**
  * @brief  .
  * @param  None
  * @retval None
  */
void openFile(void)
{  
  while( (SD_Log_Enabled != 1) &&
         (SD_CardNotPresent != 1) )
  {
    if(DATALOG_SD_Log_Enable())
    {
      SD_Log_Enabled=1;
    }
    else
    {
      DATALOG_SD_Log_Disable();
      DATALOG_SD_DeInit();
      DATALOG_SD_Init();
    }
    HAL_Delay(100);
  }
}

/**
  * @brief  .
  * @param  None
  * @retval None
  */
void closeFile(void)
{
  DATALOG_SD_Log_Disable();
  SD_Log_Enabled=0;
}

/**
 * @brief  Handles the accelerometer axes data, fast
 * @param  handle the device handle
 * @retval None
 */
static SensorAxes_t Accelero_Sensor_Handler_Light(void *handle)
{
  uint8_t id;
  SensorAxes_t acceleration;
  uint8_t status;
  
  acceleration.AXIS_X = 0;
  acceleration.AXIS_Y = 0;
  acceleration.AXIS_Z = 0;

  BSP_ACCELERO_Get_Instance(handle, &id);

  BSP_ACCELERO_IsInitialized(handle, &status);
  
  acceleration.AXIS_X = 0;
  acceleration.AXIS_Y = 0;
  acceleration.AXIS_Z = 0;

  if (status == 1)
  {
    if (BSP_ACCELERO_Get_Axes(handle, &acceleration) == COMPONENT_ERROR)
    {
      acceleration.AXIS_X = 0;
      acceleration.AXIS_Y = 0;
      acceleration.AXIS_Z = 0;
    }    
  }
  
  return acceleration;
}

/**
* @brief  Handles the gyroscope axes data getting/sending
* @param  handle the device handle
* @retval None
*/
static SensorAxes_t Gyro_Sensor_Handler_Light( void *handle )
{
  uint8_t id;
  SensorAxes_t angular_velocity;
  uint8_t status;
  
  BSP_GYRO_Get_Instance( handle, &id );
  
  BSP_GYRO_IsInitialized( handle, &status );
  
  angular_velocity.AXIS_X = 0;
  angular_velocity.AXIS_Y = 0;
  angular_velocity.AXIS_Z = 0;
  
  if ( status == 1 )
  {
    if ( BSP_GYRO_Get_Axes( handle, &angular_velocity ) == COMPONENT_ERROR )
    {
      angular_velocity.AXIS_X = 0;
      angular_velocity.AXIS_Y = 0;
      angular_velocity.AXIS_Z = 0;
    }    
  }
  
  return angular_velocity;
}

/**
* @brief  Handles the magneto axes data getting/sending
* @param  handle the device handle
* @retval None
*/
static SensorAxes_t Magneto_Sensor_Handler_Light( void *handle )
{
  uint8_t id;
  SensorAxes_t magnetic_field;
  uint8_t status;
  
  BSP_MAGNETO_Get_Instance( handle, &id );
  
  BSP_MAGNETO_IsInitialized( handle, &status );
  
  magnetic_field.AXIS_X = 0;
  magnetic_field.AXIS_Y = 0;
  magnetic_field.AXIS_Z = 0;
  
  if ( status == 1 )
  {
    if ( BSP_MAGNETO_Get_Axes( handle, &magnetic_field ) == COMPONENT_ERROR )
    {
      magnetic_field.AXIS_X = 0;
      magnetic_field.AXIS_Y = 0;
      magnetic_field.AXIS_Z = 0;
    }           
  }
  
  return magnetic_field; 
}

/**
* @brief  Handles the temperature data getting/sending
* @param  handle the device handle
* @retval None
*/
static float Temperature_Sensor_Handler_Light( void *handle )
{
  uint8_t id;
  float temperature = 0.0;
  uint8_t status;
  
  BSP_TEMPERATURE_Get_Instance( handle, &id );
  
  BSP_TEMPERATURE_IsInitialized( handle, &status );
  
  if ( status == 1 )
  {
    if ( BSP_TEMPERATURE_Get_Temp( handle, &temperature ) == COMPONENT_ERROR )
    {
      temperature = 0.0f;
    }    
  }
  
  return temperature;
}

/**
* @brief  Handles the pressure sensor data getting/sending
* @param  handle the device handle
* @retval None
*/
static float Pressure_Sensor_Handler_Light( void *handle )
{
  uint8_t id;
  float pressure;
  uint8_t status;
  
  BSP_PRESSURE_Get_Instance( handle, &id );
  
  BSP_PRESSURE_IsInitialized( handle, &status );
  
  pressure = 0.0;
  
  if( status == 1 )
  {
    if ( BSP_PRESSURE_Get_Press( handle, &pressure ) == COMPONENT_ERROR )
    {
      pressure = 0.0f;
    }    
  }
  
  return pressure;
}

/**
* @brief  Handles the humidity data getting/sending
* @param  handle the device handle
* @retval None
*/
static float Humidity_Sensor_Handler_Light( void *handle )
{
  uint8_t id;
  float humidity;
  uint8_t status;
  
  BSP_HUMIDITY_Get_Instance( handle, &id );
  
  humidity = 0.0f;
  
  BSP_HUMIDITY_IsInitialized( handle, &status );
  
  if ( status == 1 )
  {
    if ( BSP_HUMIDITY_Get_Hum( handle, &humidity ) == COMPONENT_ERROR )
    {
      humidity = 0.0f;
    }    
  }
  return humidity;
}

#endif /* ALLMEMS1_ENABLE_SD_MEMS_RECORDING */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
