addi x1, x0, 10
add x1, x2, x3
sw x2, 10(x3)
lw x4, 10(x3)
beq x0, x0, LOOP
bne x0, x0, LOOP
lui x10, 1234
auipc x10, 1234
jal x10, 1234
jalr x10, x11, 1234
