################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Middlewares/ST/STM32_MetaDataManager/MetaDataManager.c 

OBJS += \
./Middlewares/MetaDataManager/MetaDataManager.o 

C_DEPS += \
./Middlewares/MetaDataManager/MetaDataManager.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/MetaDataManager/MetaDataManager.o: C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Middlewares/ST/STM32_MetaDataManager/MetaDataManager.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -std=c99 -DUSE_HAL_DRIVER -DSTM32_NUCLEO -DSTM32L476xx -DUSE_STM32L4XX_NUCLEO -DIKS01A1 -DALLMEMS1 -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Projects/Multi/Applications/ALLMEMS1/Inc" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Drivers/CMSIS/Device/ST/STM32L4xx/Include" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Drivers/STM32L4xx_HAL_Driver/Inc" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Drivers/BSP/STM32L4xx_Nucleo" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Drivers/CMSIS/Include" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Drivers/BSP/Components/Common" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Middlewares/ST/STM32_BlueNRG/SimpleBlueNRG_HCI/includes" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Middlewares/ST/STM32_BlueNRG/Interface" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Drivers/BSP/X-NUCLEO-CCA02M1" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Drivers/BSP/X-NUCLEO-IDB0xA1" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Drivers/BSP/X_NUCLEO_IKS01A1" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Drivers/BSP/Components/hts221" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Drivers/BSP/Components/lis3mdl" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Drivers/BSP/Components/lps22hb" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Drivers/BSP/Components/lps25hb" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Drivers/BSP/Components/lsm6ds0" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Drivers/BSP/Components/lsm6ds3" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Middlewares/ST/STM32_MotionAR_Library/Inc" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Middlewares/ST/STM32_MotionCP_Library/Inc" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Middlewares/ST/STM32_MotionFX_Library/Inc" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Middlewares/ST/STM32_MotionGR_Library/Inc" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Middlewares/ST/STM32_AcousticSL_Library/Inc" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Middlewares/ST/STM32_BlueVoiceADPCM_Library/Inc" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Middlewares/ST/STM32_MetaDataManager"  -Os -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


