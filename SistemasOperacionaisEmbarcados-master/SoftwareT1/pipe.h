#ifndef PIPE_H
#define	PIPE_H

#include "types.h"
#include "semaphore.h"

#define MAX_PIPE_SIZE 4

typedef struct pipe 
{
    byte pipe_data[MAX_PIPE_SIZE];
    u_int pipe_real_size;
    u_int pipe_pos_read;
    u_int pipe_pos_write;    
    semaphore_t pipe_sem_w;
    semaphore_t pipe_sem_r;
} pipe_t;

void create_pipe(pipe_t *p);
void write_pipe(pipe_t *p, byte data);
void read_pipe(pipe_t *p, byte *data);


#endif	/* PIPE_H */

