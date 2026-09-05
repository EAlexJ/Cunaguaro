.section .text.init
.global _start

_start:
    la sp, __stack_top

    la t0, __data_load_start
    la t1, __data_start
    la t2, __data_end
copy_data_loop:
    bge t1, t2, zero_bss
    lw t3, 0(t0)
    sw t3, 0(t1)
    addi t0, t0, 4
    addi t1, t1, 4
    j copy_data_loop

zero_bss:
    la t0, __bss_start
    la t1, __bss_end
zero_bss_loop:
    bge t0, t1, enter_main
    sw zero, 0(t0)
    addi t0, t0, 4
    j zero_bss_loop

enter_main:
    call main

halt:
    j halt