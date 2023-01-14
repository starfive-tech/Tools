override CFLAGS=-Wall -O2

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

all: jh7110_uboot_spl

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

jh7110_uboot_spl: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) -o $@

clean:
	rm -f *.o jh7110_uboot_spl
