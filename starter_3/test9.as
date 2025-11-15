    lw 0 1 valX
    add 1 1 2
    add 2 2 1
    nor 1 2 3
    lw 0 4 valY
    add 3 4 5
    add 5 1 6
    sw 0 6 rst1
    lw 0 7 rst1
    beq 7 0 done
    add 7 6 1
done halt
valX .fill 4
valY .fill 12
rst1 .fill 0
