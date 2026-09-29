/* FellowNG M9.2b project-authored 68000 address-error probe.
 * Bare-metal and ROM-free. An odd word read must vector through exception 3.
 * The handler records A and stops; it deliberately does not RTE because the
 * 68000 bus/address-error frame is a special group-2 frame.
 */
        .cpu 68000
        .section .vectors,"a"
        .org 0
        .long 0x00080000
        .long start
        .org 0x0c
        .long address_error_handler

        .section .text,"ax"
        .org 0x400
start:
        lea result_area,%a0
        clr.b (%a0)
        lea odd_data,%a1
        addq.l #1,%a1
fault_site:
        move.w (%a1),%d0
unexpected_return:
        move.b #'X',result_area
done:
        bra done

address_error_handler:
        move.b #'A',result_area
handler_done:
        bra handler_done

        .section .data,"aw"
        .org 0x1000
        .global result_area
result_area:
        .space 16
odd_data:
        .word 0x1234
