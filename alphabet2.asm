.const GET_KEY 0x9000
.const PLAY_LETTER 0x900B
!main
    jmp !loop

!loop
    lod rA, [GET_KEY]
    cmp rA, 0
    je !loop
    STR [PLAY_LETTER], rA
    jmp !loop
