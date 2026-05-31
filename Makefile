export CC := clang
export LD := ld.lld

export LINKER_SCRIPT := linker.ld
export OUT := kaworu.elf

export TARGET := aarch64-none-elf
export DEBUG_FLAGS :=
export C_FLAGS := -ffreestanding -nostdlib

$(OUT): kernel
	$(LD) -m aarch64elf -T $(LINKER_SCRIPT) $(shell find . -name '*.o') -o $(OUT)

kernel:
	make -C kernel/

clean:
	make -C kernel/ clean

run:
	qemu-system-aarch64 -M virt -cpu cortex-a72 -nographic -kernel $(OUT)

.PHONY: $(OUT) kernel clean run
