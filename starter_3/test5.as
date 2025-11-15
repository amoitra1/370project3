    lw 0 1 val1
    add 1 1 2
    nor 2 1 3
    add 3 2 4
    sw 0 4 str1
    lw 0 5 str1
    add 5 4 6
    beq 6 0 done
    add 6 6 7
done halt
val1 .fill 5
str1 .fill 0