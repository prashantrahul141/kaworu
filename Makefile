CONFIG_FREE_TARGETS := help menuconfig defconfig clean cleanall cleandebug cleanconfig
# Require .config for everything else
ifeq ($(filter $(MAKECMDGOALS),$(CONFIG_FREE_TARGETS)),)
  ifeq ($(wildcard .config),)
    $(error .config was not found, run `make menuconfig` or `make defconfig`. Run `make help` for more info)
  endif
  include .config
endif

CC := clang
LD := ld.lld

LINKER_SCRIPT := kernel.ld
OUT := kaworu.elf

TARGET := aarch64-none-elf
C_FLAGS := -MD -std=gnu11 -ffreestanding -nostdlib
WARNING_FLAGS = -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Wimplicit-int-conversion -Wshorten-64-to-32 -Wpointer-to-int-cast -Wint-to-pointer-cast -Wshadow -Wundef -Wcast-qual -Wcast-align -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wswitch-enum -Wswitch-default -Wcovered-switch-default -Wnull-dereference -Wunreachable-code -Wvla -Walloca -Wwrite-strings -Wformat=2 -Wcast-function-type -Wconditional-uninitialized -Wcomma -Wextra-semi-stmt -Wno-language-extension-token -Wno-gnu-empty-struct -Wno-gnu-zero-variadic-macro-arguments
COMMON_FLAGS = $(WARNING_FLAGS)

# qemu flags for virt
QEMU_MACHINE := virt
QEMU_FLAGS := -cpu cortex-a72 -nographic -smp $(CONFIG_CPU_COUNT) -m $(CONFIG_PHYSICAL_MEMORY_MB)M -kernel $(OUT)
# qemu flags for raspi4b
# QEMU_MACHINE := raspi4b
# QEMU_FLAGS := -cpu cortex-a72 -nographic -kernel $(OUT)

# debugging?
ifeq ($(CONFIG_DEBUG_SYMBOLS),y)
	COMMON_FLAGS += -ggdb
endif

# optimize?
ifeq ($(CONFIG_OPTIMIZATIONS_LOW),y)
	COMMON_FLAGS += -O1
else ifeq ($(CONFIG_OPTIMIZATIONS_MEDIUM),y)
	COMMON_FLAGS += -O2 -flto=thin
else ifeq ($(CONFIG_OPTIMIZATIONS_HIGH),y)
	COMMON_FLAGS += -O2 -flto=full
else ifeq ($(CONFIG_OPTIMIZATIONS_NONE),y)
	COMMON_FLAGS += -O0
endif

K = kernel
D = $K/drivers
M = $K/memory
KA = $K/arch
L = lib
U = user
# files are in the order of what `fd` returns
ASM_SOURCES = $K/entry.S \
			  $K/startup.S
ASM_OBJS = $(ASM_SOURCES:=.o)

C_SOURCES = $K/cpu.c \
			$D/uart/uart.c \
			$K/entry.c \
			$K/error.c \
			$K/init.c \
			$M/kmem.c \
			$K/printf.c \
			$K/spinlock.c \
			$L/string.c

C_OBJS = $(C_SOURCES:=.o)

KI = $K/include
KID = $(KI)/drivers
KIM = $(KI)/memory
LI = $L/include

HEADERS = $(KA)/aarch64/aarch64.h \
		  $(KI)/cpu.h \
		  $(KID)/uart/uart.h \
		  $(KI)/entry.h \
		  $(KI)/error.h \
		  $(KI)/init.h \
		  $(KI)/memlayout.h \
		  $(KIM)/kmem.h \
		  $(KI)/printf.h \
		  $(KI)/spinlock.h \
		  $(LI)/common_defs.h \
		  $(LI)/string.h \
		  $(LI)/types.h

OBJS = $(ASM_OBJS) $(C_OBJS)

INCLUDES_DIR = -I$(KI)/ -I. -I$(LI) -I$(K)/arch/

DEPS := $(C_OBJS:.o=.d)

# default target which runs when none is specified
all: build

# configuration -------------------------------
menuconfig: ## Configure the kernel using ncurses tui
	menuconfig
	genconfig

defconfig: ## Use default kernel config
	defconfig configs/debugconfig
	genconfig

# building ---------------------
build: $(OUT) ## Build the kernel

$(OUT): $(OBJS) $(LINKER_SCRIPT) $(HEADERS)
	$(LD) -m aarch64elf -T $(LINKER_SCRIPT) $(OBJS) -o $(OUT)

%.S.o: %.S
	$(CC) --target=$(TARGET) $(INCLUDES_DIR) $(COMMON_FLAGS) -c $< -o $@

%.c.o: %.c
	$(CC) --target=$(TARGET) $(C_FLAGS) $(INCLUDES_DIR) $(COMMON_FLAGS) -c $< -o $@

# running ---------------------
run: $(OUT) ## Run the kernel inside qemu
	qemu-system-aarch64 -M $(QEMU_MACHINE) $(QEMU_FLAGS)

# cleaning -------------------
cleanall: clean cleanconfig cleandebug ## Clean all build, config and debug files

clean: ## Clean only build files
	rm -f $(OBJS)
	rm -f $(OUT)

cleanconfig: ## Clean only config files
	rm -f config.h .config

# hacking ------------------
cleandebug:
	rm -rf stripped-$(OUT)
	rm -rf *.objdump

gdb:
	gdb $(OUT) -ex "target remote :1234"

stripped-$(OUT): $(OUT)
	llvm-strip --strip-debug $(OUT) -o stripped-$(OUT)

objdump: $(OUT) stripped-$(OUT)
	llvm-objdump --disassemble-all --line-numbers --full-contents stripped-$(OUT) > dump.objdump


rund: $(OUT)
	qemu-system-aarch64 -M $(QEMU_MACHINE) $(QEMU_FLAGS) -s -S

qemu_dump_dts:
	qemu-system-aarch64 -machine $(QEMU_MACHINE),dumpdtb=virt.dtb $(QEMU_FLAGS)
	dtc -I dtb -O dts -o virt.dts virt.dtb

.PHONY: menuconfig defconfig run cleanall clean cleanconfig gdb rund qemu_dump_dtb help

help: # Show this help
	@sed -nE 's/^([[:alnum:]_.-]+):.*##[[:space:]]*(.*)/\1\t\2/p' $(MAKEFILE_LIST) | column -ts $$'\t'

-include $(DEPS)
