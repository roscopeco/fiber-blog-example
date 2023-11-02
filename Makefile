CFLAGS=-Iinclude -Wall -Wextra -Werror -Wpedantic -std=c17 -g $(ARCH_CFLAGS)
ASFLAGS=-f$(OBJFMT) -g
LDFLAGS=$(ARCH_LDFLAGS)
NASM?=nasm

UNAME_S := $(shell uname -s)
UNAME_P := $(shell uname -p)
ifeq ($(UNAME_S),Darwin)
	OBJFMT = macho64
ifneq ($(filter arm%,$(UNAME_P)),)
	ARCH_LDFLAGS = -arch x86_64 -L$(shell xcrun -sdk macosx --show-sdk-path) -lSystem
	ARCH_CFLAGS = -arch x86_64
else
	ARCH_LDFLAGS = -L$(shell xcrun -sdk macosx --show-sdk-path)/usr/lib  -lSystem
endif
else
ifeq ($(UNAME_P),x86_64)
	OBJFMT = elf64
endif
endif

.PHONY: all clean

all: main

main: main.o list.o qff.o qff_switch.o
	$(LD) $(LDFLAGS) -o $@ $^

clean:
	rm -rf *.o main

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o : %.asm
	nasm $(ASFLAGS) -o $@ $<


