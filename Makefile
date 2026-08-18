#
# Explanations
# ------------
#
# === General Targets ===
#
# make						Runs make all
#
# make all					Builds the kernel as kernel.elf and generates a dump
#
# make rebuild				Forces a clean rebuild: make clean && make all.
#
# make clean				Removes build files.
#
# make qemu					Builds the kernel and runs it in QEMU.
#
# make qemu_debug			Builds the kernel and runs it in QEMU with debugging options
# 							enabled. To debug in a second terminal, run:
# 							$ arm-none-eabi-gdb build/kernel.elf
# 							$ target remote localhost:1234
#
# make qemu_trace			Same as make qemu, but also prints certain MMIO accesses.
#
# make qemu_trace_mmu		Same as make qemu_trace, but only prints MMU-related messages.
#
# make gdb					Starts gdb and conntects to QEMU.
#
# make install				Builds kernel.img and transfers it to the server.
#  							The board will load it on the next reset.
#
# make home					Copies the final image to $(TFTP_PATH)
#
# make format				Formats all .c and .h files
#
#
# === Single file targets ===
#
# make kernel				Builds kernel.elf into build/ directory.
#
# make kernel.bin			Builds kernel.bin into build/ directory.
#
# make kernel.img			Builds kernel.img into build/ directory.
#
#
# === Other notes ===
#
# MODE=strict make TARGET	Builds TARGET with extra warnings from GCC and 
# 							enables undefined-behavior sanitizers.
# 

# ----------------------------
# Configurations
# ----------------------------

# Source files
SRC = \
	arch/armv7/boot.s \
	arch/armv7/handlers.c \
	arch/armv7/handlers_asm.S \
	arch/armv7/vectors.S \
	board/rpi2b/timer.c \
	board/rpi2b/uart.c \
	board/rpi2b/status_led.c \
	kernel/exception_print.c \
	kernel/kprintf.c \
	kernel/start.c \
	lib/mem.c

# User source files
USRC = user/main.c

# Compiler flags
CFLAGS = -std=gnu17

# Linker flags
LDFLAGS = -T$(LSCRIPT) -nostdlib --fatal-warnings

# Strict mode: MODE=strict make ...
ifeq ($(MODE), strict)
	CFLAGS += -Werror -fsanitize=undefined
	LDFLAGS += -fsanitize=undefined
endif

# Board image upload path
TFTP_PATH = /srv/tftp

# Toolchain install path
ARM_PREFIX ?= /Applications/ArmGNUToolchain/14.2.rel1/arm-none-eabi
QEMU_PREFIX ?= /opt/local

# Build directory
BUILD_DIR = build

# Linker script
LSCRIPT = board/rpi2b/linker.ld

# Split kernel sources -> objects in build/
KSRC_C 		= $(filter %.c, $(SRC))
KSRC_S_CAP 	= $(filter %.S, $(SRC))
KSRC_S_LOW 	= $(filter %.s, $(SRC))
KOBJ_C 		= $(addprefix $(BUILD_DIR)/,$(KSRC_C:%.c=%.o))
KOBJ_S_CAP 	= $(addprefix $(BUILD_DIR)/,$(KSRC_S_CAP:%.S=%.o))
KOBJ_S_LOW 	= $(addprefix $(BUILD_DIR)/,$(KSRC_S_LOW:%.s=%.o))
KOBJ 		= $(KOBJ_C) $(KOBJ_S_CAP) $(KOBJ_S_LOW)

# Split user sources -> objects in build/
USRC_C = $(filter %.c, $(USRC))
USRC_S = $(filter %.S, $(USRC))
UOBJ_C = $(addprefix $(BUILD_DIR)/,$(USRC_C:%.c=%.o))
UOBJ_S = $(addprefix $(BUILD_DIR)/,$(USRC_S:%.S=%.o))
UOBJ =  $(UOBJ_C) $(UOBJ_S)

# Combined objects
OBJ_C = $(KOBJ_C) $(UOBJ_C)
OBJ_S = $(KOBJ_S_CAP) $(KOBJ_S_LOW) $(UOBJ_S)
OBJ = $(KOBJ) $(UOBJ)

# Dependencies
DEP = $(OBJ:.o=.d)

