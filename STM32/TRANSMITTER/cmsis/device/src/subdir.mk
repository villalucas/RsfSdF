################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../cmsis/device/src/startup_stm32f072xb.s 

C_SRCS += \
../cmsis/device/src/system_stm32f0xx.c 

OBJS += \
./cmsis/device/src/startup_stm32f072xb.o \
./cmsis/device/src/system_stm32f0xx.o 

S_DEPS += \
./cmsis/device/src/startup_stm32f072xb.d 

C_DEPS += \
./cmsis/device/src/system_stm32f0xx.d 


# Each subdirectory must supply rules for building sources it contributes
cmsis/device/src/%.o: ../cmsis/device/src/%.s cmsis/device/src/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m0 -g3 -c -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@" "$<"
cmsis/device/src/%.o cmsis/device/src/%.su: ../cmsis/device/src/%.c cmsis/device/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DSTM32F072xB -DTRANSMITTER=1 -c -I"D:/OneDrive_UM/Polytech/SE/Semestre 10/SDF/Projet/Code/git_repos/RsfSdF/STM32/app/inc" -I"D:/OneDrive_UM/Polytech/SE/Semestre 10/SDF/Projet/Code/git_repos/RsfSdF/STM32/cmsis/core" -I"D:/OneDrive_UM/Polytech/SE/Semestre 10/SDF/Projet/Code/git_repos/RsfSdF/STM32/cmsis/device/inc" -I"D:/OneDrive_UM/Polytech/SE/Semestre 10/SDF/Projet/Code/git_repos/RsfSdF/STM32/bsp/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-cmsis-2f-device-2f-src

clean-cmsis-2f-device-2f-src:
	-$(RM) ./cmsis/device/src/startup_stm32f072xb.d ./cmsis/device/src/startup_stm32f072xb.o ./cmsis/device/src/system_stm32f0xx.d ./cmsis/device/src/system_stm32f0xx.o ./cmsis/device/src/system_stm32f0xx.su

.PHONY: clean-cmsis-2f-device-2f-src

