; x2 is the stack sp
addi x2, x0, 768

; i
add x6, x0, x0
sw x6, 4(x2)
add x6, x0, x0

; 256
addi x7, x0, 256

loop_cmp:
    lw x25, 4(x2)
    add x0, x0, x0
    add x0, x0, x0
    bge x25, x7, loop_done

loop:
    lw x6, 4(x2)
    add x0, x0, x0
    srli x20, x6, 2
    
    flw f4, 1024(x20)
    flw f5, 2048(x20)
    add x0, x0, x0
    add x0, x0, x0

    fsub.s f6, f4, f5

    lui x17, 1
    add x18, x20, x17
    fsw f6, 0(x18)
    
    lw x21, 4(x2)
    add x0, x0, x0
    add x0, x0, x0
    addi x21, x21, 1
    sw x21, 4(x2)
    add x0, x0, x0
    beq x0, x0, loop_cmp

loop_done:
    addi x30, x0, 20

