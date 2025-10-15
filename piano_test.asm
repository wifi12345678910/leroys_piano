; Description: 'a' key → Red color + 660Hz sound (special)
; 's' key → Green color + 880Hz sound (special)
; 'd' key → Blue color + 1045Hz sound (special)
; All other keys → White color + 495Hz sound (default)

; Piano Port Constants
.const GET_KEY      0x9000      ; Read key pressed
.const PLAY_NOTE    0x9001      ; Play note (type<<4 | freq)
.const SHOW_KEY     0x9002      ; Show keypress visual
.const SET_KEYNOTE  0x9003      ; Set/Get keynote
.const SET_KEYCOLOR 0x9004      ; Set/Get key color
.const SET_KEYLABEL 0x9005      ; Set/Get key label

!main
    cal !setup_keys             ; Set up all the keys with colors
    
!main_loop
    lod rA, [GET_KEY]           ; Read current key
    cmp rA, rZ                  ; Is it zero (no key)?
    je !main_loop               ; If no key, keep waiting
    
    ; We got a key! Show it and play sound immediately
    str [SHOW_KEY], rA          ; Show the key press
    
    ; Check which key and play appropriate sound
    cmp rA, 'a'
    je !play_a
    
    cmp rA, 's' 
    je !play_s
    
    cmp rA, 'd'
    je !play_d
    
    ; Default sound for other keys
    set rB, 5                   ; 220 + 5*55 = 495Hz
    str [PLAY_NOTE], rB
    jmp !main_loop              ; Go back to main loop immediately

!play_a
    set rB, 8                   ; 220 + 8*55 = 660Hz
    str [PLAY_NOTE], rB
    jmp !main_loop              ; Go back to main loop

!play_s  
    set rB, 12                  ; 220 + 12*55 = 880Hz
    str [PLAY_NOTE], rB
    jmp !main_loop              ; Go back to main loop

!play_d
    set rB, 15                  ; 220 + 15*55 = 1045Hz
    str [PLAY_NOTE], rB
    jmp !main_loop              ; Go back to main loop

!setup_keys
    ; Set up key 'a' with red color
    set rA, 'a'
    str [SET_KEYNOTE], rA       ; Select key 'a'
    set rA, 224                 ; 0xE0 = Red color
    str [SET_KEYCOLOR], rA      ; Set red color
    
    ; Set up key 's' with green color
    set rA, 's'
    str [SET_KEYNOTE], rA       ; Select key 's'
    set rA, 28                  ; 0x1C = Green color
    str [SET_KEYCOLOR], rA      ; Set green color
    
    ; Set up key 'd' with blue color
    set rA, 'd'
    str [SET_KEYNOTE], rA       ; Select key 'd'
    set rA, 3                   ; 0x03 = Blue color
    str [SET_KEYCOLOR], rA      ; Set blue color
    
    ret