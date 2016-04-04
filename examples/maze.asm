; (v2, v3) = width/height
; (v9, va) = guy location
; (vb, vc) = finish
; ve = 1
; vd = key press

reset:
    LD      vE, 1           ; constant!
    CALL    draw_level

wait:
    LD      v4, v9
    LD      v5, vA

cycle:
    LD      vD, 6
    SKNP    vD
    JP      right_pressed
    LD      vD, 4
    SKNP    vD
    JP      left_pressed
    LD      vD, 8
    SKNP    vD
    JP      down_pressed
    LD      vD, 2
    SKNP    vD
    JP      up_pressed

    JP      cycle

; in the section below, (v4, v5) represents the new
; guy location after the move and should not be destroyed

right_pressed:
    ; are we on right edge?
    ADD     v4, 1
    SNE     v4, v2
    JP      invalid_move
    JP      check_valid_move

left_pressed:
    ; are we on left edge?
    SNE     v4, 0
    JP      invalid_move
    SUB     v4, vE
    JP      check_valid_move

down_pressed:
    ; are we on bottom edge?
    ADD     v5, 1
    SNE     v5, v3
    JP      invalid_move
    JP      check_valid_move

up_pressed:
    ; are we on top edge?
    SNE     v5, 0
    JP      invalid_move
    SUB     v5, vE
    ; fall through for now!
    ;JP      check_valid_move

check_valid_move:
    CALL    get_tile
    SNE     v0, 1       ; is it a brick?
    JP      invalid_move

    CALL    draw_guy
    LD      v9, v4
    LD      vA, v5
    CALL    draw_guy

    ; check if we have a winner!
    SE      v9, vb
    JP      wait_for_key_end
    SE      vA, vc
    JP      wait_for_key_end

winning_sequence:
    CALL    draw_guy
    LD      v2, 8
winning_sequence_loop:
    LD      v0, 10
    LD      ST, v0
    LD      v1, 20
    LD      DT, v1
    CALL    draw_brick_at_guy
winning_sequence_delay:
    LD      v0, DT
    SE      v0, 0
    JP      winning_sequence_delay
    
    SNE     v2, 0
    JP      game_over
    SUB     v2, vE
    JP      winning_sequence_loop

game_over:
    JP      game_over

invalid_move:
    LD      v0, 10
    LD      ST, v0
    JP      wait_for_key_end

wait_for_key_end:
    SKNP    vD
    JP      wait_for_key_end
    JP      wait
    

; load the current level
; ouput: I = current level

load_level_pointer:
    LD      I, level1
    RET

; input: (v4, v5) is tile in question
; input: (v2, v3) is width/height
; output: v0 = tile
; destroys: v0, v1
get_tile:
    CALL    load_level_pointer  ; start I off in the right spot
    ADD     I, vE               ; add 2 to skip width/height of level
    ADD     I, vE
    ADD     I, v4               ; add x coordinate
    LD      v1, v5
get_tile_y:
    SNE     v1, 0               ; is y 0?
    JP      get_tile_done       ; finish up
    ADD     I, v2               ; add the width
    SUB     v1, vE              ; subtract 1 from y
    JP      get_tile_y          ; keep going
get_tile_done:
    LD      v0, [I]             ; load the value of the tile
    RET                         ; done

; draw the guy sprite at (v9, vA)
; destroys: v0, v1
draw_guy:
    LD      v0, v9
    SHL     v0
    SHL     v0
    LD      v1, vA
    SHL     v1
    SHL     v1
    LD      I, guy_sprite
    DRW     v0, v1, 4
    RET

; draw brick sprite at (v9, vA)
; destroys: v0, v1
draw_brick_at_guy:
    LD      v0, v9
    SHL     v0
    SHL     v0
    LD      v1, vA
    SHL     v1
    SHL     v1
    LD      I, brick
    DRW     v0, v1, 4
    RET


; draw a level, destroys v0-v8
; returns (v9, va) is starting location
; returns (vb, vc) is ending location
; returns (v2, v3) is width 
draw_level:
    CALL    load_level_pointer
    LD      v1, [I]  ; v0 = width, v1 = height
    LD      v2, v0   ; v2 = width
    LD      v3, v1   ; v3 = height
    LD      v4, 0    ; v4 = x tile (pixels)
    LD      v5, 0    ; v5 = y tile (pixels)
    LD      v6, 2    ; v6 = offset
    LD      v7, 0    ; v7 = x tile (coord)
    LD      v8, 0    ; v8 = y tile (coord)

draw_row:
    CALL    load_level_pointer
    ADD     I, v6     ; re-add the offset

    LD      v0, [I]   ; load current tile
    SE      v0, 1     ; is it a brick?
    JP      not_brick ; nope, do something else
draw_brick:
    LD      I, brick  ; draw the brick
    DRW     v4, v5, 4
not_brick:
    SE      v0, 2     ; is it starting location?
    JP      not_starting
draw_starting:
    LD      v9, v7    ; save starting x,y
    LD      vA, v8
    LD      I, guy_sprite
    DRW     v4, v5, 4
not_starting:
    SE      v0, 3     ; is it finish?
    JP      not_finishing
draw_finishing:
    LD      vB, v7    ; save finishing x,y
    LD      vC, v8
    LD      I, finish_sprite
    DRW     v4, v5, 4
not_finishing:
    ADD     v4, 4     ; add 8 to our x pixel
    ADD     v7, 1     ; add 1 to our x coordinate
    SE      v7, v2    ; at the edge of width?
    JP      after_row_reset
row_reset:
    LD      v4, 0     ; reset x pixel
    LD      v7, 0     ; reset x coordinate
    ADD     v5, 4     ; add 8 to our y pixel
    ADD     v8, 1     ; add 1 to our y coordinate
    SNE     v8, v3    ; at the edge of height?
    RET
after_row_reset:
    ADD     v6, 1     ; move to next tile
    JP      draw_row


level1:
.db 16, 8
.db 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
.db 1, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1
.db 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1
.db 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1
.db 1, 0, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1
.db 1, 1, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1
.db 1, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1
.db 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1

; hex numbers:
; 0 = 0000  4 = 0100  8 = 1000  c = 1100
; 1 = 0001  5 = 0101  9 = 1001  d = 1101
; 2 = 0010  6 = 0110  a = 1010  e = 1110
; 3 = 0011  7 = 0111  b = 1011  f = 1111
brick:
.db 0xf0, 0xf0, 0xf0, 0xf0
guy_sprite:
.db 0x00
.db 0x60
.db 0x60
.db 0x00
finish_sprite:
.db 0xa0, 0x50, 0xa0, 0x50

