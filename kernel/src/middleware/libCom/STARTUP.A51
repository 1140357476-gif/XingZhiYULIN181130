$NOMOD51

;------------------------------------------------------------------------------
;  This file is part of the C51 Compiler package
;  Copyright (c) 1988-2005 Keil Elektronik GmbH and Keil Software, Inc.
;  Version 8.01
;
;  *** <<< Use Configuration Wizard in Context Menu >>> ***
;------------------------------------------------------------------------------
;  STARTUP.A51:  This code is executed after processor reset.
;
;  To translate this file use A51 with the following invocation:
;
;     A51 STARTUP.A51
;
;  To link the modified STARTUP.OBJ file to your application use the following
;  Lx51 invocation:
;
;     Lx51 your object file list, STARTUP.OBJ  controls
;
;------------------------------------------------------------------------------
;
;  User-defined <h> Power-On Initialization of Memory
;
;  With the following EQU statements the initialization of memory
;  at processor reset can be defined:
;
; <o> IDATALEN: IDATA memory size <0x0-0x100>
;     <i> Note: The absolute start-address of IDATA memory is always 0
;     <i>       The IDATA space overlaps physically the DATA and BIT areas.
IDATALEN        EQU     0x100
;
; <o> XDATASTART: XDATA memory start address <0x0-0xFFFF> 
;     <i> The absolute start address of XDATA memory
XDATASTART      EQU     0     
;
; <o> XDATALEN: XDATA memory size <0x0-0xFFFF> 
;     <i> The length of XDATA memory in bytes.
XDATALEN        EQU     0x2000 
;

; <o> XDATASTART_INDRAM: XDATA memory start address in dram <0x4000-0xFFFF> 
;     <i> The absolute start address of XDATA memory
XDATASTART_INDRAM   EQU     0x4000
; <o> XDATASIZE_INDRAM: XDATA memory size in dram <0x0-0xC000> 
;     <i> The length of XDATA memory in bytes.
XDATASIZE_INDRAM    EQU     0xC000

;
; <o> PDATASTART: PDATA memory start address <0x0-0xFFFF> 
;     <i> The absolute start address of PDATA memory
PDATASTART      EQU     0H
;
; <o> PDATALEN: PDATA memory size <0x0-0xFF> 
;     <i> The length of PDATA memory in bytes.
PDATALEN        EQU     0H
;
;</h>
;------------------------------------------------------------------------------
;
;<h> Reentrant Stack Initialization
;
;  The following EQU statements define the stack pointer for reentrant
;  functions and initialized it:
;
; <h> Stack Space for reentrant functions in the SMALL model.
;  <q> IBPSTACK: Enable SMALL model reentrant stack
;     <i> Stack space for reentrant functions in the SMALL model.
IBPSTACK        EQU     0       ; set to 1 if small reentrant is used.
;  <o> IBPSTACKTOP: End address of SMALL model stack <0x0-0xFF>
;     <i> Set the top of the stack to the highest location.
IBPSTACKTOP     EQU     0xFF +1     ; default 0FFH+1  
; </h>
;
; <h> Stack Space for reentrant functions in the LARGE model.      
;  <q> XBPSTACK: Enable LARGE model reentrant stack
;     <i> Stack space for reentrant functions in the LARGE model.
XBPSTACK        EQU     0       ; set to 1 if large reentrant is used.
;  <o> XBPSTACKTOP: End address of LARGE model stack <0x0-0xFFFF>
;     <i> Set the top of the stack to the highest location.
XBPSTACKTOP     EQU     0x7FFF   ; default 0FFFFH+1 
; </h>
;
; <h> Stack Space for reentrant functions in the COMPACT model.    
;  <q> PBPSTACK: Enable COMPACT model reentrant stack
;     <i> Stack space for reentrant functions in the COMPACT model.
PBPSTACK        EQU     0       ; set to 1 if compact reentrant is used.
;
;   <o> PBPSTACKTOP: End address of COMPACT model stack <0x0-0xFFFF>
;     <i> Set the top of the stack to the highest location.
PBPSTACKTOP     EQU     0xFF +1     ; default 0FFH+1  
; </h>
;</h>
;------------------------------------------------------------------------------
;
;  Memory Page for Using the Compact Model with 64 KByte xdata RAM
;  <e>Compact Model Page Definition
;
;  <i>Define the XDATA page used for PDATA variables. 
;  <i>PPAGE must conform with the PPAGE set in the linker invocation.
;
; Enable pdata memory page initalization
PPAGEENABLE     EQU     0       ; set to 1 if pdata object are used.
;
; <o> PPAGE number <0x0-0xFF> 
; <i> uppermost 256-byte address of the page used for PDATA variables.
PPAGE           EQU     0
;
; <o> SFR address which supplies uppermost address byte <0x0-0xFF> 
; <i> most 8051 variants use P2 as uppermost address byte
PPAGE_SFR       DATA    0A0H
;
; </e>
;------------------------------------------------------------------------------
;  <q> FPGA verification enable
FPGA_VERIFICATION  EQU   0
;
;  <e> Dram initialized without boot parameter
;  <i> set to 1 if initialized dram in F/W start up process
DRAM_INIT       EQU      0    
;
; <o> Dram size in M-bytes 8/16/32(MB) <0x08-0x20>
DRAM_SIZE       EQU      16    ;8/16/32(MB) could be defined
;  </e>
;
;  <h> Code size setting
;  <q> 1MB code size supported
LARGE_CODE_SIZE    EQU   1
;  </h>
;
;  <q> Clear xdata in dram by using GPRM
;  <i> set 1 to enabled this function
CLR_USING_GPRM  EQU      1
;
;  <h> CPU clock setting
; <o> set the CPU chock 104/87/75/65/58(MHz) <0-104>
CPU_CLK         EQU      104    ;104/87/75/65/58(MHz) could be defined
;  </h>
;
;------------------------------------------------------------------------------


