CFLAGS=-Iinclude -Wall -Wextra -Werror -Wpedantic -std=c17 -g $(ARCH_CFLAGS)
ASFLAGS=-f$(OBJFMT) -g
LDFLAGS=$(ARCH_LDFLAGS)
NASM?=nasm

UNAME_S := $(shell uname -s)
UNAME_P := $(shell uname -p)
ifeq ($(UNAME_S),Darwin)
	OBJFMT = macho64
ifneq ($(filter arm%,$(UNAME_P)),)
	ARCH_LDFLAGS = -arch x86_64
	ARCH_CFLAGS = -arch x86_64
endif
else
ifeq ($(UNAME_P),x86_64)
	OBJFMT = elf64
endif
endif

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


