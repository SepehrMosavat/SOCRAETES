################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ..\makefile.init

RM := -del 

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include libraries\SPI\subdir.mk
-include libraries\Encoder\subdir.mk
-include libraries\ADC\subdir.mk
-include core\core\subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ..\makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
AR += \
C:\eclipse_workspace\SolarCellRecorder_Teensy\Release\arduino.ar \

ELF += \
SolarCellRecorder_Teensy.elf \

HEX += \
SolarCellRecorder_Teensy.hex \


# All Target
all: SolarCellRecorder_Teensy

# Tool invocations
SolarCellRecorder_Teensy: $(HEX)
	@echo 'Building target: $@'
	@echo 'Printing size:'
	"C:/Program Files (x86)/Arduino/hardware/teensy/../tools/arm/bin/arm-none-eabi-size" -A "C:\eclipse_workspace\SolarCellRecorder_Teensy\Release/SolarCellRecorder_Teensy.elf"
	@echo 'Finished building target: $@'
	@echo ' '

C:\eclipse_workspace\SolarCellRecorder_Teensy\Release\arduino.ar: $(AR_OBJ)
	@echo 'Starting archiver'
	"C:/Program Files (x86)/Arduino/hardware/teensy/../tools/arm/bin/arm-none-eabi-gcc-ar" rcs    C:\eclipse_workspace\SolarCellRecorder_Teensy\Release/arduino.ar $(AR_OBJ)
	@echo 'Finished building: $@'
	@echo ' '

SolarCellRecorder_Teensy.elf: $(LINK_OBJ) $(AR)
	@echo 'Starting combiner'
	"C:/Program Files (x86)/Arduino/hardware/teensy/../tools/arm/bin/arm-none-eabi-gcc" -O3 -flto -fno-fat-lto-objects -Wl,--gc-sections,--relax,--defsym=__rtc_localtime=1600336177 "-TC:\Program Files (x86)\Arduino\hardware\teensy\avr\cores\teensy3/mk20dx256.ld" -fuse-linker-plugin -mthumb -mcpu=cortex-m4 -fsingle-precision-constant -o "C:\eclipse_workspace\SolarCellRecorder_Teensy\Release/SolarCellRecorder_Teensy.elf"   $(LINK_OBJ)   C:\eclipse_workspace\SolarCellRecorder_Teensy\Release/arduino.ar  "-LC:\eclipse_workspace\SolarCellRecorder_Teensy\Release" -larm_cortexM4l_math -lm -lstdc++
	@echo 'Finished building: $@'
	@echo ' '

SolarCellRecorder_Teensy.hex: $(ELF)
	@echo 'Do all objcopy commands'
	${A.RECIPE.OBJCOPY.EEP.PATTERN}
	${A.RECIPE.OBJCOPY.HEX.PATTERN}
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(C_UPPER_DEPS)$(PDE_DEPS)$(C_DEPS)$(AR)$(CC_DEPS)$(AR_OBJ)$(C++_DEPS)$(LINK_OBJ)$(CXX_DEPS)$(ASM_DEPS)$(HEX)$(INO_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS)$(ELF)$(CPP_DEPS)
	-@echo ' '

.PHONY: all clean dependents

-include ..\makefile.targets
