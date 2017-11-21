################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Projects/Multi/Applications/ALLMEMS1/SW4STM32/STM32L476RG-Nucleo/startup_stm32l476xx.s 

C_SRCS += \
C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Projects/Multi/Applications/ALLMEMS1/Src/syscalls.c 

OBJS += \
./FP-SNS-ALLMEMS1/SW4STM32/startup_stm32l476xx.o \
./FP-SNS-ALLMEMS1/SW4STM32/syscalls.o 

C_DEPS += \
./FP-SNS-ALLMEMS1/SW4STM32/syscalls.d 


# Each subdirectory must supply rules for building sources it contributes
FP-SNS-ALLMEMS1/SW4STM32/startup_stm32l476xx.o: C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Projects/Multi/Applications/ALLMEMS1/SW4STM32/STM32L476RG-Nucleo/startup_stm32l476xx.s
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Assembler'
	@echo $(PWD)
	arm-none-eabi-as -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -I../../../../Inc -I../../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../../Drivers/BSP/STM32L4xx_Nucleo -I../../../../../../../../Drivers/CMSIS/Include -I../../../../../../../../Drivers/BSP/Components/Common -I../../../../../../../../Drivers/BSP/Components/hts221 -I../../../../../../../../Drivers/BSP/Components/lis3mdl -I../../../../../../../../Drivers/BSP/Components/lps25hb -I../../../../../../../../Drivers/BSP/Components/lsm6ds0 -I../../../../../../../../Drivers/BSP/Components/lsm6ds3 -I../../../../../../../../Drivers/BSP/X_NUCLEO_IKS01A1 -I../../../../../../../../Middlewares/ST/STM32_MotionFX_Library/Inc -I../../../../../../../../Middlewares/ST/STM32_MotionFX_Library -g -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

FP-SNS-ALLMEMS1/SW4STM32/syscalls.o: C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Projects/Multi/Applications/ALLMEMS1/Src/syscalls.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -std=c99 -DUSE_HAL_DRIVER -DSTM32_NUCLEO -DSTM32L476xx -DUSE_STM32L4XX_NUCLEO -DIKS01A1 -DALLMEMS1 -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Projects/Multi/Applications/ALLMEMS1/Inc" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Drivers/CMSIS/Device/ST/STM32L4xx/Include" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Drivers/STM32L4xx_HAL_Driver/Inc" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Drivers/BSP/STM32L4xx_Nucleo" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Drivers/CMSIS/Include" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Drivers/BSP/Components/Common" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Middlewares/ST/STM32_BlueNRG/SimpleBlueNRG_HCI/includes" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Middlewares/ST/STM32_BlueNRG/Interface" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Drivers/BSP/X-NUCLEO-CCA02M1" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Drivers/BSP/X-NUCLEO-IDB0xA1" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Drivers/BSP/X_NUCLEO_IKS01A1" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Drivers/BSP/Components/hts221" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Drivers/BSP/Components/lis3mdl" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Drivers/BSP/Components/lps22hb" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Drivers/BSP/Components/lps25hb" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Drivers/BSP/Components/lsm6ds0" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Drivers/BSP/Components/lsm6ds3" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Middlewares/ST/STM32_MotionAR_Library/Inc" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Middlewares/ST/STM32_MotionCP_Library/Inc" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Middlewares/ST/STM32_MotionFX_Library/Inc" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Middlewares/ST/STM32_MotionGR_Library/Inc" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Middlewares/ST/STM32_AcousticSL_Library/Inc" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Middlewares/ST/STM32_BlueVoiceADPCM_Library/Inc" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Middlewares/ST/STM32_MetaDataManager"  -Os -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


