CONFIG_FREE_TARGETS := help menuconfig defconfig clean cleanall cleandebug cleanconfig
# Require .config for everything else
ifeq ($(filter $(MAKECMDGOALS),$(CONFIG_FREE_TARGETS)),)
  ifeq ($(wildcard .config),)
    $(error .config was not found, run `make menuconfig` or `make defconfig`. Run `make help` for more info)
  endif
  include .config
endif

ELF = kernel/kaworu.elf

# qemu flags for virt
QEMU_MACHINE := virt
QEMU_FLAGS := -cpu cortex-a72 -nographic -smp $(CONFIG_CPU_COUNT) -m $(CONFIG_PHYSICAL_MEMORY_MB)M -kernel $(ELF)
# qemu flags for raspi4b
# QEMU_MACHINE := raspi4b
# QEMU_FLAGS := -cpu cortex-a72 -nographic -kernel $(OUT)

all: build

# configuration -------------------------------
.PHONY: menuconfig
menuconfig: _menuconfig syncconfig ## Configure the kernel using ncurses tui
_menuconfig:
	@printf "\tMENU\n"
	menuconfig > /dev/null

.PHONY: defconfig
defconfig: _defconfig syncconfig ## Use default kernel config
_defconfig:
	@printf "\tCOPY configs/debugconfig\n"
	@defconfig configs/debugconfig > /dev/null

.PHONY: syncconfig
syncconfig:
	@printf "\tSYNC\n"
	@genconfig

# building ---------------------
build: ## Build the kernel
	make -C kernel/

# running ---------------------
.PHONY: run
run: $(ELF) ## Run the kernel inside qemu
	qemu-system-aarch64 -M $(QEMU_MACHINE) $(QEMU_FLAGS)

# cleaning -------------------
.PHONY: cleanall
cleanall: clean cleanconfig cleandebug ## Clean all build, config and debug files

.PHONY: clean
clean: ## Clean only build files
	make -C ./kernel/ clean

.PHONY: cleanconfig
cleanconfig: ## Clean only config files
	rm -f config.h .config

.PHONY: cleandebug
cleandebug:
	rm -rf stripped-$(ELF)
	rm -rf *.objdump

# hacking ------------------
clang-tidy: $(ELF)  ## Run clang-tidy on the entire source
	run-clang-tidy -source-filter ".*\.(c|h)" -quiet

gdb:
	gdb $(ELF) -ex "target remote :1234"

stripped-$(ELF): $(ELF)
	llvm-strip --strip-debug $(ELF) -o stripped-$(ELF)

objdump: $(ELF) stripped-$(ELF)
	llvm-objdump --disassemble-all --line-numbers --full-contents stripped-$(ELF) > dump.objdump


rund: $(ELF)
	qemu-system-aarch64 -M $(QEMU_MACHINE) $(QEMU_FLAGS) -s -S

qemu_dump_dts:
	qemu-system-aarch64 -machine $(QEMU_MACHINE),dumpdtb=virt.dtb $(QEMU_FLAGS)
	dtc -I dtb -O dts -o virt.dts virt.dtb

.PHONY: menuconfig defconfig run cleanall clean cleanconfig gdb rund qemu_dump_dtb help

help: # Show this help
	@sed -nE 's/^([[:alnum:]_.-]+):.*##[[:space:]]*(.*)/\1\t\2/p' $(MAKEFILE_LIST) | column -ts $$'\t'

-include $(DEPS)
