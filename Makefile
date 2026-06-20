CONFIG_FREE_TARGETS := help menuconfig defconfig clean cleanall cleandebug cleanconfig
# Require .config for everything else
ifeq ($(filter $(MAKECMDGOALS),$(CONFIG_FREE_TARGETS)),)
  ifeq ($(wildcard .config),)
    $(error .config was not found, run `make menuconfig` or `make defconfig`. Run `make help` for more info)
  endif
  include .config
endif

NAME = kaworu

# Point to the extract directory of limine-binary
# LIMINE_PATH :=

# Point to the uefi firmware for qemu (not the directory, the firmware file itself)
# UEFI_FIRMWARE :=

ELF = kernel/$(NAME).elf
ISO = $(NAME).iso

# qemu flags for virt
# WARN: dont forget to update in release run scripts
QEMU_MACHINE := virt
QEMU_FLAGS := -cpu cortex-a72 \
			-m $(CONFIG_PHYSICAL_MEMORY_MB)M \
			-device ramfb \
			-device qemu-xhci \
			-device usb-kbd \
			-device usb-tablet \
			-drive if=pflash,unit=0,format=raw,file=$(UEFI_FIRMWARE),readonly=on \
			-cdrom $(ISO)
			# TODO:  -smp $(CONFIG_CPU_COUNT) \

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
build: $(ISO) ## Build kernel iso

$(ISO): setup-iso-dir
	@printf "\tXORRISO %s\n" $(ISO)
	@xorriso -as mkisofs -R -r -J \
		-hfsplus -apm-block-size 2048 \
		--efi-boot boot/limine/limine-uefi-cd.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		iso -o $(ISO) > /dev/null 2>&1
	@printf "iso file: %s\n" $(ISO)

ISO_COPIES := \
	iso/boot/$(NAME):$(ELF) \
	iso/boot/limine/limine.conf:limine.conf \
	iso/boot/limine/limine-uefi-cd.bin:$(LIMINE_PATH)/limine-uefi-cd.bin \
	iso/EFI/BOOT/BOOTAA64.EFI:$(LIMINE_PATH)/BOOTAA64.EFI

ISO_FILES := $(foreach f,$(ISO_COPIES),$(word 1,$(subst :, ,$(f))))

setup-iso-dir: create-iso-dir $(ISO_FILES)
create-iso-dir:
	@printf "\tMKDIR iso\n"
	@mkdir -p iso/{boot/limine,EFI/BOOT}

define copy-rule
$(word 1,$(subst :, ,$1)): $(word 2,$(subst :, ,$1))
	@printf "\tCOPY %s\n" $$@
	@cp $$< $$@
endef

$(foreach f,$(ISO_COPIES),$(eval $(call copy-rule,$(f))))

$(ELF):
	@make -C kernel/

# running ---------------------
.PHONY: run
run: $(ISO) ## Run the kernel inside qemu
	qemu-system-aarch64 -M $(QEMU_MACHINE) $(QEMU_FLAGS)

# cleaning -------------------
.PHONY: cleanall
cleanall: clean cleanconfig cleandebug ## Clean all build, config and debug files

.PHONY: clean
clean: ## Clean only iso and build files
	rm -f $(ISO)
	rm -rf iso
	@make -C kernel/ clean

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

rund: $(ISO)
	qemu-system-aarch64 -M $(QEMU_MACHINE) $(QEMU_FLAGS) -s -S

qemu_dump_dts:
	qemu-system-aarch64 -machine $(QEMU_MACHINE),dumpdtb=virt.dtb $(QEMU_FLAGS)
	dtc -I dtb -O dts -o virt.dts virt.dtb

.PHONY: menuconfig defconfig run cleanall clean cleanconfig gdb rund qemu_dump_dtb help

help: # Show this help
	@sed -nE 's/^([[:alnum:]_.-]+):.*##[[:space:]]*(.*)/\1\t\2/p' $(MAKEFILE_LIST) | column -ts $$'\t'

# create releases
release_kernel: $(ISO)
	mkdir -p kernel-release/
	cp $(ISO) kernel-release/
	cp ./meta/releases/kernel/* kernel-release/
	tar -czvf kernel-release.tar.gz kernel-release/
	rm -rf kernel-release

release_full: $(ISO)
	mkdir -p full-release/
	cp $(ISO) full-release/
	cp ./meta/releases/full/* full-release/
	cp $(UEFI_FIRMWARE) full-release/
	tar -czvf full-release.tar.gz full-release/
	rm -rf full-release

-include $(DEPS)
