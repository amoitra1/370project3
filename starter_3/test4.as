    lw 0 1 A
    add 1 1 2
    add 2 2 3
    beq 2 3 br1
    add 0 0 4
br1 add 3 3 5
    lw 0 6 B
    add 6 5 7
    beq 7 5 br2
    add 0 0 1
br2 sw 0 7 out
    lw 0 2 out
    halt
A .fill 5
B .fill 15
out .fill 0