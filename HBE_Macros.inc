;************************************************************************************************
;************************************************************************************************
; Common operations
;************************************************************************************************
;************************************************************************************************

LOAD macro file, literal
	movlw literal
	movwf file
	endm

MOVE macro fileTo, fileFrom
	movf  fileFrom, 0
	movwf fileTo
	endm

AND macro file, literal
	movlw literal
	andwf file, F
	endm

OR  macro file, literal
	movlw literal
	iorwf file, F
	endm

; Set/reset port bit(s) marked with '1' in bitmask. Changes value in W!
PORTA_BITS_ON MACRO bitmask
	MOVLW  bitmask
;	IORWF PORTA, 1
	IORWF PortAData, 1
	MOVF  PortAData, 0
	MOVWF PORTA
	ENDM

PORTA_BITS_OFF MACRO bitmask 
	MOVLW bitmask
	XORLW 0xFF
;	ANDWF PORTA, 1
	ANDWF PortAData, 1
	MOVF  PortAData, 0
	MOVWF PORTA
	ENDM

PORTB_BITS_ON MACRO bitmask
	MOVLW  bitmask
;	IORWF PORTB, 1
	IORWF PortBData, 1
	MOVF  PortBData, 0
	MOVWF PORTB
	ENDM

PORTB_BITS_OFF MACRO bitmask
	MOVLW bitmask
	XORLW 0xFF
;	ANDWF PORTB, 1
	ANDWF PortBData, 1
	MOVF  PortBData, 0
	MOVWF PORTB
	ENDM

; compare = r1 - r2
; r1 = r2 -> C = 1, Z = 1
; r1 > r2 -> C = 0, Z = 0
; r1 < r2 -> C = 1, Z = 1
CMP MACRO r1, r2
	MOVF  r1, 0
	SUBWF r2, 0
	ENDM

; Compare a file with a literal
; C = 0 -> f > w
; C = 1 -> f = w or f < w
; Z = 1 -> f = w
CMPFL macro file, literal
	movlw literal
	subwf file, 0
	endm

IF_EQUAL macro
	btfsc STATUS, Z
	endm

IF_NOT_EQUAL macro
	btfss STATUS, Z
	endm

IF_GREATER_OR_EQUAL MACRO
	BTFSC STATUS, C
	ENDM

IF_LESS MACRO
	BTFSS STATUS, C
	ENDM

IF_NOT_GREATER MACRO
	BTFSS STATUS, C
	ENDM

; Conditional skip.
SKIP_IF_CARRY MACRO
	BTFSS STATUS, C
	ENDM

SKIP_IF_GREATER MACRO
	BTFSS STATUS, C
	ENDM

SKIP_IF_NOT_CARRY MACRO
	BTFSC STATUS, C
	ENDM

SKIP_IF_NOT_GREATER MACRO
	BTFSC STATUS, C
	ENDM

SKIP_IF_ZERO MACRO
	BTFSS STATUS, Z
	ENDM

SKIP_IF_EQUAL MACRO
	BTFSS STATUS, Z
	ENDM

SKIP_IF_NOT_ZERO MACRO
	BTFSC STATUS, Z
	ENDM

SKIP_IF_NOT_EQUAL MACRO
	BTFSC STATUS, Z
	ENDM

;************************************************************************************************
;************************************************************************************************
; Register bit manipulations
;************************************************************************************************
;************************************************************************************************

;************************************************************************************************
; STATUS Bits 
; Accessible in all pages: no need to select page.
;************************************************************************************************

; Set page macros

BANK_0_1 MACRO     ;Indirect addressing access to page 0 and 1
	bcf STATUS, IRP
 	ENDM

BANK_2_3 MACRO     ;Indirect addressing access to page 2 and 3 
	bsf STATUS, IRP
 	ENDM

BANK0 MACRO        ;Direct addressing access to page 0 
;	BANK_0_1
	bcf STATUS, RP0
	bcf STATUS, RP1
 	ENDM

BANK1 MACRO        ;Direct addressing access to page 1  
;	BANK_0_1
	bsf STATUS, RP0
	bcf STATUS, RP1
 	ENDM

BANK2 MACRO        ;Direct addressing access to page 2
	BANK_2_3 
	bcf STATUS, RP0
	bsf STATUS, RP1
 	ENDM

