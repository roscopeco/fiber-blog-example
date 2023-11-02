/*
 * Quite Fast Fibers - the C parts
 *
 * See comments in qff.h.
 * 
 * Copyright (c) 2023 Ross Bamford
 * MIT License (see LICENSE.md)
 */
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "list.h"
#include "qff.h"

// This struct holds just enough information about our fibers
// to be able to switch to them, and to clean up when they're done...
typedef struct {
    Node            node;
    uintptr_t*      sp;
    uintptr_t*      stack_bottom;
} QffFiber;

// This List holds our runnable queue.
//
// Selecting a fiber to run just involves popping it from the head
// of this list. When it yields, it is pushed to the tail.
//
// Fibers that complete before yielding are never re-added to this
// queue. When the queue is empty, the system exits.
//
// In the real world, you might have multiple of these (for different
// priorities etc). There might also be queues for fibers that are 
// blocked etc.
//
static List *runnable_queue;

// The init_fiber is special - it represents "the continuation of the
// anonymous fiber that was running when the program started".
//
// It's just a neat way for us to get back to the main thread of 
// execution when we've finished running fibers.
//
static QffFiber init_fiber;

// This holds the current fiber being run. It's not static, since
// it's accessed in `qff_switch.asm` too.
//
QffFiber *qff_current_fiber = &init_fiber;

// `qff_switch_fiber` is the main part of this - everything else is
// auxiliary to making that work. The magic of fibers is (mostly) in
// that bit of assembly code 🙂
//
// See qff_switch.asm for the implementation.
//
void qff_switch_fiber(QffFiber *target);

/*
 * This is an exit handler, which is set up as the "return" for when
 * a fiber function returns. It handles cleanup of the fibers, frees
 * the memory allocated for their stacks, and does some fairly ropey
 * things to make that happen.
 * 
 * A bit naughty, but FWIW your OS is pulling this kind of shit all
 * the time...
 */
static void qff_handle_exit(void) {
    // We have to do a few gymnastics here, since we're about to free
    // the stack we're currently on. 
    //
    // This is still a bit dangerous, but good enough for this example - 
    // in the real world you'd likely set this up to all happen once you're
    // on a different stack (e.g. borrowing some other fiber's stack for a moment).
    //
    // These static variables (which **must** be static since they can't be 
    // stack-bound!) will hold things while we do nefarious things...
    //
    static QffFiber *temp_fiber;
    static uintptr_t *temp_stack;
    static uintptr_t ministack[128];

    // Set up our temp variables, and NULL out the `qff_current_fiber`
    // so yield doesn't try to stack registers etc when we call it later...
    //
    temp_fiber = qff_current_fiber;
    temp_stack = qff_current_fiber->stack_bottom;
    qff_current_fiber = NULL;
    
    // Switch to our temporary ministack. Normally we'd tell the compiler
    // we're going to clobber registers here, but it really doesn't like 
    // us clobbering rsp in the middle of a function (🤷) so keep it secret 🤫
    //
    // This is obviously rather naughty, but by being careful with what we
    // do within this function it can be made safe (and we can check the
    // disassembly to be sure of it before we ship it 🚢).
    //
    __asm__ volatile ("  movq    %0,%%rsp" : : "r"(ministack + 128) : );

    // Now we're on the ministack, we can go ahead and free our fiber memory,
    // then yield to the next fiber.
    free(temp_fiber);
    free(temp_stack);

    // From the perspective of this (now gone) fiber, this will never return.
    // This works because in our implementation, `qff_yield` **always** switches
    // the stack (either to another fiber, or back to the synthetic "init fiber"
    // when everything else is finished).
    //
    qff_yield();
}

void qff_dump_f(QffFiber *fiber) {
    printf("Current task is 0x%016lx\n", (uintptr_t)fiber);
    printf("  stack @ 0x%016lx\n", (uintptr_t)fiber->stack_bottom);
    printf("  sp    @ 0x%016lx\n", (uintptr_t)fiber->sp);
    printf("\n");
}

void qff_dump(void) {
    qff_dump_f(qff_current_fiber);
}

/*
 * Runs some fibers to completion!
 * 
 * You give this a single "init" fiber, which is just a void function
 * that calls `qff_schedule` to set up some more fibers.
 * 
 * Once all the fibers are done, this function will return and you 
 * can go on with your life.
 */
void qff_run(qff_task init) {
    runnable_queue = malloc(sizeof(List));
    list_init(runnable_queue);

    init();
    qff_yield();

    list_free(runnable_queue);
    free(runnable_queue);
}

/*
 * Schedule a fiber to run. The "scheduler" in use in this example is
 * the most basic (and arguably useless) round-robin scheduler you can
 * possibly imagine.
 *
 * You can call this from your init fiber, or from any other fiber.
 * 
 * Don't call it outside the context of `qff_run` though 💣💥
 */
void qff_schedule(qff_task fiber_func) {  
    // We need a fiber struct to keep track of this - allocate one!  
    QffFiber *fiber = malloc(sizeof(QffFiber));

    // We need to allocate a stack for this fiber.
    //
    // Currently, we're using a default stack size of 512KiB, which might
    // seem like overkill - because frankly, it is. 
    //
    // However, it's the default pthread stack size used on macOS, and
    // seemed like a reasonable number. At least it stops things like 
    // printf blowing the stack - since we're just mallocing memory in
    // user space here we can't do clever OS tricks like setting up guard 
    // pages - overwriting other fiber's stacks will work, and will break
    // everything in super weird ways...
    //
    fiber->stack_bottom = (uintptr_t*)malloc(QFF_STACK_SIZE);
    fiber->sp = (uintptr_t*)(fiber->stack_bottom + QFF_STACK_WORDS);

    // Set up our stack 🥞. We do:
    //
    //   * Stack the `qff_handle_exit` function as a final "return address"
    //   * Stack the fiber function itself as our immediate "return address"
    //   * Reserve 6 qwords on the stack for the initial register load
    //
    // If we wanted to support things like passing arguments into our
    // fiber functions, we could do that by boxing a bit clever here, and
    // setting up the arguments as initial values for `rdi` and `rsi` for
    // example, but I won't complicate things here...
    //
    *--fiber->sp = (uintptr_t)qff_handle_exit;
    *--fiber->sp = (uintptr_t)fiber_func;
    fiber->sp -= 6;
    
    // Add the new fiber to the end of the run queue.
    list_add_tail(runnable_queue, (Node*)fiber);
}

/*
 * Yield to the next runnable fiber.
 *
 * You should only call this within fibers, or odd things are
 * likely to happen 👻...
 */
void qff_yield(void) {
    if (list_empty(runnable_queue)) {
        if (!qff_current_fiber) {
            // We don't have anything runnable, and there's no current fiber - game over
            //
            // In this case, we switch back to the "init fiber", which is the continuation
            // of the computation that made the first call to `qff_yield`.
            //
            // The result will be returning from that call (and the `qff_run` that kicked
            // us off in the first place).
            //
            qff_switch_fiber(&init_fiber);
        } else {
            // We don't have anything runnable, but there is a current fiber - just continue that
            return;
        }
    } else {
        // There's more runnable things!
        if (qff_current_fiber && qff_current_fiber != &init_fiber) {
            // We're running a fiber already, and it wants to yield - send it to the back of the queue
            list_add_tail(runnable_queue, (Node*)qff_current_fiber);
        } 

        qff_switch_fiber((QffFiber*)list_pop_head(runnable_queue));
    }
}
