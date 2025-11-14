add 0 0 1
add 0 0 2
nor 1 2 3
add 3 3 4
add 4 3 5
lw 0 6 v
add 6 5 7
sw 0 7 sink
lw 0 2 sink
add 2 7 1
halt
v .fill 7
sink .fill 0