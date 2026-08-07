################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/Core/BissReader/Breader_Biss.c \
../Src/Core/BissReader/Breader_Dma.c \
../Src/Core/BissReader/Breader_I2c.c \
../Src/Core/BissReader/Breader_Lcd.c \
../Src/Core/BissReader/Breader_Main.c \
../Src/Core/BissReader/Breader_Spi.c \
../Src/Core/BissReader/Breader_Timer.c \
../Src/Core/BissReader/Breader_Usart.c \
../Src/Core/BissReader/stm32g4xx_it.c \
../Src/Core/BissReader/syscalls.c \
../Src/Core/BissReader/sysmem.c \
../Src/Core/BissReader/system_stm32g4xx.c 

OBJS += \
./Src/Core/BissReader/Breader_Biss.o \
./Src/Core/BissReader/Breader_Dma.o \
./Src/Core/BissReader/Breader_I2c.o \
./Src/Core/BissReader/Breader_Lcd.o \
./Src/Core/BissReader/Breader_Main.o \
./Src/Core/BissReader/Breader_Spi.o \
./Src/Core/BissReader/Breader_Timer.o \
./Src/Core/BissReader/Breader_Usart.o \
./Src/Core/BissReader/stm32g4xx_it.o \
./Src/Core/BissReader/syscalls.o \
./Src/Core/BissReader/sysmem.o \
./Src/Core/BissReader/system_stm32g4xx.o 

C_DEPS += \
./Src/Core/BissReader/Breader_Biss.d \
./Src/Core/BissReader/Breader_Dma.d \
./Src/Core/BissReader/Breader_I2c.d \
./Src/Core/BissReader/Breader_Lcd.d \
./Src/Core/BissReader/Breader_Main.d \
./Src/Core/BissReader/Breader_Spi.d \
./Src/Core/BissReader/Breader_Timer.d \
./Src/Core/BissReader/Breader_Usart.d \
./Src/Core/BissReader/stm32g4xx_it.d \
./Src/Core/BissReader/syscalls.d \
./Src/Core/BissReader/sysmem.d \
./Src/Core/BissReader/system_stm32g4xx.d 


# Each subdirectory must supply rules for building sources it contributes
Src/Core/BissReader/%.o Src/Core/BissReader/%.su Src/Core/BissReader/%.cyclo: ../Src/Core/BissReader/%.c Src/Core/BissReader/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G431xx -c -I../Src/Core/Inc -I../Src/Drivers/STM32G4xx_HAL_Driver/Inc -I../Src/Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Src/Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Src/Drivers/CMSIS/Include -I../Src/Drivers/STM32G4xx_HAL_Driver -I../Src/Core/EncoderReader -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src-2f-Core-2f-BissReader

clean-Src-2f-Core-2f-BissReader:
	-$(RM) ./Src/Core/BissReader/Breader_Biss.cyclo ./Src/Core/BissReader/Breader_Biss.d ./Src/Core/BissReader/Breader_Biss.o ./Src/Core/BissReader/Breader_Biss.su ./Src/Core/BissReader/Breader_Dma.cyclo ./Src/Core/BissReader/Breader_Dma.d ./Src/Core/BissReader/Breader_Dma.o ./Src/Core/BissReader/Breader_Dma.su ./Src/Core/BissReader/Breader_I2c.cyclo ./Src/Core/BissReader/Breader_I2c.d ./Src/Core/BissReader/Breader_I2c.o ./Src/Core/BissReader/Breader_I2c.su ./Src/Core/BissReader/Breader_Lcd.cyclo ./Src/Core/BissReader/Breader_Lcd.d ./Src/Core/BissReader/Breader_Lcd.o ./Src/Core/BissReader/Breader_Lcd.su ./Src/Core/BissReader/Breader_Main.cyclo ./Src/Core/BissReader/Breader_Main.d ./Src/Core/BissReader/Breader_Main.o ./Src/Core/BissReader/Breader_Main.su ./Src/Core/BissReader/Breader_Spi.cyclo ./Src/Core/BissReader/Breader_Spi.d ./Src/Core/BissReader/Breader_Spi.o ./Src/Core/BissReader/Breader_Spi.su ./Src/Core/BissReader/Breader_Timer.cyclo ./Src/Core/BissReader/Breader_Timer.d ./Src/Core/BissReader/Breader_Timer.o ./Src/Core/BissReader/Breader_Timer.su ./Src/Core/BissReader/Breader_Usart.cyclo ./Src/Core/BissReader/Breader_Usart.d ./Src/Core/BissReader/Breader_Usart.o ./Src/Core/BissReader/Breader_Usart.su ./Src/Core/BissReader/stm32g4xx_it.cyclo ./Src/Core/BissReader/stm32g4xx_it.d ./Src/Core/BissReader/stm32g4xx_it.o ./Src/Core/BissReader/stm32g4xx_it.su ./Src/Core/BissReader/syscalls.cyclo ./Src/Core/BissReader/syscalls.d ./Src/Core/BissReader/syscalls.o ./Src/Core/BissReader/syscalls.su ./Src/Core/BissReader/sysmem.cyclo ./Src/Core/BissReader/sysmem.d ./Src/Core/BissReader/sysmem.o ./Src/Core/BissReader/sysmem.su ./Src/Core/BissReader/system_stm32g4xx.cyclo ./Src/Core/BissReader/system_stm32g4xx.d ./Src/Core/BissReader/system_stm32g4xx.o ./Src/Core/BissReader/system_stm32g4xx.su

.PHONY: clean-Src-2f-Core-2f-BissReader

