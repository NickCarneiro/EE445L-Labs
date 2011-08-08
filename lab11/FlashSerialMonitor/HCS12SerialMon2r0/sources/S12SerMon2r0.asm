;.pagewidth  120t
;*********************************************************************
;* Title:  S12SerMonxrx.asm        Copyright (c) Motorola 2003
;*********************************************************************
;* Author: Jim Sibigtroth - Motorola TSPG - 8/16 Bit Division
;* Author: Jim Williams - Motorola TSPG - 8/16 Bit Division
;*
;* Description: Bootloader/Monitor program for HCS9S12
;* bootloader will reside in 2K of block protected memory at the
;* end of the memory map of an HCS9S12 MCU ($F7FF-$FFFF).
;*
;* Since this code is located in the vector space, all interrupt
;* vectors will be mirrored to the pseudo vector table in user
;* erasable and reprogrammable flash memory just before the start
;* of the protected bootloader code.
;*
;* If a non-FFFF user reset vector is programmed into the
;* pseudo-reset vector, the bootloader will jump to that routine
;* so the user can control all options including write-once bits.
;*
;* This monitor program implements 23 primitive monitor commands that
;* are very similar to BDM commands. Third-party tool vendors can
;* adapt their existing BDM-based tools to work through a serial I/O
;* cable rather than a BDM pod, simply by providing a set of alternate
;* interface routines. Although this monitor approach has some
;* limitations compared to the BDM approach, it provides a free or
;* very low cost alternative for the most cost-sensitive users.
;*
;* This monitor uses SCI0 as the primary interface to the target MCU
;* system and SCI0 Rx interrupts are used to break out of a running
;* user program. This implies that some monitor functions will not be
;* available if the I bit in the CCR is not clear during execution of
;* the user's program. During debug of user initialization programs
;* and interrupt service routines when the I bit is not clear, trace
;* and breakpoint functions still work as expected because these
;* functions use on-chip breakpoint logic. 
;*
;*
;*
;* Revision History: not yet released
;* Rev #     Date      Who     Comments
;* -----  -----------  ------  ---------------------------------------
;*  2.00   04-SEP-03   JPW     First Release.

;	Eduardo Montanez emailed me to make the following change for E128
; comment out FPrtoBlkSz equ $C7 ; protect code for boot block ($C7 - 2K)
; remove comment on FPrtoBlkSz equ $ff ; protect code for boot block (none)

;
;
;
;
softwareID1:  equ   $0309     ;Software revision (date)
softwareID2:  equ   $2003     ;Software revision (year)
softwareID3:  equ   $0200     ;Software revision (ver)

;*
;*
;*********************************************************************
;*********************************************************************
;* Motorola reserves the right to make changes without further notice
;* to any product herein to improve reliability, function, or design.
;* Motorola does not assume any liability arising out of the
;* application or use of any product, circuit, or software described
;* herein; neither does it convey any license under its patent rights
;* nor the rights of others.  Motorola products are not designed,
;* intended, or authorized for use as components in systems intended
;* for surgical implant into the body, or other applications intended
;* to support life, or for any other application in which the failure
;* of the Motorola product could create a situation where personal
;* injury or death may occur.  Should Buyer purchase or use Motorola
;* products for any such intended or unauthorized application, Buyer
;* shall indemnify and hold Motorola and its officers, employees,
;* subsidiaries, affiliates, and distributors harmless against all
;* claims, costs, damages, and expenses, and reasonable attorney fees
;* arising out of, directly or indirectly, any claim of personal
;* injury or death associated with such unintended or unauthorized
;* use, even if such claim alleges that Motorola was negligent
;* regarding the design or manufacture of the part.
;*
;* Motorola is a registered trademark of Motorola, Inc.
;*********************************************************************

            XDEF Startup         ;make symbol visible to the MW linker
;*********************************************************************
;* Include standard definitions that are common to all derivatives
;*********************************************************************
;             base    10           ;ensure default number base to decimal
             nolist               ;turn off listing
             include "S12SerMon2r0.inc"
             include "S12SerMon2r0.def"
             list                 ;turn listing back on
;*********************************************************************
;* general equates for bootloader/monitor program valid for all
;* derivatives
;*********************************************************************
BootStart:   equ    $F800         ;start of protected boot block
RamLast:     equ    $3fff         ;last RAM location (all devices)
Window:      equ    $8000         ;PPAGE Window start
RomStart:    equ    $4000         ;start of flash
VecTabSize:  equ    $80           ;size of vector table
VectorTable: equ    $10000-VecTabSize ;start of vector table
PVecTable:   equ    BootStart-VecTabSize ;start of pseudo vector table
FProtStart:  equ    $FF00         ;start of FLASH protection/security
FProtBlksz:  equ    $C7           ;protect code for boot block ($C7 2K)
;FProtBlksz:  equ    $FF           ;protect code for boot block (none)
FSecure:     equ    $BE           ;Disable Security and backdoor access
;FSecure:     equ    $00           ;Enable Security and backdoor access

BusFreq:     equ    ((OscFreq/(initREFDV+1))*(initSYNR+1))
baud115200:  equ    (BusFreq/16)*10/1152  ;sets baud rate to 115,200
longBreak:   equ    1500          ;delay time for >30-bit break
; make TxD low at least 300us (30 bits @ 115200 baud)
; 5~ * 42ns/~ * 1500 = 315us (not exact, just >30 bit times)
asciiCR:     equ    $0D           ;ascii carriage return

flagReg:     equ    SCI0CR1       ;SCI control1 reg of SCI0
RunFlag:     equ    WAKE          ;SCI Wake bit used as run/mon flag
ArmFlag:     equ    RSRC          ;SCI RSRC bit used for ARM storage
TraceFlag:   equ    ILT           ;SCI Idle bit used as trace flag
; 1=SWI caused by return from Trace1; 0=SWI from breakpoint or DBG

initSCI0CR2: equ    $0C           ;SCI0 Control Register 2
traceOne:    equ    $80           ;BKPCT0 pattern for trace1 cmd
;
;CPU HCS12 CCR immediately after reset is:
initUCcr:    equ    %11010000     ;initial value for user's CCR
;                    SX-I----     ;I interrupts masked
								  ;(SXHINZVC=11x1xxxx).

ErrNone:     equ    $E0           ;code for no errors
ErrCmnd:     equ    $E1           ;command not recognized
ErrRun:      equ    $E2           ;command not allowed in run mode
ErrSP:       equ    $E3           ;SP was out of range
ErrWriteSP:  equ    $E4           ;attempted to write bad SP value
ErrByteNVM:  equ    $E5           ;write_byte attempt NVM
ErrFlash:    equ    $E6           ;FACCERR or FPVIOL error
ErrFlErase:  equ    $E7           ;Error code not implemented
ErrGoVec:    equ    $E8           ;Error code not implemented
ErrEeErase   equ    $E9			  ;EACCERR or EPVIOL error

StatHalt:    equ    $02           ;stopped by Halt command
StatTrace:   equ    $04           ;returned from a Trace1 command
StatBreak:   equ    $06           ;Breakpoint or DBG (SWI) request
StatCold:    equ    $08           ;just did a cold reset
StatWarm:    equ    $0C           ;warm start because int with bad SP

;*********************************************************************
;* User CPU registers stack frame...
;*   +0  UCcr   <- Monitor's SP
;*   +1  UDreg   (B:A)
;*   +3  UXreg
;*   +5  UYreg
;*   +7  UPc
;*   +9  ---     <- User's SP
; Offsets from actual SP to user CPU regs while in monitor
;*********************************************************************

UCcr:        equ    0             ;user's CCR register
UDreg:       equ    1             ;user's D register (B:A)
UXreg:       equ    3             ;user's X register
UYreg:       equ    5             ;user's Y register
UPc:         equ    7             ;user's PC
SPOffset:    equ    9             ;offset of stack pointer while in monitor

MaxMonStack  equ   35             ;maximum number of bytes used by Monitor
LowSPLimit   equ   RamStart+MaxMonStack-SPOffset
HighSPLimit  equ   RamLast-SPOffset+1

; named locations on stack if SWI with bad SP value
;*********************************************************************
;* Start of code and/or constant data
;*********************************************************************
             org    BootStart      ;beginning of protected flash
;*********************************************************************
;* Main startup program - real reset vector points to here
;*********************************************************************
;*********************************************************************
;  THIS LOCKS RAM / REGS / AND EEPROM BY USING ONLY WRITE USER CODE 
;  WILL NOT EFFECT THESE SETTINGS...This is for derivative compatibility
;  Please refer to EB388
;*********************************************************************
ColdStart:  
Startup:
main:
             movb  #$00,INITRG    ;set registers at $0000 
             movb  #$39,INITRM    ;set ram to end at $3fff 
             brclr  MEMSIZ0,eep_sw1+eep_sw0,ColdStart1  ;Check if device has EEprom
             movb  #$09,INITEE    ;set eeprom to end at $0fff 
             
