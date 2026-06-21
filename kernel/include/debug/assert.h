#ifndef _ASSERT_H_
#define _ASSERT_H_

#include "panic.h"

#define ASSERT(exp, msg) \
	if (exp)         \
	panic("assertion failed: %s : %s", #exp, msg)

#endif // _ASSERT_H_
