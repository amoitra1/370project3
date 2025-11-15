    lw 0 1 memA
    sw 0 1 memB
    lw 0 2 memB
    add 1 2 3
    sw 0 3 memC
    lw 0 4 memC
    add 2 4 5
    lw 0 6 memD
    add 5 6 7
    beq 7 0 skip
    add 7 1 2
skip sw 0 2 rst1
    halt
memA .fill 8
memB .fill 0
memC .fill 0
memD .fill 4
rst1 .fill 0
