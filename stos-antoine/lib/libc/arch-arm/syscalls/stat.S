/* autogenerated by gensyscalls.py */
#include <sys/stos-syscalls.h>

    .text
    .type stat, #function
    .globl stat
    .align 4
    .fnstart

stat:
    .save   {r4, r7}
    stmfd   sp!, {r4, r7}
    ldr     r7, =__NR_stat
    swi     #0
    ldmfd   sp!, {r4, r7}
    movs    r0, r0
    bxpl    lr
    b       __set_syscall_errno
    .fnend
