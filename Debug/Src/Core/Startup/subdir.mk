################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Src/Core/Startup/startup_stm32g431kbtx.s 

OBJS += \
./Src/Core/Startup/startup_stm32g431kbtx.o 

S_DEPS += \
./Src/Core/Startup/startup_stm32g431kbtx.d 


# Each subdirectory must supply rules for building sources it contributes
Src/Core/Startup/%.o: ../Src/Core/Startup/%.s Src/Core/Startup/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -DDEBUG -c -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-Src-2f-Core-2f-Startup

clean-Src-2f-Core-2f-Startup:
	-$(RM) ./Src/Core/Startup/startup_stm32g431kbtx.d ./Src/Core/Startup/startup_stm32g431kbtx.o

.PHONY: clean-Src-2f-Core-2f-Startup

