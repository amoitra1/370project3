    lw 0 1 data1
    add 1 1 2
    add 0 0 3
    lw 0 4 data2
    add 4 3 5
    sw 0 5 data3
    lw 0 6 data3
    add 6 5 7
    halt
data1 .fill 11
data2 .fill 22
data3 .fill 0