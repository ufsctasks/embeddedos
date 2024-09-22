#ifndef TYPES_H
#define	TYPES_H

#include "config.h"

#define STACK_SIZE          32

#define ROUND_ROBIN_SCH     1
#define PRIORITY_SCH        2

typedef void(*f_ptr)(void);
typedef unsigned int u_int;
typedef unsigned char byte;
typedef enum {READY, RUNNING, WAITING} state_t;

typedef struct context
{
    #if IDLE_TASK == ENABLE
    __uint24 stack_data[STACK_SIZE];
    #else
    __uint24 stack_data[STACK_SIZE];
    #endif
    u_int stack_real_size;
    byte r_BSR;
    byte r_STATUS;
    byte r_WORK;    
} context_t;

typedef struct tcb
{
    f_ptr task_func;
    u_int task_id;
    u_int task_priority;
    state_t task_state;
    context_t task_context;
    u_int delay_time;
} tcb_t;

typedef struct fila_aptos
{
    tcb_t tasks[MAX_TASKS];
    u_int fila_aptos_size;
    u_int running_task;
} fila_aptos_t;

#endif	/* TYPES_H */

