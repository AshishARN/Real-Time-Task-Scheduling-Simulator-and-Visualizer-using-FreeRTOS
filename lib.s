.syntax unified
.thumb

/*
 * void *memcpy(void *dest, const void *src, size_t n)
 * r0 = dest, r1 = src, r2 = n
 */
.global memcpy
.thumb_func
memcpy:
    /* Preserve the original destination pointer to return it */
    push {r0, r4}

    /* Loop and copy byte by byte */
.Lmemcpy_loop:
    cmp r2, #0
    beq .Lmemcpy_done
    ldrb r3, [r1], #1   @ Load byte from src and post-increment src pointer
    strb r3, [r0], #1   @ Store byte to dest and post-increment dest pointer
    subs r2, r2, #1
    b .Lmemcpy_loop
.Lmemcpy_done:
    /* Restore the original destination pointer into r0 (the return register) */
    pop {r0, r4}
    bx lr               @ Return


/*
 * void *memset(void *s, int c, size_t n)
 * r0 = s, r1 = c, r2 = n
 */
.global memset
.thumb_func
memset:
    /* Preserve the original destination pointer to return it */
    push {r0, r4}

    /* Loop and set byte by byte */
.Lmemset_loop:
    cmp r2, #0
    beq .Lmemset_done
    strb r1, [r0], #1   @ Store the byte and post-increment dest pointer
    subs r2, r2, #1
    b .Lmemset_loop
.Lmemset_done:
    /* Restore the original destination pointer into r0 (the return register) */
    pop {r0, r4}
    bx lr               @ Return