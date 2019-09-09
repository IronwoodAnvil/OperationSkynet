################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include stm32lib/STM32F7xx_HAL_Driver/Src/subdir.mk
-include stm32lib/CMSIS/Device/ST/STM32F7xx/Source/Templates/subdir.mk
-include startup/subdir.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
LAB-01-Template.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
LAB-01-Template.list \

OBJCOPY_BIN += \
LAB-01-Template.bin \


# All Target
all: LAB-01-Template.elf secondary-outputs

# Tool invocations
LAB-01-Template.elf: $(OBJS) $(USER_OBJS) C:\Users\lovep\Documents\School\19F\MPS\workspace\LAB-01-Template\LinkerScript.ld
	arm-none-eabi-gcc -o "LAB-01-Template.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m7 -T"C:\Users\lovep\Documents\School\19F\MPS\workspace\LAB-01-Template\LinkerScript.ld" --specs=nosys.specs -Wl,-Map="LAB-01-Template.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

LAB-01-Template.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "LAB-01-Template.list"
	@echo 'Finished building: $@'
	@echo ' '

LAB-01-Template.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "LAB-01-Template.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
