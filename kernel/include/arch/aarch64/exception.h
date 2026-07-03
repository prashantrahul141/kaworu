#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include "types.h"

/*
 * This should exactly match the structure given in exception.S
 */
typedef struct {
	u64 x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14,
		x15, x16, x17, x18, x19, x20, x21, x22, x23, x24, x25, x26, x27,
		x28, x29, x30, ELR_EL1, SPSR_EL1, ESR_EL1, FAR_EL1, SP_EL0;
} ExceptionFrame;

void exception_handler(void);

void irq_handler(void);

void unhandled_exception_handler(ExceptionFrame *frame);

#endif // _EXCEPTION_H_
