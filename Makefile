CONFIG_FREE_TARGETS := help menuconfig defconfig
# Require .config for everything else
ifeq ($(filter $(MAKECMDGOALS),$(CONFIG_FREE_TARGETS)),)
  ifeq ($(wildcard .config),)
    $(error .config was not found, run `make menuconfig` or `make defconfig`. Run `make help` for more info)
  endif
  include .config
endif

CC := clang
LD := ld.lld

LINKER_SCRIPT := linker.ld
OUT := kaworu.elf

TARGET := aarch64-none-elf
C_FLAGS := -std=gnu11 -ffreestanding -nostdlib
WARNING_FLAGS = -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Wimplicit-int-conversion -Wshorten-64-to-32 -Wpointer-to-int-cast -Wint-to-pointer-cast -Wshadow -Wundef -Wcast-qual -Wcast-align -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wswitch-enum -Wswitch-default -Wcovered-switch-default -Wnull-dereference -Wunreachable-code -Wvla -Walloca -Wwrite-strings -Wformat=2 -Wcast-function-type -Wconditional-uninitialized -Wcomma -Wextra-semi-stmt -Wno-language-extension-token -Wno-gnu-empty-struct
COMMON_FLAGS = $(WARNING_FLAGS)
QEMU_MACHINE := virt
QEMU_FLAGS := -cpu cortex-a72 -nographic -kernel $(OUT)

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
	COMMON_FLAGS += -O3 -flto=full
else ifeq ($(CONFIG_OPTIMIZATIONS_NONE),y)
	COMMON_FLAGS += -O0
endif

ASM_SOURCES = kernel/startup.S \
			  kernel/entry.S
ASM_OBJS = $(ASM_SOURCES:=.o)

C_SOURCES =  kernel/init.c \
			 kernel/entry.c \
			 kernel/aarch64.c \
			 kernel/uart.c \
			 kernel/printf.c
C_OBJS = $(C_SOURCES:=.o)

HEADERS = kernel/ktypes.h \
		  kernel/entry.h \
		  kernel/aarch64.h \
		  kernel/memlayout.h \
		  kernel/init.h \
		  kernel/uart.h \
		  kernel/printf.h

OBJS = $(ASM_OBJS) $(C_OBJS)

INCLUDES_DIR = -Ikernel/

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
cleanall: clean cleanconfig ## Clean all build and config files

clean: ## Clean only build files
	rm -f $(OBJS)
	rm -f $(OUT)

cleanconfig: ## Clean only config files
	rm -f config.h .config

# hacking ------------------
gdb:
	gdb $(OUT) -ex "target remote :1234"

rund: $(OUT)
	qemu-system-aarch64 -M $(QEMU_MACHINE) $(QEMU_FLAGS) -s -S

qemu_dump_dts:
	qemu-system-aarch64 -machine $(QEMU_MACHINE),dumpdtb=virt.dtb $(QEMU_FLAGS)
	dtc -I dtb -O dts -o virt.dts virt.dtb

.PHONY: menuconfig defconfig run cleanall clean cleanconfig gdb rund qemu_dump_dtb help

help: # Show this help
	@sed -nE 's/^([[:alnum:]_.-]+):.*##[[:space:]]*(.*)/\1\t\2/p' $(MAKEFILE_LIST) | column -ts $$'\t'
