#ifndef KERNEL_H
#define	KERNEL_H

#include "types.h"
#include "timer.h"
#include "config.h"
#include <xc.h>

extern fila_aptos_t f_aptos;
extern u_int quantum_rr;

void config_os(void);
void create_task(u_int priority, f_ptr task);
void start_os(void);
void delay_task(u_int tempo);
void yield_task(void);
void chante_state_to_waiting(void);

void idle_task(void);


// Rotinas de troca de contexto (macros)
#define SAVE_CONTEXT(state) { \
  do { \
    u_int task_pos = f_aptos.running_task; \
    di(); \
      if (f_aptos.tasks[task_pos].task_state == RUNNING) { \
        f_aptos.tasks[task_pos].task_context.r_BSR    = BSR; \
        f_aptos.tasks[task_pos].task_context.r_STATUS = STATUS; \
        f_aptos.tasks[task_pos].task_context.r_WORK   = WREG; \
        u_int s_stack = 0; \
        while (STKPTR) { \
          f_aptos.tasks[task_pos].task_context.stack_data[s_stack] = TOS; \
          asm("POP"); \
          s_stack += 1; \
        } \
        f_aptos.tasks[task_pos].task_state                   = state; \
        f_aptos.tasks[task_pos].task_context.stack_real_size = s_stack; \
      } \
    ei(); \
  } while (0); \
} \

#define RESTORE_CONTEXT() { \
  do { \
    u_int task_pos = f_aptos.running_task; \
    di(); \
      u_int s_stack = f_aptos.tasks[task_pos].task_context.stack_real_size; \
      STKPTR = 0; \
      if (s_stack > 0) { \
        BSR      = f_aptos.tasks[task_pos].task_context.r_BSR; \
        STATUS   = f_aptos.tasks[task_pos].task_context.r_STATUS; \
        WREG     = f_aptos.tasks[task_pos].task_context.r_WORK; \
        while (s_stack > 0) { \
          s_stack -= 1; \
          asm("PUSH"); \
          TOS = f_aptos.tasks[task_pos].task_context.stack_data[s_stack]; \
        } \
        f_aptos.tasks[task_pos].task_context.stack_real_size = s_stack; \
      } \
      else { \
        asm("PUSH"); \
        TOS = f_aptos.tasks[task_pos].task_func; \
      } \
      f_aptos.tasks[task_pos].task_state = RUNNING; \
    ei(); \
  } while (0); \
} \

#define DISPATCHER(state) { \
  do { \
    SAVE_CONTEXT(state); \
    f_aptos.running_task = default_scheduler(); \
    quantum_rr = QUANTUM_RR; \
    RESTORE_CONTEXT(); \
  } while (0); \
} \


#endif	/* KERNEL_H */