ColdStart1:     clra                 ;A=0=cold start; see tsta @ ChkCold

;**********************************************************************
; Decide whether to go to user reset or bootloader/monitor
;**********************************************************************
;  (a) default to monitor if high byte user pseudo-vector is erased ($FF)
;**********************************************************************
             ldab   vector00-($10000-BootStart) ;check for user reset Vector
             comb                 ;if erased COMB result will be 0
             beq    Monitor       ;default to monitor if no vector

;**********************************************************************
; Test the state of some  pins to force entering the monitor
;   Depending on the hardware configuration enable/disable/modify the
;   sections below
;**********************************************************************
             bset   SwPullup,mSwPullup ;enable pullup on force monitor sw |
             bset   PPSS,PPSS0    ;enable pullup on RxD0 pin
             bset   PERS,PERS0    ;on RxD0 pin
             clrb
             dbne   b,*           ;delay to allow sw pin to pull up
;**********************************************************************
;  (b) force monitor if SwPort bit SWITCH = 0 
;  Note: this port is configured after reset as input with pull-up
;  With no connection to this pin the monitor jumps to run mode
;**********************************************************************
             ldab   SwPort        ;get port value
             bitb   #Switch       ;test the sw bit
             beq    Monitor

;**********************************************************************
;  (c) force monitor if RxD low (from host)  PORTS bit 0
;      This is true if the host holds RxD on break level
;  Note: this port is configured after reset as input with pull-up
;**********************************************************************
             brclr  PTS,PTS0,Monitor ;to monitor if RxD low
             bclr   PPSS,PPSS0    ;restore reset state on RxD0 pin
             bclr   PERS,PERS0    ;restore reset state on RxD1 pin

;**********************************************************************
;  finally jump to the user application (by pseudo vector)
;**********************************************************************
             jmp    [vector00-($10000-BootStart),pcr] ;go where
                                        ;user reset vector points
;*********************************************************************
;  Formal start of Monitor code
;*********************************************************************
Monitor:
; Initialize clock generator and PLL
;
             bclr    SwPullup,mSwPullup ;restore reset state
             bclr    CLKSEL,PLLSEL      ;disengage PLL to system
             bset    PLLCTL,PLLON       ;turn on PLL
             movb    #initSYNR,SYNR     ;set PLL multiplier 
             movb    #initREFDV,REFDV   ;set PLL divider
             nop
             nop
             brclr   CRGFLG,LOCK,*+0    ;while (!(crg.crgflg.bit.lock==1))
             bset    CLKSEL,PLLSEL      ;engage PLL to system
;
; set flash/EEPROM clock to 200 kHz
;
             movb  #((OscFreq/200)-1),FCLKDIV ;Flash CLK = 200 kHz
             movb  #((OscFreq/200)-1),ECLKDIV    ;Eeprom CLK = 200 kHz
;
; Set stack pointer to last (highest) RAM location
;
stackInit:   lds    #RamLast+1    ;point one past RAM

; Setup initial user CPU register values (user register stack frame)
;   A holds the initial state value
initUregs:   ldx   BootStart-2    ;load user reset vector
             pshx                 ;$00 to user UPc      $3FFe
             ldx   #$0000
             pshx                 ;$00 to user UYreg    $3ffc
             pshx                 ;$00 to user UXreg    $3ffa
             pshx                 ;$00 to user UDreg    $3ff8
             ldab  #initUCcr      ;initial value for user CCR
             pshb                 ;to UCcr              $3ff7

;
; set baud rate to 115.2 kbaud and turn on Rx and Tx
;
             movb  #baud115200,SCI0BDL  ;..BDH=0 so baud = 115.2 K
             movb  #initSCI0CR2,SCI0CR2 ;Rx and Tx on
;
; if warm start, skip break. A is a flag to indicate cold vs warm start.
; Avoid using A above here *****
;
ChkCold:     tsta                ;0=cold start, non-zero=warm start
             beq    coldBrk      ;if cold send break
;
; Send a warm start prompt and wait for new commands
;
             ldaa  #ErrSP        ;error code for bad SP
             jsr    PutChar      ;send error code (1st prompt char)
             ldaa  #StatWarm     ;status code for warm start
             bra    EndPrompt    ;finish warm start prompt
;
; Cold start so Generate long break to host
;
coldBrk:     brclr  SCI0SR1,TDRE,* ;wait for Tx (preamble) empty
             bset   SCI0CR2,SBK   ;start sending break after preamble
             ldx   #longBreak     ;at least 30 bit times for Windows
BrkLoop:     cpx   #0             ;[2]done?
             dbne   x,BrkLoop     ;[3]
             bclr   SCI0CR2,SBK   ;stop sending breaks

waitforCR:   jsr    GetChar       ;should be asciiCR or $00 with FE=1
             cmpa  #asciiCR       ;.eq. if 115.2K baud OK
             bne    waitforCR

;*********************************************************************
;* end of reset initialization, begin body of program
;*********************************************************************
;
; Send a cold start prompt and wait for new commands
;
             ldaa  #ErrNone       ;code for no errors ($E0)
             jsr    PutChar       ;send error code (1st prompt char)
             ldaa  #StatCold      ;status code for cold start ($08)
             bra    EndPrompt     ;finish warm start prompt
;
; normal entry point after a good command
; Prompt is an alt entry point if an error occurred during a command
; endPrompt is an alternate entry for Trace1, Break (SWI), Halt,
; or warm/cold resets so an alternate status value can be sent
; with the prompt
;

CommandOK:   ldaa  #ErrNone       ;code for no errors ($E0)
Prompt:      jsr    PutChar       ;send error code
             ldaa   flagReg       ;0 means monitor active mode
             anda  #RunFlag       ;mask for run/monitor flag (SCI WAKE)
             lsra                 ;shift flag to LSB
             lsra                 ; for output as status
             lsra                 ;$00=monitor active, $01=run
EndPrompt:   jsr    PutChar       ;send status code
             ldaa  #'>'
             jsr    PutChar       ;send 3rd character of prompt seq
             
;test flagReg for run / DBG arm status.
             brclr  flagReg,RunFlag,Prompt1  ;no exit if run flag clr
             brclr  flagReg,ArmFlag,PromptRun  ;If DBG was not armed just run
             bset	DBGC1,ARM	  ;re-arm DBG module
PromptRun:   jmp    GoCmd         ;run mode so return to user program


Prompt1:     jsr    GetChar       ;get command code character
             ldx   #commandTbl    ;point at first command entry
CmdLoop:     cmpa    ,x           ;does command match table entry?
             beq    DoCmd          ;branch if command found
             leax   3,x
             cpx   #tableEnd      ;see if past end of table
             bne    CmdLoop       ;if not, try next entry
             ldaa  #ErrCmnd       ;code for unrecognized command
             bra    Prompt        ;back to prompt; command error
             
DoCmd:       ldx    1,x           ;get pointer to command routine
             jmp     ,x           ;go process command
;
; all commands except GO, Trace_1, and Reset to user code - jump to
; Prompt after done. Trace_1 returns indirectly via a SWI.
;
;*********************************************************************
;* Command table for bootloader/monitor commands
;*  each entry consists of an 8-bit command code + the address of the
;*  routine to be executed for that command.
;*********************************************************************
commandTbl:  fcb   $A1
             fdb  RdByteCmd     ;read byte
             fcb   $A2
             fdb  WtByteCmd     ;write byte
             fcb   $A3
             fdb  RdWordCmd     ;read word of data 
             fcb   $A4
             fdb  WtWordCmd     ;write word of data 
             fcb   $A5
             fdb  RdNextCmd     ;read next word
             fcb   $A6
             fdb  WtNextCmd     ;write next word
             fcb   $A7
             fdb  ReadCmd       ;read n bytes of data
             fcb   $A8
             fdb  WriteCmd      ;write n bytes of data
             fcb   $A9
             fdb  RdRegsCmd     ;read CPU registers
             fcb   $AA
             fdb  WriteSpCmd    ;write SP
             fcb   $AB
             fdb  WritePcCmd    ;write PC
             fcb   $AC
             fdb  WriteIYCmd    ;write IY
             fcb   $AD
             fdb  WriteIXCmd    ;write IX
             fcb   $AE
             fdb  WriteDCmd     ;write D
             fcb   $AF
             fdb  WriteCcrCmd   ;write CCR
             fcb   $B1
             fdb  GoCmd         ;go
             fcb   $B2
             fdb  Trace1Cmd     ;trace 1
             fcb   $B3
             fdb  HaltCmd       ;halt
             fcb   $B4
             fdb  ResetCmd      ;reset - to user vector or monitor
