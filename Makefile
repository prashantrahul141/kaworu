ifneq ($(wildcard .config),)
    include .config
else
    $(info .config was not found, skipping.)
endif

CC := clang
LD := ld.lld

LINKER_SCRIPT := linker.ld

OUT := kaworu.elf

TARGET := aarch64-none-elf

COMMON_FLAGS =
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

C_FLAGS := -ffreestanding -nostdlib

ASM_SOURCES = kernel/startup.S \
			  kernel/exception_table.S
ASM_OBJS = $(ASM_SOURCES:.S=.o)

C_SOURCES = kernel/init.c \
			kernel/exception.c
C_OBJS = $(C_SOURCES:.c=.o)

OBJS = $(ASM_OBJS) $(C_OBJS)

$(OUT): $(OBJS)
	$(LD) -m aarch64elf -T $(LINKER_SCRIPT) $(OBJS) -o $(OUT)

clean:
	rm -f $(OBJS)
	rm -f $(OUT)

cleanconfig:
	rm -f config.h .config

menuconfig:
	menuconfig
	genconfig

defconfig:
	defconfig configs/debugconfig
	genconfig

run: $(OUT)
	qemu-system-aarch64 -M virt -cpu cortex-a72 -nographic -kernel $(OUT)

rund: $(OUT)
	qemu-system-aarch64 -M virt -cpu cortex-a72 -s -S -nographic -kernel $(OUT)

gdb:
	gdb $(OUT) -ex  "target remote :1234"

%.o: %.S
	$(CC) --target=$(TARGET) $(COMMON_FLAGS) -c $< -o $@

%.o: %.c
	$(CC) --target=$(TARGET) $(C_FLAGS) $(COMMON_FLAGS) -c $< -o $@

.PHONY: $(OUT) kernel clean run rund gdb menuconfig defconfig genconfig
