# kaworu

A WIP unix-like aarch64 kernel

## Building

If you use nix, `nix develop` sets up everything you will need in order to
build and run the kernel under qemu.

Building requires the following packages:

```sh
clang
lld

limine # binaries, used in creation of iso image
xorriso # to create iso image

qemu # to run
gnumake # ofcourse
python313Packages.kconfiglib

# llvm # not required, but contains useful tools like objdump, objcopy
# gdb # for debugging
# bear # if you want to generate compile_commands.json
```

You will need limine-binaries in order to create the iso image, which can you
download from: https://github.com/Limine-Bootloader/Limine/releases

Download the `limine-binary` one and extract it somewhere, then set `LIMINE_PATH`
env to the extracted directory. In a way that `$(LIMINE_PATH)/limine-uefi-cd.bin`
is accessible.

This is done automatically if you're using nix.


Then generate a config using:

```sh
# Use the default config
make defconfig
# Or configure the kernel yourself and save it
make menuconfig
```

Then build the kernel

```sh
make
```

## Running

I have yet to test this on actual hardware, I have only tested this on qemu
under the `virt` platform.

Since aarch64 only supports UEFI and not bios, you will need to provide an UEFI firmware for qemu.
You can just download the `full-release` from the [releases page](https://github.com/prashantrahul141/kaworu/releases) as it contains
both a built iso image and a firmware you need to emulate under qemu.

You can also download the firmware from the releases
page of:
https://github.com/osdev0/edk2-ovmf-stable-bins/releases/

Then set `UEFI_FIRMWARE` env pointing directly to the firmware.

This is done automatically if you're using nix.

run under qemu:

```sh
make run
```


## Todo

- [x] Startup
    - [x] lower exception level to el1
    - [x] Exception vectors
    - [x] Setup system registers
    - [x] Setup per cpu stack
    - [x] Enable interrupts

- [x] Spinlock

- [x] Uart driver
    - [x] init pl011
    - [x] `uart_putchar`/`uart_getchar`

- [x] Printf implementation
    - [x] `printf`
    - [x] `panic`

- [x] Physical page allocator
    - [x] `kmem_alloc`/`kmem_free`

- [ ] MMU
    - [ ] page table setup
    - [ ] map kernel
    - [ ] map mmio

- [ ] Virtual Memory
    - [ ] kernel virtual allocator
    - [ ] `vm_map`/`vm_unmap`
    - [ ] Guard pages for kernel stacks

- [ ] Kernel heap
    - [ ] `kalloc`/`kfree`

- [ ] Exception handling
    - [ ] Decode ESR_EL1, FAR_EL1, ELR_EL1
    - [ ] Pretty exception dump

- [ ] Interrupts
    - [ ] GIC distributor init and cpu interface init
    - [ ] IRQ framework

- [ ] Timer
    - [ ] Generic timer init
    - [ ] Periodic timer interrupt
    - [ ] Tick counter

- [ ] Scheduler
    - [ ] Concept of task, task states
    - [ ] Run queue
    - [ ] Context switch

- [ ] Kernel Threads
    - [ ] `kthread_create`/`kthread_exit`
    - [ ] Idle thread
    - [ ] Scheduler (maybe round robin?)

- [ ] Userspace
    - [ ] EL0 entry support
    - [ ] User stack
    - [ ] User address space

- [ ] Syscalls
    - [ ] SVC handler
    - [ ] Syscall table
    - [ ] Basic ones like: read, write, exit, yield

- [ ] FS
    - [ ] Decide on a simple filesystem to implement.


## Thirdparty

[limine](https://github.com/limine-bootloader/limine) - kaworu's bootloader


## Resources

https://wiki.osdev.org \
https://krinkinmu.github.io                                 # thank you @krinkinmu \
https://kernel-internals.org \
https://df.lth.se/~getz/ARM/A64/ \
https://github.com/Elite-zx/Tiny-OS                         # thank you @Elite-zx \
https://github.com/mit-pdos/xv6-riscv \
https://github.com/arryndoestech/adt_os                     # thank you @arryndoestech  \
https://xv6-guide.github.io/xv6-riscv-book \
https://github.com/gmarino2048/64bit-os-tutorial            # thank you @gmarino2048  \
https://lowenware.com/blog/aarch64-mmu-programming/ \
https://developer.arm.com/documentation/ddi0487/latest \
https://developer.arm.com/documentation/ddi0601/latest \
https://cs140e.sergio.bz/docs/ARMv8-Reference-Manual.pdf
