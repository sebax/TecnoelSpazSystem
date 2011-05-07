################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Moduli/TECNOEL_CPU40_MODBV00_0002.c 

OBJS += \
./Moduli/TECNOEL_CPU40_MODBV00_0002.o 

C_DEPS += \
./Moduli/TECNOEL_CPU40_MODBV00_0002.d 


# Each subdirectory must supply rules for building sources it contributes
Moduli/%.o: ../Moduli/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


