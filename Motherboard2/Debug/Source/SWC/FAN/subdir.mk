################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Source/SWC/FAN/FAN.c 

OBJS += \
./Source/SWC/FAN/FAN.o 

C_DEPS += \
./Source/SWC/FAN/FAN.d 


# Each subdirectory must supply rules for building sources it contributes
Source/SWC/FAN/%.o Source/SWC/FAN/%.su Source/SWC/FAN/%.cyclo: ../Source/SWC/FAN/%.c Source/SWC/FAN/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I"/home/lord448/Documentos/TEC/9no/ControlDigital/PneumaticLevitator/Motherboard/Drivers/VL53L0X/Api/core/inc" -I"/home/lord448/Documentos/TEC/9no/ControlDigital/PneumaticLevitator/Motherboard/Drivers/VL53L0X/Api/platform/inc" -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"/home/lord448/Documentos/TEC/9no/ControlDigital/PneumaticLevitator/Motherboard2/Source/SWC" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Source-2f-SWC-2f-FAN

clean-Source-2f-SWC-2f-FAN:
	-$(RM) ./Source/SWC/FAN/FAN.cyclo ./Source/SWC/FAN/FAN.d ./Source/SWC/FAN/FAN.o ./Source/SWC/FAN/FAN.su

.PHONY: clean-Source-2f-SWC-2f-FAN

