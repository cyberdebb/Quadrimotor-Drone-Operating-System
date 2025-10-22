#include "pipe.h"
#include "sync.h"
#include <xc.h>

void create_pipe(pipe_t *p)
{
    p->pipe_pos_read    = 0;
    p->pipe_pos_write   = 0;
    // Inicializa os semáforos de controle do pipe
    sem_init(&p->pipe_sem_read, 0);
    sem_init(&p->pipe_sem_write, PIPE_MAX_SIZE);
}

void read_pipe(pipe_t *p, char *buffer)
{
    di();
    
    // Testa o semáforo de leitura
    sem_wait(&p->pipe_sem_read);
    
    *buffer = p->pipe_data[p->pipe_pos_read];
    p->pipe_pos_read = (p->pipe_pos_read + 1) % PIPE_MAX_SIZE;
    
    // Libera o semáforo da escrita
    sem_post(&p->pipe_sem_write);
    
    ei();
}

void write_pipe(pipe_t *p, char buffer)
{
    di();
    
    // Testa o semáforo de escrita
    sem_wait(&p->pipe_sem_write);
    
    p->pipe_data[p->pipe_pos_write] = buffer;
    p->pipe_pos_write = (p->pipe_pos_write + 1) % PIPE_MAX_SIZE;
    
    // Libera o semáforo da leitura
    sem_post(&p->pipe_sem_read);
    
    ei();    
}

