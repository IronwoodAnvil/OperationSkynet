################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../startup/startup_stm32f769nihx.s 

OBJS += \
./startup/startup_stm32f769nihx.o 


# Each subdirectory must supply rules for building sources it contributes
startup/%.o: ../startup/%.s
	arm-none-eabi-gcc -mcpu=cortex-m7 -g3 -c -I"C:/Users/lovep/Documents/School/19F/MPS/workspace/LAB-01-Template/inc" -x assembler-with-cpp --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

