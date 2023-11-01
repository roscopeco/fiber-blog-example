#include "qff.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct {
    uintptr_t*      sp;
    uintptr_t*      stack_bottom;
    qff_task        func;
    bool            active;
} QffFiber;

static QffFiber tasks[2];
static int next_task;

QffFiber *qff_current_fiber;

void qff_switch_fiber(QffFiber *target);

static void qff_handle_exit(void) {
    if (qff_current_fiber) {
        qff_current_fiber->active = false;
    } else {
        printf("WARN: no current fiber in handle_exit");
    }
    free(qff_current_fiber->stack_bottom);
    qff_yield();
}

void qff_dump(void) {
    printf("Current task is 0x%016lx\n", (uintptr_t)qff_current_fiber);
    printf("  stack @ 0x%016lx\n", (uintptr_t)qff_current_fiber->stack_bottom);
    printf("  sp    @ 0x%016lx\n", (uintptr_t)qff_current_fiber->sp);
    printf("  func  @ 0x%016lx\n", (uintptr_t)qff_current_fiber->func);
    printf("  act   = %s", qff_current_fiber->active ? "true" : "false");
    printf("\n");
}

noreturn void qff_start(qff_task init) {
    init();
    qff_yield();
    __builtin_unreachable();
}

void qff_schedule(qff_task task) {    
    tasks[next_task].func = task;
    tasks[next_task].active = true;
    tasks[next_task].stack_bottom = (uintptr_t*)malloc(524288);
    tasks[next_task].sp = (uintptr_t*)(tasks[next_task].stack_bottom + 65536);

    *--tasks[next_task].sp = (uintptr_t)qff_handle_exit;
    *--tasks[next_task].sp = (uintptr_t)task;
    *--tasks[next_task].sp = 0xaaaaaaaaaaaaaaaa;
    *--tasks[next_task].sp = 0x5555555555555555;
    *--tasks[next_task].sp = 0xaaaaaaaaaaaaaaaa;
    *--tasks[next_task].sp = 0x5555555555555555;
    *--tasks[next_task].sp = 0xaaaaaaaaaaaaaaaa;
    *--tasks[next_task].sp = 0x5555555555555555;
    *--tasks[next_task].sp = 0xaaaaaaaaaaaaaaaa;
    *--tasks[next_task].sp = 0x5555555555555555;
    *--tasks[next_task].sp = 0xaaaaaaaaaaaaaaaa;
    *--tasks[next_task].sp = 0x5555555555555555;
    *--tasks[next_task].sp = 0xaaaaaaaaaaaaaaaa;
    *--tasks[next_task].sp = 0x5555555555555555;
    *--tasks[next_task].sp = 0xaaaaaaaaaaaaaaaa;
    *--tasks[next_task].sp = 0x5555555555555555;
    *--tasks[next_task].sp = 0xaaaaaaaaaaaaaaaa;
    *--tasks[next_task].sp = 0x5555555555555555;
    
    next_task = (next_task + 1) & 1;
}

void qff_yield(void) {
    while (true) {
        int switch_to = next_task;
        next_task = (next_task + 1) & 1;
        QffFiber *next = &tasks[switch_to];

        if (next->active) {
            qff_switch_fiber(next);
            break;
        }
    }
}
