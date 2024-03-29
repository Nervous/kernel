/* autogenerated by gensyscalls.py */
#include <sys/stos-syscalls.h>

    .text
    .type fstat, @function
    .globl fstat
    .align 4

fstat:

    /* invoke trap */
    mov.l   0f, r3  /* trap num */
    trapa   #(2 + 0x10)

    /* check return value */
    cmp/pz  r0
    bt      __NR_fstat_end

    /* keep error number */
    sts.l   pr, @-r15
    mov.l   1f, r1
    jsr     @r1
    mov     r0, r4
    lds.l   @r15+, pr

__NR_fstat_end:
    rts
    nop

    .align  2
0:  .long   __NR_fstat
1:  .long   __set_syscall_errno
