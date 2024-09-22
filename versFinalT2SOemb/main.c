
/* Standard includes. */
#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "io.h"
#pragma config POSCMOD = HS
#pragma config FNOSC = PRI
#pragma config FWDTEN = OFF
void system_boot(void* ptr);
int main(void)
{
    hardware_init();
	xTaskCreate(system_boot, "system_boot", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES, NULL);
	vTaskStartScheduler();
    while(1);
	return 0;
}