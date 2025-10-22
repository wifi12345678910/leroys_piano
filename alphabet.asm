.const GET_KEY 0x9000

!main
    jmp !loop

!loop
    lod rA, [GET_KEY]
    cmp rA, 0
    je !loop
    jmp !loop
