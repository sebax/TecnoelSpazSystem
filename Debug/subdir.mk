################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../TECNOEL_CPU40_CARSPA_0000_MAIN.c \
../deparker.c \
../engine.c \
../logger.c \
../parker.c \
../piston.c \
../runner.c \
../safeguard.c \
../traslaholder.c 

OBJS += \
./TECNOEL_CPU40_CARSPA_0000_MAIN.o \
./deparker.o \
./engine.o \
./logger.o \
./parker.o \
./piston.o \
./runner.o \
./safeguard.o \
./traslaholder.o 

C_DEPS += \
./TECNOEL_CPU40_CARSPA_0000_MAIN.d \
./deparker.d \
./engine.d \
./logger.d \
./parker.d \
./piston.d \
./runner.d \
./safeguard.d \
./traslaholder.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


