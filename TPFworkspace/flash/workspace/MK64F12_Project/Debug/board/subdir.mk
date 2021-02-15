################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../board/board.c \
../board/clock_config.c \
../board/peripherals.c \
../board/pin_mux.c 

OBJS += \
./board/board.o \
./board/clock_config.o \
./board/peripherals.o \
./board/pin_mux.o 

C_DEPS += \
./board/board.d \
./board/clock_config.d \
./board/peripherals.d \
./board/pin_mux.d 


# Each subdirectory must supply rules for building sources it contributes
board/%.o: ../board/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MK64FN1M0VLL12 -DCPU_MK64FN1M0VLL12_cm4 -DFSL_RTOS_BM -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=0 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -DSERIAL_PORT_TYPE_UART=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -D__REDLIB__ -I"C:\Users\Matias\Desktop\ITBA\TPF-LABmicros\TPFworkspace\flash\workspace\MK64F12_Project\board" -I"C:\Users\Matias\Desktop\ITBA\TPF-LABmicros\TPFworkspace\flash\workspace\MK64F12_Project\source" -I"C:\Users\Matias\Desktop\ITBA\TPF-LABmicros\TPFworkspace\flash\workspace\MK64F12_Project" -I"C:\Users\Matias\Desktop\ITBA\TPF-LABmicros\TPFworkspace\flash\workspace\MK64F12_Project\drivers" -I"C:\Users\Matias\Desktop\ITBA\TPF-LABmicros\TPFworkspace\flash\workspace\MK64F12_Project\utilities" -I"C:\Users\Matias\Desktop\ITBA\TPF-LABmicros\TPFworkspace\flash\workspace\MK64F12_Project\component\serial_manager" -I"C:\Users\Matias\Desktop\ITBA\TPF-LABmicros\TPFworkspace\flash\workspace\MK64F12_Project\component\uart" -I"C:\Users\Matias\Desktop\ITBA\TPF-LABmicros\TPFworkspace\flash\workspace\MK64F12_Project\component\lists" -I"C:\Users\Matias\Desktop\ITBA\TPF-LABmicros\TPFworkspace\flash\workspace\MK64F12_Project\device" -I"C:\Users\Matias\Desktop\ITBA\TPF-LABmicros\TPFworkspace\flash\workspace\MK64F12_Project\CMSIS" -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

