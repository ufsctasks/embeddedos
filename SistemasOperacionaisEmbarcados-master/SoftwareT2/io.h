#ifndef IO_H
#define	IO_H

#include <xc.h>
#include <stdio.h>

// estados
#define ON 1
#define OFF 0
void hardware_init(void);

// PWM functions

// ADC functions
void adc_init(void);
int16_t adc_read();
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
// Sistema de climatização
void heating_system(uint8_t);
void cooling_system(uint8_t);
void stable_temperature(uint8_t);
// Elevator Control
void elevator_down(void);
void elevator_up(void);
void elevator_stop(void);
// Fire Alarm Control
void fire_alarm_buzzer(uint8_t);
void fire_alarm_warning_lights(uint8_t);
void fire_alarm_warning_lights_invert(void);
// Manual Controls
uint8_t read_fire_alarm_button(void);
uint8_t read_temperature_increment_button(void);
uint8_t read_temperature_decrement_button(void);
// Keypad
void enable_keypad_column(short);
uint8_t read_keypad_row(void); // retorna o n.a linha precionada ou 0 caso contrário
// UART
void uart_init(void);
void uart_send(char);
char uart_read(void);
void uart_print(const char*);
#endif	/* IO_H */

