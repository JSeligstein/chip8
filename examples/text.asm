    CLS
draw_numbers:
    LD v1, 5
    LD v2, 0
draw_row:
    LD v0, $16
draw_col:
    LD F, v2
    DRW v0,v1,5
    ADD v2, 1
    ADD v0, 5
    SNE v2, 0x10
    JP draw_box
    SE v0, 0x2A
    JP draw_col
    ADD v1, 6
    JP draw_row

left_box:
draw_box:
    LD v0, 0x14
    LD v1, 3
    LD I, left_top
    DRW v0, v1, 14
    LD v1, 17
    LD I, left_bottom
    DRW v0,v1,13
right_box:
    LD v0, 0x23
    LD v1, 3
    LD I, right_top
    DRW v0, v1, 14
    LD v1, 17
    LD I, right_bottom
    DRW v0, v1, 13
middle_box:
    LD v0, 0x1c
    LD v1, 3
    LD I, middle
    DRW v0, v1, 1
    LD v1, 0x1d
    DRW v0, v1, 1

hang:
    JP hang

left_top:
.db 0xFF, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80
left_bottom:
.db 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xFF, 0x00
right_top:  
.db 0xFF, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01
right_bottom:
.db 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0xFF, 0x00
middle:
.db 0xfe, 0x00





     




