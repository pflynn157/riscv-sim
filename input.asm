addi x1, x0, 10
addi x2, x0 20
slti x3, x2, 10
sltiu x4, x2, 10
xori x5, x2, 10
ori x6, x2, 10
andi x7, x2, 10
slli x8, x2, 10
srli x9, x2, 10
srai x10, x2, 10

add x3, x1, x2
sub x4, x1, x2
sll x5, x1, x2
srl x6, x1, x2
sra x7, x1, x2
and x8, x1, x2
or x9, x1, x2
xor x10, x1, x2
slt x11, x1, x2
sltu x12, x1, x2

beq x1, x2, LOOP
bne x1, x2, LOOP
blt x1, x2, LOOP
bge x1, x2, LOOP
bltu x1, x2, LOOP
bgeu x1, x2, LOOP
