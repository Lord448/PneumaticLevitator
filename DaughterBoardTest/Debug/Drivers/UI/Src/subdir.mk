################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/UI/Src/Bitmaps.c \
../Drivers/UI/Src/UI.c 

OBJS += \
./Drivers/UI/Src/Bitmaps.o \
./Drivers/UI/Src/UI.o 

C_DEPS += \
./Drivers/UI/Src/Bitmaps.d \
./Drivers/UI/Src/UI.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/UI/Src/%.o Drivers/UI/Src/%.su Drivers/UI/Src/%.cyclo: ../Drivers/UI/Src/%.c Drivers/UI/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/home/lord448/Documentos/TEC/9no/ControlDigital/PneumaticLevitator/DaughterBoardTest/Drivers/LCD" -I"/home/lord448/Documentos/TEC/9no/ControlDigital/PneumaticLevitator/DaughterBoardTest/Drivers/UGUI" -I"/home/lord448/Documentos/TEC/9no/ControlDigital/PneumaticLevitator/DaughterBoardTest/Drivers/UGUI/Fonts" -I"/home/lord448/Documentos/TEC/9no/ControlDigital/PneumaticLevitator/DaughterBoardTest/Drivers/UI/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-UI-2f-Src

clean-Drivers-2f-UI-2f-Src:
	-$(RM) ./Drivers/UI/Src/Bitmaps.cyclo ./Drivers/UI/Src/Bitmaps.d ./Drivers/UI/Src/Bitmaps.o ./Drivers/UI/Src/Bitmaps.su ./Drivers/UI/Src/UI.cyclo ./Drivers/UI/Src/UI.d ./Drivers/UI/Src/UI.o ./Drivers/UI/Src/UI.su

.PHONY: clean-Drivers-2f-UI-2f-Src

