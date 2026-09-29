/* FellowNG M9.2d project-authored 68020 VBR/exception-frame probe.
 * Bare-metal and ROM-free. Moves VBR away from zero and proves that TRAP #0
 * uses the relocated vector table and a 68020 format-0 exception frame.
 */
        .cpu 68020
        .section .vectors,"a"
        .org 0
        .long 0x00080000
        .long start

        .section .text,"ax"
start:
        lea relocated_vectors,%a0
        movec %a0,%vbr
        lea result_area,%a1
        clr.b (%a1)
        trap #0
after_trap:
        move.b #'R',result_area+1
done:
        bra done

trap0_handler:
        move.b #'V',result_area
        rte

        .section .data,"aw"
        .balign 256
relocated_vectors:
        .space 0x80
        .long trap0_handler
        .space 0x7c

        .global result_area
result_area:
        .space 16
