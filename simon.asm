
.const GET_KEY 0x9000
.const SHOW_KEY 0x9003
.const SET_KEY_COLOR 0x9006
.const SET_KEY_FREQ 0x9004
.const PLAY_WAV_ID 0x9002
.const PLAY_FREQUENCY 0x9001
.const RAND    0x8010
.const RED 224
.const GREEN 28
.const BLUE 8
.const YELLOW 252
.const PURPLE 163

    SET rC, rZ
    
    SET rB, RED
    SET rA, 49
    STR [SET_KEY_FREQ], rA
    STR [SET_KEY_COLOR], rB
    STR [SET_KEY_FREQ], rA
    SET rA, 0x0188
    STR [SET_KEY_FREQ], rA
    
    SET rB, GREEN
    SET rA, 50
    STR [SET_KEY_FREQ], rA
    STR [SET_KEY_COLOR], rB
    STR [SET_KEY_FREQ], rA
    SET rA, 0x020B
    STR [SET_KEY_FREQ], rA
    
    SET rB, BLUE
    SET rA, 51
    STR [SET_KEY_FREQ], rA
    STR [SET_KEY_COLOR], rB
    STR [SET_KEY_FREQ], rA
    SET rA, 0x0293
    STR [SET_KEY_FREQ], rA
    
    SET rB, YELLOW
    SET rA, 52
    STR [SET_KEY_FREQ], rA
    STR [SET_KEY_COLOR], rB
    STR [SET_KEY_FREQ], rA
    SET rA, 0x0310
    STR [SET_KEY_FREQ], rA
    
    SET rB, PURPLE
    SET rA, 53
    STR [SET_KEY_FREQ], rA
    STR [SET_KEY_COLOR], rB
    STR [SET_KEY_FREQ], rA
    SET rA, 0x0416
    STR [SET_KEY_FREQ], rA

    SET rB, 0
    SET rA, 120
    STR [SET_KEY_FREQ], rA
    STR [SET_KEY_COLOR], rB

    set rA, rZ
    set rB, rZ

!main 

!input_loop_main    
    LOD rA, [GET_KEY]
    SET rB, 48
    CMP rA, rB
    JE !game_loop
    JMP !input_loop_main    

!game_loop    
    JMP !simon_new
!game_loop1
    JMP !simon_pattern
!game_loop2
    JMP !simon_listen
!game_loop3
    CMP rE, rZ 
    JE !game_loop
    
    JMP !main

!simon_new 
    ADD rC, 1
    LOD rA, [RAND]
    MOD rA, 5
    ADD rA, 49
    PSH rA
    JMP !game_loop1

!simon_pattern
    SET rD, rC
    ADD SP, rC
!simon_loop
    CMP rD, rZ
    JE !stack_play_empty 
    LOD rA, [SP]
    STR [SHOW_KEY], rA
    SET rA, 3
    DLY 50
    SUB rD, 1
    SUB SP, 1
    JMP !simon_loop
!stack_play_empty
    JMP !game_loop2

!simon_listen
    SET rE, rZ
    SET rD, rC
    ADD SP, rC
!input_loop    
    LOD rA, [GET_KEY]
    CMP rA, rZ
    JE !input_loop

    CMP rA, 53
    JG !fail_state
    CMP rA, 49
    JL !fail_state

    STR [SHOW_KEY], rA
    LOD rB, [SP]
    CMP rA, rB
    JNE !fail_state
    SUB rD, 1
    SUB SP, 1
    CMP rD, rZ
    JE !input_ret
    JMP !input_loop

!fail_state
    SET rA, 120
    STR [SHOW_KEY], rA
    SET rA, 0x0FD1
    STR [PLAY_FREQUENCY], rA
    SET rE, rC
    SUB rE, rD
    ADD PC, rE
    SET rE, 1
    SET rC, rZ
    SET SP, 0x7FFF 
!input_ret
    JMP !game_loop3

!exit
    JMP !exit