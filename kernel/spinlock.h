#ifndef _SPINLOCK_H_
#define _SPINLOCK_H_

/* spinlock implementation */

#include "types.h"
#include "config.h"
#include "cpu.h"

typedef struct {
	/* is spinlock locked */
	bool locked;

	/* name of this spinlock, for debugging. */
	const u8 *name;

	/* which cpu has acquired this lock*/
	cpu *cpu;
} spinlock;

void spinlock_init(spinlock *sp, const u8 *name);
void spinlock_acquire(spinlock *sp);
void spinlock_release(spinlock *sp);

#endif // _SPINLOCK_H_
