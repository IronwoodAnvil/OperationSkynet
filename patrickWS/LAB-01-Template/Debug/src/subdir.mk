################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ansi_util.c \
../src/circ_buf.c \
../src/coord.c \
../src/gpio.c \
../src/init.c \
../src/main.c \
../src/maze.c \
../src/maze_gen.c \
../src/uart.c 

OBJS += \
./src/ansi_util.o \
./src/circ_buf.o \
./src/coord.o \
./src/gpio.o \
./src/init.o \
./src/main.o \
./src/maze.o \
./src/maze_gen.o \
./src/uart.o 

C_DEPS += \
./src/ansi_util.d \
./src/circ_buf.d \
./src/coord.d \
./src/gpio.d \
./src/init.d \
./src/main.d \
./src/maze.d \
./src/maze_gen.d \
./src/uart.d 


# Each subdirectory must supply rules for building sources it contributes
src/ansi_util.o: ../src/ansi_util.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F769xx -DUSE_STM32F769I_DISCO -c -I"C:/Users/lovep/Documents/School/19F/MPS/workspace/LAB-01-Template/inc" -I"C:/Users/lovep/Documents/School/19F/MPS/workspace/stm32lib/CMSIS/Include" -I"C:/Users/lovep/Documents/School/19F/MPS/workspace/stm32lib/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/Users/lovep/Documents/School/19F/MPS/workspace/stm32lib/STM32F7xx_HAL_Driver/Inc" -Og -ffunction-sections -Wall -Wdouble-promotion -fstack-usage -MMD -MP -MF"src/ansi_util.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
src/circ_buf.o: ../src/circ_buf.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F769xx -DUSE_STM32F769I_DISCO -c -I"C:/Users/lovep/Documents/School/19F/MPS/workspace/LAB-01-Template/inc" -I"C:/Users/lovep/Documents/School/19F/MPS/workspace/stm32lib/CMSIS/Include" -I"C:/Users/lovep/Documents/School/19F/MPS/workspace/stm32lib/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/Users/lovep/Documents/School/19F/MPS/workspace/stm32lib/STM32F7xx_HAL_Driver/Inc" -Og -ffunction-sections -Wall -Wdouble-promotion -fstack-usage -MMD -MP -MF"src/circ_buf.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
src/coord.o: ../src/coord.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F769xx -DUSE_STM32F769I_DISCO -c -I"C:/Users/lovep/Documents/School/19F/MPS/workspace/LAB-01-Template/inc" -I"C:/Users/lovep/Documents/School/19F/MPS/workspace/stm32lib/CMSIS/Include" -I"C:/Users/lovep/Documents/School/19F/MPS/workspace/stm32lib/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/Users/lovep/Documents/School/19F/MPS/workspace/stm32lib/STM32F7xx_HAL_Driver/Inc" -Og -ffunction-sections -Wall -Wdouble-promotion -fstack-usage -MMD -MP -MF"src/coord.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
src/gpio.o: ../src/gpio.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F769xx -DUSE_STM32F769I_DISCO -c -I"C:/Users/lovep/Documents/School/19F/MPS/workspace/LAB-01-Template/inc" -I"C:/Users/lovep/Documents/School/19F/MPS/workspace/stm32lib/CMSIS/Include" -I"C:/Users/lovep/Documents/School/19F/MPS/workspace/stm32lib/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/Users/lovep/Documents/School/19F/MPS/workspace/stm32lib/STM32F7xx_HAL_Driver/Inc" -Og -ffunction-sections -Wall -Wdouble-promotion -fstack-usage -MMD -MP -MF"src/gpio.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
src/init.o: ../src/init.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F769xx -DUSE_STM32F769I_DISCO -c -I"C:/Users/lovep/Documents/School/19F/MPS/workspace/LAB-01-Template/inc" -I"C:/Users/lovep/Documents/School/19F/MPS/workspace/stm32lib/CMSIS/Include" -I"C:/Users/lovep/Documents/School/19F/MPS/workspace/stm32lib/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/Users/lovep/Documents/School/19F/MPS/workspace/stm32lib/STM32F7xx_HAL_Driver/Inc" -Og -ffunction-sections -Wall -Wdouble-promotion -fstack-usage -MMD -MP -MF"src/init.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
src/main.o: ../src/main.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F769xx -DUSE_STM32F769I_DISCO -c -I"C:/Users/lovep/Documents/School/19F/MPS/workspace/LAB-01-Template/inc" -I"C:/Users/lovep/Documents/School/19F/MPS/workspace/stm32lib/CMSIS/Include" -I"C:/Users/lovep/Documents/School/19F/MPS/workspace/stm32lib/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/Users/lovep/Documents/School/19F/MPS/workspace/stm32lib/STM32F7xx_HAL_Driver/Inc" -Og -ffunction-sections -Wall -Wdouble-promotion -fstack-usage -MMD -MP -MF"src/main.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
src/maze.o: ../src/maze.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F769xx -DUSE_STM32F769I_DISCO -c -I"C:/Users/lovep/Documents/School/19F/MPS/workspace/LAB-01-Template/inc" -I"C:/Users/lovep/Documents/School/19F/MPS/workspace/stm32lib/CMSIS/Include" -I"C:/Users/lovep/Documents/School/19F/MPS/workspace/stm32lib/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/Users/lovep/Documents/School/19F/MPS/workspace/stm32lib/STM32F7xx_HAL_Driver/Inc" -Og -ffunction-sections -Wall -Wdouble-promotion -fstack-usage -MMD -MP -MF"src/maze.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
src/maze_gen.o: ../src/maze_gen.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F769xx -DUSE_STM32F769I_DISCO -c -I"C:/Users/lovep/Documents/School/19F/MPS/workspace/LAB-01-Template/inc" -I"C:/Users/lovep/Documents/School/19F/MPS/workspace/stm32lib/CMSIS/Include" -I"C:/Users/lovep/Documents/School/19F/MPS/workspace/stm32lib/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/Users/lovep/Documents/School/19F/MPS/workspace/stm32lib/STM32F7xx_HAL_Driver/Inc" -Og -ffunction-sections -Wall -Wdouble-promotion -fstack-usage -MMD -MP -MF"src/maze_gen.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
src/uart.o: ../src/uart.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F769xx -DUSE_STM32F769I_DISCO -c -I"C:/Users/lovep/Documents/School/19F/MPS/workspace/LAB-01-Template/inc" -I"C:/Users/lovep/Documents/School/19F/MPS/workspace/stm32lib/CMSIS/Include" -I"C:/Users/lovep/Documents/School/19F/MPS/workspace/stm32lib/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/Users/lovep/Documents/School/19F/MPS/workspace/stm32lib/STM32F7xx_HAL_Driver/Inc" -Og -ffunction-sections -Wall -Wdouble-promotion -fstack-usage -MMD -MP -MF"src/uart.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