BANK3 MACRO        ;Direct addressing access to page 3 
	BANK_2_3
	bsf STATUS, RP0
	bsf STATUS, RP1
 	ENDM

;Wake-up macros
;NOT_TO = 1 -> After power-up, CLRWDT instruction, or SLEEP instruction
;NOT_TO = 0 -> A WDT timeout occurred

IF_WATCHDOG_TIMEOUT MACRO
  BTFSS STATUS, NOT_TO
 	ENDM

IF_NOT_WATCHDOG_TIMEOUT MACRO
  BTFSC STATUS, NOT_TO
 	ENDM

;PD: Power-down bit
;PD = 1 -> After power-up or by the CLRWDT instruction
;PD = 0 -> By execution of the SLEEP instruction

IF_SLEEP MACRO
  BTFSS STATUS, NOT_PD
  ENDM

IF_NOT_SLEEP MACRO
  BTFSC STATUS, NOT_PD
  ENDM

;Z: Zero bit
;1 = The result of an arithmetic or logic operation is zero
;0 = The result of an arithmetic or logic operation is not zero

IF_ZERO MACRO
  BTFSC STATUS, Z
  ENDM

IF_NOT_ZERO MACRO
  BTFSS STATUS, Z
  ENDM

;DC: Digit carry/borrow bit (ADDWF, ADDLW,SUBLW,SUBWF instructions) (for borrow the polarity
;is reversed)
;1 = A carry-out from the 4th low order bit of the result occurred
;0 = No carry-out from the 4th low order bit of the result

IF_DIGIT_CARRY MACRO
  BTFSS STATUS, DC
  ENDM
  
IF_NOT_DIGIT_CARRY MACRO
  BTFSC STATUS, DC
  ENDM
  
IF_DIGIT_BORROW MACRO
  BTFSS STATUS, DC
  ENDM
  
IF_NOT_DIGIT_BORROW MACRO
  BTFSC STATUS, DC
  ENDM

;C: Carry/borrow bit (ADDWF, ADDLW,SUBLW,SUBWF instructions)
;1 = A carry-out from the Most Significant bit of the result occurred
;0 = No carry-out from the Most Significant bit of the result occurred

IF_CARRY MACRO
  BTFSS STATUS, C
  ENDM
  
IF_NOT_CARRY MACRO
  BTFSC STATUS, C
  ENDM
  
IF_BORROW MACRO
  BTFSS STATUS, C
  ENDM
  
IF_NOT_BORROW MACRO
  BTFSC STATUS, C
  ENDM

CLEAR_CARRY MACRO
  BCF STATUS, C
  ENDM

;************************************************************************************************
; INTCON Interrupt control bits 
; Accessible in all pages: no need to select page. 
;************************************************************************************************

; Global interrupt

GLOBAL_INTERRUPT_ENABLE MACRO
	bsf INTCON, GIE
 	ENDM

GLOBAL_INTERRUPT_DISABLE MACRO
	bcf INTCON, GIE
 	ENDM

;Periferal interrupt
PEIE                         EQU     H'0006'
PERIFERAL_INTERRUPT_ENABLE MACRO
	bsf INTCON, PEIE
 	ENDM
 	
PERIFERAL_INTERRUPT_DISABLE MACRO
	bcf INTCON, PEIE
 	ENDM

;T0IE                         EQU     H'0005'
ENABLE_TMR0_INTERRUPTS MACRO
	bsf INTCON, T0IE
 	ENDM

DISABLE_TMR0_INTERRUPTS MACRO
	bcf INTCON, T0IE
 	ENDM

;INTE                         EQU     H'0004'
;RBIE                         EQU     H'0003'
;T0IF                         EQU     H'0002'
CLEAR_TMR0_INTERRUPT_FLAG MACRO
	bcf INTCON, T0IF
 	ENDM

