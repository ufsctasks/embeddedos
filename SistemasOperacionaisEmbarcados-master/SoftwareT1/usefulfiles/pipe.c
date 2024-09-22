#include "pipe.h"
#include "kernel.h"
#include "memory.h"
#include <xc.h>

void create_pipe(pipe_t *p)
{
    p->pipe_real_size = 0;
    p->pipe_pos_write = 0;
    p->pipe_pos_read  = 0;
    sem_create(&p->pipe_sem_w, MAX_PIPE_SIZE);
    sem_create(&p->pipe_sem_r, 0);   
}

void write_pipe(pipe_t *p, byte data)
{
    sem_wait(&p->pipe_sem_w);
    p->pipe_data[p->pipe_pos_write] = data;
    p->pipe_pos_write = (p->pipe_pos_write + 1) % MAX_PIPE_SIZE;
    p->pipe_real_size += 1;
    sem_post(&p->pipe_sem_r);
}

void read_pipe(pipe_t *p, byte *data)
{
    sem_wait(&p->pipe_sem_r);
    *data = p->pipe_data[p->pipe_pos_read];
    p->pipe_pos_read = (p->pipe_pos_read +1) % MAX_PIPE_SIZE;
    p->pipe_real_size -= 1;
    sem_post(&p->pipe_sem_w);
}