# Toolchain commands
CC = $(ARM_PREFIX)/bin/arm-none-eabi-gcc
LD = $(ARM_PREFIX)/bin/arm-none-eabi-ld
OBJCOPY = $(ARM_PREFIX)/bin/arm-none-eabi-objcopy
OBJDUMP = $(ARM_PREFIX)/bin/arm-none-eabi-objdump
IMG = $(ARM_PREFIX)/bin/mkimage
QEMU = $(QEMU_PREFIX)/bin/qemu-system-arm
QEMU_IMG = $(QEMU_PREFIX)/bin/qemu-img
GDB = $(ARM_PREFIX)/bin/arm-none-eabi-gdb
FORMAT = $(ARM_PREFIX)/bin/clang-format

# Flags
CFLAGS += -Wall -Wextra -ggdb -ffreestanding \
		  -mcpu=cortex-a7 -mfloat-abi=soft -mno-unaligned-access
CFLAGS += -Iinclude

OBJCOPYFLAGS = -Obinary -S --set-section-flags .bss=contents,alloc,load,data
OBJDUMPFLAGS = -dfzh
IMGFLAGS = -A arm -T standalone -C none -a 0x8000

QEMUFLAGS = -M raspi2b -nographic -icount shift=9
QEMU_REPLAY_FLAGS = -icount rrfile=$(BUILD_DIR)/replay.bin

FORMATFLAGS = --style="file:.clang-format" -i


# ----------------------------
# Rules
# ----------------------------

.PHONY: all kernel kernel.bin kernel.img install clean qemu gdb rebuild

# Default target
all: kernel dump

rebuild: clean all

# Include auto generated targets
-include $(DEP)

# Single file targets
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -MP -o $@ -c $<

$(BUILD_DIR)/%.o: %.S
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -MP -o $@ -c $<

$(BUILD_DIR)/%.o: %.s
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -MP -o $@ -c $<

$(BUILD_DIR)/kernel.elf: $(OBJ) $(LSCRIPT)
ifneq ($(words $(OBJ)), $(words $(SRC) $(USRC)))
	@echo The number of objects files given to the linker does not match the\
		number of kernel and user files specified. Check your file extensions.; exit 1
else
	$(LD) -o $@ $(wordlist 2,$(words $^),x $^) $(LDFLAGS)
endif


$(BUILD_DIR)/kernel.bin: $(BUILD_DIR)/kernel.elf
	$(OBJCOPY) $(OBJCOPYFLAGS) $< $@

$(BUILD_DIR)/kernel.img: $(BUILD_DIR)/kernel.bin
	$(IMG) $(IMGFLAGS) -d $< $@

$(BUILD_DIR)/kernel_dump.s: $(BUILD_DIR)/kernel.elf
	$(OBJDUMP) $(OBJDUMPFLAGS) $< > $@


dump: $(BUILD_DIR)/kernel_dump.s
kernel: $(BUILD_DIR)/kernel.elf
kernel.bin: $(BUILD_DIR)/kernel.bin
kernel.img: $(BUILD_DIR)/kernel.img


# General targets
install: $(BUILD_DIR)/kernel.img
	arm-install-image $(BUILD_DIR)/kernel.img

home: $(BUILD_DIR)/kernel.img
	cp -v $(BUILD_DIR)/kernel.img $(TFTP_PATH)

$(BUILD_DIR)/vmstate.qcow2: $(BUILD_DIR)/kernel.elf
	mkdir -p $(BUILD_DIR)
	$(QEMU_IMG) create -f qcow2 $@ 32M

qemu: $(BUILD_DIR)/kernel.elf
	$(QEMU) $(QEMUFLAGS) -kernel $(BUILD_DIR)/kernel.elf

qemu_debug: $(BUILD_DIR)/kernel.elf
	$(QEMU) $(QEMUFLAGS) -s -S -kernel $(BUILD_DIR)/kernel.elf

qemu_trace: $(BUILD_DIR)/kernel.elf
	$(QEMU) $(QEMUFLAGS) -d "trace:bcm283*,trace:pl011*" -kernel $<

qemu_trace_mmu: $(BUILD_DIR)/kernel.elf
	$(QEMU) $(QEMUFLAGS) -d "mmu" -kernel $<

gdb: $(BUILD_DIR)/kernel.elf
	$(GDB) --ex="target remote localhost:1234" --ex "set scheduler-locking on" $<

format:
	@$(FORMAT) $(FORMATFLAGS) $$(find . -name '*.c' -o -name '*.h')
	@echo "Code formatted."

clean:
	rm -rf $(BUILD_DIR)
