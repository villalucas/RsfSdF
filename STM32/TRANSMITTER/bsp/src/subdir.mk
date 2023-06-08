################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../bsp/src/LED_control.c \
../bsp/src/SX1272.c \
../bsp/src/bsp.c \
../bsp/src/comSX1272.c \
../bsp/src/delay.c \
../bsp/src/frames.c 

OBJS += \
./bsp/src/LED_control.o \
./bsp/src/SX1272.o \
./bsp/src/bsp.o \
./bsp/src/comSX1272.o \
./bsp/src/delay.o \
./bsp/src/frames.o 

C_DEPS += \
./bsp/src/LED_control.d \
./bsp/src/SX1272.d \
./bsp/src/bsp.d \
./bsp/src/comSX1272.d \
./bsp/src/delay.d \
./bsp/src/frames.d 


# Each subdirectory must supply rules for building sources it contributes
bsp/src/%.o bsp/src/%.su: ../bsp/src/%.c bsp/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DSTM32F072xB -DTRANSMITTER=1 -c -I"D:/OneDrive_UM/Polytech/SE/Semestre 10/SDF/Projet/Code/git_repos/RsfSdF/STM32/app/inc" -I"D:/OneDrive_UM/Polytech/SE/Semestre 10/SDF/Projet/Code/git_repos/RsfSdF/STM32/cmsis/core" -I"D:/OneDrive_UM/Polytech/SE/Semestre 10/SDF/Projet/Code/git_repos/RsfSdF/STM32/cmsis/device/inc" -I"D:/OneDrive_UM/Polytech/SE/Semestre 10/SDF/Projet/Code/git_repos/RsfSdF/STM32/bsp/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-bsp-2f-src

clean-bsp-2f-src:
	-$(RM) ./bsp/src/LED_control.d ./bsp/src/LED_control.o ./bsp/src/LED_control.su ./bsp/src/SX1272.d ./bsp/src/SX1272.o ./bsp/src/SX1272.su ./bsp/src/bsp.d ./bsp/src/bsp.o ./bsp/src/bsp.su ./bsp/src/comSX1272.d ./bsp/src/comSX1272.o ./bsp/src/comSX1272.su ./bsp/src/delay.d ./bsp/src/delay.o ./bsp/src/delay.su ./bsp/src/frames.d ./bsp/src/frames.o ./bsp/src/frames.su

.PHONY: clean-bsp-2f-src

