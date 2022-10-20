    ; i = 0
    add x5, x0, x0
    ; max = 10
    addi x6, x0, 10
    
    ; Store offsets
    ; Array A -> 0x0A00
    addi x10, x0, 2560
    
    ; Array B -> 0x0B00
    addi x11, x0, 2816
    
    ; Array C -> 0x0C00
    addi x12, x0, 3072
    
loop_cmp:
    beq x5, x6, done
loop:
    ; offset = i * 4
    srli x20, x5, 2
    
    add x21, x20, x10
    add x22, x20, x11
    add x23, x20, x12
    
    sw x5, 0(x21)
    sw x5, 0(x22)
    sw x5, 0(x23)
    
    addi x5, x5, 1
    beq x0, x0, loop_cmp
    
done:
    addi x20, x0, 10
    
;;-------------------------------------
    ; i = 0
    add x5, x0, x0
    
add_loop_cmp:
    beq x5, x6, add_done
    
add_loop:
    ; offset = i * 4
    srli x20, x5, 2
    
    add x21, x20, x10
    add x22, x20, x11
    add x23, x20, x12
    
    lw x7, 0(x21)
    lw x8, 0(x22)
    add x0, x0, x0
    add x0, x0, x0
    add x0, x0, x0
    
    add x9, x7, x8
    
    sw x9, 0(x23)
    
    addi x5, x5, 1
    beq x0, x0, add_loop_cmp

add_done:
    addi x20, x0, 20
    
