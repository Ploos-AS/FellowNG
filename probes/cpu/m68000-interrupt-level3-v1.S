/* FellowNG M9.2c project-authored 68000 interrupt probe.
 * Bare-metal and ROM-free. Level 3 autovector must enter supervisor mode,
 * use the supervisor stack, record the handler, and return with RTE.
 */
        .cpu 68000
        .section .vectors,"a"
        .org 0
        .long 0x00080000
        .long start
        .org 0x6c
        .long level3_handler

        .section .text,"ax"
start:
        lea result_area,%a0
        clr.b (%a0)
        clr.b 1(%a0)
        /* User mode, interrupt mask 0: host injects level 3 after this. */
        move.w #0x0000,%sr
wait:
        nop
        bra wait

level3_handler:
        move.b #'I',result_area
        rte
after_interrupt:
        move.b #'R',result_area+1
done:
        bra done

        .section .data,"aw"
        .global result_area
result_area:
        .space 16