;            $B5 - Command not implemented
             fcb   $B6          ;code - erase flash command
             fdb  EraseAllCmd   ;erase all flash and eeprom command routine
             fcb   $B7          ;return device ID
             fdb  DeviceCmd
             fcb   $B8          ;erase current flash bank selected in PPAGE
             fdb  ErsPage
             fcb   $B9			;Bulk erase eeprom if available
             fdb  EraseEECmd	;
tableEnd:    equ    *           ;end of command table marker

;*********************************************************************
;* Device ID Command -  Ouputs hex word from device ID register
;*********************************************************************
DeviceCmd:   ldaa   #$DC         ;get part HCS12 descripter
             jsr    PutChar      ;out to term
             ldaa   PARTIDH      ;get part ID high byte
             jsr    PutChar      ;out to term
             ldaa   PARTIDL      ;get part ID low byte
             jsr    PutChar      ;out to term
             ldaa   #ErrNone     ;error code for no errors
             jmp    Prompt       ;ready for next command


;*********************************************************************
;* Halt Command - halts user application and enters Monitor
;*   This command is normally sent by the debugger while the user
;*   application is running. It changes the state variable in order
;*   to stay in the monitor
;*********************************************************************
HaltCmd:     bclr   flagReg,RunFlag ;run/mon flag = 0; monitor active
             ldaa  #ErrNone        ;error code for no errors
             jsr    PutChar        ;send error code
             ldaa  #StatHalt       ;status code for Halt command
             jmp    EndPrompt      ;send status and >
;*********************************************************************
;* Halt or continue user code by Rx interrupt of SCI. User code will 
;* continue if Run load switch is in run position and a resonable
;* Sci user vector is found.
;*********************************************************************
SciIsr:      brclr  DBGC1,ARM,SciIsr1 ;Arm not set so continue
									 ;above must be brclr as COF will be
									 ;Stored in trace buffer
			 bset   flagReg,ArmFlag  ;Save ARM flag
             bclr   DBGC1,ARM        ;Arm bit in Dbgc1 cleared to stop DBG
SciIsr1:     bset   SwPullup,mSwPullup ;enable pullup on monitor sw
             bset   flagReg,RunFlag  ;set run/mon flag (run mode)
			 ldab  #AllowSci0		 ; defined in the .def file
			 cmpb  #$01				 ; is it set?
			 bne    SciIsrExit       ; if AllowSci0 is set
									 ; Test run switch to allow user
									 ; Sci0 function to run
;**********************************************************************
;*  Force monitor if SwPort bit SWITCH = 0 
;*  Note: this port is configured after reset as input with pull-up
;*   if this pin in not connect sci0 will be directed to user sci0
;**********************************************************************
             ldab   SwPort           ;get port value
             bitb   #Switch          ;test the sw bit
             beq    SciIsrExit
;*********************************************************************
;* This routine checks for an unprogrammed SCI0 user interrupt
;* vector and returns to monitor if execution of an unprogrammed
;* user SCI0 vector is attempted
;*********************************************************************
			 ldy	 $F000+(vector20-BootStart) ; Get user SCI vector
             cpy     #$FFFF			 ;is it programmed?	
             beq     SciIsrExit		 ; if not exit
             jmp     0,Y			 ;if programmed the go there.

SciIsrExit:  bclr    SwPullup,mSwPullup ;restore reset state
             jmp     Prompt1
;* unlike most ISRs, this one does not end in an RTI. If/when we
;* return to running the user program, we will re-enable Rx interrupts

;*********************************************************************
;* Reset Command - forces a reset - if user pseudo-vector is not blank
;*  (or some other conditions are met - see ColdStart:) processing will
;*  start at the user-specified reset pseudo-vector location and the
;*  user has full control of all write-once registers. Otherwise reset
;*  causes the bootloader/monitor program to cold start.
;*********************************************************************
ResetCmd:     ldaa  #RSBCK|!CR2|!CR1|CR0 ;Cop disabled in BDM
              staa   COPCTL        ; turn on cop monitor
              cmpa   COPCTL        ; load to see if user touched it
              beq    CopLock       ; wait for COP reset
              jmp    ColdStart     ; can't use COP just start over
CopLock:      orcc  #$10           ; disable interrupts
              bra	 *

;*********************************************************************
;* SWI service routine - trace1 or breakpoint from user code
;*  SWI saves user CPU registers on stack and returns to monitor
;*  control at a new command prompt.
;*  User CPU registers stack frame...
;*
;*   +0  UCcr   <- SP after SWI stacking and on entry to this ISR
;*   +1  UDreg   (B:A)
;*   +3  UXreg
;*   +5  UYreg
;*   +7  UPc
;*   +9  ---     <- User's SP
;*********************************************************************
Breakpoint:  clr    DBGC2            ;Bkpct0 cleared to disabled
             clr    DBGC1            ;Dbgc1 cleared to disarm DBG
             bclr   flagReg,RunFlag  ;run/mon flag = monitor active
             ldab   #StatTrace     ;set status to Trace (SWI) -> B
                                   ;and enter monitor

;*********************************************************************
;* This is the entrypoint to the monitor from the user application
;*   A contains the status value that reflects run status
;*
;* If SP isn't within valid RAM, it can't support the monitor so the
;* monitor is forced to initialize the SP and user registers.
;*********************************************************************
ReenterMon:  
			 cps   #LowSPLimit+1  ;check against lower limit
             blo    badSP         ; note: +1 => A is not pushed yet
             cps   #HighSPLimit+1 ;check against upper limit
             bhi    badSP
             ldaa  #ErrNone       ;error code for no errors
             jsr    PutChar       ;send error code
             tba                  ;status code from B to A
             brclr  flagReg,TraceFlag,SWIdone  ;0 indicates not Trace
             bclr   flagReg,TraceFlag  ;acknowledge trace flag
             ldaa  #StatTrace     ;status code for Trace1 return
SWIdone:     jmp    EndPrompt     ;send status and >
badSP:       ldaa  #ErrSP         ;set error code to bad stack pointer
             jmp    stackInit
             bclr   flagReg,ArmFlag  ;Save ARM flag

;*********************************************************************
;* Erase EE Command -  mass
;*  erase all EEPROM locations
;*
;* Eeprom erasure assumes no protection. (Mass command will fail)
;*********************************************************************
EraseEECmd:  jsr    abClr         ;abort commands and clear errors

             brclr  MEMSIZ0,eep_sw1+eep_sw0,ErsPageErr1  ;Check if device has EEprom
             ldy   #EEpromStart   ; get device eeprom start
             std    0,y           ; write to eeprom (latch address)
                                  ; data is don't care (but needed)

             movb  #$41,ECMD      ;mass erase command
             movb  #CBEIF,ESTAT   ;register the command
             nop                  ; wait a few cycles for
             nop                  ; command to sync.
             nop
ChkDoneE:    ldaa   ESTAT         ;wait for CBEIF=CCIF=1 (cmnd done)
             bpl    ChkDoneE      ;loop if command buffer full (busy)
             asla                 ;moves CCIF to MSB (set/clear N bit)
             bpl    ChkDoneE      ;loop if CCIF=0 (not done)
             ldaa   FSTAT
             anda  #$30           ;mask all but PVIOL or ACCERR
             bne    ErsPageErr1   ;back to prompt-flash error
             ldaa   #ErrNone      ;code for no errors ($E0)
             jmp    Prompt        ;ready for next command

ErsPageErr1: ldaa   #ErrEeErase   ;Erase error code ($E9)
             jmp    Prompt        ;ready for next command

;*********************************************************************
;* Erase Command - Use repeated page erase commands to erase all flash
;*  except bootloader in protected block at the end of flash, and mass
;*  erase all EEPROM locations
;*
;* Eeprom erasure assumes no protection. (Mass command will fail)
;*********************************************************************
EraseAllCmd: jsr    abClr         ;abort commands and clear errors

             brclr  MEMSIZ0,eep_sw1+eep_sw0,ErsBlk0  ;Check if device has EEprom
             ldy   #EEpromStart   ; get device eeprom start
             std    0,y           ; write to eeprom (latch address)
                                  ; data is don't care (but needed)

             movb  #MassErase,ECMD      ;mass erase command
             movb  #CBEIF,ESTAT   ;register the command
             nop                  ; wait a few cycles for
             nop                  ; command to sync.
             nop
ChkDoneE1:   ldaa   ESTAT         ;wait for CBEIF=CCIF=1 (cmnd done)
             bpl    ChkDoneE1     ;loop if command buffer full (busy)
             asla                 ;moves CCIF to MSB (set/clear N bit)
             bpl    ChkDoneE1     ;loop if CCIF=0 (not done)
;
; erase flash pages from RomStart to start of protected bootloader
; no need to check for errors because we cleared them before EE erase
;

