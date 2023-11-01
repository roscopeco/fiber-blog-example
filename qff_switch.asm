    bits 64
    default rel
    global  _qff_switch_fiber
    extern  _qff_current_fiber
    section .text


_qff_switch_fiber:
    ; Save registers in outgoing task's stack
    pushfq
    push    rax
    push    rbx
    push    rcx
    push    rdx
    push    rdi
    push    rsi
    push    rbp
    push    r8
    push    r9
    push    r10
    push    r11
    push    r12
    push    r13
    push    r14
    push    r15

    ; Switch stacks
    mov     rsi,[_qff_current_fiber]
    test    rsi,rsi
    je      .switch_new

    mov     [rsi],rsp

.switch_new:
    mov     [_qff_current_fiber],rdi
    mov     rsp,[rdi]

    ; Restore registers from new task's stack
    pop     r15
    pop     r14
    pop     r13
    pop     r12
    pop     r11
    pop     r10
    pop     r9
    pop     r8
    pop     rbp
    pop     rsi
    pop     rdi
    pop     rdx
    pop     rcx
    pop     rbx
    pop     rax
    popfq

    ; return to new task
    ret
