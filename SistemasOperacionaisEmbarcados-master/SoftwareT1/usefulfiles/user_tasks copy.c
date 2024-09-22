#include "user_tasks.h"
#include "semaphore.h"
#include "io.h"
#include "kernel.h"
#include <xc.h>
#include<stdio.h>

const byte btab[4][3] =
{
    {1, 2, 3},
    {4, 5, 6},
    {7, 8, 9},
    {10, 0, 11}
};
int current_floor = 1;
int next_floor = 1;
short moving = 0;

int temperature = 21;

void config_tasks()
{
    asm("GLOBAL _climate_control, _temperature_control, _elevator_control, _elevator_move, _fire_alarm_control");  
}

void climate_control(void)
{
    int t1, t2, t;
    char buffer[21];
    
    while(1) {
        
        t1 = adc_read(0);
        t2 = adc_read(1);
        
        t = t1 - t2;
        
        di();
        lcd_set_cursor(2, 1);
        sprintf(buffer, "Temp. Ambt.: %d C", (t>>1));
        lcd_write_string(buffer);
        ei();
        
        if(t > (temperature+temperature+1)) {
            heating_system(OFF);
            cooling_system(ON);
            stable_temperature(OFF);
        } else if (t < (temperature+temperature-1)) {
            heating_system(ON);
            cooling_system(OFF);
            stable_temperature(OFF);
        } else {
            heating_system(OFF);
            cooling_system(OFF);
            stable_temperature(ON);
        }
        
        delay_task(500);
    }
}

void fire_alarm_control(void)
{
    while(1) {
        
        if(read_fire_alarm_button() == 0) {
            
            fire_alarm_buzzer(ON);
            fire_alarm_warning_lights(ON);
            
            do {
                fire_alarm_warning_lights_invert();
                delay_task(250);
            } while (read_fire_alarm_button() == 0); 
            
            fire_alarm_buzzer(OFF);
            fire_alarm_warning_lights(OFF);
        } else {
            fire_alarm_buzzer(OFF);
            fire_alarm_warning_lights(OFF);
        }
        
        delay_task(500);
    }
}

void temperature_control(void)
{
    char buffer[21];
    
    di();
    lcd_set_cursor(1, 1);
    sprintf(buffer, "Temp. Cont.: %d C", temperature);
    lcd_write_string(buffer);
    ei();
    
    while (1) {
        if(read_temperature_increment_button() == 0) {
            
            delay_task(1000);
            
            temperature++;
            
            di();
            lcd_set_cursor(1, 1);
            sprintf(buffer, "Temp. Cont.: %d C", temperature);
            lcd_write_string(buffer);
            ei();
            
        } else if (read_temperature_decrement_button() == 0) {
            
            delay_task(1000);
            
            temperature--;
            
            di();
            lcd_set_cursor(1, 1);
            sprintf(buffer, "Temp. Cont.: %d C", temperature);
            lcd_write_string(buffer);
            ei();
        }
    }
}

void elevator_control(void)
{
    char buffer[21];
    int column = 1, row = -1;
    di();
    lcd_set_cursor(3, 1);
    sprintf(buffer, "Piso. Selec: %d", next_floor);
    lcd_write_string(buffer);
    ei();
    while(1)
    {
        enable_keypad_column(column);
        di();
        __delay_ms(10);
        ei();
        row = read_keypad_row();
        di();
        if(row)
            if(btab[row-1][column-1] > 0 && btab[row-1][column-1] <= 9 && current_floor == next_floor)
            {
                next_floor = btab[row-1][column-1];
                lcd_set_cursor(3, 1);
                sprintf(buffer, "Piso. Selec: %d", next_floor);
                lcd_write_string(buffer);
            }   
        ei();
        column++;
        if(column > 3) column = 1;
    }
}
void elevator_move(void)
{
    char buffer[21];
    di();
    lcd_set_cursor(4, 1);
    sprintf(buffer, "Piso. Atual: %d", current_floor);
    lcd_write_string(buffer);
    ei();
    while(1)
    {
        if(next_floor != current_floor)
        {
            if(!moving)
            {
                di();
                moving = 1;
                if(next_floor < current_floor) elevator_down();
                if(next_floor > current_floor) elevator_up();
                ei();
            }
            delay_task(500);
            di();
            if(next_floor < current_floor) current_floor--;
            if(next_floor > current_floor) current_floor++;
            lcd_set_cursor(4, 1);
            sprintf(buffer, "Piso. Atual: %d", current_floor);
            lcd_write_string(buffer);
            ei();
        }
        else 
        {
            di();
            if(moving) 
            {
                moving = 0;
                elevator_stop();
            }
            ei();
        }
    }
}