################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../buffer.c \
../lcd.c \
../main.c \
../rprintf.c \
../uart.c 

OBJS += \
./buffer.o \
./lcd.o \
./main.o \
./rprintf.o \
./uart.o 

C_DEPS += \
./buffer.d \
./lcd.d \
./main.d \
./rprintf.d \
./uart.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I"/home/gehorvath/avr_workspace/PWMlight_mesaure" -Wall -g2 -gstabs -O2 -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega8 -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

