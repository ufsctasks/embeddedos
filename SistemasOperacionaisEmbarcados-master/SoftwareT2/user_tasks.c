#include "xc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "io.h"
#include "user_tasks.h"
#include "semphr.h"
const TickType_t delay_s = configTICK_RATE_HZ;
const TickType_t delay_ms = ((double)configTICK_RATE_HZ/1000.0);
const TickType_t delay_us = ((double)configTICK_RATE_HZ/1000000.0);
const uint8_t btab[4][3] = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}, {10, 0, 11}};
const double fat_corr = 500.0/1023.0;

SemaphoreHandle_t cfh;
SemaphoreHandle_t nfh;
SemaphoreHandle_t tch;
SemaphoreHandle_t tah;

uint8_t current_floor = 1;
uint8_t next_floor = 1;
int16_t temp_ctrl = 25;
int16_t temp_amb = 0;

void system_boot(void* ptr)
{
    lcd_init();
    
    cfh = xSemaphoreCreateMutex();
    nfh = xSemaphoreCreateMutex();
    tch = xSemaphoreCreateMutex();
    tah = xSemaphoreCreateMutex();
    
    if(cfh == NULL || nfh == NULL || tch == NULL || tah == NULL)
    {
        lcd_set_cursor(1, 1);
        lcd_write_string("======= ERRO =======");
        lcd_set_cursor(2, 1);
        lcd_write_string("  Nao foi possivel");
        lcd_set_cursor(3, 1);
        lcd_write_string(" criar os semaforos");
        lcd_set_cursor(4, 1);
        lcd_write_string("====================");
    }
    else
    {
        lcd_set_cursor(1, 1);
        lcd_write_string("Temp. Cont.: --- \xDF""C");
        lcd_set_cursor(2, 1);
        lcd_write_string("Temp. Ambt.: --- \xDF""C");
        lcd_set_cursor(3, 1);
        lcd_write_string("Piso. Selec: ---");
        lcd_set_cursor(4, 1);
        lcd_write_string("Piso. Atual: ---");

        xTaskCreate(temperature_control, "temperature_control", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
        xTaskCreate(climate_control, "climate_control", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
        xTaskCreate(fire_alarm_control, "fire_alarm_control", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES, NULL);
        xTaskCreate(lcd_output, "lcd_output", 255, NULL, 4, NULL);
        xTaskCreate(elevator_control, "elevator_control", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
        xTaskCreate(elevator_move, "elevator_move", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
        xTaskCreate(uart_control, "uart_control", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
        
    }
    vTaskDelete(0);
}
void fire_alarm_control(void* ptr)
{
    while(1){
        if(read_fire_alarm_button() == 0) {
            fire_alarm_buzzer(ON);
            fire_alarm_warning_lights(ON);
            do {
                fire_alarm_warning_lights_invert();
                vTaskDelay(250 * delay_ms);
            } while (read_fire_alarm_button() == 0); 
            fire_alarm_buzzer(OFF);
            fire_alarm_warning_lights(OFF);
        } else {
            fire_alarm_buzzer(OFF);
            fire_alarm_warning_lights(OFF);
        }
        vTaskDelay(500 * delay_ms);
    }
}
void temperature_control(void* ptr)
{
    while (1) {
        if(read_temperature_increment_button() == 0) {
            vTaskDelay(500*delay_ms);
            xSemaphoreTake(tch, portMAX_DELAY);
            temp_ctrl++;    
            xSemaphoreGive(tch);
        } else if (read_temperature_decrement_button() == 0) {
            vTaskDelay(500*delay_ms);
            xSemaphoreTake(tch, portMAX_DELAY);
            temp_ctrl--;
            xSemaphoreGive(tch);
        }
    }
}
void climate_control(void* ptr)
{
    int16_t temp_amb_int, temp_ctrl_int;    
    while(1) {
        temp_amb_int = adc_read()*fat_corr;
        xSemaphoreTake(tah, portMAX_DELAY);
        temp_amb  = temp_amb_int;
        xSemaphoreGive(tah);
        xSemaphoreTake(tch, portMAX_DELAY);
        temp_ctrl_int = temp_ctrl;
        xSemaphoreGive(tch);
        if(temp_amb_int > temp_ctrl_int) {
            heating_system(OFF);
            cooling_system(ON);
            stable_temperature(OFF);
        } else if (temp_amb_int < temp_ctrl_int) {
            heating_system(ON);
            cooling_system(OFF);
            stable_temperature(OFF);
        } else {
            heating_system(OFF);
            cooling_system(OFF);
            stable_temperature(ON);
        }
    }
}
void lcd_output(void* ptr)
{
    int16_t temp_ctrl_int = 25, temp_amb_int = 0;
    uint8_t current_floor_int = 1, next_floor_int = 1;
    char* numbuf;
    numbuf = pvPortMalloc(4*sizeof(char));
    while(1) {
        
        if(xSemaphoreTake(tch, 5))
        {
            temp_ctrl_int = temp_ctrl;
            xSemaphoreGive(tch);
        }
        if(xSemaphoreTake(tah, 5))
        {
            temp_amb_int = temp_amb;
            xSemaphoreGive(tah);
        }
        if(xSemaphoreTake(cfh, 5))
        {
            current_floor_int = current_floor;
            xSemaphoreGive(cfh);
        }
        if(xSemaphoreTake(nfh, 5))
        {
            next_floor_int = next_floor;
            xSemaphoreGive(nfh);
        }
        sprintf(numbuf, "%3d", temp_ctrl_int);
        lcd_set_cursor(1, 14);
        lcd_write_string(numbuf);
        
        sprintf(numbuf, "%3d", temp_amb_int);
        lcd_set_cursor(2, 14);
        lcd_write_string(numbuf);
        
        sprintf(numbuf, "%3d", next_floor_int);
        lcd_set_cursor(3, 14);
        lcd_write_string(numbuf);
        
        sprintf(numbuf, "%3d", current_floor_int);
        lcd_set_cursor(4, 14);
        lcd_write_string(numbuf);
    }
}

void elevator_control(void* ptr)
{
    int column = 1, row = 0;
    uint8_t current_floor_int;
    while(1)
    {
        enable_keypad_column(column);
        vTaskDelay(10*delay_ms);
        row = read_keypad_row();
        xSemaphoreTake(cfh, portMAX_DELAY);
        current_floor_int = current_floor;
        xSemaphoreGive(cfh);
        if(row) if(btab[row-1][column-1] > 0 && btab[row-1][column-1] <= 9 && current_floor_int == next_floor)
        {
            xSemaphoreTake(nfh, portMAX_DELAY);
            next_floor = btab[row-1][column-1];
            xSemaphoreGive(nfh);
        }
        column++;
        if(column > 3) column = 1;
    }
}
void elevator_move(void* ptr)
{
    uint8_t moving = 0;
    uint8_t next_floor_int = 1;
    while(1)
    {
        xSemaphoreTake(nfh, portMAX_DELAY);
        next_floor_int = next_floor;
        xSemaphoreGive(nfh);
        if(next_floor_int != current_floor)
        {
            if(!moving)
            {
                moving = 1;
                if(next_floor_int < current_floor) elevator_down();
                if(next_floor_int > current_floor) elevator_up();
            }
            vTaskDelay(delay_s);
            xSemaphoreTake(cfh, portMAX_DELAY);
            if(next_floor_int < current_floor) current_floor--;
            if(next_floor_int > current_floor) current_floor++;
            xSemaphoreGive(cfh);
        }
        else 
        {
            if(moving) 
            {
                moving = 0;
                elevator_stop();
            }
        }
    }
}
void uart_control(void* ptr)
{
    const char* init_msg = "Sistema de Sprinklers\r\n";
    const char* inva_cmd = "Comando Inv�lido\rn";
    char sector;
    uint8_t state;
    char buffer[22];
    uart_print(init_msg);
    while(1)
    {
        sector = uart_read();
        state = uart_read();
        state = state-'0';
        if(state != 0 && state != 1)
            uart_print(inva_cmd);
        else 
        {
            
            sprintf(buffer, "Setor %c - %s\r\n", sector, (state ? "Ativar":"Desativar"));
            uart_print(buffer);
        }
    }
}