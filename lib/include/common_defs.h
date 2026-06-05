#ifndef _COMMON_DEFS_H_
#define _COMMON_DEFS_H_

/*
 * shamelessly stolen from:
 * https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/include/linux/math.h?id=ddd664bbff63e09e7a7f9acae9c43605d4cf185f#n10
 */
#define __round_mask(x, y) ((__typeof__(x))((y) - 1))
#define round_up(x, y)	   ((((x) - 1) | __round_mask(x, y)) + 1)
#define round_down(x, y)   ((x) & ~__round_mask(x, y))

#define NONNULL(...) __attribute__((nonnull(__VA_ARGS__)))

#endif // _COMMON_DEFS_H_
