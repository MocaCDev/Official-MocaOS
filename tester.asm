TITLE:  db 'M', 'O', 'C', 'A', 'F', 0x0             ;   ID
APS:    dd 0x03                                     ;   Amount Per Scanline
LNGTH:  dd 0x05                                     ;   Length(amount of lines)
TOTAL:  dd 0x03 * 0x05                              ;   Total bytes the image takes up

dd 0x20, 0xFFFF, 0xFF00, 0xFF00, 257
dd 0x20, 0xFF0000, 0xFFFF, 0xFF0000, 257
dd 0x20, 0xFFFF, 0xFF0000, 0xFFFF, 257
dd 0x20, 0xFF0000, 0xFFFF, 0xFF0000, 257
dd 0x20, 0xFFFF, 0xFF0000, 0xFFFF, 257

db 'E'

times 1024 - ($ - $$) db 0x0