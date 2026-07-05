#ifndef _INIT_H_
#define _INIT_H_

#include "aarch64/aarch64.h"
#include "common_defs.h"
#include "console.h"
#include "config.h"
#include "debug/log.h"
#include "limine.h"
#include "memory/kmem.h"
#include "memory/paging.h"
#include "printf.h"
#include "limine_responses.h"
#include "uart/uart.h"

/*
 * Kernel's entry point in C.
 */
void kernel_main(void);

#endif // _INIT_H_
