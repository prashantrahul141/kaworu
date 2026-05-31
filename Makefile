ifneq ($(wildcard .config),)
    include .config
else
    $(info .config was not found, skipping.)
endif

export CC := clang
export LD := ld.lld

export LINKER_SCRIPT := linker.ld
export OUT := kaworu.elf

export TARGET := aarch64-none-elf

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

export C_FLAGS := -ffreestanding -nostdlib
export COMMON_FLAGS

OBJS = kernel/boot.o \
	   kernel/init.o

$(OUT): kernel
	$(LD) -m aarch64elf -T $(LINKER_SCRIPT) $(OBJS) -o $(OUT)

kernel:
	make -C kernel/

clean:
	make -C kernel/ clean

cleanconfig:
	rm -f config.h .config

menuconfig:
	menuconfig

defconfig:
	defconfig configs/debugconfig

genconfig:
	genconfig

run: kernel
	qemu-system-aarch64 -M virt -cpu cortex-a72 -nographic -kernel $(OUT)

rund: kernel
	qemu-system-aarch64 -M virt -cpu cortex-a72 -s -S -nographic -kernel $(OUT)

gdb:
	gdb -tui $(OUT) -ex "target remote :1234"

.PHONY: $(OUT) kernel clean run
