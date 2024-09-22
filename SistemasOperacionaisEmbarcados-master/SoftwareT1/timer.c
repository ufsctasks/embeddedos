#include "timer.h"
#include "types.h"

extern u_int quantum_rr;
extern fila_aptos_t f_aptos;

void configTimer0()
{    
    T0CONbits.PSA        = 0;
    T0CONbits.T0PS       = 0b010;
    T0CONbits.T0CS       = 0;
    INTCONbits.TMR0IE    = 1;
    INTCONbits.PEIE      = 1;
    TMR0L                = 0;
}

void __interrupt() ISR_TIMER()
{
    if (INTCONbits.TMR0IF) 
    {
        release_task_delay();
        INTCONbits.TMR0IF = 0;
        quantum_rr -= 1;
        if (quantum_rr == 0)
            DISPATCHER(READY);
        TMR0L = 0;   
    }
}
void release_task_delay()
{
   // Verifica se existem tarefas em espera (delay_task)
   u_int i;
   for (i = 0; i < f_aptos.fila_aptos_size; i++) {
      if (f_aptos.tasks[i].task_state == WAITING) {
         if (f_aptos.tasks[i].delay_time > 0) {
            f_aptos.tasks[i].delay_time -= 1;
            if (f_aptos.tasks[i].delay_time == 0) {
               f_aptos.tasks[i].task_state = READY;
            }
         }
      }
   }
} 