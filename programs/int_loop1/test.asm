    ; i = 0
    add x5, x0, x0
    ; max = 10
    addi x6, x0, 10
    
loop_cmp:
    beq x5, x6, done
loop:
    ; offset = i * 4
    srli x10, x5, 2
    
    sw x5, 2816(x10)
    sw x5, 3072(x10)
    sw x5, 3328(x10)
    
    addi x5, x5, 1
    beq x0, x0, loop_cmp
    
done:
    addi x20, x0, 10
