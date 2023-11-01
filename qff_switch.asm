;
; Quite Fast Fibers - the important bit
;
; See comments in qff.h.
; 
; Copyright (c) 2023 Ross Bamford
; MIT License (see LICENSE.md)
;
    bits    64                                  ; 64-bits is all the rage these days...
    default rel                                 ; Try to do relative addressing
    global  _qff_switch_fiber                   ; Export the `gff_switch_fiber` function
    extern  _qff_current_fiber                  ; Import the `gff_current_fiber` variable
    section .text                               ; Stick all this in the `.text` section 📚


; This is really the main bit of this example, and what the blog post 
; is all about. 
; 
; Everything else you see here has been leading us to this point - 
; the point at which I reveal to you just how simple a task switch is.
;
; So, 🎉 I guess...... 🥳😅
;
_qff_switch_fiber:
    ; If we don't have a current fiber, it means that it finished, 
    ; so don't save anything as we've already freed the stack...
    ;
    mov     rsi,[_qff_current_fiber]            ; Get the current fiber pointer
    test    rsi,rsi                             ; Is it null?
    jz      .do_switch                          ; If so, skip this next bit...

    ; Save CPU flags and registers in outgoing fiber's stack
    ;
    ; In the real world, there'd be more to save than this. The CPU has a
    ; whole bunch of registers for floating point, SSE, and all that other 
    ; stuff that I've been quite careful to avoid using in this example - 
    ; saving all the general-purpose registers is long enough and makes 
    ; the example hard to follow, without adding in a ton more.
    ;
    ; We only save the registers required to be callee-saved (except rsp since
    ; that's literally this functions reason to exist) by the SystemV ABI 
    ; for x86_64 here - since I know this is only being called from C.
    ;
    ; Again, for a more general solution you might just save _all_ the
    ; general-purpose registers (for example).
    ;
    ; If this were a full preemptive multitasking switch (e.g. in an OS kernel),
    ; we'd definitely need to save **everything** (including the `qflags` and 
    ; various control registers). So let's consider ourselves lucky it's not 🍀
    ;
    push    rbx
    push    rbp
    push    r12
    push    r13
    push    r14
    push    r15

    ; Switch stacks
    mov     [rsi],rsp                           ; Save the current stack pointer in the fiber struct

.do_switch
    mov     [_qff_current_fiber],rdi            ; Load the new fiber argument into the current fiber pointer
    mov     rsp,[rdi]                           ; And switch to the new fiber's stack

    ; Restore registers from new fiber's stack
    ;
    ; All we need to do here is make sure we pop them in the reverse 
    ; order we pushed them in earlier...
    ;
    ; Again, IRL you'd need to restore a bunch more stuff, but we're 
    ; fine with these today.
    ;
    pop     r15
    pop     r14
    pop     r13
    pop     r12
    pop     rbp
    pop     rbx

    ; return to new fiber
    ret
