#include "scheduler.h"

extern fila_aptos_t f_aptos;

u_int default_scheduler()
{
    u_int next_task;

    #if DEFAULT_SCH == ROUND_ROBIN_SCH
       next_task = round_robin_scheduler();
    #else
       next_task = priority_scheduler();
    #endif

    return next_task;
}

u_int round_robin_scheduler()
{
    u_int next_task = f_aptos.running_task;

    do {
       next_task = (next_task+1) % f_aptos.fila_aptos_size;
    } while (f_aptos.tasks[next_task].task_state != READY);

    return next_task;
}

u_int priority_scheduler()
{
    u_int next_task;

    return next_task;
}