;INTF                         EQU     H'0001'
;RBIF                         EQU     H'0000'
;
;************************************************************************************************
; PIR1 Peripheral Interrupt Register 1 
; Accessible in page0 only.
;************************************************************************************************
;
;EEIF                         EQU     H'0007'
;CMIF                         EQU     H'0006'
;RCIF                         EQU     H'0005'
;TXIF                         EQU     H'0004'
;CCP1IF                       EQU     H'0002'
;TMR2IF                       EQU     H'0001'
;TMR1IF                       EQU     H'0000'
;
;************************************************************************************************
; T1CON Bits 
; Accessible in page0 only.
;************************************************************************************************
;T1CKPS1                      EQU     H'0005'
;T1CKPS0                      EQU     H'0004'
;T1OSCEN                      EQU     H'0003'
;NOT_T1SYNC                   EQU     H'0002'
;TMR1CS                       EQU     H'0001'
;TMR1ON                       EQU     H'0000'
;
;************************************************************************************************
; T2CON Bits 
; Accessible in page0 only.
;************************************************************************************************
;TOUTPS3                      EQU     H'0006'
;TOUTPS2                      EQU     H'0005'
;TOUTPS1                      EQU     H'0004'
;TOUTPS0                      EQU     H'0003'
;TMR2ON                       EQU     H'0002'
;T2CKPS1                      EQU     H'0001'
;T2CKPS0                      EQU     H'0000'
;
;************************************************************************************************
; CCP1CON Bits 
; Accessible in page0 only.
;************************************************************************************************
;CCP1X                        EQU     H'0005'
;CCP1Y                        EQU     H'0004'
;CCP1M3                       EQU     H'0003'
;CCP1M2                       EQU     H'0002'
;CCP1M1                       EQU     H'0001'
;CCP1M0                       EQU     H'0000'
;
;************************************************************************************************
; RCSTA: RECEIVE STATUS AND CONTROL REGISTER 
; Accessible in page0 only.
;************************************************************************************************
;SPEN                         EQU     H'0007'
;RX9                          EQU     H'0006'
;SREN                         EQU     H'0005'
;CREN                         EQU     H'0004'
;ADEN                         EQU     H'0003'
;FERR                         EQU     H'0002'
;OERR                         EQU     H'0001'
;RX9D                         EQU     H'0000'

;bit 7 SPEN: Serial Port Enable bit
;(Configures RB1/RX/DT and RB2/TX/CK pins as serial port pins when bits TRISB<2:17> are set)
;1 = Serial port enabled
;0 = Serial port disabled
ENABLE_SERIAL_PORT MACRO
	bcf STATUS, RP0
	bcf STATUS, RP1
	bsf RCSTA, SPEN 
 	ENDM
DISABLE_SERIAL_PORT MACRO
	bcf STATUS, RP0
	bcf STATUS, RP1
	bcf RCSTA, SPEN
 	ENDM

;bit 6 RX9: 9-bit Receive Enable bit
;1 = Selects 9-bit reception
;0 = Selects 8-bit reception
ENABLE_RX_BIT_9 MACRO
	bcf STATUS, RP0
	bcf STATUS, RP1
	bsf RCSTA, RX9
 	ENDM
DISABLE_RX_BIT_9 MACRO
	bcf STATUS, RP0
	bcf STATUS, RP1
	bcf RCSTA, RX9
 	ENDM

;bit 5 SREN: Single Receive Enable bit
;Asynchronous mode:
;Don’t care
;Synchronous mode - master:
;1 = Enables single receive
;0 = Disables single receive
;This bit is cleared after reception is complete.
;Synchronous mode - slave:
;Unused in this mode
ENABLE_SINGLE_RX MACRO
	bcf STATUS, RP0
	bcf STATUS, RP1
	bsf RCSTA, SREN
 	ENDM
DISABLE_SINGLE_RX MACRO
	bcf STATUS, RP0
	bcf STATUS, RP1
	bcf RCSTA, SREN
 	ENDM

;bit 4 CREN: Continuous Receive Enable bit
;Asynchronous mode:
;1 = Enables continuous receive
;0 = Disables continuous receive
;Synchronous mode:
;1 = Enables continuous receive until enable bit CREN is cleared (CREN overrides SREN)
;0 = Disables continuous receive
ENABLE_CONTINUOUS_RX MACRO
	bcf STATUS, RP0
	bcf STATUS, RP1
	bsf RCSTA, CREN
 	ENDM
DISABLE_CONTINUOUS_RX MACRO
	bcf STATUS, RP0
	bcf STATUS, RP1
	bcf RCSTA, CREN
 	ENDM
