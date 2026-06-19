#ifndef _PANIC_H_
#define _PANIC_H_

#define panic(fmt, ...)                                                \
	do {                                                           \
		printf("PANIC:%s:%s:%d: " fmt, __FILE__, __FUNCTION__, \
		       __LINE__, ##__VA_ARGS__);                       \
		brk(0);                                                \
	} while (0)

#endif // _PANIC_H_