; Standard SFR Symbols 
ACC     DATA    0E0H
B       DATA    0F0H
SP      DATA    081H
DPL     DATA    082H
DPH     DATA    083H
PCON    DATA    087H
TMOD    DATA    089H
TH1     DATA    08DH
SCON0   DATA    098H
IE      DATA    0A8H
T2CON   DATA    0C8H
RCAP2L  DATA    0CAH
RCAP2H  DATA    0CBH
TL2     DATA    0CCH
TH2     DATA    0CDH


TR1     BIT     088H.6
TR2     BIT     0C8H.2
TCLK    BIT     0C8H.4
RCLK    BIT     0C8H.5
TI      BIT     098H.1
EA      BIT     0A8H.7

                NAME    ?C_STARTUP


?C_C51STARTUP   SEGMENT   CODE
?STACK          SEGMENT   IDATA

                RSEG    ?STACK
                DS      1

                EXTRN CODE (?C_START)
                PUBLIC  ?C_STARTUP

                CSEG    AT      0
?C_STARTUP:     LJMP    STARTUP1

                RSEG    ?C_C51STARTUP

STARTUP1:

IF IDATALEN <> 0
                MOV     R0,#IDATALEN - 1
                CLR     A
IDATALOOP:      MOV     @R0,A
                DJNZ    R0,IDATALOOP
ENDIF

IF XDATALEN <> 0 ;XDATALEN <> 0

;------------------------------------------------------------------------------
;   Disable IMEM shadow
;------------------------------------------------------------------------------
;
;               XBYTE[0x2C20] &= 0xFC;
;
                MOV     DPTR, #0x2C20
                MOVX    A, @DPTR
                ANL     A, #0FCH ;bit0(Imem),bit 1(Dmem) disable
                MOVX    @DPTR, A    
;------------------------------------------------------------------------------             
;   Enable Sram, IMEM, RomPage, DataDram
;------------------------------------------------------------------------------
;
;               XBYTE[0x2C00] = 0x1F;
;
                MOV     DPTR, #0x2C00
                MOV     A, #01FH
                MOVX    @DPTR, A

;------------------------------------------------------------------------------
;   Disable RTC
;------------------------------------------------------------------------------ 
;               XBYTE[0x2067] = 0x01;
;           
                MOV     DPTR, #0x2067
                MOV     A, #0x01
                MOVX    @DPTR, A
