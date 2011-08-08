;HCS12
;****************************************************************************************
;* Title: HCS12_Serial_Monitor.inc                            copyright (c) Motorola 2003
;****************************************************************************************
;* Author:  Rat579
;*
;* Description:  register and bit name definitions for HCS12_Serial_Monitor.asm
;*
;* Documentation:
;*
;* Include files:  none
;*
;* Assembler:
;*
;* Revision History:
;* Rev #      Date       Who     Comments
;* ------  -----------   ------  -----------------------------------------------------
;* 1.0      08-Aug-03    JPW     initial release
;*
;****************************************************************************************
;****************************************************************************************
;*
;*   Motorola reserves the right to make changes without further notice to any product
;*   herein to improve reliability, function, or design.  Motorola does not assume any
;*   liability arising out of the application or use of any product, circuit, or software
;*   described herein; neither does it convey any license under its patent rights nor the
;*   rights of others.  Motorola products are not designed, intended, or authorized for
;*   use as components in systems intended for surgical implant into the body, or other
;*   applications intended to support life, or for any other application in which the
;*   failure of the Motorola product could create a situation where personal injury or
;*   death may occur.  Should buyer purchase or use Motorola products for any such
;*   intended or unauthorized application, buyer shall indemnify and hold Motorola and
;*   its officers, employees, subsidiaries, affiliates, and distributors harmless against
;*   all claims, costs, damages, and expenses, and reasonable attorney fees arising out
;*   of, directly or indirectly, any claim of personal injury or death associated with
;*   such unintended or unauthorized use, even if such claim alleges that motorola was
;*   negligent regarding the design or manufacture of the part.
;*
;*   Motorola and the Motorola logo are registered trademarks of Motorola Ltd.
;****************************************************************************************

CORE1:       equ    $0000         ;ports A, B, E, modes, inits, test
DEVID:       equ    $001A         ;device ID register
CORE2:       equ    $001C         ;memsiz, IRQ, HPRIO
CORE3:       equ    $0020         ;debug register
CORE4:       equ    $0030         ;PPAGE, port K
CRGV4:       equ    $0034         ;clock and reset generator
SCI0V3:      equ    $00C8         ;serial communications interface SCI0
FTS:         equ    $0100         ;Flash control registers
EE			 equ	$0110		  ;EEPROM control registers
PIM:         equ    $0240         ;port integration module

;****  Input/Output (I/O) Ports  *******************************************************
;*
;****  Input/Output (I/O) Ports  *******************************************************
;*
PORTA:       equ    CORE1+$00     ;I/O port A 8-bit data register
;
PTA7:        equ    %10000000     ;port A bit 7
PTA6:        equ    %01000000     ;port A bit 6
PTA5:        equ    %00100000     ;port A bit 5
PTA4:        equ    %00010000     ;port A bit 4
PTA3:        equ    %00001000     ;port A bit 3
PTA2:        equ    %00000100     ;port A bit 2
PTA1:        equ    %00000010     ;port A bit 1
PTA0:        equ    %00000001     ;port A bit 0

PORTB:       equ    CORE1+$01	    ;I/O port B 8-bit data register
;
PTB7:        equ    %10000000     ;port B bit 7
PTB6:        equ    %01000000     ;port B bit 6
PTB5:        equ    %00100000     ;port B bit 5
PTB4:        equ    %00010000     ;port B bit 4
PTB3:        equ    %00001000     ;port B bit 3
PTB2:        equ    %00000100     ;port B bit 2
PTB1:        equ    %00000010     ;port B bit 1
PTB0:        equ    %00000001     ;port B bit 0

DDRA:        equ    CORE1+$02     ;I/O port A 8-bit data direction register
;
DDRA7:       equ    %10000000     ;port A bit 7
DDRA6:       equ    %01000000     ;port A bit 6
DDRA5:       equ    %00100000     ;port A bit 5
DDRA4:       equ    %00010000     ;port A bit 4
DDRA3:       equ    %00001000     ;port A bit 3
DDRA2:       equ    %00000100     ;port A bit 2
DDRA1:       equ    %00000010     ;port A bit 1
DDRA0:       equ    %00000001     ;port A bit 0

DDRB:        equ    CORE1+$03	  ;I/O port B 8-bit data direction register
;
DDRB7: 	     equ    %10000000     ;port B bit 7
DDRB6:       equ    %01000000     ;port B bit 6
DDRB5:       equ    %00100000     ;port B bit 5
DDRB4:       equ    %00010000     ;port B bit 4
DDRB3:       equ    %00001000     ;port B bit 3
DDRB2:       equ    %00000100     ;port B bit 2
DDRB1:       equ    %00000010     ;port B bit 1
DDRB0:       equ    %00000001     ;port B bit 0

