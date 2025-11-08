.syntax unified
.arch armv7-m

.section .vector_table
.word   _stack_top
.word   _reset_handler + 1
.word   NMI_Handler + 1
.word   HardFault_Handler + 1
.word   0, 0, 0, 0, 0, 0, 0
.word   SVC_Handler
.word   0, 0
.word   PendSV_Handler
.word   SysTick_Handler

.section .text
.thumb
.global _reset_handler
_reset_handler:
    /* Set the stack pointer */
    ldr r0, =_stack_top
    mov sp, r0

    /*
     * This is the robust, byte-by-byte copy loop for .data section.
     */
    ldr r0, =_sidata  /* Start of .data in Flash (Source) */
    ldr r1, =_sdata   /* Start of .data in RAM (Destination) */
    ldr r2, =_edata   /* End of .data in RAM */
.L_data_copy_loop:
    cmp r1, r2
    bhs .L_bss_init   /* If dest >= end, branch to BSS init */
    ldrb r3, [r0], #1 /* Load BYTE from source and post-increment */
    strb r3, [r1], #1 /* Store BYTE to destination and post-increment */
    b .L_data_copy_loop
.L_bss_init:

    /*
     * This is the robust, byte-by-byte clear loop for .bss section.
     */
    ldr r0, =__bss_start__
    ldr r1, =__bss_end__
    movs r2, #0
.L_bss_zero_loop:
    cmp r0, r1
    bhs .L_call_main
    strb r2, [r0], #1 /* Store a zero BYTE and post-increment */
    b .L_bss_zero_loop
.L_call_main:

    /* Call the C main function */
    bl main

_loop:
    b _loop

/* Default handlers */
.thumb_func
.weak NMI_Handler
NMI_Handler:
    b .

.thumb_func
.weak HardFault_Handler
HardFault_Handler:
    b .