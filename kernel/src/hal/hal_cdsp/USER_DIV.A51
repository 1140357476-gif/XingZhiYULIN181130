;************************  USERDIV FUNCTION  **********************************
;                                                                             *
; User define division function.                                              *
;                                                                             *
; Prototype:                                                                  *
;                                                                             *
; extern unsigned int USERDIV(unsigned int divX, unsigned int divY);          *
;                                                                             *
;******************************************************************************

$NOMOD51
$INCLUDE (REG320.INC)

;******************************************************************************
;                                                                             *
?DIV_ADDR    EQU    0FFA8H   ; DEFINE the user division function address here *
;                                                                             *
;******************************************************************************

NAME         ?USER_DIV

?CO?USER_DIV             SEGMENT    CODE
?PR?_USERDIV?USER_DIV    SEGMENT    CODE
?XD?_USERDIV?USER_DIV    SEGMENT    XDATA    OVERLAYABLE

PUBLIC        _USERDIV

RSEG         ?XD?_USERDIV?USER_DIV

?_USERDIV?USER_DIV:
Divindend:   DS    2
             ORG   2
Divisor:     DS    2

RSEG         ?CO?USER_DIV

USER_DIV:    DB 0xBC, 0x00, 0x0B, 0xBE, 0x00, 0x29, 0xEF, 0x8D
             DB 0xF0, 0x84, 0xFF, 0xAD, 0xF0, 0x22, 0xE4, 0xCC
             DB 0xF8, 0x75, 0xF0, 0x08, 0xEF, 0x2F, 0xFF, 0xEE
             DB 0x33, 0xFE, 0xEC, 0x33, 0xFC, 0xEE, 0x9D, 0xEC
             DB 0x98, 0x40, 0x05, 0xFC, 0xEE, 0x9D, 0xFE, 0x0F
             DB 0xD5, 0xF0, 0xE9, 0xE4, 0xCE, 0xFD, 0x22, 0xED
             DB 0xF8, 0xF5, 0xF0, 0xEE, 0x84, 0x20, 0xD2, 0x1C
             DB 0xFE, 0xAD, 0xF0, 0x75, 0xF0, 0x08, 0xEF, 0x2F
             DB 0xFF, 0xED, 0x33, 0xFD, 0x40, 0x07, 0x98, 0x50
             DB 0x06, 0xD5, 0xF0, 0xF2, 0x22, 0xC3, 0x98, 0xFD
             DB 0x0F, 0xD5, 0xF0, 0xEA, 0x22

RSEG         ?PR?_USERDIV?USER_DIV

_USERDIV:    LJMP    ?DIV_ADDR

END
