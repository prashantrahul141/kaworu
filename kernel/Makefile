ASM_SOURCES = boot.S
ASM_OBJS = $(ASM_SOURCES:.S=.o)

C_SOURCES = init.c
C_OBJS = $(C_SOURCES:.c=.o)

all: $(C_OBJS) $(ASM_OBJS)
	@printf "kernel sources built ----------------\n"

%.o: %.S
	$(CC) --target=$(TARGET) $(COMMON_FLAGS) -c $^ -o $@

%.o: %.c
	$(CC) --target=$(TARGET) $(C_FLAGS) $(COMMON_FLAGS) -c $^ -o $@

clean:
	rm -f *.o
