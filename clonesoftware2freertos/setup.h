#ifndef SETUP_H
#define	SETUP_H

#include <xc.h>
#include <pic18f4520.h>
#include <stdio.h>

#define _XTAL_FREQ 20000000

// estados
#define ON 1
#define OFF 0

// pinos do LCD
#define RS PORTCbits.RC4
#define EN PORTCbits.RC5
#define D4 PORTCbits.RC0
#define D5 PORTCbits.RC1
#define D6 PORTCbits.RC2
#define D7 PORTCbits.RC3

void hardware_init(void);

// PWM functions

// ADC functions
void adc_init(void);
int adc_read(unsigned char);
// LCD functions
void lcd_port(char);
void lcd_cmd(char);
void lcd_clear(void);
void lcd_set_cursor(char, char);
void lcd_init(void);
void lcd_write_char(char);
void lcd_write_string(char *);
void lcd_shift_right(void);
void lcd_shift_left(void);
// Sistema de climatiza��o
void heating_system(short);
void cooling_system(short);
void stable_temperature(short);
// Door control
void close_door(void);
void open_door(void);
// Invasion Alarm Control
void invasion_alarm_buzzer(short);
void invasion_alarm_warning_lights(short);
void invasion_alarm_warning_lights_invert(void);
// Manual Controls
int read_free_door_acess(void);
int read_door_state(void);
int read_moviment(void);
int read_temperature_increment_button(void);
int read_temperature_decrement_button(void);

#endif	/* SETUP_H */

