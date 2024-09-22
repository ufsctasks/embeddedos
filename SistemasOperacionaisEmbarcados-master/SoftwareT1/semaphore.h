#ifndef SEMAPHORE_H
#define	SEMAPHORE_H

#include "config.h"
#include <xc.h>
#include "types.h"

typedef struct semaphore 
{
    int contador;
    u_int fila_bloqueados[MAX_TASKS];
    u_int fila_size;
    u_int fila_insere;
    u_int fila_retira;
    
} semaphore_t;

void sem_create(semaphore_t *s, u_int valor);
void sem_wait(semaphore_t *s);
void sem_post(semaphore_t *s);


#endif	/* SEMAPHORE_H */

