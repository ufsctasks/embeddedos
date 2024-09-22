#ifndef SETUP_H
#define	SETUP_H

#include <xc.h>
#include <stdio.h>

// estados
#define ON 1
#define OFF 0

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
void lcd_write_string(const char *);
void lcd_shift_right(void);
void lcd_shift_left(void);

// Sistema de climatiza��o
void heating_system(uint8_t);
void cooling_system(uint8_t);
void stable_temperature(uint8_t);

//Manual controls
uint8_t read_temperature_increment_button(void);
uint8_t read_temperature_decrement_button(void);

#endif	/* SETUP_H */