;   
;               XBYTE[0x206B] = 0xD0;
;           
                MOV     DPTR, #0x206B
                MOV     A, #0xD0
                MOVX    @DPTR, A
;   
;               XBYTE[0x206C] = 0x00;
;                           
                MOV     DPTR, #0x206C
                CLR     A
                MOVX    @DPTR, A
;   
;               XBYTE[0x2069] = 0x01;
;                                           
                MOV     DPTR, #0x2069
                MOV     A, #0x01
                MOVX    @DPTR, A
;   
;               XBYTE[0x206B] = 0xC0;
;                                           
                MOV     DPTR, #0x206B
                MOV     A, #0xC0
                MOVX    @DPTR, A
;   
;               XBYTE[0x206C] = 0x00;
;                                                           
                MOV     DPTR, #0x206C
                CLR     A
                MOVX    @DPTR, A
;   
;               XBYTE[0x2069] = 0x01;
;                                                           
                MOV     DPTR, #0x2069
                MOV     A, #0x01
                MOVX    @DPTR, A

;------------------------------------------------------------------------------
;   Init dram
;------------------------------------------------------------------------------     
;               XBYTE[0x2750] = 0x02;   //refers to "DRAM_SIZE"
;                                                           
IF DRAM_INIT <> 0 ;Init dram start
                MOV     DPTR,#0x2750
                MOV     A,#0x02   ; set 8MB as the default size
IF (DRAM_SIZE = 8)              
                MOV     A,#0x02
ENDIF               
IF (DRAM_SIZE = 16)             
                MOV     A,#0x03
ENDIF               
IF (DRAM_SIZE = 32)             
                MOV     A,#0x04
ENDIF               
                MOVX    @DPTR,A
;   
;               XBYTE[0x275C] = 0x08;
;                                                           
                MOV     DPTR,#0x275C
                MOV     A,#0x08
                MOVX    @DPTR,A
;   
;               XBYTE[0x275B] = 0x00;
;                                                                           
                MOV     DPTR,#0x275B
                CLR     A
                MOVX    @DPTR,A
;   
;               XBYTE[0x2751] = 0x06;
;                                                                           
                MOV     DPTR,#0x2751
                MOV     A,#0x06
                MOVX    @DPTR,A
;   
;               XBYTE[0x2754] = 0xE8;
;                                                                           
                MOV     DPTR,#0x2754
                MOV     A,#0xE8
                MOVX    @DPTR,A
;   
;               XBYTE[0x2755] = 0x03;
;                                                                           
                INC     DPTR
                MOV     A,#0x03
                MOVX    @DPTR,A
;   
;               XBYTE[0x2025] = 0x05;
;                                                                           
                MOV     DPTR,#0x2025
                MOV     A,#0x05
                MOVX    @DPTR,A
;   
;               XBYTE[0x200E] = 0x01;
;                                                                           
                MOV     DPTR,#0x200E
                MOV     A,#0x01
                MOVX    @DPTR,A
;   
;               XBYTE[0x27E6] = 0x02;
;                                                                           
                MOV     DPTR,#0x27E6
                MOV     A,#0x02
                MOVX    @DPTR,A
ENDIF ;Init dram end

