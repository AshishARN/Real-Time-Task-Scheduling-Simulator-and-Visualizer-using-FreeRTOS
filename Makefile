# Define the C compiler and flags
CC = arm-none-eabi-gcc
CFLAGS = -mcpu=cortex-m3 -mthumb -std=c11 -O0 -g -ffreestanding -nostdlib

# Define the final output file name
TARGET = freertos_project

# List of all C and Assembly source files
C_SOURCES = main.c
S_SOURCES = startup.s lib.s

# FreeRTOS source files --- PATHS UPDATED HERE ---
FREERTOS_SOURCES = \
FreeRTOS-Kernel/source/tasks.c \
FreeRTOS-Kernel/source/queue.c \
FreeRTOS-Kernel/source/list.c \
FreeRTOS-Kernel/source/timers.c \
FreeRTOS-Kernel/source/portable/MemMang/heap_4.c \
FreeRTOS-Kernel/source/portable/GCC/ARM_CM3/port.c

# Include paths for headers --- PATHS UPDATED HERE ---
INCLUDE_PATHS = -I./ -I./FreeRTOS-Kernel/source/include -I./FreeRTOS-Kernel/source/portable/GCC/ARM_CM3

# Automatically generate object file names from source files
OBJS = $(C_SOURCES:.c=.o) $(S_SOURCES:.s=.o) $(FREERTOS_SOURCES:.c=.o)

# Default target
all: $(TARGET).elf

# Rule to link the final ELF file
$(TARGET).elf: $(OBJS) linker.ld
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -T linker.ld -o $@ $(OBJS)

# Rule to compile C files into object files
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -c $< -o $@

# Rule to assemble files into object files
%.o: %.s
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -c $< -o $@

# Target to run the application in QEMU
run: all
#qemu-system-arm -M mps2-an385 -nographic -nodefaults -serial stdio -kernel $(TARGET).elf
	qemu-system-arm -M lm3s6965evb -nographic -nodefaults -serial stdio -kernel $(TARGET).elf

# Target to clean up the build directory
clean:
	rm -f *.o *.elf $(FREERTOS_SOURCES:.c=.o)

# (Your existing Makefile rules are above this)

# Target to run QEMU in a state paused for a debugger
debug: all
	qemu-system-arm -M lm3s6965evb -nographic -nodefaults -serial stdio \
	-kernel $(TARGET).elf -S -s