ErsBlk0:                          ; sector erase all full blocks
             ldab   #PagesBlk     ; Get number of banks/blocks
             decb                 ; erase all but last
             stab   1,-sp         ; save counter
             ldaa   #$3f          ; highest bank
             sba                  ; Compute lowest page-1
             staa   PPAGE         ; PPAGE for first 16K page of block 0
                                  ; (passed in the A accumulator).
             clr    FCNFG         ; set block select bits to 0.
ErsBlk0Lp:   ldx    #SectorSize   ; select sector size
             ldd    #$4000        ; Window size
             idiv                 ; compute total number of sectors
             tfr    x,d           ; get number of sectors in B
             ldx   #Window        ; point to the start of the PPAGE window.
             bsr    ErsSectors    ; go erase the PPAGE window a sector at a time.
             inc    PPAGE         ; go to the next PPAGE.
             dec    0,sp          ; done with all full PPAGE blocks?
             bne    ErsBlk0Lp     ;   no? then erase more blocks.

             ldx    #SectorSize   ; select sector size
             ldd    #((BootStart-$c000)) ; get size - protected amount
             idiv                 ; compute total number of sectors
                                  ; minus the bootblock.
             tfr    x,d           ; get number of sectors in B
             ldx   #Window        ; point to the start of the PPAGE window.
             bsr    ErsSectors    ; go erase the PPAGE window a sector at a time.
             pulb                 ; remove the page count from the stack.

; erase all sectors outside the bootblock.
;
;********************************************************************
;bulk erase all the rest
;********************************************************************

             ldab  #FlashBlks    ; select lowest page of the highest bank
             decb                 ;
             beq    EraseDone     ; if single block device quit
             ldab  #LowestPage    ; select lowest bank
BlockLoop:   stab   PPAGE         ; must match array selection
             lsrb                 ; calculate the value of the block select bits based
             lsrb                 ; on bits 3:2 of the PPAGE register value. (<256k)
             ldy   #SectorSize    ; get high byte of size
             cpy   #$0200         ; if larger than $200 shift again
             beq    nBlockLoop    ; otherwise skip ahead
             lsrb                 ; on bits 4:3 of the PPAGE register value. (512k)
nBlockLoop:  comb
             andb  #$03           ; mask off all but the lower 2 bits.
             stab   FCNFG         ; select the block to erase.
             bsr    BulkErase     ; erase it
             ldab   PPAGE         ;get ppage back
             addb  #PagesBlk      ;
             cmpb  #($3F-PagesBlk) ; see if last block
             bmi    BlockLoop

EraseDone:   movb  #$3D,PPAGE     ;select bank in array0

OkCommand:   jmp    CommandOK     ;back to no error and prompt

BulkErase:   pshx                 ;save address
             ldx    #Window       ;must point into bank
             staa   ,x            ;latch address to erase
             movb  #MassErase,FCMD      ; Select mass erase
             movb  #CBEIF,FSTAT   ;register the command
             nop                  ;wait a few cycles for
             nop                  ;command to sync.
             nop
ChkDoneF:    ldaa   FSTAT         ;wait for CBEIF=CCIF=1 (cmnd done)
             bpl    ChkDoneF      ;loop if command buffer full (busy)
             asla                 ;moves CCIF to MSB (set/clear N bit)
             bpl    ChkDoneF      ;loop if CCIF=0 (not done)
             pulx                 ;get address back
             rts
;Erase 'b' (accumulator) sectors beginning at address 'x' (index register)
;
ErsSectors:  exg    b,y           ;put the sector count in y.
ErsSectLp:   std    ,x
             movb  #SecErase,FCMD ;perform a sector erase.
             jsr    DoOnStack     ;finish command from stack-based sub
             tsta                 ;check for 0=OK
             bne    ErsSectErr    ;back to prompt-flash erase error
             leax   SectorSize,x  ;point to the next sector.
             dbne   y,ErsSectLp   ;continue to erase remaining sectors.
             rts

ErsSectErr:  puld                 ; clear stack
             bra    ErsPageErr

ErsPage:     jsr    abClr         ; abort commands and clear errors
	         ldab   PPAGE         ; get current ppage

             lsrb                 ; calculate the value of the block select bits based
             lsrb                 ; on bits 3:2 of the PPAGE register value. (<256k)
             ldy   #SectorSize    ; get high byte of size
             cpy   #$0200         ; if larger than $200 shift again
             beq    ErsPage1      ; otherwise skip ahead
             lsrb                 ; on bits 4:3 of the PPAGE register value. (512k)
ErsPage1:    comb
             andb  #$03           ; mask off all but the lower 2 bits.
             stab   FCNFG         ; select the block to erase.
             ldab   PPAGE         ; get current ppage
             cmpb  #$3F		      ; is it the page with the monitor
             bne   ErsFullPage    ; no then erase all of page
             ldx   #SectorSize    ; select sector size
             ldd   #((BootStart-$c000)) ; get size - protected amount
             idiv                 ; compute total number of sectors
                                  ; minus the bootblock.
             tfr    x,d           ; get number of sectors in B
             ldx   #Window        ; point to the start of the PPAGE window.
             bsr    ErsSectors    ; go erase the PPAGE window a sector at a time.
             bra    EraPageStat   ; back to no error and prompt

ErsFullPage: ldx   #SectorSize    ; select sector size
             ldd   #$4000         ; Window size
             idiv                 ; compute total number of sectors
             tfr    x,d           ; get number of sectors in B
             ldx   #Window        ; point to the start of the PPAGE window.
             bsr    ErsSectors    ; go erase the PPAGE window a sector at a time.
             bra    EraPageStat     ;back to no error and prompt

EraPageStat: ldaa   FSTAT
             anda  #$30           ;mask all but PVIOL or ACCERR
             bne    ErsPageErr    ;back to prompt-flash error
             ldaa   #ErrNone      ;code for no errors ($E0)
             jmp    Prompt        ;ready for next command

ErsPageErr: ldaa   #ErrFlash      ;code for Flash error ($E6)
             jmp    Prompt        ;ready for next command

;*********************************************************************
;* Read Byte Command - read specified address and return the data
;*  8-bit command code from host to SCI0 RxD
;*  16-bit address (high byte first) from host to SCI0 RxD
;*  8-bit data sent back to host through SCI0 TxD
;*********************************************************************
RdByteCmd:   jsr    getX          ;get address to read from
             ldaa   ,x            ;read the requested location
             jsr    PutChar       ;send it out SCI0
             jmp    CommandOK     ;ready for next command

;*********************************************************************
;* Read Word Command - read specified block of data
;*  8-bit command code from host to SCI0 RxD
;*  16-bit address (high byte first) from host to SCI0 RxD
;*  16-bit number sent back to host through SCI0 TxD
;* Special case of block read.
;*********************************************************************
RdWordCmd:   jsr    getX          ;get address to read from
sendExit:    ldd    ,x            ;read the requested location
             jsr    PutChar       ;send it out SCI0
             tba
             jsr    PutChar       ;send it out SCI0
             jmp    CommandOK     ;ready for next command
             
;*********************************************************************
;* Read Command - read specified block of data
;*  8-bit command code from host to SCI0 RxD
;*  16-bit address (high byte first) from host to SCI0 RxD
;*  8-bit number of bytes-1 to sent back to host through SCI0 TxD
;*********************************************************************
ReadCmd:     jsr    getX          ;get address to read from
             jsr    GetChar       ;get number of bytes to read
             tab
             incb                 ;correct counter (0 is actually 1)
ReadNext:    ldaa   ,x            ;read the requested location
			 jsr    PutChar       ;send it out SCI0
             inx
             decb   
             bne    ReadNext             
             ldaa  #ErrNone       ;code for no errors ($E0)
xPrompt:     jmp    Prompt        ;ready for next command

;*********************************************************************
;* Write Command - write specified block of data
;*  8-bit command code from host to SCI0 RxD
;*  16-bit address (high byte first) from host to SCI0 RxD
;*  8-bit number of bytes-1 to write from host to SCI0 TxD
;*  8-bit values to write
;* this function used Word writes whenever possible. This is:
;* a) when more than one byte is still to write
;* b) and the address is even
;*********************************************************************
WriteCmd:    jsr    getX          ;get address to write to
             jsr    GetChar       ;get number of bytes to read
             tab
             incb                 ;correct counter (0 is actually 1)
WriteNext:   cmpb   #1            ;if only one byte left
             pshb                 ;preserve byte counter
             beq    WriteByte     ;write it 
             tfr    x,a           ;is address odd
             bita   #1
             bne    WriteByte     ;write a byte first
             
