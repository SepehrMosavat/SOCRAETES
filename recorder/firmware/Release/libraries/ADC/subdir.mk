################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
C:\Program\ Files\ (x86)\Arduino\hardware\teensy\avr\libraries\ADC\ADC.cpp \
C:\Program\ Files\ (x86)\Arduino\hardware\teensy\avr\libraries\ADC\ADC_Module.cpp \
C:\Program\ Files\ (x86)\Arduino\hardware\teensy\avr\libraries\ADC\AnalogBufferDMA.cpp 

LINK_OBJ += \
.\libraries\ADC\ADC.cpp.o \
.\libraries\ADC\ADC_Module.cpp.o \
.\libraries\ADC\AnalogBufferDMA.cpp.o 

CPP_DEPS += \
.\libraries\ADC\ADC.cpp.d \
.\libraries\ADC\ADC_Module.cpp.d \
.\libraries\ADC\AnalogBufferDMA.cpp.d 


# Each subdirectory must supply rules for building sources it contributes
libraries\ADC\ADC.cpp.o: C:\Program\ Files\ (x86)\Arduino\hardware\teensy\avr\libraries\ADC\ADC.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"C:/Program Files (x86)/Arduino/hardware/teensy/../tools/arm/bin/arm-none-eabi-g++" -c -O3 -flto -fno-fat-lto-objects -g -Wall -ffunction-sections -fdata-sections -nostdlib -MMD -fno-exceptions -fpermissive -felide-constructors -std=gnu++14 -Wno-error=narrowing -fno-rtti -mthumb -mcpu=cortex-m4 -fsingle-precision-constant -D__MK20DX256__ -DTEENSYDUINO=153 -DARDUINO=10812 -DARDUINO_TEENSY32 -DF_CPU=96000000 -DUSB_SERIAL -DLAYOUT_US_INTERNATIONAL "-IC:\eclipse_workspace\SolarCellRecorder_Teensy\Release/pch"   -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\cores\teensy3" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\ADC" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Encoder\utility" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Encoder" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\SPI" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -D__IN_ECLIPSE__=1 -x c++ "$<"   -o "$@"
	@echo 'Finished building: $<'
	@echo ' '

libraries\ADC\ADC_Module.cpp.o: C:\Program\ Files\ (x86)\Arduino\hardware\teensy\avr\libraries\ADC\ADC_Module.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"C:/Program Files (x86)/Arduino/hardware/teensy/../tools/arm/bin/arm-none-eabi-g++" -c -O3 -flto -fno-fat-lto-objects -g -Wall -ffunction-sections -fdata-sections -nostdlib -MMD -fno-exceptions -fpermissive -felide-constructors -std=gnu++14 -Wno-error=narrowing -fno-rtti -mthumb -mcpu=cortex-m4 -fsingle-precision-constant -D__MK20DX256__ -DTEENSYDUINO=153 -DARDUINO=10812 -DARDUINO_TEENSY32 -DF_CPU=96000000 -DUSB_SERIAL -DLAYOUT_US_INTERNATIONAL "-IC:\eclipse_workspace\SolarCellRecorder_Teensy\Release/pch"   -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\cores\teensy3" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\ADC" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Encoder\utility" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Encoder" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\SPI" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -D__IN_ECLIPSE__=1 -x c++ "$<"   -o "$@"
	@echo 'Finished building: $<'
	@echo ' '

libraries\ADC\AnalogBufferDMA.cpp.o: C:\Program\ Files\ (x86)\Arduino\hardware\teensy\avr\libraries\ADC\AnalogBufferDMA.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"C:/Program Files (x86)/Arduino/hardware/teensy/../tools/arm/bin/arm-none-eabi-g++" -c -O3 -flto -fno-fat-lto-objects -g -Wall -ffunction-sections -fdata-sections -nostdlib -MMD -fno-exceptions -fpermissive -felide-constructors -std=gnu++14 -Wno-error=narrowing -fno-rtti -mthumb -mcpu=cortex-m4 -fsingle-precision-constant -D__MK20DX256__ -DTEENSYDUINO=153 -DARDUINO=10812 -DARDUINO_TEENSY32 -DF_CPU=96000000 -DUSB_SERIAL -DLAYOUT_US_INTERNATIONAL "-IC:\eclipse_workspace\SolarCellRecorder_Teensy\Release/pch"   -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\cores\teensy3" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\ADC" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Encoder\utility" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Encoder" -I"C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\SPI" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -D__IN_ECLIPSE__=1 -x c++ "$<"   -o "$@"
	@echo 'Finished building: $<'
	@echo ' '


