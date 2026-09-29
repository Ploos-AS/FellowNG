/* FellowNG M9.2a project-authored 68000 exception probe.
 * Bare-metal, no Kickstart/AmigaOS dependencies.
 *
 * The probe records one byte per successful exception handler in result_area:
 *   I = illegal instruction, D = divide by zero, T = TRAP #0,
 *   P = privilege violation.
 * Address-error is kept as a separate follow-up because its 68000 stack frame
 * is implementation-sensitive and must not be conflated with the simple frame.
 */
        .cpu 68000
        .section .vectors,"a"
        .org 0
        .long 0x00080000
        .long start
        .org 0x10
        .long illegal_handler
        .org 0x14
        .long divzero_handler
        .org 0x20
        .long privilege_handler
        .org 0x80
        .long trap0_handler

        .section .text,"ax"
start:
        lea result_area,%a0
        moveq #0,%d0
        move.b %d0,(%a0)
        move.b %d0,1(%a0)
        move.b %d0,2(%a0)
        move.b %d0,3(%a0)

        illegal
after_illegal:
        moveq #1,%d0
        divu %d0,%d1
        moveq #0,%d0
        divu %d0,%d1
after_divzero:
        .global trap_site
trap_site:
        trap #0
after_trap:
        /* Enter user mode, then execute privileged STOP. */
        move.w #0x0000,%sr
        stop #0x2000
after_privilege:
done:
        bra done

illegal_handler:
        move.b #'I',result_area
        addq.l #2,2(%sp)
        rte
divzero_handler:
        move.b #'D',result_area+1
        /* DIVU-by-zero stacks the PC of the following instruction. */
        rte
trap0_handler:
        move.b #'T',result_area+2
        /* TRAP stacks the return PC (the instruction after TRAP), so unlike
         * illegal/divzero/privilege the handler must not adjust stacked PC. */
        rte
privilege_handler:
        move.b #'P',result_area+3
        addq.l #4,2(%sp)
        rte

        .section .data,"aw"
        .global result_area
result_area:
        .space 16
