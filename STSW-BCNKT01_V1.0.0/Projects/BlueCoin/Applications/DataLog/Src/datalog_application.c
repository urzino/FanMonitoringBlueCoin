/**
  ******************************************************************************
  * @file    DataLog/Src/datalog_application.c
  * @author  Central Labs
  * @version V1.0.0
  * @date    27-April-2017
  * @brief   This file provides a set of functions to handle the datalog
  *          application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
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
#include "datalog_application.h"
#include "main.h"
#include "usbd_cdc_interface.h"
#include "string.h"
#include "BlueCoin.h"
#include <math.h>
    
/* FatFs includes component */
#include "ff_gen_drv.h"
#include "sd_diskio.h"

FRESULT res;                                          /* FatFs function common result code */
uint32_t byteswritten, bytesread;                     /* File write/read counts */
FATFS SDFatFs;  /* File system object for SD card logical drive */
FIL MyFile;     /* File object */
char SDPath[4]; /* SD card logical drive path */
    
volatile uint8_t SD_Log_Enabled = 0;

char newLine[] = "\r\n";

extern void *LSM6DSM_G_0_handle;
extern void *LSM303AGR_M_0_handle;
extern void *LSM303AGR_X_0_handle;
extern void *VL53L0X_0_handler;


/**
  * @brief  Start SD-Card demo
  * @param  None
  * @retval None
  */
void DATALOG_SD_Init(void)
{
  if(FATFS_LinkDriver(&SD_Driver, SDPath) == 0)
  {
    /* Register the file system object to the FatFs module */
    if(f_mount(&SDFatFs, (TCHAR const*)SDPath, 0) != FR_OK)
    {
      /* FatFs Initialization Error */
      while(1)
      {
        BSP_LED_On(LED1);
        HAL_Delay(500);
        BSP_LED_Off(LED1);
        HAL_Delay(100);
      }
    }
  }
}
  
/**
  * @brief  Start SD-Card demo
  * @param  None
  * @retval None
  */
uint8_t DATALOG_SD_Log_Enable(void)
{
  static uint16_t sdcard_file_counter = 0;
  char header[] = "T [ms],AccX [mg],AccY [mg],AccZ [mg],GyroX [mdps],GyroY [mdps],GyroZ [mdps],MagX [mgauss],MagY [mgauss],MagZ [mgauss]\r\n";
  uint32_t byteswritten; /* written byte count */
  char file_name[30] = {0};
  
  sprintf(file_name, "%s%.3d%s", "Log_N", sdcard_file_counter, ".csv");
  sdcard_file_counter++;

  HAL_Delay(100);

  if(f_open(&MyFile, (char const*)file_name, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
  {
    sdcard_file_counter--;
    return 0;
  }
  
  if(f_write(&MyFile, (const void*)&header, sizeof(header)-1, (void *)&byteswritten) != FR_OK)
  {
    return 0;
  }
  return 1;
}

uint8_t DATALOG_SD_writeBuf(char *s, uint32_t size)
{
  uint32_t byteswritten;
  
  if(f_write(&MyFile, s, size, (void *)&byteswritten) != FR_OK)
  {
    return 0;
  }
  return 1;
}


/**
  * @brief  Disable SDCard Log
  * @param  None
  * @retval None
  */
void DATALOG_SD_Log_Disable(void)
{
  f_close(&MyFile);
}

void DATALOG_SD_DeInit(void)
{
  FATFS_UnLinkDriver(SDPath);
}

/**
  * @brief  Write New Line to file
  * @param  None
  * @retval None
  */
void DATALOG_SD_NewLine(void)
{
  uint32_t byteswritten; /* written byte count */
  f_write(&MyFile, (const void*)&newLine, 2, (void *)&byteswritten);
}


DrvStatusTypeDef getSensorsData(T_SensorsData *mptr)
{
  DrvStatusTypeDef ret = COMPONENT_OK;
  uint8_t proxStatus, pDataready;
  
  mptr->ms_counter = HAL_GetTick();
  
  if ( BSP_ACCELERO_Get_Axes( LSM303AGR_X_0_handle, &mptr->acc ) == COMPONENT_ERROR )
  {
    mptr->acc.AXIS_X = 0;
    mptr->acc.AXIS_Y = 0;
    mptr->acc.AXIS_Z = 0;
    ret = COMPONENT_ERROR;
  }
  
  if ( BSP_GYRO_Get_Axes( LSM6DSM_G_0_handle, &mptr->gyro ) == COMPONENT_ERROR )
  {
    mptr->gyro.AXIS_X = 0;
    mptr->gyro.AXIS_Y = 0;
    mptr->gyro.AXIS_Z = 0;
    ret = COMPONENT_ERROR;
  }
  
  if ( BSP_MAGNETO_Get_Axes( LSM303AGR_M_0_handle, &mptr->mag ) == COMPONENT_ERROR )
  {
    mptr->mag.AXIS_X = 0;
    mptr->mag.AXIS_Y = 0;
    mptr->mag.AXIS_Z = 0;
    ret = COMPONENT_ERROR;
  }
  
  BSP_PROX_IsInitialized( VL53L0X_0_handler, &proxStatus );
  if(proxStatus == 1)
  {
    BSP_PROX_Start_Measurement(VL53L0X_0_handler);
    do
    {
      BSP_PROX_Get_Measurement_DataReady( VL53L0X_0_handler, &pDataready);
    }
    while(!pDataready);
    BSP_PROX_Get_Range(VL53L0X_0_handler, &mptr->range);
  }
  
  return ret;
}


void print_readme() {
	FIL readme;
	char file_name[30] = "readme.txt";
	char text[] = "Dalla Longa Milani Urzino";
	uint32_t byteswritten;

	f_open(&readme, (char const*) file_name, 0x02 | 0x08);
	f_write(&readme, (const void*) &text, sizeof(text) - 1,
			(void *) &byteswritten);
	f_close(&readme);
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