;------------------------------------------------------------------------------             
;   Set fw code size & global data mapping area in dram (0x4000 ~ 0xFFFF)
;------------------------------------------------------------------------------
;
;           static UINT32 code fwSize _at_ 0x7FF0; //Defined in sys_mem_init.c
;
;           static void sysMemMapSet(void) 
;           { 
;               UINT16 data bank;
;               UINT16 data size; 
;                   
;               //size = fwSize >> 8; 
;               *((UINT8 data*)&size + 0) = *((UINT8 code*)&fwSize + 1); 
;
                    MOV      DPTR,#0x7FF1
                    CLR      A
                    MOVC     A,@A+DPTR
                    MOV      R0,#0x6B
                    MOV      @R0,A
;
;               *((UINT8 data*)&size + 1) = *((UINT8 code*)&fwSize + 2); 
;               
                    INC      DPTR
                    CLR      A
                    MOVC     A,@A+DPTR
                    INC      R0
                    MOV      @R0,A
;
;               bank = (size >> (11-8)); //fwSize/2KB 
;
                    MOV      A,0x6C
                    MOV      R6,0x6B
                    MOV      R0,#0x03
BANK_SIZE_LOOP1:    XCH      A,R6
                    CLR      C
                    RRC      A
                    XCH      A,R6
                    RRC      A
                    DJNZ     R0,BANK_SIZE_LOOP1
                    MOV      0x6A,A
                    MOV      0x69,R6
;
;               bank--; 
;
                    DEC      0x6A
                    JNZ      BANK_JMP_TAG1
                    DEC      0x69
;
;               XBYTE[0x2C2A] = READ8(bank, 1); 
;
BANK_JMP_TAG1:      MOV      DPTR,#0x2C2A
                    ;MOV      A,0x6A
                    MOV      A,#0xFF
                    MOVX     @DPTR,A
;
;               XBYTE[0x2C2B] = READ8(bank, 0); 
;               
                    INC      DPTR
                    ;MOV      A,0x69
                    MOV      A,#0x1F
                    MOVX     @DPTR,A
;
;               bank = (size >> (14-8)); //fwSize/16KB 
;
                    MOV      A,0x6C
                    MOV      R6,0x6B
                    MOV      R0,#0x06
BANK_SIZE_LOOP2:    XCH      A,R6
                    CLR      C
                    RRC      A
                    XCH      A,R6
                    RRC      A
                    DJNZ     R0,BANK_SIZE_LOOP2
                    MOV      0x6A,A
                    MOV      0x69,R6
;
;               XBYTE[0x2C2E] = READ8(bank, 1);
;
                    MOV      DPTR,#0x2C2E
                    MOVX     @DPTR,A
;
;               XBYTE[0x2C2F] = READ8(bank, 0); 
;
                    INC      DPTR
                    MOV      A,0x69
                    MOVX     @DPTR,A
;
;               bank <<= 1; 
;
                    MOV      A,0x6A
                    ADD      A,0xE0
                    MOV      0x6A,A
                    MOV      A,0x69
                    RLC      A
                    MOV      0x69,A
;
;               bank += 2; 
;
                    MOV      A,#0x02
                    ADD      A,0x6A
                    MOV      0x6A,A
                    CLR      A
                    ADDC     A,0x69
                    MOV      0x69,A
;
;               XBYTE[0x2C34] = READ8(bank, 1); 
;
                    MOV      DPTR,#0x2C34
                    MOV      A,0x6A
                    MOVX     @DPTR,A
;
;               XBYTE[0x2C35] = READ8(bank, 0); 
;
                    INC      DPTR
                    MOV      A,0x69
                    MOVX     @DPTR,A
;
;               bank++; 
;
                    INC      0x6A
                    MOV      A,0x6A
                    JNZ      BANK_JMP_TAG2
                    INC      0x69
;
;               XBYTE[0x2C36] = READ8(bank, 1); 
;
BANK_JMP_TAG2:      INC      DPTR
                    MOVX     @DPTR,A
;
;               XBYTE[0x2C37] = READ8(bank, 0); 
;
                    INC      DPTR
                    MOV      A,0x69
                    MOVX     @DPTR,A
;
;               bank++; 
;
                    INC      0x6A
                    MOV      A,0x6A
                    JNZ      BANK_JMP_TAG3
                    INC      0x69
;
;               XBYTE[0x2C38] = READ8(bank, 1); 
;
BANK_JMP_TAG3:      INC      DPTR
                    MOVX     @DPTR,A
;
;               XBYTE[0x2C39] = READ8(bank, 0); 
;
                    INC      DPTR
                    MOV      A,0x69
                    MOVX     @DPTR,A
;
;               bank++; 
;
                    INC      0x6A
                    MOV      A,0x6A
                    JNZ      BANK_JMP_TAG4
                    INC      0x69
;
;               XBYTE[0x2C3A] = READ8(bank, 1); 
;
BANK_JMP_TAG4:      INC      DPTR
                    MOVX     @DPTR,A
;
;               XBYTE[0x2C3B] = READ8(bank, 0); 
;
                    INC      DPTR
                    MOV      A,0x69
                    MOVX     @DPTR,A
;   
;               XBYTE[0x2C01] = 0xF0;
;               
                    MOV     DPTR, #0x2C01
                    MOV     A, #0xF0
                    MOVX    @DPTR, A
;           }