WriteWord:   jsr    GetChar       ;get high byte
             tab                  ;save in B
             dec    ,sp           ;decrement byte counter (on stack)
             jsr    GetChar       ;get low byte
             exg    a,b           ;flip high and low byte
             jsr    WriteD2IX     ;write or program data to address
             pulb                 ;restore byte counter                
             bne    WriteError    ;error detected
             inx                  ;increment target address
             bra    Write1         

WriteByte:   jsr    GetChar       ;get data to write
             jsr    WriteA2IX     ;write or program data to address
             pulb                 ;restore byte counter       
             bne    WriteError    ;error detected
Write1:      inx                  ;increment target address
             decb                 ;decrement byte counter 
             bne    WriteNext
             ldaa   #ErrNone      ;code for no errors ($E0)             
             bra    xPrompt       ;then back to prompt
                 
SkipBytes:   jsr    GetChar       ;read remaining bytes                               
WriteError:  decb                 ;
             bne    SkipBytes                           
             ldaa   #ErrFlash     ;code for Flash error ($E6)
WriteDone:   bra    xPrompt       ;then back to prompt

;*********************************************************************
;* Read Next Command - IX=IX+2; read m(IX,IX=1) and return the data
;*  8-bit command code from host to SCI0 RxD
;*  16-bit data sent back to host through SCI0 TxD
;*  uses current value of IX from user CPU regs stack frame
;*********************************************************************
RdNextCmd:   brclr  flagReg,RunFlag,notRun  ;do command if not run
             clra                 ;data = $00 (can't read real data)
             jsr    PutChar       ;send $00 instead of read_next data
             jsr    PutChar       ;send $00 instead of read_next data
             ldaa   #ErrRun       ;code for run mode error
xCmnd:       jmp    Prompt        ;back to prompt; run error
notRun:      bsr    preInc        ;get, pre-inc, & update user IX
             jmp    sendExit      ;get data, send it, & back to prompt

;*********************************************************************
;* Write Byte Command - write specified address with specified data
;*  8-bit command code from host to SCI0 RxD
;*  16-bit address (high byte first) from host to SCI0 RxD
;*  8-bit data from host to SCI0 RxD
;*********************************************************************
WtByteCmd:   jsr    getX          ;get address to write to
WriteNext2:  jsr    GetChar       ;get data to write
             jsr    CheckModule
             beq    isRAMbyte
             bra    WriteByteNVM  ;deny access (byte NVM access)

isRAMbyte:   staa   0,x           ;write to RAM or register
             clra                 ;force Z=1 to indicate OK

WriteExit:   ldaa  #ErrNone       ;code for no errors ($E0)
             jmp    Prompt        ;ready for next command

WriteByteNVM: ldaa #ErrByteNVM    ;code for byte NVM error ($E5)
             jmp    Prompt        ;ready for next command


;*********************************************************************
;* Write Word Command - write word of data
;*  8-bit command code from host to SCI0 RxD
;*  16-bit address (high byte first) from host to SCI0 RxD
;*  16-bit value to write
;*********************************************************************
WtWordCmd:   jsr    getX          ;get address to write to
			 ldab  #02            ;one word +1
			 pshb				  ;save it on stack
             bra    WriteWord     ;get & write data, & back to prompt
             
;*********************************************************************
;* Write Next Command - IX=IX+1; write specified data to m(IX)
;*  8-bit command code from host to SCI0 RxD
;*  16-bit data from host to SCI0 RxD
;*
;*  uses current value of IX from user CPU regs stack frame
;*********************************************************************
WtNextCmd:   brclr  flagReg,RunFlag,notRunW  ;do command if not run
             jsr    getX          ;clear data
             ldaa   #ErrRun       ;code for run mode error
xCmndW:      jmp    Prompt        ;back to prompt; run error

notRunW:     bsr    preInc        ;get, pre-inc, & update user IX
			 ldab  #02            ;one word +1
			 pshb				  ;save it on stack
             bra    WriteWord     ;get & write data, & back to prompt

;*********************************************************************
;* utility to get IX from stack frame and pre increment it by 2
;* assumes interrupts are blocked while in monitor
;*********************************************************************
preInc:      leas 2,sp
             ldx    UXreg,sp      ;get user X
             inx                  ;pre-increment
             inx                  ;pre-increment
             stx    UXreg,sp      ;put adjusted user X back on stack
             leas -2,sp
             rts                  ;pre-incremented IX still in IX

;*********************************************************************
;* Read Registers Command - read user's CPU register values
;*
;*  16-bit SP value (high byte first) sent to host through SCI0 TxD
;*  16-bit PC value (high byte first) sent to host through SCI0 TxD
;*  16-bit IY value (high byte first) sent to host through SCI0 TxD
;*  16-bit IX value (high byte first) sent to host through SCI0 TxD
;*  16-bit D  value (high byte first) sent to host through SCI0 TxD
;*   8-bit CCR value sent to host through SCI0 TxD
;*
;* User CPU registers stack frame...
;*
;*   +0  UCcr   <- Monitor's SP
;*   +1  UDreg   (B:A)
;*   +3  UXreg
;*   +5  UYreg
;*   +7  UPc
;*   +9  ---     <- User's SP
;*********************************************************************
RdRegsCmd:   tsx                  ;IX = Monitor SP +2
             leax   SPOffset,x    ;correct SP value
             jsr    put16         ;send user SP out SCI0
             ldx    UPc,sp        ;user PC to IX
             jsr    put16         ;send user PC out SCI0
             ldx    UYreg,sp      ;user IY to IX
             jsr    put16         ;send user IY out SCI0
             ldx    UXreg,sp      ;user IX to IX
             jsr    put16         ;send user IX out SCI0
             ldx    UDreg,sp      ;user D to IX
             exg    d,x
             exg    a,b           ;flip as D is stacked B:A
             exg    d,x
             jsr    put16         ;send user D out SCI0
             ldaa   UCcr,sp       ;user CCR to A
             jsr    PutChar       ;send user CCR out SCI0
             jmp    CommandOK     ;back to prompt

;*********************************************************************
;* Write CCR Command - write user's CCR register value
;*  8-bit command code from host to SCI0 RxD
;*  8-bit data for CCR from host to SCI0 RxD
;*********************************************************************
WriteCcrCmd: jsr    GetChar       ;read new CCR value
             staa   UCcr,sp       ;replace user CCR value
             jmp    CommandOK     ;back to no error and prompt

;*********************************************************************
;* Write D Command - write user's D register value
;*  8-bit command code from host to SCI0 RxD
;*  16-bit data (high byte first) for D from host to SCI0 RxD
;*********************************************************************
WriteDCmd:   jsr    getX          ;read new D value
             exg    d,x
             exg    a,b           ;flip as D is stacked B:A
             exg    d,x
             stx    UDreg,sp      ;replace user D value
             jmp    CommandOK     ;back to no error and prompt

;*********************************************************************
;* Write IX Command - write user's IX register value
;*  8-bit command code from host to SCI0 RxD
;*  16-bit data (high byte first) for IX from host to SCI0 RxD
;*********************************************************************
WriteIXCmd:  jsr    getX          ;read new IX value
             stx    UXreg,sp      ;replace user IX value
             jmp    CommandOK     ;back to no error and prompt

;*********************************************************************
;* Write IY Command - write user's IY register value
;*  8-bit command code from host to SCI0 RxD
;*  16-bit data (high byte first) for IY from host to SCI0 RxD
;*********************************************************************
WriteIYCmd:  jsr    getX          ;read new IY value
             stx    UYreg,sp      ;replace user IY value
             jmp    CommandOK     ;back to no error and prompt

;*********************************************************************
;* Write PC Command - write user's PC register value
;*  8-bit command code from host to SCI0 RxD
;*  16-bit data (high byte first) for PC from host to SCI0 RxD
;*********************************************************************
WritePcCmd:  jsr    getX          ;read new PC thru SCI0 to IX
             stx    UPc,sp       ;replace user PC value
             jmp    CommandOK     ;back to no error and prompt

;*********************************************************************
;* Write SP Command - write user's SP register value
;*  8-bit command code from host to SCI0 RxD
;*  16-bit data (high byte first) for SP from host to SCI0 RxD
;*
;*  Since other user CPU register values are stored on the stack, the
;*  host will need to re-write the other user registers after SP is
;*  changed. This routine just changes SP itself.
;*
;*  SP value is user's SP & it is adjusted (-10) to accommodate the
;*  user CPU register stack frame.
;*
;*  If the host attempts to set the user SP value <RamStart or >RamLast
;*  then the change is ignored, because such values would not support
;*  proper execution of the monitor firmware.
;*********************************************************************
WriteSpCmd:  bsr    getX         ;new SP value now in IX
             leax  -SPOffset,x   ;correct SP value
             cpx   #LowSPLimit   ;check against lower limit
             blo    spBad
             cpx   #HighSPLimit  ;check against upper limit
             bhi    spBad
             txs                 ;IX -> SP
             jmp    CommandOK    ;back to no error and prompt
spBad:       ldaa    #ErrWriteSP      ;error code for stack errors
;             bsr    PutChar      ;send error code
             jmp    Prompt       ;send status and >

;*********************************************************************
;* Trace 1 Command - trace one user instruction starting at current PC
;*  8-bit command code from host to SCI0 RxD
;*
;*  if an interrupt was already pending, the user PC will point at the
;*  ISR after the trace and the opcode at the original address will
;*  not have been executed. (because the interrupt response is
;*  considered to be an instruction to the CPU)
;*********************************************************************
;
pagebits:   fcb      $3D		;$0000-$3FFF is PPAGE $3D
            fcb      $3E		;$4000-$7FFF is PPAGE $3E
            fcb      $3F		;$C000-$FFFF is PPAGE $3F
pagebitsaddr:
            fdb     pagebits     ;$0000-$3FFF : Use constant $3D
            fdb     pagebits+1   ;$4000-$7FFF : Use constant $3E (2nd last page)
            fdb     $0030        ;$8000-$BFFF : Use window PPAGE
            fdb     pagebits+2   ;$C000-$FFFF : Use constant $3F (last page)
Trace1Cmd:
            bset    flagReg,TraceFlag  ;so at SWI we know it was Trace
            ldx     UPc,sp       ;PC of go address
            inx                  ;IX points at go opcode +1
            inx                  ;IX points at go opcode +2
 	        xgdx
            andb   #$FE
            std     DBGACH       ;(BKP0H) debugger trigger address
            std     DBGBCH       ;(BKP1H) same for second address to have it initialized
            rola
            rolb
            rola
            rolb				 ;get ready to search pagebits table
            andb   #$03          ;what range 0-3FFF,4000-7FFF,8000-BFFF,or C000-FFFF?
            clra
            lsld
            xgdx
            ldx     pagebitsaddr,x
            ldaa    0,x

;???  below needs modified for DBG v. BRKPT difference
            staa    DBGACX       ;(BKP0X) set page byte of address
            staa    DBGBCX       ;(BKP1X) same for second address to have it initialized
            ldaa   #traceOne     ; enable, arm, CPU force
            staa    DBGC2        ;(BKPCT0) arm DBG to trigger after 1 instr.
            rti                  ; restore regs and go to user code

;*********************************************************************
;* Go Command - go to user's program at current PC address
;*  8-bit command code from host to SCI0 RxD
;* - no promt is issued 
;*  typically, an SWI will cause control to pass back to the monitor
;*********************************************************************
GoCmd:       bset SCI0CR2,RIE     ;need to enable SCI0 Rx interrupts to
                                  ; enter monitor on any char received
             bclr  flagReg,TraceFlag ; run flag clr
             rti                  ;restore regs and exit
;*********************************************************************
;* Utility to send a 16-bit value out X through SCI0
;*********************************************************************
put16:       exg    d,x           ;move IX to A
             bsr    PutChar       ;send high byte
             tba                  ;move B to A
             bsr    PutChar       ;send low byte
             rts

;*********************************************************************
;* Utility to get a 16-bit value through SCI0 into X
;*********************************************************************
getX:        bsr    GetChar       ;get high byte
             tab                  ;save in B
             bsr    GetChar       ;get low byte
             exg    a,b           ;flip high and low byte
             exg    d,x           ;16-bit value now in IX
             rts
;*********************************************************************
;* GetChar - wait indefinitely for a character to be received
;*  through SCI0 (until RDRF becomes set) then read char into A
;*  and return. Reading character clears RDRF. No error checking.
;*
;* Calling convention:
;*            bsr    GetChar
;*
;* Returns: received character in A
;*********************************************************************
GetChar:     brset  SCI0SR1,RDRF,RxReady ;exit loop when RDRF=1
             bra    GetChar              ;loop till RDRF set
RxReady:     ldaa   SCI0DRL              ;read character into A
             rts                         ;return

;*********************************************************************
;* PutChar - sends the character in A out SCI0
;*
;* Calling convention:
;*            ldaa    data          ;character to be sent
;*            bsr    PutChar
;*
;* Returns: nothing (A unchanged)
;*********************************************************************
PutChar:     brclr   SCI0SR1,TDRE,PutChar ;wait for Tx ready
             staa    SCI0DRL       ;send character from A
             rts

;*********************************************************************
;* CheckModule - check in what memory type the address in IX points to
;*  The location may be RAM, FLASH, EEPROM, or a register
;*  if the vector table is addresses, IX is changed to point to the
;*  same vector in the pseudo vector table
;*  returns in B: 1 FLASH or EEPROM
;*                0 RAM or register (all the rest of the address space)
;*               -1 access denied (monitor or pseudo vector)
;*  all registers are preserved except B
;*********************************************************************
CheckModule: pshd                 ;preserve original data
             cpx    #RomStart
             blo    check4EE      ;skip if not flash
             cpx    #VectorTable
             bhs    isVector      ;is it in the real vector table
             cpx    #PVecTable
             blo    isToProgram   ;pseudo vector table or monitor area
             ldab   #$FF          ;access denied (N=1, Z=0)
             puld                 ;restore original data (D)
             rts

isVector:    leax   BootStart,x   ;access pseudo vector table
             bra    isToProgram

check4EE:    brclr  MEMSIZ0,eep_sw1+eep_sw0,isRAM  ;Check if device has EEprom
			 cpx   #EEpromStart
             blo    isRAM         ;treat as RAM or registers
			 cpx   #EEpromEnd	  ;Greater than allocated EE space?
             bhi    isRAM         ;must be registers or RAM
isToProgram: ldab   #1            ;set flgs - signal FLASH (N=0, Z=0)
             puld                 ;restore original data (D)
             rts

isRAM:       clrb                 ;signal RAM  (N=0, Z=1)
             puld                 ;restore original data (D)
             rts

;*********************************************************************
;* WriteD2IX - Write the data in D (word) to the address in IX
;*  The location may be RAM, FLASH, EEPROM, or a register
;*  if FLASH or EEPROM, the operation is completed before return
;*  IX and A preserved, returns Z=1 (.EQ.) if OK
;*
;*********************************************************************
WriteD2IX:   pshx                 ;preserve original address
             pshd                 ;preserve original data
             bsr    CheckModule
             bmi    ExitWrite     ;deny access (monitor or pseudo vector)
             beq    isRAMword
             cpd    0,x           ;FLASH or EEPROM needs programming
             beq    ExitWrite     ;exit (OK) if already the right data
             pshd                 ;temp save data to program
             tfr    x,b           ;low byte of target address -> B
             bitb   #1            ;is B0 = 1?
             bne    oddAdrErr     ;then it's odd addr -> exit
             ldd    0,x           ;$FFFF if it was erased
             cpd    #$FFFF        ;Z=1 if location was erased first
oddAdrErr:   puld                 ;recover data, don't change CCR
             bne    ExitWrite     ;exit w/ Z=0 to indicate error
             bra    DoProgram

isRAMword:   std    0,x           ;write to RAM or register
             clra                 ;force Z=1 to indicate OK
             bra    ExitWrite

;*********************************************************************
;* WriteA2IX - Write the data in A (byte) to the address in IX
;*  The location may be RAM, FLASH, EEPROM, or a register
;*  if FLASH or EEPROM, the operation is completed before return
;*  IX and A preserved, returns Z=1 (.EQ.) if OK
;*
;* Note: Byte writing to the FLASH and EEPROM arrays is a violation
;*       of the HC9S12 specification. Doing so, will reduce long term
;*       data retention and available prog / erase cycles
;*
;*********************************************************************

WriteA2IX:   pshx                 ;preserve original address
             pshd                 ;preserve original data
             bsr    CheckModule
             bmi    ExitWrite     ;deny access (monitor or pseudo vector)
             beq    isWRAMbyte      
             cmpa   0,x           ;FLASH or EEPROM needs programming 
             beq    ExitWrite     ;exit (OK) if already the right data
             ldab   0,x           ;$FF if it was erased
             incb                 ;Z=1 if location was erased first
             bne    ExitWrite     ;exit w/ Z=0 to indicate error
        
             tfr    x,b           ;test least significant bit
             bitb   #1            ;is B0 = 1?
             bne    isOddAdr      ;then it's odd addr.             
isEvenAdr:   ldab   1,x           ;low byte of D (A:B) from memory
             bra    DoProgram                     
isOddAdr:    tab                  ;move to low byte of D (A:B)
             dex                  ;point to even byte
             ldaa   ,x            ;high byte of D (A:B) from memory  
             bra    DoProgram                        
                                     
isWRAMbyte:  staa   0,x           ;write to RAM or register
             clra                 ;force Z=1 to indicate OK
             bra    ExitWrite 

; Programs D to IX in either FLASH or EEPROM
DoProgram:   bsr    abClr         ;abort commands and clear errors
             cpx    #RomStart     ;simple test only
             blo    itsEE         ; details already verified
             bsr    ProgFlash     ;program the requested location
             bra    ExitWrite     ;exit (Z indicates good or bad)
itsEE:       bsr    ProgEE        ;program the requested location
; exit Write?2IX functions (Z indicates good or bad)
ExitWrite:   puld                 ;restore original data (D)
             pulx                 ;restore original address (IX)
             rts

;*********************************************************************
;* Progee - program a single word in the HCS9S12 EEPROM
;*  the location is assumed to be previously erased. This routine
;*  waits for the command to complete.
;*
;* On entry... IX - points at the EEPROM address to be programmed
;*  A - holds the data value to be programmed
;*
;* Calling convention:
;*           bsr    Prog1ee
;*
;* Returns: IX unchanged and A = ESTAT shifted left by 2 bits
;*  Z=1 if OK, Z=0 if protect violation or access error
;*********************************************************************
ProgEE:      std     ,x           ;latch address & data to program
             ldaa    #ProgWord    ;Select program word command
             staa    ECMD         ;issue word program command
             ldaa    #CBEIF
             staa    ESTAT        ;[pwpp] register command
             nop                  ;[p]
             nop
             nop
ChkDoneEE:   ldaa    ESTAT        ;[prpp] (min 4~ before 1st read)
             anda   #$C0          ; mask all but 2 msb
             lsla                 ;CCIF now in MSB
             bpl    ChkDoneEE     ;wait for queued commands to finish
             asla                 ;A=00 & Z=1 unless PVIOL or ACCERR
xProgEE      rts

;
; utility sub to abort previous commands in flash and EEPROM
; and clear any pending errors
;
abClr:       psha
             ldaa    #PVIOL+ACCERR ;mask
             staa    ESTAT         ;abort any command and clear errors
             staa    FSTAT         ;abort any command and clear errors
             pula
             rts

;*********************************************************************
;* Progflash - programs one byte of HCS9S12 FLASH
;*  This routine waits for the command to complete before returning.
;*  assumes location was blank. This routine can be run from FLASH
;*
;* On entry... IX - points at the FLASH byte to be programmed
;*             A holds the data for the location to be programmed
;*
;* Calling convention:
;*           bsr    Prog1flash
;*
;* Uses: DoOnStack which uses SpSub
;* Returns: IX unchanged and A = FSTAT bits PVIOL and ACCERR only
;*  Z=1 if OK, Z=0 if protect violation or access error
;*********************************************************************
ProgFlash:   pshd
             cpx   #$8000         ; if <$8000 then bank 3E
             blo    its3E         ;set ppage to 3E
             cpx   #$C000         ; if > $BFFF then bank 3F
             blo    ProgFlash1    ;set ppage 3F
             movb  #$3F,PPAGE     ;
             bra   ProgFlash1
its3E:       movb  #$3E,PPAGE     ;

ProgFlash1:  ldab   PPAGE
             lsrb                 ; calculate the value of the block select bits based
             lsrb                 ; on bits 3:2 of the PPAGE register value. (<256k)
             ldy   #SectorSize   ; get high byte of size
             cpy   #$0200         ; if larger than $200 shift again
             beq    nBlockLoopb
             lsrb                 ; on bits 4:3 of the PPAGE register value. (512k)

nBlockLoopb: comb
             andb  #$03           ; mask off all but the lower 2 bits.
             stab   FCNFG         ; select the block to program.
             cmpb  #$00           ; if block zero use DoOnStack method
             puld
             beq    ProgFlashSP

ProgFlshRom: std    ,x            ;latch address & data to program
             ldaa  #ProgWord         ;Select program word command
             staa   FCMD          ;issue byte program command
             ldaa  #CBEIF
             bsr    SpSub         ;register command & wait to finish
             ldaa   FSTAT
             anda  #$30           ;mask all but PVIOL or ACCERR
             rts

ProgFlashSP: std    ,x            ;latch address and data
             ldaa  #ProgWord         ;Select program word command
             staa   FCMD          ;issue byte program command
;
; DoOnStack will register the command then wait for it to finish
;  in this unusual case where DoOnStack is the next thing in program
;  memory, we don't need to call it. The rts at the end of DoOnStack
;  will return to the code that called Prog1flash.
;
;*********************************************************************
;* DoOnStack - copy SpSub onto stack and call it (see also SpSub)
;*  De-allocates the stack space used by SpSub after returning from it.
;*  Allows final steps in a flash prog/erase command to execute out
;*  of RAM (on stack) while flash is out of the memory map
;*  This routine can be used for flash word-program or erase commands
;*
;* Calling Convention:
;*           bsr    DoOnStack
;*
;* Uses 22 bytes on stack + 2 bytes if BSR/bsr used to call it
;* returns IX unchanged
;********************************************************************
DoOnStack:   pshx                 ;save IX
             ldx   #SpSubEnd-2    ;point at last word to move to stack
SpmoveLoop:  ldd    2,x-          ;read from flash
             pshd                 ;move onto stack
             cpx   #SpSub-2       ;past end?
             bne    SpmoveLoop    ;loop till whole sub on stack
             tfr    sp,x          ;point to sub on stack
             ldaa  #CBEIF         ;preload mask to register command
             jsr    ,x            ;execute the sub on the stack
             leas   SpSubEnd-SpSub,sp  ;de-allocate space used by sub
             ldaa   FSTAT         ;get result of operation
             anda  #$30           ;and mask all but PVIOL or ACCERR
             pulx                 ;restore IX
             rts                  ;to flash where DoOnStack was called

;*********************************************************************
;* SpSub - register flash command and wait for Flash CCIF
;*  this subroutine is copied onto the stack before executing
;*  because you can't execute out of flash while a flash command is
;*  in progress (see DoOnStack to see how this is used)
;*
;* Uses 18 bytes on stack + 2 bytes if a BSR/bsr calls it
;*********************************************************************
             EVEN   			  ;Make code start word aliened
SpSub:       
		     tfr    ccr,b		  ;get copy of ccr
			 orcc  #$10			  ;disable interrupts
             staa   FSTAT         ;[PwO] register command
             nop                  ;[O] wait min 4~ from w cycle to r
             nop                  ;[O]
             nop                  ;[O]
             brclr  FSTAT,CCIF,*  ;[rfPPP] wait for queued commands to finish
             tfr	b,ccr		  ;restore ccr and int condition
             rts                  ;back into DoOnStack in flash
SpSubEnd:
;*********************************************************************
;* User Pseudo-vector Equates (just before protected block)
;*  real vectors point here, each pseudo-vector is a bra instruction
;*  to the user's ISR.
;*********************************************************************
BSRTable:
uvector63:	bsr    ISRHandler    ; /* vector 63 */
uvector62:	bsr    ISRHandler    ; /* vector 62 */
uvector61:	bsr    ISRHandler    ; /* vector 61 */
uvector60:	bsr    ISRHandler    ; /* vector 60 */
uvector59:	bsr    ISRHandler    ; /* vector 59 */
uvector58:	bsr    ISRHandler    ; /* vector 58 */
uvector57:	bsr    ISRHandler    ; /* vector 57 */
uvector56:	bsr    ISRHandler    ; /* vector 56 */
uvector55:	bsr    ISRHandler    ; /* vector 55 */
uvector54:	bsr    ISRHandler    ; /* vector 54 */
uvector53:	bsr    ISRHandler    ; /* vector 53 */
uvector52:	bsr    ISRHandler    ; /* vector 52 */
uvector51:	bsr    ISRHandler    ; /* vector 51 */
uvector50:	bsr    ISRHandler    ; /* vector 50 */
uvector49:	bsr    ISRHandler    ; /* vector 49 */
uvector48:	bsr    ISRHandler    ; /* vector 48 */
uvector47:	bsr    ISRHandler    ; /* vector 47 */
uvector46:	bsr    ISRHandler    ; /* vector 46 */
uvector45:	bsr    ISRHandler    ; /* vector 45 */
uvector44:	bsr    ISRHandler    ; /* vector 44 */
uvector43:	bsr    ISRHandler    ; /* vector 43 */
uvector42:	bsr    ISRHandler    ; /* vector 42 */
uvector41:	bsr    ISRHandler    ; /* vector 41 */
uvector40:	bsr    ISRHandler    ; /* vector 40 */
uvector39:	bsr    ISRHandler    ; /* vector 39 */
uvector38:	bsr    ISRHandler    ; /* vector 38 */
uvector37:	bsr    ISRHandler    ; /* vector 37 */
uvector36:	bsr    ISRHandler    ; /* vector 36 */
uvector35:	bsr    ISRHandler    ; /* vector 35 */
uvector34:	bsr    ISRHandler    ; /* vector 34 */
uvector33:	bsr    ISRHandler    ; /* vector 33 */
uvector32:	bsr    ISRHandler    ; /* vector 32 */
uvector31:	bsr    ISRHandler    ; /* vector 31 */
uvector30:	bsr    ISRHandler    ; /* vector 30 */
uvector29:	bsr    ISRHandler    ; /* vector 29 */
uvector28:	bsr    ISRHandler    ; /* vector 28 */
uvector27:	bsr    ISRHandler    ; /* vector 27 */
uvector26:	bsr    ISRHandler    ; /* vector 26 */
uvector25:	bsr    ISRHandler    ; /* vector 25 */
uvector24:	bsr    ISRHandler    ; /* vector 24 */
uvector23:	bsr    ISRHandler    ; /* vector 23 */
uvector22:	bsr    ISRHandler    ; /* vector 22 */
uvector21:	bsr    ISRHandler    ; /* vector 21 */
uvector20:	bsr    ISRHandler    ; SCI0, used for monitor (place holder only)
uvector19:	bsr    ISRHandler    ; /* vector 19 */
uvector18:	bsr    ISRHandler    ; /* vector 18 */
uvector17:	bsr    ISRHandler    ; /* vector 17 */
uvector16:	bsr    ISRHandler    ; /* vector 16 */
uvector15:	bsr    ISRHandler    ; /* vector 15 */
uvector14:	bsr    ISRHandler    ; /* vector 14 */
uvector13:	bsr    ISRHandler    ; /* vector 13 */
uvector12:	bsr    ISRHandler    ; /* vector 12 */
uvector11:	bsr    ISRHandler    ; /* vector 11 */
uvector10:	bsr    ISRHandler    ; /* vector 10 */
uvector09:	bsr    ISRHandler    ; /* vector 09 */
uvector08:	bsr    ISRHandler    ; /* vector 08 */
uvector07:	bsr    ISRHandler    ; /* vector 07 */
uvector06:	bsr    ISRHandler    ; /* vector 06 */
uvector05:	bsr    ISRHandler    ; /* vector 05 */
			bsr    ISRHandler    ; SWI, used for breakpoints (place holder only)
uvector03:	bsr    ISRHandler    ; /* vector 03 */
uvector02:	bsr    ISRHandler    ; /* vector 02 */
uvector01:	bsr    ISRHandler    ; /* vector 01 */
            bsr    ISRHandler    ; /* Reset vector */

;*********************************************************************
;* ISRHandler this routine checks for unprogrammed interrupt
;*  vectors and returns an $E3 error code if execution of an
;*  unprogrammed vector is attempted
;*********************************************************************
ISRHandler:  pulx    ;pull bsr return address off stack
             ldy     (PVecTable-BSRTable-2),X
             cpy     #$FFFF
             beq     BadVector
             jmp     ,Y

;*********************************************************************
;* Invalid (erased) vector fetched
;*   low byte of vector address is Status (passed in B to ReenterMon)
;* Leave stack frame from ISR on stack to refresh monitor registers.
;*********************************************************************

BadVector:   leax   (PVecTable-BSRTable-2),X
             xgdx                 ;low byte of vector address in B
             jmp    ReenterMon    ;and enter monitor
EndMonitor:

  IF EndMonitor >= FProtStart
    FAIL "Monitor code overflows into protection/security area"
  ENDIF
;*********************************************************************
;* Jump table for external use of routines.
;*********************************************************************
             org    FProtStart-26 ;immediately before the vector table
             jmp    PutChar
             jmp    GetChar
             jmp    EraseAllCmd
             jmp    DoOnStack
             jmp    WriteD2IX

             org    FProtStart-8
             fdb    softwareID4	;Software device type (deviceID)
             fdb    softwareID1	;Software revision (date)
             fdb    softwareID2	;Software revision (year)
             fdb    softwareID3	;Software revision (ver)
;
;*********************************************************************
;* FLASH configuration: protection, security
;*********************************************************************
             org    FProtStart   ; enable protection
             fdb    $FFFF        ; Skip Backdoor Key
             fdb    $FFFF        ; Skip Backdoor Key
             fdb    $FFFF        ; Skip Backdoor Key
             fdb    $FFFF        ; Skip Backdoor Key

             fdb    $FFFF        ; Skip Reserved

             fcb    $FF          ; protection block 3
             fcb    $FF          ; protection block 2
             fcb    $FF          ; protection block 1
             fcb    FProtBlksz   ; protection block 0
             fcb    $FF          ; Skip
             fcb    FSecure      ; set security and backdoor access
;*********************************************************************
;* Define all vectors even if program doesn't use them all
;*********************************************************************

           org    VectorTable
vector63:  fdb    uvector63  ; /* vector 63 */
vector62:  fdb    uvector62  ; /* vector 62 */
vector61:  fdb    uvector61  ; /* vector 61 */
vector60:  fdb    uvector60  ; /* vector 60 */
vector59:  fdb    uvector59  ; /* vector 59 */
vector58:  fdb    uvector58  ; /* vector 58 */
vector57:  fdb    uvector57  ; /* vector 57 */
vector56:  fdb    uvector56  ; /* vector 56 */
vector55:  fdb    uvector55  ; /* vector 55 */
vector54:  fdb    uvector54  ; /* vector 54 */
vector53:  fdb    uvector53  ; /* vector 53 */
vector52:  fdb    uvector52  ; /* vector 52 */
vector51:  fdb    uvector51  ; /* vector 51 */
vector50:  fdb    uvector50  ; /* vector 50 */
vector49:  fdb    uvector49  ; /* vector 49 */
vector48:  fdb    uvector48  ; /* vector 48 */
vector47:  fdb    uvector47  ; /* vector 47 */
vector46:  fdb    uvector46  ; /* vector 46 */
vector45:  fdb    uvector45  ; /* vector 45 */
vector44:  fdb    uvector44  ; /* vector 44 */
vector43:  fdb    uvector43  ; /* vector 43 */
vector42:  fdb    uvector42  ; /* vector 42 */
vector41:  fdb    uvector41  ; /* vector 41 */
vector40:  fdb    uvector40  ; /* vector 40 */
vector39:  fdb    uvector39  ; /* vector 39 */
vector38:  fdb    uvector38  ; /* vector 38 */
vector37:  fdb    uvector37  ; /* vector 37 */
vector36:  fdb    uvector36  ; /* vector 36 */
vector35:  fdb    uvector35  ; /* vector 35 */
vector34:  fdb    uvector34  ; /* vector 34 */
vector33:  fdb    uvector33  ; /* vector 33 */
vector32:  fdb    uvector32  ; /* vector 32 */
vector31:  fdb    uvector31  ; /* vector 31 */
vector30:  fdb    uvector30  ; /* vector 30 */
vector29:  fdb    uvector29  ; /* vector 29 */
vector28:  fdb    uvector28  ; /* vector 28 */
vector27:  fdb    uvector27  ; /* vector 27 */
vector26:  fdb    uvector26  ; /* vector 26 */
vector25:  fdb    uvector25  ; /* vector 25 */
vector24:  fdb    uvector24  ; /* vector 24 */
vector23:  fdb    uvector23  ; /* vector 23 */
vector22:  fdb    uvector22  ; /* vector 22 */
vector21:  fdb    uvector21  ; /* vector 21 */
vector20:  fdb    SciIsr     ; /* vector 20 */
vector19:  fdb    uvector19  ; /* vector 19 */
vector18:  fdb    uvector18  ; /* vector 18 */
vector17:  fdb    uvector17  ; /* vector 17 */
vector16:  fdb    uvector16  ; /* vector 16 */
vector15:  fdb    uvector15  ; /* vector 15 */
vector14:  fdb    uvector14  ; /* vector 14 */
vector13:  fdb    uvector13  ; /* vector 13 */
vector12:  fdb    uvector12  ; /* vector 12 */
vector11:  fdb    uvector11  ; /* vector 11 */
vector10:  fdb    uvector10  ; /* vector 10 */
vector09:  fdb    uvector09  ; /* vector 09 */
vector08:  fdb    uvector08  ; /* vector 08 */
vector07:  fdb    uvector07  ; /* vector 07 */
vector06:  fdb    uvector06  ; /* vector 06 */
vector05:  fdb    uvector05  ; /* vector 05 */
vector04:  fdb    Breakpoint ; /* vector 04 */
vector03:  fdb    uvector03  ; /* vector 03 */
vector02:  fdb    ColdStart  ; /* vector 02 */
vector01:  fdb    uvector01  ; /* vector 01 */
vector00:  fdb    ColdStart  ; /* Reset vector */

.nolist                      ;skip the symbol table

;*****************************************************************