;***###***
;;bit 3 ADEN: Address Detect Enable bit
;;Asynchronous mode 9-bit (RX9 = 1):
;;1 = Enables address detection, enable interrupt and load of the receive buffer when RSR<8> is set
;;0 = Disables address detection, all bytes are received, and ninth bit can be used as PARITY bit
;;Asynchronous mode 8-bit (RX9=0):
;;Unused in this mode
;;Synchronous mode
;;Unused in this mode
;SET_ MACRO
;	bcf STATUS, RP0
;	bcf STATUS, RP1
;	bsf RCSTA, 
; 	ENDM
;SET_ MACRO
;	bcf STATUS, RP0
;	bcf STATUS, RP1
;	bcf RCSTA, 
; 	ENDM
;
;;bit 2 FERR: Framing Error bit
;;1 = Framing error (Can be updated by reading RCREG register and receive next valid byte)
;;0 = No framing error
;SET_ MACRO
;	bcf STATUS, RP0
;	bcf STATUS, RP1
;	bsf RCSTA, 
; 	ENDM
;SET_ MACRO
;	bcf STATUS, RP0
;	bcf STATUS, RP1
;	bcf RCSTA, 
; 	ENDM
;
;;bit 1 OERR: Overrun Error bit
;;1 = Overrun error (Can be cleared by clearing bit CREN)
;;0 = No overrun error
;SET_ MACRO
;	bcf STATUS, RP0
;	bcf STATUS, RP1
;	bsf RCSTA, 
; 	ENDM
;SET_ MACRO
;	bcf STATUS, RP0
;	bcf STATUS, RP1
;	bcf RCSTA, 
; 	ENDM
;
;;bit 0 RX9D: 9th bit of received data (Can be PARITY bit);
;SET_ MACRO
;	bcf STATUS, RP0
;	bcf STATUS, RP1
;	bsf RCSTA, 
; 	ENDM
;SET_ MACRO
;	bcf STATUS, RP0
;	bcf STATUS, RP1
;	bcf RCSTA, 
; 	ENDM

;************************************************************************************************
; CMCON Bits 
; Accessible in page0 only.
;************************************************************************************************
;
;C2OUT                        EQU     H'0007'
;C1OUT                        EQU     H'0006'
;C2INV                        EQU     H'0005'
;C1INV                        EQU     H'0004'
;CIS                          EQU     H'0003'
;CM2                          EQU     H'0002'
;CM1                          EQU     H'0001'
;CM0                          EQU     H'0000'
;
;************************************************************************************************
; OPTION Bits 
; Accessible in page1 and page3 only.
;************************************************************************************************
; Since OPTION is in Bank1, always select bank1
;bit 7 NOT_RBPU: PORTB Pull-up Enable bit
;1 = PORTB pull-ups are disabled
;0 = PORTB pull-ups are enabled by individual port latch values

;bit 6 INTEDG: Interrupt Edge Select bit
;1 = Interrupt on rising edge of RB0/INT pin
;0 = Interrupt on falling edge of RB0/INT pin

;bit 5 T0CS: TMR0 Clock Source Select bit
;1 = Transition on RA4/T0CKI pin
;0 = Internal instruction cycle clock (CLKOUT)
SET_TMR0_TIMER_MODE MACRO        ;Clear counter mode for timer0
	BANK1
	bsf STATUS, RP0
	bcf STATUS, RP1
	bcf OPTION_REG, T0CS
 	ENDM
SET_TMR0_COUNTER_MODE MACRO      ;Set counter mode for timer0
	BANK1	
	bsf STATUS, RP0
	bcf STATUS, RP1
	bsf OPTION_REG, T0CS
 	ENDM

;bit 4 T0SE: TMR0 Source Edge Select bit
;1 = Increment on high-to-low transition on RA4/T0CKI pin
;0 = Increment on low-to-high transition on RA4/T0CKI pin

;bit 3 PSA: Prescaler Assignment bit
;1 = Prescaler is assigned to the WDT
;0 = Prescaler is assigned to the Timer0 module
SET_PRESCALER_TO_TMR0 MACRO      ;Set prescaler for use with timer0
	BANK1
	bsf STATUS, RP0
	bcf STATUS, RP1
	bcf OPTION_REG, PSA
 	ENDM
