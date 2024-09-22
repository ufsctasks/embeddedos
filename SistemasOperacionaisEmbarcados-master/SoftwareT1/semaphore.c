#include "semaphore.h"
#include "kernel.h"

extern fila_aptos_t f_aptos;

void sem_create(semaphore_t *s, u_int valor)
{
    s->contador = (int) valor;
    s->fila_insere = 0;
    s->fila_retira = 0;
    s->fila_size = 0;
}

void sem_wait(semaphore_t *s)
{
    di();

    s->contador -= 1;
    if (s->contador < 0) 
    {
        // Bloqueia a tarefa
        s->fila_bloqueados[s->fila_insere] = f_aptos.running_task;
        s->fila_size += 1;
        s->fila_insere = (s->fila_insere+1) % MAX_TASKS;
        DISPATCHER(WAITING);
    }

    ei();
}

void sem_post(semaphore_t *s)
{
    di();

    s->contador += 1;
    if (s->contador <= 0) 
    {
        // Desbloqueia tarefa mais antiga
        f_aptos.tasks[s->fila_retira].task_state = READY;
        s->fila_retira = (s->fila_retira+1) % MAX_TASKS;
        s->fila_size -= 1;
    }

    ei();
}

