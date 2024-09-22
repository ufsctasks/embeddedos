#include "kernel.h"
#include "user_tasks.h"
#include "scheduler.h"
#include "memory.h"
#include "io.h"

fila_aptos_t f_aptos;
u_int quantum_rr;

void config_os()
{
    f_aptos.fila_aptos_size    = 0;
    f_aptos.running_task       = 0;
   
    #if IDLE_TASK == ENABLE
    create_task(0, idle_task);
    #endif
   
    config_tasks();
}

void create_task(u_int priority, f_ptr task)
{
    tcb_t new_task;
   
    new_task.task_func      = task;
    new_task.task_priority  = priority;
    new_task.task_id        = f_aptos.fila_aptos_size+1;
    new_task.delay_time     = 0;

    new_task.task_context.stack_real_size = 0;
    new_task.task_state                   = READY;

    // Configuração inicial do contexto da tarefa

    f_aptos.tasks[f_aptos.fila_aptos_size] = new_task;
    f_aptos.fila_aptos_size += 1;
}

void start_os()
{
    quantum_rr     = QUANTUM_RR;  
    configTimer0();

    #if DYNAMIC_MEMORY == ENABLE
     SRAMInitHeap();
    #endif  

    ei();
    T0CONbits.TMR0ON     = 1;  
    adc_init();
}

void delay_task(u_int tempo)
{
    di();

    f_aptos.tasks[f_aptos.running_task].delay_time = tempo;

    DISPATCHER(WAITING);

    ei();
}

void yield_task()
{
    di();

    DISPATCHER(READY);

    ei();
}

void chante_state_to_waiting()
{
    di();

    DISPATCHER(WAITING);

    ei();
}

void idle_task()
{
    while (1)
        Nop();
}