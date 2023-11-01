/*
 * Quite Fast Fibers - the C header for your comfort & convenience
 *
 * A fairly dumb (but quite fast) implementation of fibers made purely 
 * to illustrate the simplicity of implementation of the core concept.
 * 
 * You shouldn't use this in production (unless you're totally bonkers)!
 * It's intended as a companion to a blog post and deliberately omits 
 * important things for the sake of keeping the example understandable 😇.
 * 
 * Copyright (c) 2023 Ross Bamford
 * MIT License (see LICENSE.md)
 */

#ifndef __ANOS_KERNEL_DRIVERS_LOCAL_APIC_H
#define __ANOS_KERNEL_DRIVERS_LOCAL_APIC_H

#include <stdint.h>

#ifndef QFF_STACK_SIZE
#define QFF_STACK_SIZE      524288
#endif

#define QFF_STACK_WORDS     ((QFF_STACK_SIZE / sizeof(uintptr_t)))

typedef void (*qff_task)(void);

void qff_dump(void);
void qff_run(qff_task init);
void qff_schedule(qff_task task);
void qff_yield(void);

#endif//__ANOS_KERNEL_DRIVERS_LOCAL_APIC_H
