#ifndef __UART_PL011_H_
#define __UART_PL011_H_

#include "error.h"
#include "manager/manager.h"

errno_t pl011_probe(Device *device);

errno_t pl011_remove(Device *device);

#endif // __UART_PL011_H_
