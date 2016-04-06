################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
PWMlight_mesaure.lss \

FLASH_IMAGE += \
PWMlight_mesaure.hex \

EEPROM_IMAGE += \
PWMlight_mesaure.eep \

SIZEDUMMY += \
sizedummy \

AVRDUDEDUMMY += \
avrdudedummy \


# All Target
all: PWMlight_mesaure.elf secondary-outputs

# Tool invocations
PWMlight_mesaure.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,PWMlight_mesaure.map -mmcu=atmega8 -o "PWMlight_mesaure.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

PWMlight_mesaure.lss: PWMlight_mesaure.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S PWMlight_mesaure.elf  >"PWMlight_mesaure.lss"
	@echo 'Finished building: $@'
	@echo ' '

PWMlight_mesaure.hex: PWMlight_mesaure.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex PWMlight_mesaure.elf  "PWMlight_mesaure.hex"
	@echo 'Finished building: $@'
	@echo ' '

PWMlight_mesaure.eep: PWMlight_mesaure.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex PWMlight_mesaure.elf  "PWMlight_mesaure.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: PWMlight_mesaure.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega8 PWMlight_mesaure.elf
	@echo 'Finished building: $@'
	@echo ' '

avrdudedummy: PWMlight_mesaure.elf
	@echo 'Invoking: AVRDude'
	/usr/bin/avrdude -pm8 -cusbasp -u -Uflash:w:PWMlight_mesaure.hex:a -Ulfuse:w:0xe4:m -Uhfuse:w:0xd9:m
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(EEPROM_IMAGE)$(FLASH_IMAGE)$(ELFS)$(LSS)$(AVRDUDEDUMMY)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS) PWMlight_mesaure.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY) $(AVRDUDEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
