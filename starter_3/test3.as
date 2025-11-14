    add 0 0 1
    add 0 0 2
    beq 1 2 equal
    add 0 0 3
equal add 0 0 4
    add 0 0 5
    beq 4 5 skip
    add 0 0 6
skip add 0 0 7
    beq 6 7 done
    add 0 0 1
done halt