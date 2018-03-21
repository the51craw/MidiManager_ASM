;************************************************************************************************
; MIDI_mapper is a system for MIDI merge, channel assign and transpose.
; See file README.txt in the folder ..\..\PIC18F4550\MIDI_mapper.
; MIDI_mapper on PIC16F628 is a MIDI input for the system MIDI_mapper.
; MIDI_mapper on PIC16F628 is i communicating with a PIC18F4550.
; All bytes coming in on RX pin are communicated over a parallell bus to the PIC18F4550.
;
; VERY IMPORTANT NOTE!!!
; The MIDI-Mapper uses PIC16F628 OR PIC16F628A.
; Select corresponding header file before build/flash, and change the include statement!
; Also select the device under Configure -> Select Device...
;
;************************************************************************************************
;                           ___________
;                          |    '-'    |
;               D2 RA2 <---|1    P   10|---> RA1 D1
;               D3 RA3 <---|2    I   11|---> RA0 D0
;      Chip select RA4 --->|3    C   12|<--- RA7 Clock in 10 MHz
; MCLR, 10 kOhm to VCC --->|4    1   13|---> RA6 Data ready
;                  VSS ----|5    6   14|---- VCC
;                          |6    F   15|---> RB7 D7 
;            Rx    RB1 --->|7    6   16|---> RB6 D6
;                          |8    2   17|---> RB5 D5 
;                          |9    8   18|---> RB4 D4
;                          |___________|
;                                    
;************************************************************************************************
; Configuration bits when set from code:
; 2007 = 0x3FB8
;************************************************************************************************
; Use "P16F628.INC" for pre-declared constants:
	include "P16F628.INC"

; Set configuration bits:
	__config _EXTCLK_OSC & _WDT_OFF & _PWRTE_ON & _MCLRE_ON & _BODEN_OFF & _LVP_OFF & _DATA_CP_OFF & _CP_OFF
	;__config _INTRC_OSC_NOCLKOUT & _WDT_OFF & _PWRTE_ON & _MCLRE_ON & _BODEN_OFF & _LVP_OFF & _DATA_CP_OFF & _CP_OFF
	;_INTRC_OSC_NOCLKOUT  EQU  H'3FFC'    ; INTRC oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN
	;_WDT_OFF             EQU  H'3FFB'    ; WatchDogTimer disabled
	;_PWRTE_ON            EQU  H'3FF7'    ; Poweron Timer enabled
	;_MCLRE_ON            EQU  H'3FDF'    ; RA5/MCLR pin function is MCLR
	;_MCLRE_OFF           EQU  H'3FDF'    ; RA5/MCLR pin function is digital input, MCLR internally tied to VDD
	;_BODEN_OFF           EQU  H'3FBF'    ; Brown Out Reset disabled
	;_LVP_OFF             EQU  H'3F7F'    ; RB4/PGM pin has digital I/O function, HV on MCLR must be used for programming
	;DATA_CP_OFF          EQU  H'3FFF'    ; Data memory code protection off
	;_CP_OFF              EQU  H'3FFF'    ; Program memory code protection off

;************************************************************************************************
; Use "HBE_Macros.inc" for my macros:
	include "HBE_Macros.inc"

;************************************************************************************************

;************************************************************************************************
; RAM usage:
; PAGE0 0x20 - 0x6f Variables
;************************************************************************************************

InData                EQU 0x21
OutData               EQU 0x22
InPtr				  EQU 0x23
OutPtr				  EQU 0x24
CircBuffStart		  EQU 0x25
CircBuffEnd			  EQU 0x7f

;************************************************************************************************
; Set up the code section. Reset vector at 0x0000 to jump to start, static data starts at 0x0005:
.section code
    ORG 0x0000
    GOTO start ; Skip vectors

;************************************************************************************************
; Main start point:
    ORG 0x0005
	;BANK0
	;movlw 0x07                 ; Turn off comparators
	;movwf CMCON
	;clrf  PORTA
start
	;BANK1
	;bcf   TRISA, 1
	;BANK0
	;bsf   PORTA, 1
	;goto start
; Initiate ports:
	BANK0
	movlw 0x07                 ; Turn off comparators
	movwf CMCON
	clrf  PORTA
	clrf  PORTB
    BANK1                      ; TrisA/B are in bank 1.
    MOVLW 0xbf 
    MOVWF TRISA                ; Set port A bit 0111 1111. (0 = ouptut, 1 = high imp, RA7 = data ready, the rest are inputs).
    movlw 0xff
    MOVWF TRISB                ; Set port B bit 1111 1111. (RB1 = Rx
    						   ; are inputs not to disturb the parallell bus.)

; Disable interrupts:
	BANK0
	clrf INTCON
	clrf PORTB
	BANK1
	clrf  PIE1