; $04 to $07 reserved

PORTE:       equ    CORE1+$08	  ;I/O port E 8-bit data register
;
PTE7:        equ    %10000000     ;port E bit 7
PTE6:        equ    %01000000     ;port E bit 6
PTE5:        equ    %00100000     ;port E bit 5
PTE4:        equ    %00010000     ;port E bit 4
PTE3:        equ    %00001000     ;port E bit 3
PTE2:        equ    %00000100     ;port E bit 2
PTE1:        equ    %00000010     ;port E bit 1
PTE0:        equ    %00000001     ;port E bit 0

DDRE:        equ    CORE1+$09	  ;I/O port E 8-bit data direction register
;
DDRE7:       equ    %10000000     ;port E bit 7
DDRE6:       equ    %01000000     ;port E bit 6
DDRE5:       equ    %00100000     ;port E bit 5
DDRE4:       equ    %00010000     ;port E bit 4
DDRE3:       equ    %00001000     ;port E bit 3
DDRE2:       equ    %00000100     ;port E bit 2
DDRE1:       equ    %00000010     ;port E bit 1
DDRE0:       equ    %00000001     ;port E bit 0

PUCR:        equ    CORE1+$0C     ;pullup control register
;
PUPKE:       equ    %10000000     ;pullup port K enable
PUPEE: 	     equ    %00010000     ;pullup port E enable
PUPBE:	     equ    %00000010     ;pullup port B enable
PUPAE:       equ    %00000001     ;pullup port A enable

INITRM:      equ    CORE1+$10     ;initialization of internal RAM position register
INITRG:      equ    CORE1+$11     ;initialization of internal registers position register
INITEE:      equ    CORE1+$12     ;initialization of internal EEPROM registers position register

;****  Device ID Registers (PARTID)  ************************************************
;*
PARTIDH:     equ    DEVID+$00     ;device ID high register
PARTIDL:     equ    DEVID+$01     ;device ID low register

;****  CORE2 (MEMSIZ, IRQ, HPRIO)  ************************************************
;*
MEMSIZ0:     equ    CORE2+$00     ;memory size register 0
eep_sw1:     equ    %00100000     ;allocated system EEPROM memory space
eep_sw0:     equ    %00010000     ;allocated system EEPROM memory space

;****  Debug Module    (DBG)  ***********************************************
;*
DBGC1:       equ    CORE3+$00     ;debug control register 1
DBGEN:       equ    %10000000     ;DBG mode enable bit
ARM:         equ    %01000000     ;arm bit

DBGSC:       equ    CORE3+$01     ;debug control and status register

DBGTBH:      equ    CORE3+$02     ;debug trace buffer high register

DBGTBL:      equ    CORE3+$03     ;debug trace buffer low register

DBGCNT:      equ    CORE3+$04     ;debug count register

DBGCCX:      equ    CORE3+$05     ;debug comparator C expansion register

DBGCCH:      equ    CORE3+$06     ;debug comparator C high register

DBGCCL:      equ    CORE3+$07     ;debug comparator C low register

DBGC2:       equ    CORE3+$08     ;debug control register 2

DBGC3:       equ    CORE3+$09     ;debug control register 3

DBGACX:      equ    CORE3+$0A     ;debug comparator A expansion register
; ??? Not pre lastest specification
DBGACH:      equ    CORE3+$0B     ;debug comparator A high register

DBGACL:      equ    CORE3+$0C     ;debug comparator A low register

DBGBCX:      equ    CORE3+$0D     ;debug comparator B expansion register

DBGBCH:      equ    CORE3+$0E     ;debug comparator B high register

DBGBCL:      equ    CORE3+$0F     ;debug comparator B low register

;****  CORE4 (PPAGE, Port K)  ************************************************
;*
PPAGE:       equ    CORE4+$00     ;program page index register
;
;****  Clock and Reset Generator (PLL, RTI, COP)  ************************************************
;*
SYNR:        equ    CRGV4+$00     ;CRG synthesizer register

REFDV:       equ    CRGV4+$01     ;CRG reference divider register

CRGFLG:      equ    CRGV4+$03     ;CRG flags register
LOCK:        equ    %00001000     ;lock status bit

CLKSEL:      equ    CRGV4+$05     ;CRG clock select register
PLLSEL:      equ    %10000000     ;PLL select bit

PLLCTL:      equ    CRGV4+$06     ;CRG PLL control register
PLLON:       equ    %01000000     ;phase lock loop on bit

COPCTL:      equ    CRGV4+$08     ;CRG COP control register
RSBCK:       equ    %01000000     ;COP and RTI stop in active BDM mode bit
CR2:         equ    %00000100     ;COP watchdog timer rate select
CR1:         equ    %00000010     ;COP watchdog timer rate select
CR0:         equ    %00000001     ;COP watchdog timer rate select

