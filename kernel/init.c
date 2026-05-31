#define UART ((volatile unsigned int *)0x09000000)

void kernel_main(void)
{
	for (;;)
		UART[0] = 'k';
}
