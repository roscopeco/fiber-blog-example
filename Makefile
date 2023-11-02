CFLAGS=-Iinclude -Wall -Wextra -Werror -Wpedantic -std=c17 -g
ASFLAGS=-f$(ARCH) -g
LDFLAGS=$(ARCH_LDFLAGS)
NASM?=nasm
ARCH?=macho64

.PHONY: all clean

all: main

main: main.o list.o qff.o qff_switch.o
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	rm -rf *.o main

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o : %.asm
	nasm $(ASFLAGS) -o $@ $<
