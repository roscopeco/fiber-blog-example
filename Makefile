CFLAGS=-Iinclude -Wall -Wextra -Werror -Wpedantic -std=c17 -g $(ARCH_CFLAGS)
ASFLAGS=-f$(OBJFMT) -g
LDFLAGS=$(ARCH_LDFLAGS)
NASM?=nasm
ARCH_LDFLAGS?=-arch x86_64
ARCH_CFLAGS?=-arch x86_64
HOST_OS:=`uname`
HOST_ARCH:=`uname -p`

UNAME_S := $(shell uname -s)
UNAME_P := $(shell uname -p)
ifneq ($(filter %86,$(UNAME_P)),)
	$(error x86 is not supported at all)
endif
ifeq ($(UNAME_S),Linux)
ifneq ($(filter arm%,$(UNAME_P)),)
	$(error ARM is not supported on Linux)
endif
ifeq ($(UNAME_P),x86_64)
	OBJFMT = elf64
endif
endif
ifeq ($(UNAME_S),Darwin)
ifneq ($(filter arm%,$(UNAME_P)),)
	OBJFMT = macho64
	ARCH_LDFLAGS = -arch x86_64
	ARCH_CFLAGS = -arch x86_64
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


