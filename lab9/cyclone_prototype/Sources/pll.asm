; filename ******** pll.asm ************** 
;    Jonathan W. Valvano 8/3/09
 
;********COMPUTER TWO*********

;***********PLL routines*********************
SYNR     equ $0034  ; CRG Synthesizer Register
REFDV    equ $0035  ; CRG Reference Divider Register
CRGFLG   equ $0037  ; CRG Flags Register
CLKSEL   equ $0039  ; CRG Clock Select Register
PLLCTL   equ $003A  ; CRG PLL Control Register

   absentry PLL_Init
;********* PLL_Init ****************
; Active PLL so the 9S12 runs at 24 MHz
; Inputs: none
; Outputs: none
; Errors: will hang if PLL does not stabilize 
; WARNING: you can not single step PLL_Init in Metrowerks
PLL_Init  
      movb #$02,SYNR       ; 9S12C32 OSCCLK is 8 MHz, 9S12DP512 OSCCLK is 16 MHz
      movb #1,REFDV        ; (REFDV=0 if 9S12C32 with 8 MHz crystal)
      movb #0,CLKSEL       ; PLLCLK = 2 * OSCCLK * (SYNR + 1) / (REFDV + 1)
      movb #$D1,PLLCTL     ; Clock monitor, PLL On, high bandwidth filter 
      brclr CRGFLG,#$08,*  ; wait for PLLCLK to stabilize.
      bset CLKSEL,#$80     ; Switch to PLL clock
      rts
      