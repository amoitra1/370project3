    lw 0 1 m1
    sw 0 1 m2
    lw 0 2 m2
    add 2 1 3
    lw 0 4 m3
    add 3 4 5
    nor 5 3 6
    beq 6 0 skip
    sw 0 5 m4
skip lw 0 7 m4
    halt
m1 .fill 3
m2 .fill 0
m3 .fill 7
m4 .fill 0