;------------------------------------------------------------------------------
;   Set CPU clock & Initialize RS232
;------------------------------------------------------------------------------     
;   
;               XBYTE[0x2024] = 0x04; //CPU clock 522Mhz/5 = 104.4Mhz
;               
                MOV     DPTR, #0x2024
                MOV     A, #04H
                MOVX    @DPTR, A
;   
;               XBYTE[0x2023] = 0x02; //P3oeSel? why?
;                               
                MOV     DPTR, #0x2C03
                CLR     A
                MOV     A, #02H
                MOVX    @DPTR, A                                    
;   
;               SCON0  = 0xD8;
;               PCON  |= 0x80;
;               RCLK   = 0x01;
;               TCLK   = 0x01;
;               T2CON  = 0x30;
;               RCAP2H = 0xFF;
;               RCAP2L = 0xF9;
;               EA     = 0x00;
;               TR2    = 0x01;
;               TI     = 0x01;
;
                MOV     SCON0, #0D8H
                ORL     PCON, #080H
                SETB    RCLK
                SETB    TCLK
                MOV     T2CON, #030H
                MOV     RCAP2H, #0FFH
IF (FPGA_VERIFICATION = 1)                
                MOV     RCAP2L, #0F9H   ; for fpga 13MHZ OSC
ELSE
                MOV     RCAP2L, #0C7H   ; for real chip 104MHZ
ENDIF ;(FPGA_VERIFICATION = 1)        
;               MOV     RCAP2L, #0E6H   ; for real chip 48MHZ     
                CLR     EA              
                SETB    TR2
                SETB    TI      

;------------------------------------------------------------------------------
;   Enable clock phase for XDATA cleared in dram by using GPRM
;------------------------------------------------------------------------------     
;   
;               XBYTE[0x2013] = 0xFF;
;               XBYTE[0x2014] = 0xFF;
;               XBYTE[0x2015] = 0xFF;
;               
                MOV     DPTR, #0x2013
                MOV     A, #0xFF
                MOVX    @DPTR, A
                INC     DPTR
                MOVX    @DPTR, A
                INC     DPTR
                MOVX    @DPTR, A
;------------------------------------------------------------------------------
;   Clear XDATA area in SRAM
;------------------------------------------------------------------------------     
;   
                MOV     DPTR,#XDATASTART
                MOV     R7,#LOW (XDATALEN)
  IF (LOW (XDATALEN)) <> 0
                MOV     R6,#(HIGH (XDATALEN)) +1
  ELSE
                MOV     R6,#HIGH (XDATALEN)
  ENDIF
                CLR     A
XDATALOOP:      MOVX    @DPTR,A
                INC     DPTR
                DJNZ    R7,XDATALOOP
                DJNZ    R6,XDATALOOP
ENDIF ;XDATALEN <> 0

;------------------------------------------------------------------------------
;   Clear XDATA area in DRAM by CPU
;------------------------------------------------------------------------------     

IF (CLR_USING_GPRM = 0) ;CLR_USING_GPRM = 0

                MOV     DPTR,#XDATASTART_INDRAM
                MOV     R7,#LOW (XDATASIZE_INDRAM)
  IF (LOW (XDATASIZE_INDRAM)) <> 0
                MOV     R6,#(HIGH (XDATASIZE_INDRAM)) +1
  ELSE
                MOV     R6,#HIGH (XDATASIZE_INDRAM)
  ENDIF
                CLR     A
XDATALOOP2:     MOVX    @DPTR,A
                INC     DPTR
                DJNZ    R7,XDATALOOP2
                DJNZ    R6,XDATALOOP2

ELSE
;------------------------------------------------------------------------------
;   Clear XDATA area in DRAM by GPRM DMA engine
;------------------------------------------------------------------------------     
;
;               XBYTE[0x2F51] = 0x01; //using GPRM DMA channel 0
;
                MOV     DPTR, #0x2F51
                MOV     A,    #0x01
                MOVX    @DPTR, A