SET_PRESCALER_TO_WD MACRO      ;Set prescaler for use with watchdog
	BANK1
	bsf STATUS, RP0
	bcf STATUS, RP1
	bsf OPTION_REG, PSA
 	ENDM

;bit 2-0 PS2:PS0: Prescaler Rate Select bits
;Legend:
;R = Readable bit W = Writable bit U = Unimplemented bit, read as ‘0’
;-n = Value at POR ’1’ = Bit is set ’0’ = Bit is cleared x = Bit is unknown
;Val TMR0     WD
;000 1:2      1:1
;001 1:4      1:2  
;010 1:8      1:4  
;011 1:16     1:8  
;100 1:32     1:16 
;101 1:64     1:32 
;110 1:128    1:64 
;111 1:256    1:128
SET_PRESCALER MACRO val
	BANK1
	bsf STATUS, RP0
	bcf STATUS, RP1
	bcf OPTION_REG, 0
	bcf OPTION_REG, 1
	bcf OPTION_REG, 2
	movlw val
	andlw 0x07
	addwf OPTION_REG, f
	ENDM

;************************************************************************************************
; PIE1 Bits 
; Accessible in page1 only.
;************************************************************************************************
;
;EEIE                         EQU     H'0007'
;CMIE                         EQU     H'0006'
;RCIE                         EQU     H'0005'
;TXIE                         EQU     H'0004'
;CCP1IE                       EQU     H'0002'
;TMR2IE                       EQU     H'0001'
;TMR1IE                       EQU     H'0000'
;
;************************************************************************************************
; PCON Bits 
; Accessible in page1 only.
;************************************************************************************************
;
;OSCF                         EQU     H'0003'
;NOT_POR                      EQU     H'0001'
;NOT_BO                       EQU     H'0000'
;NOT_BOR                      EQU     H'0000'
;NOT_BOD                      EQU     H'0000' ;Backwards compatability to 16F62X
;
;************************************************************************************************
; TXSTA: TRANSMIT STATUS AND CONTROL REGISTER
; Accessible in page1 only.
;************************************************************************************************
;CSRC                         EQU     H'0007'
SET_CLOCK_MODE_MASTER MACRO   ;Set clock mode to master mode (only valid for synchronus mode).  
	bsf STATUS, RP0
	bcf STATUS, RP1
	bsf TXSTA, CSRC
 	ENDM
SET_CLOCK_MODE_SLAVE MACRO    ;Set clock mode to slave mode (only valid for synchronus mode).
	bsf STATUS, RP0
	bcf STATUS, RP1
	bcf TXSTA, CSRC
 	ENDM

;TX9                          EQU     H'0006'
SET_TX_9_BITS MACRO           ;Data format is 9 data bits
	bsf STATUS, RP0
	bcf STATUS, RP1
	bsf TXSTA, TX9
 	ENDM
SET_TX_8_BITS MACRO           ;Data format is 8 data bits
	bsf STATUS, RP0
	bcf STATUS, RP1
	bcf TXSTA, TX9
 	ENDM

;TXEN                         EQU     H'0005'
TX_ENABLE MACRO               ;Enable transmission
	bsf STATUS, RP0
	bcf STATUS, RP1
    bsf TRISB, 2
    bsf TRISB, 3
	bsf TXSTA, TXEN
 	ENDM
TX_DISABLE MACRO              ;Disable transmission
	bsf STATUS, RP0
	bcf STATUS, RP1
	bcf TXSTA, TXEN
 	ENDM

RX_ENABLE macro
    BANK1
    bsf TRISB, 2
    bsf TRISB, 3
	BANK0
	bsf  RCSTA, CREN
	endm

RX_DISABLE macro
	BANK0
	bcf  RCSTA, CREN
	endm

;SYNC                         EQU     H'0004'
SET_SYNCHRONUS_MODE MACRO     ;Set synchronus mode
	BANK1
	bsf TXSTA, SYNC
 	ENDM

SET_ASYNCHRONUS_MODE MACRO    ;Set asynchronus mode
	BANK1
	bcf TXSTA, SYNC
 	ENDM

