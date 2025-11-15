    add 0 0 1
    lw 0 2 dataC
    beq 1 0 brA
    add 2 2 3
brA add 2 2 4
    beq 4 2 brB
    lw 0 5 dataD
    add 4 5 6
brB nor 6 4 7
    sw 0 7 rst1
    halt
dataC .fill 6
dataD .fill 3
rst1 .fill 0
