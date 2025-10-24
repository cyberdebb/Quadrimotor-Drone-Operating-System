#include "pipe.h"
#include "sync.h"
#include "mem.h"
#include <xc.h>

void create_pipe(pipe_t *p)
{
    unsigned char i;

    p->pipe_pos_read    = 0;
    p->pipe_pos_write   = 0;

    // Inicializa os sem�foros de controle do pipe
    sem_init(&p->pipe_sem_read, 0);

    // Aloca bloco de forma dinamica para o pipe
    if (p->pipe_data == 0) {
        p->pipe_data = (char *)SRAMalloc(PIPE_MAX_SIZE);
    }

    for (i = 0; i < PIPE_MAX_SIZE; i++) {
        p->pipe_data[i] = 0;
    }

    // Inicializa semaforo de escrita
    sem_init(&p->pipe_sem_write, PIPE_MAX_SIZE);
}

void read_pipe(pipe_t *p, char *buffer)
{

    di();
    
    // Testa o sem�foro de leitura
    sem_wait(&p->pipe_sem_read);
    
    *buffer = p->pipe_data[p->pipe_pos_read];
    p->pipe_pos_read = (p->pipe_pos_read + 1) % PIPE_MAX_SIZE;
    
    // Libera o sem�foro da escrita
    sem_post(&p->pipe_sem_write);
    
    ei();
}

void write_pipe(pipe_t *p, char buffer)
{

    di();
    
    // Testa o sem�foro de escrita
    sem_wait(&p->pipe_sem_write);
    
    p->pipe_data[p->pipe_pos_write] = buffer;
    p->pipe_pos_write = (p->pipe_pos_write + 1) % PIPE_MAX_SIZE;
    
    // Libera o sem�foro da leitura
    sem_post(&p->pipe_sem_read);
    
    ei();    
}