;               
;               XBYTE[0x2F64] = *(((UINT8 code*)0x7FF0)+3); 
;               XBYTE[0x2F65] = *(((UINT8 code*)0x7FF0)+2);
;               XBYTE[0x2F66] = *(((UINT8 code*)0x7FF0)+1);
;               XBYTE[0x2F67] = *(((UINT8 code*)0x7FF0)+0);
;               
                MOV     DPTR, #0x7FF3
                CLR     A
                MOVC    A, @A+DPTR
                MOV     DPTR, #0x2F64
                MOVX    @DPTR, A

                MOV     DPTR, #0x7FF2
                CLR     A
                MOVC    A, @A+DPTR
                MOV     DPTR, #0x2F65
                MOVX    @DPTR, A

                MOV     DPTR, #0x7FF1
                CLR     A
                MOVC    A, @A+DPTR
                MOV     DPTR, #0x2F66
                MOVX    @DPTR, A

                MOV     DPTR, #0x7FF0
                CLR     A
                MOVC    A, @A+DPTR
                MOV     DPTR, #0x2F67
                MOVX    @DPTR, A
;               
;               XBYTE[0x2F68] = 0xFF; //fill size 0xC000 (bytes) (16+8+8+8+8)
;               XBYTE[0x2F69] = 0xBF;
;               XBYTE[0x2F6A] = 0x00;
;   
                INC     DPTR
                MOV     A, #0xFF
                MOVX    @DPTR, A
                INC     DPTR
                MOV     A, #0xBF
                MOVX    @DPTR, A
                INC     DPTR
                CLR     A
                MOVX    @DPTR, A
;               
;               XBYTE[0x2F52] = 0x00; //fill data is 0
;               
;               CLR     A
                MOV     DPTR, #0x2F52
                MOVX    @DPTR, A
;               
;               XBYTE[0x2F00] = 0x01; //clear GPRM interrupt status
;               
                MOV     DPTR, #0x2F00
                MOV     A, #0x01
                MOVX    @DPTR, A
;               
;               XBYTE[0x2F6F] = 0x01; //trigger GPRM DMA
;               
                MOV     DPTR, #0x2F6F
                MOV     A, #0x01
                MOVX    @DPTR, A
;               
;               while(XBYTE[0x2F54] & 0x01); //wait GPRM DMA ready
;                               
                Loop_Wait:
                MOV    DPTR, #0x2F54
                MOVX   A, @DPTR
                ANL    A, #0x01
                JNZ     Loop_Wait
                
;               
;               XBYTE[0x2F00] = 0x01; //clear GPRM interrupt status
;               
                MOV     DPTR, #0x2F00
                MOV     A, #0x01
                MOVX    @DPTR, A

ENDIF ;CLR_USING_GPRM = 0


IF PPAGEENABLE <> 0
                MOV     PPAGE_SFR,#PPAGE
ENDIF

IF PDATALEN <> 0
                MOV     R0,#LOW (PDATASTART)
                MOV     R7,#LOW (PDATALEN)
                CLR     A
PDATALOOP:      MOVX    @R0,A
                INC     R0
                DJNZ    R7,PDATALOOP
ENDIF

IF IBPSTACK <> 0
EXTRN DATA (?C_IBP)

                MOV     ?C_IBP,#LOW IBPSTACKTOP
ENDIF

IF XBPSTACK <> 0
EXTRN DATA (?C_XBP)

                MOV     ?C_XBP,#HIGH XBPSTACKTOP
                MOV     ?C_XBP+1,#LOW XBPSTACKTOP
ENDIF

IF PBPSTACK <> 0
EXTRN DATA (?C_PBP)
                MOV     ?C_PBP,#LOW PBPSTACKTOP
ENDIF

                MOV     SP,#?STACK-1

; This code is required if you use L51_BANK.A51 with Banking Mode 4
;<h> Code Banking
; <q> Select Bank 0 for L51_BANK.A51 Mode 4
#if 0   
;     <i> Initialize bank mechanism to code bank 0 when using L51_BANK.A51 with Banking Mode 4.
EXTRN CODE (?B_SWITCH0)
                CALL    ?B_SWITCH0      ; init bank mechanism to code bank 0
#endif
;</h>
                LJMP    ?C_START

                END
