#ifndef _ENTRY_H_
#define _ENTRY_H_

typedef struct {
} ExceptionFrame;

void exception_handler(void);

void irq_handler(void);

void unhandled_exception_vector(void);

#endif // _ENTRY_H_
