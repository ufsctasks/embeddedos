.global _hardware_init, _adc_init, _adc_read
.global _lcd_port
.global _aquecendo_
.global _resfriando_, _temperatura_estavel
.global _ler_botao_aumento_temperatura
.global _ler_botao_decremento_temperatura

    
_hardware_init:
    ;	   FEDCBA9876543210
    MOV #0b0000000000000011, W0 ;TRISB
    MOV #0b0001111000000000, W1 ;TRISD
    MOV #0b0000000001110100, W2 ;TRISF
    MOV #0b0000000000000000, W3 ;LATB
    MOV #0b1110000111000000, W4 ;LATD
    MOV #0b0001000100000000, W5 ;LATF
    MOV W0, TRISB
    MOV W1, TRISD
    MOV W2, TRISF
    MOV W3, LATB
    MOV W4, LATD
    MOV W5, LATF
    CALL _adc_init
    RETURN

_adc_init:
    ;	   FEDCBA9876543210
    MOV #0b0000000011100000, W0 ;AD1CON1
    MOV #0b0000010000000100, W1 ;AD1CON2
    MOV #0b0000000100000010, W2 ;AD1CON3
    MOV #0b0000000000000011, W3 ;AD1CSSL
    MOV #0b1111111111111100, W4 ;AD1PCFG
    MOV W0, AD1CON1
    MOV W1, AD1CON2
    MOV W2, AD1CON3
    MOV W3, AD1CSSL
    MOV W4, AD1PCFG
    BCLR IFS0, #13
    BSET AD1CON1, #15
    RETURN


    
_adc_read:
    BSET AD1CON1, #2
    AD1IF_loop:
	BTSS IFS0, #13
	GOTO AD1IF_loop
    BCLR AD1CON1, #2
    BCLR IFS0, #13
    MOV ADC1BUF0, W1
    MOV ADC1BUF1, W2
    SUB W1, W2, W0
    RETURN
    
_lcd_port:
    BCLR LATD, #0
    BCLR LATD, #1
    BCLR LATD, #2
    BCLR LATD, #3
    BTSC W0, #0
    BSET LATD, #0
    BTSC W0, #1
    BSET LATD, #1
    BTSC W0, #2
    BSET LATD, #2
    BTSC W0, #3
    BSET LATD, #3
    RETURN
    

_aquecendo_:
    BTSC W0, #0
    GOTO heat_on
    GOTO heat_off
    heat_on:
	BCLR LATD, #13
	RETURN
    heat_off:
	BSET LATD, #13
	RETURN

_resfriando_:
    BTSC W0, #0
    GOTO cool_on
    GOTO cool_off
    cool_on:
	BCLR LATD, #15
	RETURN
    cool_off:
	BSET LATD, #15
	RETURN

_temperatura_estavel:
    BTSC W0, #0
    GOTO stable_on
    GOTO stable_off
    stable_on:
	BCLR LATD, #14
	RETURN
    stable_off:
	BSET LATD, #14
	RETURN


_ler_botao_aumento_temperatura:

    MOV PORTF, W0
    LSR W0, #5, W0
    AND #1, W0
    RETURN

_ler_botao_decremento_temperatura:

    MOV PORTF, W0
    LSR W0, #6, W0
    AND #1, W0
    RETURN

