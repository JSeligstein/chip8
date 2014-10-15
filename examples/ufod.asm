BCD_LOC = 0x2f8

    LD   I, BOTTOM_UFO_SPRITE ; draw bottom ufo at (38, 8) x 3
    LD   v9, 0x38
    LD   va, 0x08
    DRW  v9, va, 0x3

    LD   I, TOP_UFO_SPRITE    ; draw top ufo at (0, 3) x 3
    LD   vb, 0x00
    LD   vc, 0x03
    DRW  vb, vc, 0x3

    LD   I, PLATFORM_SPRITE   ; draw line at (0x1d, 0x1f) x 1
    LD   v4, 0x1d
    LD   v5, 0x1f
    DRW  v4, v5, 0x1

    LD   v7, 0x00             ; player score = 0
    LD   v8, 0x0f             ; shots = 15
    CALL DRAW_SCORE


REDRAW:
    CALL DRAW_SHOT_COUNTS

    SNE  v8, 0x00            ; shots == 0 ?
GAME_OVER:
    JP   GAME_OVER           ; if yes, jump indefinitely

    LD   v4, 0x1e            ; draw missile at (30, 28) x 3
    LD   v5, 0x1c
    LD   I, MISSILE_SPRITE
    DRW  v4, v5, 0x3

    LD   ve, 0x00            ; shot not in progress
CHECK_KEYS:
    LD   v6, 0x80
    LD   vd, 0x04            ; key 4?
    SKNP vd                  ; skip if not
    LD   v6, 0xff            
    LD   vd, 0x05            ; key 5?
    SKNP vd                  ; skip if not
    LD   v6, 0x00
    LD   vd, 0x06            ; key 6?
    SKNP vd                  ; skip if not
    LD   v6, 0x01            ; v6 = 0x80 if no key, 0xff if 4, 0x00 if 5, 0x1 if 6
    SE   v6, 0x80            ; skip if no key pressed
    CALL MARK_SHOT_AND_PLAY_SOUND

GAME_LOOP:
    LD   I, TOP_UFO_SPRITE   ; erase top ufo
    DRW  vb, vc, 0x3

    RND  vd, 0x01            ; vd = rand() % 2
    ADD  vb, vd              ; move top ufo sprite to right
    DRW  vb, vc, 0x3         ; draw top ufo again
    SE   vf, 0x00            ; was there a collision?
    JP   TOP_UFO_HIT         ; YES!

                             ; no collision
    LD   I, BOTTOM_UFO_SPRITE ; erase bottom ufo
    DRW  v9, va, 0x3

    RND  vd, 0x01            ; vd = rand() % 2
    SE   vd, 0x00            ; if vd == 0, skip
    LD   vd, 0xff            ; if vd == 1, vd = 0xff
    ADD  v9, 0xfe            ; ufo moves to left or nothing
    DRW  v9, va, 0x3         ; draw ufo again
    SE   vf, 0x00            ; was there a collision?
    JP   BOTTOM_UFO_HIT      ; YES!

    SNE  ve, 0x00            ; if shot not in progress
    JP   CHECK_KEYS          ; jump to here

                             ; shot is in progress here
    LD   I, MISSILE_SPRITE   ; erase the missile
    DRW  v4, v5, 0x3

    SNE  v5, 0x00            ; if missile is at top of screen
    JP   SUBTRACT_SHOT       ; we missed, subtract a shot

    ADD  v5, 0xff            ; subtract 1 from missile y

    ADD  v4, v6              ; add or subtract 1 from missile x based on keypress
    DRW  v4, v5, 0x3         ; draw new missile

    SE   vf, 0x01            ; was there a collision?
    JP   GAME_LOOP           ; no? move again   

                             ; missile moved and there's now a collision
    LD   vd, 0x08            
    AND  vd, v5              ; vd = 0x8 & v5 (missile y)
    SNE  vd, 0x08            ; basically, is missile y == (8-15) ?
    JP   BOTTOM_UFO_HIT      ; if yes, it was the bottom ufo
    JP   TOP_UFO_HIT         ; if no, it was the top ufo

SUBTRACT_SHOT:
    CALL DRAW_SHOT_COUNTS   ; erase shots
    ADD  v8, 0xff           ; subtract 1 from shots
    JP   REDRAW             

BOTTOM_UFO_HIT:
    CALL DRAW_SCORE         ; erase score
    ADD  v7, 0x05           ; add 5 points to score
    JP   UFO_HIT
TOP_UFO_HIT:
    CALL DRAW_SCORE         ; erase score
    ADD  v7, 0x0f           ; add 15 points to score
UFO_HIT:
    CALL DRAW_SCORE         ; draw score
    LD   vd, 0x03           ; play sound for 3 clicks
    LD   ST, vd
    LD   I, MISSILE_SPRITE  ; erase missile
    DRW  v4, v5, 0x3
    JP   SUBTRACT_SHOT

; draw player's score (v7)

DRAW_SCORE:
    LD   I, BCD_LOC
    LD   B, v7
    LD   v3, 0x00
    CALL DRAW_BCD
    RET

DRAW_SHOT_COUNTS:
    LD   I, BCD_LOC
    LD   B, v8
    LD   v3, 0x32
    CALL DRAW_BCD
    RET


; draw a 3-digit BCD number 
; IN: I = address of BCD
;     v3 = xcoordinate
; DESTROYS: vD, v0, v1, v2, v3
DRAW_BCD:
    LD   vd, 27     ; vd = 27
    LD   v2, [I]    ; v0 = [I], v1=[I+1], v2=[I+2]
    LD   F, v0      ; load font at v0
    DRW  v3, vd, 5  ; draw it at (v3, vd)
    ADD  v3, 5      ; v3 += 3
    LD   F, v1      
    DRW  v3, vd, 5
    ADD  v3, 5
    LD   F, v2
    DRW  v3, vd, 5
    RET

.db 0x01 ; what?
BOTTOM_UFO_SPRITE:
.db 0x7c, 0xfe, 0x7c
TOP_UFO_SPRITE:
.db 0x60, 0xf0, 0x60
MISSILE_SPRITE:
.db 0x40, 0xe0, 0xa0
PLATFORM_SPRITE:
.db 0xf8
.db 0xd4 ; what?
    
MARK_SHOT_AND_PLAY_SOUND:
    LD   ve, 0x01  ; mark a shot in progress
    LD   vd, 0x10  ; play sound for 16 counts
    LD   ST, vd
    RET

