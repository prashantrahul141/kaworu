# kaworu

A WIP unix-like aarch64 kernel

## Building

Building requires the following packages:

```sh
clang
lld
# llvm # not required, but contains useful tools like objdump, objcopy
qemu
gnumake
python313Packages.kconfiglib
# gdb # for debugging
# bear # if you want to generate compile_commands.json
```

Now you can either:

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

run under qemu:

```sh
make run
```

## Resources

https://wiki.osdev.org

https://krinkinmu.github.io                                 # thank you @krinkinmu

https://kernel-internals.org

https://df.lth.se/~getz/ARM/A64/

https://github.com/Elite-zx/Tiny-OS                         # thank you @Elite-zx

https://github.com/mit-pdos/xv6-riscv

https://github.com/arryndoestech/adt_os                     # thank you @arryndoestech

https://xv6-guide.github.io/xv6-riscv-book

https://github.com/gmarino2048/64bit-os-tutorial            # thank you @gmarino2048

https://lowenware.com/blog/aarch64-mmu-programming/

https://developer.arm.com/documentation/ddi0487/latest

https://developer.arm.com/documentation/ddi0601/latest

https://cs140e.sergio.bz/docs/ARMv8-Reference-Manual.pdf
