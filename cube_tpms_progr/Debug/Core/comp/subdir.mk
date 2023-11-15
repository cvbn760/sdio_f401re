################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/comp/fw_updater.c \
../Core/comp/gpio.c \
../Core/comp/i2c_manager.c \
../Core/comp/indication.c \
../Core/comp/tasks_cmp.c 

OBJS += \
./Core/comp/fw_updater.o \
./Core/comp/gpio.o \
./Core/comp/i2c_manager.o \
./Core/comp/indication.o \
./Core/comp/tasks_cmp.o 

C_DEPS += \
./Core/comp/fw_updater.d \
./Core/comp/gpio.d \
./Core/comp/i2c_manager.d \
./Core/comp/indication.d \
./Core/comp/tasks_cmp.d 


# Each subdirectory must supply rules for building sources it contributes
Core/comp/%.o Core/comp/%.su Core/comp/%.cyclo: ../Core/comp/%.c Core/comp/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-comp

clean-Core-2f-comp:
	-$(RM) ./Core/comp/fw_updater.cyclo ./Core/comp/fw_updater.d ./Core/comp/fw_updater.o ./Core/comp/fw_updater.su ./Core/comp/gpio.cyclo ./Core/comp/gpio.d ./Core/comp/gpio.o ./Core/comp/gpio.su ./Core/comp/i2c_manager.cyclo ./Core/comp/i2c_manager.d ./Core/comp/i2c_manager.o ./Core/comp/i2c_manager.su ./Core/comp/indication.cyclo ./Core/comp/indication.d ./Core/comp/indication.o ./Core/comp/indication.su ./Core/comp/tasks_cmp.cyclo ./Core/comp/tasks_cmp.d ./Core/comp/tasks_cmp.o ./Core/comp/tasks_cmp.su

.PHONY: clean-Core-2f-comp

