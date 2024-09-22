#ifndef IO_H
#define	IO_H

#include <xc.h>
#include <stdio.h>

// estados
#define ON 1
#define OFF 0
void hardware_init(void);




void adc_init(void);
int16_t adc_read();

void lcd_port(char);
void lcd_cmd(char);
void lcd_clear(void);
void lcd_set_cursor(char, char);
void lcd_init(void);
void lcd_write_char(char);
void lcd_write_string(const char *);
void lcd_shift_right(void);
void lcd_shift_left(void);

void aquecendo_(uint8_t);
void resfriando_(uint8_t);
void temperatura_estavel(uint8_t);


uint8_t ler_botao_aumento_temperatura(void);
uint8_t ler_botao_decremento_temperatura(void);

#endif	/* IO_H */

