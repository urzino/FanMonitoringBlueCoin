################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Projects/Multi/Applications/ALLMEMS1/Src/system_stm32f4xx.c 

OBJS += \
./Drivers/CMSIS/system_stm32f4xx.o 

C_DEPS += \
./Drivers/CMSIS/system_stm32f4xx.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/CMSIS/system_stm32f4xx.o: C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Projects/Multi/Applications/ALLMEMS1/Src/system_stm32f4xx.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DUSE_HAL_DRIVER -DSTM32F446xx -DUSE_STM32F4XX_NUCLEO -DSTM32_BLUECOIN -DUSE_BLUECOIN -DARM_MATH_CM4 -DALLMEMS1 -DDISABLE_FOTA -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Projects/Multi/Applications/ALLMEMS1/Inc" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Middlewares/ST/STM32_BlueNRG/SimpleBlueNRG_HCI/includes" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Middlewares/ST/STM32_BlueNRG/Interface" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Drivers/STM32F4xx_HAL_Driver/Inc" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Drivers/BSP/STM32F4xx-Nucleo" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Drivers/BSP/Components/Common" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Drivers/BSP/Components/lsm303agr" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Drivers/BSP/Components/lps22hb" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Drivers/BSP/Components/lps25hb" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Drivers/BSP/Components/lsm6dsm" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Drivers/BSP/BlueCoin" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Middlewares/ST/STM32_MotionAR_Library/Inc" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Middlewares/ST/STM32_MotionCP_Library/Inc" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Middlewares/ST/STM32_MotionFX_Library/Inc" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Middlewares/ST/STM32_MotionGR_Library/Inc" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Middlewares/ST/STM32_BlueVoiceADPCM_Library/Inc" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Middlewares/ST/STM32_AcousticBF_Library/Inc" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Middlewares/ST/STM32_AcousticSL_Library/Inc" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Middlewares/ST/STM32_MetaDataManager" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Middlewares/ST/STM32_Audio/Addons/PDM" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Drivers/CMSIS/Include"  -Os -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

