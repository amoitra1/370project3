    lw 0 1 dataA
    add 1 1 2
    add 2 2 3
    add 3 3 1
    nor 1 2 4
    lw 0 5 dataB
    add 4 5 6
    beq 6 0 end
    sw 0 6 tsttt
end lw 0 7 tsttt
    halt
dataA .fill 2
dataB .fill 10
tsttt .fill 0