/* Standard includes. */
#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

SemaphoreHandle_t sem;


void ledAmarelo();
void ledVermelho();

/*
 * Create the demo tasks then start the scheduler.
 */
int main(void) {

    sem = xSemaphoreCreateBinary();

    /* Create the test tasks defined within this file. */
    xTaskCreate(ledAmarelo, "LAmarelo", 128, NULL, 2, NULL);
    xTaskCreate(ledVermelho, "LVermelho", 128, NULL, 5, NULL);

    /* Finally start the scheduler. */
    vTaskStartScheduler();

    /* Will only reach here if there is insufficient heap available to start
    the scheduler. */
    return 0;
}

/*-----------------------------------------------------------*/

void ledAmarelo() {
    TRISCbits.TRISC2 = 0;

    while (1) {
        //xSemaphoreTake(sem, portMAX_DELAY);
        LATCbits.LATC2 = ~PORTCbits.RC2;
    }
}

void ledVermelho() {
    TRISCbits.TRISC1 = 0;

    while (1) {
        LATCbits.LATC1 = ~PORTCbits.RC1;
        vTaskDelay(50);
        //xSemaphoreGive(sem);
    }
}