;****  Serial Communication Interface 0 (SCI0)  ****************************************
;*
SCI0BDH:     equ    SCI0V3+$00    ;SCI baud rate registers

SCI0BDL:     equ    SCI0V3+$01    ;SCI baud rate registers

SCI0CR1:     equ    SCI0V3+$02    ;SCI control register 1
WAKE:        equ    %00001000     ;wakeup condition bit
ILT:         equ    %00000100     ;idle line type bit
RSRC:		 equ    %00100000     ;rec source bit

SCI0CR2:     equ    SCI0V3+$03    ;SCI control register 2
RIE:         equ    %00100000     ;receiver full interrupt enable bit
SBK:         equ    %00000001     ;send break bit

SCI0SR1:     equ    SCI0V3+$04    ;SCI status register 1
TDRE:        equ    %10000000     ;transmit data register empty flag
RDRF:        equ    %00100000     ;receive data register full flag

SCI0DRL:     equ    SCI0V3+$07    ;SCI data register

;****  Flash Control Register  *********************************************************
;*
FCLKDIV:     equ    FTS+$00 ;flash clock divider register

FSEC:        equ    FTS+$01 ;flash security register

FCNFG:       equ    FTS+$03 ;flash configuration register
CBEIE:       equ    %10000000     ;command buffer empty interrupt enable
CCIE:        equ    %01000000     ;command complete interrupt enable

FSTAT:       equ    FTS+$05 ;flash status register
;
CBEIF:       equ    %10000000     ;command buffer empty interrupt flag
CCIF:        equ    %01000000     ;command complete interrupt flag
PVIOL:       equ    %00100000     ;protection violation
ACCERR:      equ    %00010000     ;flash access error

FCMD:        equ    FTS+$06 ;flash command register
;
CMDB6:       equ    %01000000     ;valid flash user mode commands
CMDB5:       equ    %00100000     ;valid flash user mode commands
CMDB2:       equ    %00000100     ;valid flash user mode commands
CMDB0:       equ    %00000001     ;valid flash user mode commands

MassErase:   equ     CMDB6+CMDB0
SecErase:    equ     CMDB6
ProgWord:    equ     CMDB5

;****  EEPROM Control Register  ********************************************************
;*
ECLKDIV:     equ    EE+$00  ;EEPROM Clock Divider Register

ECNFG:       equ    EE+$03  ;EEPROM configuration register
;
ESTAT:       equ    EE+$05  ;EEPROM status register
;
ECMD:        equ    EE+$06  ;EEPROM command buffer and register

;****  Port Integration Module (PIM)  *******************************************************
;*
PTS:         equ    PIM+$08 ;port S I/O register
PTS6:        equ    %01000000
PTS5:        equ    %00100000
PTS4:        equ    %00010000
PTS3:        equ    %00001000
PTS2:        equ    %00000100
PTS1:        equ    %00000010
PTS0:        equ    %00000001

PERS:        equ    PIM+$0C ;port S pull device enable register
PERS0:       equ    %00000001

PPSS:        equ    PIM+$0D ;port S polarity select register
PPSS0:       equ    %00000001

PTP:         equ    PIM+$18 ;port P I/O register
;
PTP7:        equ    %00100000
PTP6:        equ    %00010000
PTP5:        equ    %00100000
PTP4:        equ    %00010000
PTP3:        equ    %00001000
PTP2:        equ    %00000100
PTP1:        equ    %00000010
PTP0:        equ    %00000001

PTIP:        equ    PIM+$19 ;port P input register
;
DDRP:        equ    PIM+$1A ;port P data direction register
;
RDRP:        equ    PIM+$1B ;port P reduced drive register
;
PERP:        equ    PIM+$1C ;port P pull device enable register
;
PPSP:        equ    PIM+$1D ;port P polarity select register
;
PTQ:         equ    PIM+$20 ;port Q I/O register
;
PTQ7:        equ    %10000000
PTQ6:        equ    %01000000
PTQ5:        equ    %00100000
PTQ4:        equ    %00010000
PTQ3:        equ    %00001000
PTQ2:        equ    %00000100
PTQ1:        equ    %00000010
PTQ0:        equ    %00000001

PTIQ:        equ    PIM+$21 ;port Q input register
;
DDRQ:        equ    PIM+$22 ;port Q data direction register
;
RDRQ:        equ    PIM+$23 ;port Q reduced drive register
;
PERQ:        equ    PIM+$24 ;port Q pull device enable register
;
PPSQ:        equ    PIM+$25 ;port Q polarity select register
;



