include ../.config

CC := clang
LD := ld.lld

LINKER_SCRIPT := kernel.ld
OUT := kaworu.elf

TARGET := aarch64-none-elf
C_FLAGS := -MD -std=gnu23 -ffreestanding -nostdlib
WARNING_FLAGS = -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Wimplicit-int-conversion -Wshorten-64-to-32 -Wpointer-to-int-cast -Wint-to-pointer-cast -Wshadow -Wundef -Wcast-qual -Wcast-align -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wswitch-enum -Wswitch-default -Wcovered-switch-default -Wnull-dereference -Wunreachable-code -Wvla -Walloca -Wwrite-strings -Wformat=2 -Wcast-function-type -Wconditional-uninitialized -Wcomma -Wextra-semi-stmt -Wno-language-extension-token -Wno-gnu-empty-struct -Wno-gnu-zero-variadic-macro-arguments
COMMON_FLAGS = $(WARNING_FLAGS)

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

K = .
L = ../lib
U = ../user
# files are in the order of what `fd` returns
ASM_SOURCES = $K/entry.S \
			  $K/startup.S
ASM_OBJS = $(ASM_SOURCES:=.o)

C_SOURCES = $K/cpu.c \
			$K/debug/printf.c \
			$K/drivers/uart/uart.c \
			$K/entry.c \
			$K/error.c \
			$K/init.c \
			$K/memory/kmem.c \
			$K/spinlock.c \
			$L/string.c

C_OBJS = $(C_SOURCES:=.o)

KI = $K/include
LI = $L/include
HEADERS = $(KI)/arch/aarch64/aarch64.h \
		  $(KI)/cpu.h \
		  $(KI)/debug/panic.h \
		  $(KI)/printf.h \
		  $(KI)/drivers/uart/uart.h \
		  $(KI)/entry.h \
		  $(KI)/error.h \
		  $(KI)/init.h \
		  $(KI)/memlayout.h \
		  $(KI)/memory/kmem.h \
		  $(KI)/spinlock.h \
		  $(LI)/common_defs.h \
		  $(LI)/string.h \
		  $(LI)/types.h


OBJS = $(ASM_OBJS) $(C_OBJS)

INCLUDES_DIR = -I$(KI)/ -I. -I$(LI) -I$(KI)/arch -I ../

DEPS := $(C_OBJS:.o=.d)

# default target which runs when none is specified
all: build

# building ---------------------
build: $(OUT) ## Build the kernel

$(OUT): $(OBJS) $(LINKER_SCRIPT) $(HEADERS)
	$(LD) -m aarch64elf -T $(LINKER_SCRIPT) $(OBJS) -o $(OUT)

%.S.o: %.S
	$(CC) --target=$(TARGET) $(INCLUDES_DIR) $(COMMON_FLAGS) -c $< -o $@

%.c.o: %.c
	$(CC) --target=$(TARGET) $(C_FLAGS) $(INCLUDES_DIR) $(COMMON_FLAGS) -c $< -o $@

# cleaning -------------------
.PHONY: clean
clean: ## Clean only build files
	@rm -f $(DEPS)
	@rm -f $(OBJS)
	@rm -f $(OUT)

-include $(DEPS)
