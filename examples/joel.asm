; bounce joel around the screen
; JOEL is 23 pixels wide and 5 pixels tall
; v1 = x
; v2 = y
; v3 = x direction (0 = right, 1 = left)
; v4 = y direction (0 = down, 1 = left)
; v5 = 1
; v6 = delay time
; v7 = delay counter
; v8 = sound time

start:
    LD      v1, 0
    LD      v2, 0
    LD      v5, 1
    LD      v6, 3
    LD      v8, 5

frame:
    CLS
    CALL    draw_joel

    LD      DT, v6
delay:
    LD      v7, DT
    SE      v7, 0
    JP      delay

move_x:
    SE      v3, 0
    JP      move_x_left
move_x_right:
    ADD     v1, 1
    SE      v1, 45
    JP      move_y
    LD      v3, 1
    LD      ST, v8
move_x_left:
    SUB     v1, v5
    SE      v1, 0
    JP      move_y
    LD      v3, 0
    LD      ST, v8

move_y:
    SE      v4, 0
    JP      move_y_up
move_y_down:
    ADD     v2, 1
    SE      v2, 27
    JP      frame
    LD      v4, 1
    LD      ST, v8
move_y_up:
    SUB     v2, v5
    SE      v2, 0
    JP      frame
    LD      v4, 0
    LD      ST, v8
    JP      frame

; draws 4 sprites starting at (v1, v2)
; destroys vA
draw_joel:
    LD      vA, v1
    LD      I, letter_j
    DRW     vA, v2, 5
    ADD     vA, 5
    LD      I, letter_o
    DRW     vA, v2, 5
    ADD     vA, 5
    LD      I, letter_e
    DRW     vA, v2, 5
    ADD     vA, 5
    LD      I, letter_l
    DRW     vA, v2, 5
    RET

letter_j:
.db 0xf0, 0x20, 0x20, 0xA0, 0xe0, 0x00
letter_o:
.db 0xf0, 0x90, 0x90, 0x90, 0xf0, 0x00
letter_e:
.db 0xf0, 0x80, 0xf0, 0x80, 0xf0, 0x00
letter_l:
.db 0x80, 0x80, 0x80, 0x80, 0xf0, 0x00

