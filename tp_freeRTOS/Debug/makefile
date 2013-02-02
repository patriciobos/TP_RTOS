################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
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

# All Target
all: tp_freeRTOS.axf

# Tool invocations
tp_freeRTOS.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"/home/pato/workspace/FreeRTOS_Library/demo_code" -L"/home/pato/workspace/Lib_CMSIS2p0_and_Drivers_17XX/Debug" -L"/home/pato/workspace/FreeRTOS_Library/Debug" -L"/home/pato/workspace/CMSISv1p30_LPC17xx/Debug" -Xlinker -Map="tp_freeRTOS.map" -Xlinker --gc-sections -mcpu=cortex-m3 -mthumb -T "tp_freeRTOS_Debug.ld" -o "tp_freeRTOS.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) tp_freeRTOS.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "tp_freeRTOS.axf"; # arm-none-eabi-objcopy -O binary "tp_freeRTOS.axf" "tp_freeRTOS.bin" ; checksum -p LPC1769 -d "tp_freeRTOS.bin";
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
