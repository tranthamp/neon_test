CC=$(CROSS_COMPILE)gcc
OBJS=main.c neonblit.s

all: test

test: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -mcpu=cortex-a8 -mfpu=neon -mfloat-abi=softfp -o neon_test

clean:
	rm neon_test
