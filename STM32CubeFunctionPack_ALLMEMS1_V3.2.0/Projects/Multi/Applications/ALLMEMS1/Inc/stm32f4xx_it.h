/**
  ******************************************************************************
  * @file    stm32f4xx_it.h 
  * @author  Central LAB
  * @version V3.2.0
  * @date    30-Sep-2017
  * @brief   This file contains the headers of the interrupt handlers.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F4xx_IT_H
#define __STM32F4xx_IT_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Exported functions ------------------------------------------------------- */
void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);
void TIM4_IRQHandler(void);
void TIM5_IRQHandler(void);
void TIM1_CC_IRQHandler(void);
void AUDIO_IN_I2S_IRQHandler(void);

#ifdef ALLMEMS1_ENABLE_SD_RECORDING
void TIM2_IRQHandler(void);
#endif /* ALLMEMS1_ENABLE_SD_RECORDING */

#ifdef ALLMEMS1_ACOUSTIC_BEAM_FORMING
void EXTI1_IRQHandler(void);
#endif /* ALLMEMS1_ACOUSTIC_BEAM_FORMING */

void EXTI3_IRQHandler(void);
void SW_BV_send_Callback(void);

void EXTI2_IRQHandler(void);

#ifdef STM32_NUCLEO
void EXTI0_IRQHandler(void);
void EXTI15_10_IRQHandler(void);
#ifdef IKS01A1
void EXTI4_IRQHandler( void );
#elif IKS01A2
void EXTI9_5_IRQHandler( void );
#endif /* IKS01A1 */
#elif STM32_BLUECOIN
void I2C1_ER_IRQHandler(void);
void I2C1_EV_IRQHandler(void);
void EXTI9_5_IRQHandler(void);
void EXTI0_IRQHandler(void);
void EXTI4_IRQHandler(void);
void BNRG_SPI_EXTI_IRQHandler(void);
void PVD_IRQHandler(void);
#endif /* STM32_NUCLEO */

#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_IT_H */

/******************* (C) COPYRIGHT 2017 STMicroelectronics *****END OF FILE****/
