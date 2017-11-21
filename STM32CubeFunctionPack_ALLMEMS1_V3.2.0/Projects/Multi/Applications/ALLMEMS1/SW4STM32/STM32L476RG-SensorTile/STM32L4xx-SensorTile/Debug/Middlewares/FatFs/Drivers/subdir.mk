################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Middlewares/Third_Party/FatFs/src/drivers/sd_diskio.c 

OBJS += \
./Middlewares/FatFs/Drivers/sd_diskio.o 

C_DEPS += \
./Middlewares/FatFs/Drivers/sd_diskio.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/FatFs/Drivers/sd_diskio.o: C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Middlewares/Third_Party/FatFs/src/drivers/sd_diskio.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -std=c99 -DUSE_HAL_DRIVER -DSTM32_SENSORTILE -DSTM32L476xx -DUSE_STM32L4XX_NUCLEO -DALLMEMS1 -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Projects/Multi/Applications/ALLMEMS1/Inc" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Drivers/CMSIS/Device/ST/STM32L4xx/Include" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Drivers/STM32L4xx_HAL_Driver/Inc" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Drivers/BSP/SensorTile" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Drivers/CMSIS/Include" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Drivers/BSP/Components/Common" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Middlewares/ST/STM32_BlueNRG/SimpleBlueNRG_HCI/includes" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Middlewares/ST/STM32_BlueNRG/Interface" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Drivers/BSP/Components/lsm6dsm" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Drivers/BSP/Components/hts221" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Drivers/BSP/Components/lps22hb" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Drivers/BSP/Components/lsm303agr" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Drivers/BSP/Components/stc3115" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Middlewares/ST/STM32_MotionAR_Library/Inc" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Middlewares/ST/STM32_MotionCP_Library/Inc" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Middlewares/ST/STM32_MotionFX_Library/Inc" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Middlewares/ST/STM32_MotionGR_Library/Inc" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Middlewares/ST/STM32_BlueVoiceADPCM_Library/Inc" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Middlewares/ST/STM32_MetaDataManager" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Middlewares/Third_Party/FatFs/src" -I"C:/Users/Urzi/Desktop/BLUE-COIN/en.fp-sns-allmems1_firmware/STM32CubeFunctionPack_ALLMEMS1_V3.2.0/Middlewares/Third_Party/FatFs/src/drivers"  -Os -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


