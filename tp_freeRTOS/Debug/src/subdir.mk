################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/cr_startup_lpc176x.c \
../src/main.c 

OBJS += \
./src/cr_startup_lpc176x.o \
./src/main.o 

C_DEPS += \
./src/cr_startup_lpc176x.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/cr_startup_lpc176x.o: ../src/cr_startup_lpc176x.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DDEBUG -D__CODE_RED -D__USE_CMSIS=CMSISv1p30_LPC17xx -I"/home/pato/workspace/FreeRTOS_Library/demo_code" -I"/home/pato/workspace/FreeRTOS_Library/include" -I"/home/pato/workspace/FreeRTOS_Library/portable" -I"/home/pato/workspace/Lib_CMSIS2p0_and_Drivers_17XX/Drivers/include" -I"/home/pato/workspace/Lib_CMSIS2p0_and_Drivers_17XX/Core/CM3/CoreSupport" -I"/home/pato/workspace/Lib_CMSIS2p0_and_Drivers_17XX/Core/CM3/DeviceSupport/NXP/LPC17xx" -I"/home/pato/workspace/CMSISv1p30_LPC17xx/inc" -Os -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"src/cr_startup_lpc176x.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DDEBUG -D__CODE_RED -D__USE_CMSIS=CMSISv1p30_LPC17xx -I"/home/pato/workspace/FreeRTOS_Library/demo_code" -I"/home/pato/workspace/FreeRTOS_Library/include" -I"/home/pato/workspace/FreeRTOS_Library/portable" -I"/home/pato/workspace/Lib_CMSIS2p0_and_Drivers_17XX/Drivers/include" -I"/home/pato/workspace/Lib_CMSIS2p0_and_Drivers_17XX/Core/CM3/CoreSupport" -I"/home/pato/workspace/Lib_CMSIS2p0_and_Drivers_17XX/Core/CM3/DeviceSupport/NXP/LPC17xx" -I"/home/pato/workspace/CMSISv1p30_LPC17xx/inc" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

