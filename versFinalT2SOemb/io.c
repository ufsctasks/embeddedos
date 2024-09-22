#include "io.h"
#include "FreeRTOS.h"
#include "task.h"
extern const TickType_t delay_ms;
extern const TickType_t delay_us;
void lcd_cmd(char a)
{
	asm("BCLR LATD, #4");             // => RS = 0
	lcd_port(a);
	asm("BSET LATD, #5");             // => E = 1
    vTaskDelay(4 * delay_ms);
    asm("BCLR LATD, #5");             // => E = 0
}

void lcd_clear()
{
	lcd_cmd(0);
	lcd_cmd(1);
}

void lcd_set_cursor(char a, char b)
{
	char temp,z,y;
	if(a == 1) {
	  temp = 0x80 + b - 1;
		z = temp>>4;
		y = temp & 0x0F;
		lcd_cmd(z);
		lcd_cmd(y);
	} else if(a == 2) {
		temp = 0xC0 + b - 1;
		z = temp>>4;
		y = temp & 0x0F;
		lcd_cmd(z);
		lcd_cmd(y);
	} else if(a == 3) {
        temp = 0x94 + b - 1;
		z = temp>>4;
		y = temp & 0x0F;
		lcd_cmd(z);
		lcd_cmd(y);
    } else if(a == 4) {
        temp = 0xD4 + b - 1;
		z = temp>>4;
		y = temp & 0x0F;
		lcd_cmd(z);
		lcd_cmd(y);
    }
}

void lcd_init()
{
    lcd_port(0x00);
    vTaskDelay(20 * delay_ms);
    lcd_cmd(0x03);
    vTaskDelay(5 * delay_ms);
    lcd_cmd(0x03);
	vTaskDelay(16 * delay_ms);
    lcd_cmd(0x03);
    lcd_cmd(0x02);
    lcd_cmd(0x02);
    lcd_cmd(0x08);
    lcd_cmd(0x00);
    lcd_cmd(0x0C);
    lcd_cmd(0x00);
    lcd_cmd(0x06);
}

void lcd_write_char(char a)
{
   char temp,y;
   temp = a&0x0F;
   y = a&0xF0;
   asm("BSET LATD, #4");
   lcd_port(y>>4);
   vTaskDelay(40 * delay_us);
   asm("BSET LATD, #5");
   vTaskDelay(40 * delay_us);
   asm("BCLR LATD, #5");
   lcd_port(temp);
   asm("BSET LATD, #5");
   vTaskDelay(40 * delay_us);
   asm("BCLR LATD, #5");
}

void lcd_write_string(const char *a)
{
	int i;
	for(i=0;a[i]!='\0';i++)
	   lcd_write_char(a[i]);
}

void lcd_shift_right(void)
{
	lcd_cmd(0x01);
	lcd_cmd(0x0C);
}

void lcd_shift_left(void)
{
	lcd_cmd(0x01);
	lcd_cmd(0x08);
}