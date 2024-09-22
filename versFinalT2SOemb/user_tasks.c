#include "xc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "io.h"
#include "user_tasks.h"
#include "semphr.h"
const TickType_t delay_s = configTICK_RATE_HZ;
const TickType_t delay_ms = ((double)configTICK_RATE_HZ/1000.0);
const TickType_t delay_us = ((double)configTICK_RATE_HZ/1000000.0);
const double fat_corr = 500.0/1023.0;

SemaphoreHandle_t temperatura_controle;
SemaphoreHandle_t temperatura_ambiente;

int16_t temp_ctrl = 25;
int16_t temp_amb = 0;

void system_boot(void* ptr)
{
    lcd_init();
    
    temperatura_controle = xSemaphoreCreateMutex();
    temperatura_ambiente = xSemaphoreCreateMutex();
    
    if(temperatura_controle == NULL || temperatura_ambiente == NULL)
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
        lcd_write_string("Temp. ArCond.: --- \xDF""C");
        lcd_set_cursor(2, 1);
        lcd_write_string("Temp. Ambt.: --- \xDF""C");

        xTaskCreate(controle_temperatura, "controle_temperatura", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
        xTaskCreate(controle_clima, "controle_clima", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
        xTaskCreate(lcd_output, "lcd_output", 255, NULL, 4, NULL);
        
    }
    vTaskDelete(0);
}
void controle_temperatura(void* ptr)
{
    while (1) {
        if(ler_botao_aumento_temperatura() == 0) {
            vTaskDelay(500*delay_ms);
            xSemaphoreTake(temperatura_controle, portMAX_DELAY);
            temp_ctrl++;    
            xSemaphoreGive(temperatura_controle);
        } else if (ler_botao_decremento_temperatura() == 0) {
            vTaskDelay(500*delay_ms);
            xSemaphoreTake(temperatura_controle, portMAX_DELAY);
            temp_ctrl--;
            xSemaphoreGive(temperatura_controle);
        }
    }
}
void controle_clima(void* ptr)
{
    int16_t temp_amb_int, temp_ctrl_int;    
    while(1) {
        temp_amb_int = adc_read()*fat_corr;
        xSemaphoreTake(temperatura_ambiente, portMAX_DELAY);
        temp_amb  = temp_amb_int;
        xSemaphoreGive(temperatura_ambiente);
        xSemaphoreTake(temperatura_controle, portMAX_DELAY);
        temp_ctrl_int = temp_ctrl;
        xSemaphoreGive(temperatura_controle);
        if(temp_amb_int > temp_ctrl_int) {
            aquecendo_(OFF);
            resfriando_(ON);
            temperatura_estavel(OFF);
        } else if (temp_amb_int < temp_ctrl_int) {
            aquecendo_(ON);
            resfriando_(OFF);
            temperatura_estavel(OFF);
        } else {
            aquecendo_(OFF);
            resfriando_(OFF);
            temperatura_estavel(ON);
        }
    }
}
void lcd_output(void* ptr)
{
    int16_t temp_ctrl_int = 25, temp_amb_int = 0;
    char* numbuf;
    numbuf = pvPortMalloc(4*sizeof(char));
    while(1) {
        
        if(xSemaphoreTake(temperatura_controle, 5))
        {
            temp_ctrl_int = temp_ctrl;
            xSemaphoreGive(temperatura_controle);
        }
        if(xSemaphoreTake(temperatura_ambiente, 5))
        {
            temp_amb_int = temp_amb;
            xSemaphoreGive(temperatura_ambiente);
        }
        sprintf(numbuf, "%3d", temp_ctrl_int);
        lcd_set_cursor(1, 14);
        lcd_write_string(numbuf);
        
        sprintf(numbuf, "%3d", temp_amb_int);
        lcd_set_cursor(2, 14);
        lcd_write_string(numbuf);
        
    }
}