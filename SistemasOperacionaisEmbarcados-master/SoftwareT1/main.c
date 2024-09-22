#include "io.h"
#include "kernel.h"
#include "user_tasks.h"

int main()
{
    hardware_init();
    
    config_os();

    create_task(3, climate_control);
    create_task(3, temperature_control);
    create_task(2, elevator_control);
    create_task(2, elevator_move);
    create_task(1, fire_alarm_control);
   
    start_os();
   
    while (1);
   
    return 0;
}
