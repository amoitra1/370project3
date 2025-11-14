add 0 0 1
beq 1 0 L1
add 0 0 2
beq 2 2 L2
halt
L2 add 0 0 3
beq 3 0 L3
add 0 0 4
L1 beq 4 4 L3
add 0 0 5
L3 halt