; Setup comm for 31250 baud:
	SET_HIGH_SPEED
	SET_BAUD 0x13                ; Exact MIDI speed on 10 MHz clock 0x07 = 7, 10/4 * 7 = 17,5 = 0x11
	SET_BAUD 0x13                                                              
	SET_ASYNCHRONUS_MODE
	ENABLE_SERIAL_PORT
	;TX_ENABLE
	RX_ENABLE
	SET_TX_8_BITS
	
; Wait for master controller to send a pulse on PORTA4, while reading RCREG to avoid overrun hangup:
	;BANK1
	;bcf   TRISA, 1
	BANK0
Wait1:
	IF_RX_READY
	movf  RCREG, w
	btfss PORTA, 4
	goto Wait1
	;bsf   PORTA, 1;
Wait2:
	IF_RX_READY
	movf  RCREG, w
	btfsc PORTA, 4
	goto Wait2
	;bcf   PORTA, 1;

	BANK0
	bcf PORTA, 6
	movlw 0x25
	movwf InPtr
	movwf OutPtr

;************************************************************************************************
; Main loop
Main
    call   RxIn
    movf   InPtr, w
    subwf  OutPtr, w
    btfsc  STATUS, 2			; Skip back if InPtr = OutPtr, continue if InPtr <> OutPtr
	goto   Main
	call   ParallellOut
	goto   Main
    
;************************************************************************************************
; Receive data, in-bound traffic
RxIn
	IF_RX_NOT_READY
	return
	BANK0
	btfsc  RCSTA, FERR			; Check for framing error
	goto   Ferr
	btfsc  RCSTA, OERR			; Check for overrun error
	goto   Oerr
	movf   RCREG, w             ; Get the received data
	movwf  InData
	andlw  0xf8					; Is this a System Real Time Messages?
	sublw  0xf8
	btfsc  STATUS, 2
	return						; Do not send System Real Time Messages
	subwf  InData, w
    ;call   ParallellOut
    call   PutInBuffer
	return

; Handle framing error by reading next byte:
Ferr:
	movf   RCREG, w             ; Get the received data
	bcf    PORTA, 6
	return
		
; Receiver needs to be reset due to overrun error:
Oerr:
	bcf    CMCON, CREN
	bsf    CMCON, CREN
	movf   RCREG, w             ; Get the received data
	bcf    PORTA, 6
	return

;************************************************************************************************
; Put in buffer for later transmit via parallel out
PutInBuffer:
    movf   InPtr, w		; Use indirect access to buffer between 0x25 and 0x7f
    movwf  FSR
    movf   InData, w
    movwf  INDF
	incf   InPtr, f
	btfss  InPtr, 7		; Reset pointer to 0x25 if bit 7 is set ( > 0x7f)
	return
	movlw  0x25
	movwf  InPtr
	return	

;************************************************************************************************
; Pull from buffer to transmit via parallel out
PullFromBuffer:
    movf   OutPtr, w	; Use indirect access to buffer between 0x25 and 0x7f
    movwf  FSR
    movf   INDF, w
    movwf  OutData
	incf   OutPtr, f 
	btfss  OutPtr, 7	; Reset pointer to 0x25 if bit 7 is set ( > 0x7f)
	return
	movlw  0x25
	movwf  OutPtr
	return

;************************************************************************************************
; ParallellOut, wait for PIC18F4550 to pic up data
ParallellOut:
	call   PullFromBuffer
	bsf    PORTA, 6				; Flag to PIC18F4550 that we have data to put on parallell bus.
WaitForDSR:
	call   RxIn
	btfss  PORTA, 4				; Check the chip enable input pin
	goto   WaitForDSR			; We will be stuck here until PIC18F4550 asks for the data
	movf   OutData, w
	movwf  PORTA				; Only low nibble will be used
	movwf  PORTB				; Only high nibble will be used
	call   EnableBus
	bcf    PORTA, 6				; Flag that data is available on bus
	bsf    PORTA, 4
WaitForDone:
	call   RxIn
	btfsc  PORTA, 4				; Check the chip enable input pin
	goto   WaitForDone			; Wait for PIC18F4550 to flag that data has been fetched
	call   DisableBus
	return

;************************************************************************************************
; Enable parallell bus
EnableBus
	BANK1
	movlw  0xb0
	movwf  TRISA
	movlw  0x0f
	movwf  TRISB
	BANK0
	return
    
;************************************************************************************************
; Disable parallell bus
DisableBus
	BANK1
	movlw  0xbf
	movwf  TRISA
	movlw  0xff
	movwf  TRISB
	BANK0
	return
    
;************************************************************************************************
; Debug stopper, go here to stop all as a test
stop:
	BANK1
	clrf TRISA
	BANK0
	movlw 0xff
	movwf PORTA
	goto stop

;************************************************************************************************
; Last!
    END

