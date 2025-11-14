lw 0 1 val
add 1 1 2
sw 0 2 out
lw 0 3 out
add 3 2 4
nor 4 2 5
beq 5 0 done
add 0 0 6
done halt
val .fill 9
out .fill 0