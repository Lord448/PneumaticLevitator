################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/LCD/images.c \
../Drivers/LCD/lcd.c 

OBJS += \
./Drivers/LCD/images.o \
./Drivers/LCD/lcd.o 

C_DEPS += \
./Drivers/LCD/images.d \
./Drivers/LCD/lcd.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/LCD/%.o Drivers/LCD/%.su Drivers/LCD/%.cyclo: ../Drivers/LCD/%.c Drivers/LCD/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I"/home/lord448/Documentos/TEC/9no/ControlDigital/PneumaticLevitator/PeripherialTest/Drivers/LCD" -I"/home/lord448/Documentos/TEC/9no/ControlDigital/PneumaticLevitator/PeripherialTest/Drivers/UGUI" -I"/home/lord448/Documentos/TEC/9no/ControlDigital/PneumaticLevitator/PeripherialTest/Drivers/UGUI/Fonts" -I"/home/lord448/Documentos/TEC/9no/ControlDigital/PneumaticLevitator/PeripherialTest/Drivers/VL53L0X/Api/core/inc" -I"/home/lord448/Documentos/TEC/9no/ControlDigital/PneumaticLevitator/PeripherialTest/Drivers/VL53L0X/Api/platform/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-LCD

clean-Drivers-2f-LCD:
	-$(RM) ./Drivers/LCD/images.cyclo ./Drivers/LCD/images.d ./Drivers/LCD/images.o ./Drivers/LCD/images.su ./Drivers/LCD/lcd.cyclo ./Drivers/LCD/lcd.d ./Drivers/LCD/lcd.o ./Drivers/LCD/lcd.su

.PHONY: clean-Drivers-2f-LCD
