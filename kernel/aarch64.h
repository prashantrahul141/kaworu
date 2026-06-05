#ifndef _ARCH64_H_
#define _ARCH64_H_

#include "types.h"

#define DAIF_I (1 << 7)
#define DAIF_F (1 << 6)

/* break */
#define brk(imm) asm volatile("brk #" #imm)

/* read mpidr_el1 */
u64 r_mpidr_el1(void);

/* read daif */
u64 r_daif(void);

/* are device interrupts enabled? */
bool r_intrd_enabled(void);

/* disable device interrupts */
void w_intrd_disable(void);

/* enable device interrupts */
void w_intrd_enable(void);

/* halt */
#define hlt(imm) asm volatile("hlt #", #imm)

#endif // _ARCH64_H_
