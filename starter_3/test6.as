    add 0 0 1
    add 1 1 2
    beq 1 2 tkn1
    add 0 0 3
    add 0 0 4
tkn1 lw 0 5 val2
    add 5 5 6
    beq 6 5 tkn2
    add 6 5 7
    sw 0 7 str2
tkn2 lw 0 1 str2
    add 1 7 2
    halt
val2 .fill 8
str2 .fill 0