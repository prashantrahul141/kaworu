#include "aarch64.h"
#include "types.h"

/* read mpidr_el1 */
inline u64 r_mpidr_el1(void)
{
	u64 mpidr;
	asm volatile("mrs %0, MPIDR_EL1" : "=r"(mpidr));
	return mpidr;
}

/* read daif  */
inline u64 r_daif(void)
{
	u64 daif;
	asm volatile("mrs %0, daif" : "=r"(daif));
	return daif;
}

/* are device interrupts enabled? */
inline bool r_intrd_enabled(void)
{
	u64 daif = r_daif();
	u64 masked = (daif & (DAIF_F | DAIF_I));
	return masked == 0;
}

/* disable device interrupts */
inline void w_intrd_disable(void)
{
	asm volatile("msr daifset, #0b0011");
}

/* enable device interrupts */
inline void w_intrd_enable(void)
{
	asm volatile("msr daifclr, #0b0011");
}