;BRGH                         EQU     H'0002'
SET_HIGH_SPEED MACRO          ;Set high baudrate (only valid in asynchronus mode)
	BANK1
	bsf TXSTA, BRGH
 	ENDM
SET_LOW_SPEED MACRO           ;Set low baudrate (only valid in asynchronus mode)
	BANK1
	bcf TXSTA, BRGH
 	ENDM

IF_TX_READY MACRO             ;Transmit shift register is empty
	BANK1
	BTFSC TXSTA, TRMT           ;(Skip if not ready: skip if cleared. Do next opcode if set.)
 	ENDM

IF_TX_NOT_READY MACRO         ;Transmit shift register is full
	BANK1
	BTFSS TXSTA, TRMT
 	ENDM

IF_RX_READY MACRO             ;Transmit shift register is empty
	BANK0
	BTFSC PIR1, RCIF           ;(Skip if not ready: skip if cleared. Do next opcode if set.)
 	ENDM

IF_RX_NOT_READY MACRO         ;Transmit shift register is full
	BANK0
	BTFSS PIR1, RCIF
 	ENDM

;TX9D                         EQU     H'0000'
SET_BIT_9 MACRO               ;Set bit 9 to a '1'
	BANK1
	bsf TXSTA, TX9D
 	ENDM

RESET_BIT_9 MACRO             ;Set bit 9 to a '0'
	BANK1
	bcf TXSTA, TX9D
 	ENDM

;************************************************************************************************
; SPBRG: Baudrate generation register.
; Accessible in page1 only.
; High speed, BRGH in TXSTA = 1 (use macro SET_HIGH_SPEED)
; X = Fosc/(BaudRate * 16) - 1
; Low speed, BRGH in TXSTA = 0 (use macro SET_LOW_SPEED)
; X = Fosc/(BaudRate * 64) - 1
;************************************************************************************************
SET_BAUD MACRO Value
    MOVLW Value
	BANK1
    MOVWF SPBRG
    ENDM

;************************************************************************************************
; EECON1 Bits 
; Accessible in page1 only.
;************************************************************************************************
;WRERR                        EQU     H'0003'
;WREN                         EQU     H'0002'
;WR                           EQU     H'0001'
;RD                           EQU     H'0000'
;
;************************************************************************************************
; VRCON Bits 
; Accessible in page1 only.
;************************************************************************************************
;
;VREN                         EQU     H'0007'
;VROE                         EQU     H'0006'
;VRR                          EQU     H'0005'
;VR3                          EQU     H'0003'
;VR2                          EQU     H'0002'
;VR1                          EQU     H'0001'
;VR0                          EQU     H'0000'

;************************************************************************************************
;************************************************************************************************
; Other macros
;************************************************************************************************
;************************************************************************************************

;************************************************************************************************
; USART macros
;************************************************************************************************
; Steps to follow when setting up an Asynchronous Transmission:
;
; TXSTA is in BANK1
; RCSTA is in BANK0
; SPBRG is in BANK1
; TXREG is in BANK0
; RCREG is in BANK0
;
; 1. Port settings:
; TRISB<1> and TRISB<2> should both be set to ‘1’ to configure the RB1/RX/DT and RB2/TX/CK
; pins as inputs. Output drive, when required, is controlled by the peripheral circuitry. <- NOTE!
;    BANK1
;    bsf TRISB, 1
;    bsf TRISB, 2
; 2. Initialize the SPBRG register for the appropriate
;   baud rate. If a high speed baud rate is desired,
;   set bit BRGH. (Section 12.1) Macro: SET_HIGH_SPEED, else SET_LOW_SPEED
;
; 3. Enable the asynchronous serial port by clearing
;   bit SYNC of TXSTA and setting bit SPEN of RCSTA (yes, even for transmission!).
;
; 4. If interrupts are desired, then set enable bit TXIE.
;
; 5. If 9-bit transmission is desired, then set transmit
;   bit TX9.
;
; 6. Enable the transmission by setting bit TXEN of TXSTA,
;   which will also set bit TXIF.
;
; 7. If 9-bit transmission is selected, the ninth bit
;   should be loaded in bit TX9D.
;
; 8. Check transmit progress before sending, and skip out if not ready,
;	IF_RX_NOT_READY
;	return
;
; 9. Load data to the TXREG register (starts transmission). 
