/*
 * Quite Fast Fibers - A naive fiber implementation for x86_64
 *
 * Copyright (c)2023 Ross Bamford & Contributors
 */

#ifndef __ANOS_KERNEL_DRIVERS_LOCAL_APIC_H
#define __ANOS_KERNEL_DRIVERS_LOCAL_APIC_H

#include <stdnoreturn.h>

typedef void (*qff_task)(void);

void qff_dump(void);
noreturn void qff_start(qff_task init);
void qff_schedule(qff_task task);
void qff_yield(void);

#endif//__ANOS_KERNEL_DRIVERS_LOCAL_APIC_H
