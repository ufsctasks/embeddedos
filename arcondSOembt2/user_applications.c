//Bruno Antonelli - 20150457
//Emir Braz - 18203862
#include "xc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "setup.h"
#include "user_applications.h"
#include "semphr.h"

const TickType_t delay_s = configTICK_RATE_HZ;
const TickType_t delay_ms = ((double)configTICK_RATE_HZ/1000.0);
const TickType_t delay_us = ((double)configTICK_RATE_HZ/1000000.0);
const double fat_corr = 500.0/1023.0;

SemaphoreHandle_t tch;
SemaphoreHandle_t tah;

int16_t temp_ctrl = 25;
int16_t temp_amb = 0;

void lcd_output(void* ptr)
{
    int16_t temp_ctrl_int = 25, temp_amb_int = 0;
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
        sprintf(numbuf, "%3d", temp_ctrl_int);
        lcd_set_cursor(1, 14);
        lcd_write_string(numbuf);
        
        sprintf(numbuf, "%3d", temp_amb_int);
        lcd_set_cursor(2, 14);
        lcd_write_string(numbuf);
    }
}
void system_boot(void* ptr)
{
    lcd_init();
    
    tch = xSemaphoreCreateMutex();
    tah = xSemaphoreCreateMutex();
    
    if(cdh != NULL || tch != NULL || tah != NULL)
    {
        lcd_set_cursor(1, 1);
        lcd_write_string("Temp. ArCond.: --- \xDF""C");
        lcd_set_cursor(2, 1);
        lcd_write_string("Temp. Ambt.: --- \xDF""C");

        xTaskCreate(temperature_control, "temperature_control", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
        xTaskCreate(climate_control, "climate_control", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
        xTaskCreate(lcd_output, "lcd_output", 255, NULL, 4, NULL);
    }
    vTaskDelete(0);
}

void temperature_control(void* ptr)
{
    while (1) {
        if(increment_temp_buttom() == 0) {
            vTaskDelay(500*delay_ms);
            xSemaphoreTake(tch, portMAX_DELAY);
            temp_ctrl++;    
            xSemaphoreGive(tch);
        } else if (decrement_temp_buttom() == 0) {
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