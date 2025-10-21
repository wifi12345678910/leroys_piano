; Keys 1-5 play .wav file and light up different colors
; The rest of the keys are just playing one note 
; This was to test the .wav files and the highlighting aspects


.const GET_KEY 0x9000
.const SHOW_KEY 0x9003
.const SET_KEY_COLOR 0x9006
.const SET_KEY_FREQ 0x9004
.const PLAY_WAV_ID 0x9002
.const PLAY_FREQUENCY 0x9001

!main
    jmp !input_loop

!input_loop
    ; Get keyboard input
    lod rA, [GET_KEY]
    cmp rA, 0
    je !input_loop
    
    ; Handle specific keys
    cmp rA, 49              ; '1' key
    je !key_1
    cmp rA, 50              ; '2' key  
    je !key_2
    cmp rA, 51              ; '3' key
    je !key_3
    cmp rA, 52              ; '4' key
    je !key_4
    cmp rA, 53              ; '5' key
    je !key_5
    cmp rA, 97              ; 'a' key
    je !key_a
    cmp rA, 27              ; ESC
    je !exit
    
    ; Default keys - just play beep
    set rC, 440
    str [PLAY_FREQUENCY], rC
    jmp !input_loop

!key_1
    ; Select key and set color FIRST
    str [SET_KEY_FREQ], rA  ; Select key '1'
    set rB, 224             ; Red color
    str [SET_KEY_COLOR], rB ; Set color immediately
    str [SHOW_KEY], rA      ; Show with color already set
    set rC, 0               ; Applause
    str [PLAY_WAV_ID], rC
    jmp !input_loop

!key_2  
    ; Select key and set color FIRST
    str [SET_KEY_FREQ], rA  ; Select key '2'
    set rB, 3               ; Blue color
    str [SET_KEY_COLOR], rB ; Set color immediately
    str [SHOW_KEY], rA      ; Show with color already set
    set rC, 1               ; Arrow
    str [PLAY_WAV_ID], rC
    jmp !input_loop

!key_3
    ; Select key and set color FIRST
    str [SET_KEY_FREQ], rA  ; Select key '3'
    set rB, 28              ; Green color
    str [SET_KEY_COLOR], rB ; Set color immediately
    str [SHOW_KEY], rA      ; Show with color already set
    set rC, 2               ; bicycle bell
    str [PLAY_WAV_ID], rC
    jmp !input_loop

!key_4
    ; Select key and set color FIRST
    str [SET_KEY_FREQ], rA  ; Select key '4'
    set rB, 252             ; Yellow color
    str [SET_KEY_COLOR], rB ; Set color immediately
    str [SHOW_KEY], rA      ; Show with color already set
    set rC, 3               ; Blip
    str [PLAY_WAV_ID], rC
    jmp !input_loop

!key_5
    ; Select key and set color FIRST
    str [SET_KEY_FREQ], rA  ; Select key '5'
    set rB, 163             ; Purple color
    str [SET_KEY_COLOR], rB ; Set color immediately
    str [SHOW_KEY], rA      ; Show with color already set
    set rC, 4               ; Bloop
    str [PLAY_WAV_ID], rC
    jmp !input_loop

!key_a
    ; Blue color + Arrow sound WITH visual (no beep!)
    str [SET_KEY_FREQ], rA  ; Select key 'a'
    set rB, 3               ; Blue color
    str [SET_KEY_COLOR], rB ; Set color
    str [SHOW_KEY], rA      ; Show visual (NO beep because no default frequency!)
    set rC, 1               ; Arrow only
    str [PLAY_WAV_ID], rC
    jmp !input_loop

!exit
    jmp !exit