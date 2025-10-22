#include "pipe.h"
#include "sync.h"
#include "mem.h"
#include <xc.h>

pipe_t* pipe_create(uint8_t size)
{
    if (size == 0 || size > PIPE_MAX_SIZE) {
        return NULL;
    }

    if (sizeof(pipe_t) > 0x7E) {
        return NULL; // Estrutura grande demais para o alocador atual
    }

    uint8_t pipe_struct_size = (uint8_t)sizeof(pipe_t);
    pipe_t *p = (pipe_t *)SRAMalloc(pipe_struct_size);
    if (p == NULL) {
        return NULL;
    }

    unsigned char *buffer = SRAMalloc(size);
    if (buffer == NULL) {
        SRAMfree((unsigned char *)p);
        return NULL;
    }

    p->pipe_data = (char *)buffer;
    p->pipe_capacity = size;
    p->pipe_pos_read = 0;
    p->pipe_pos_write = 0;
    sem_init(&p->pipe_sem_read, 0);
    sem_init(&p->pipe_sem_write, size);

    for (uint8_t i = 0; i < MAX_TASKS_ON_READY_QUEUE; i++) {
        p->pipe_sem_read.sem_queue[i] = 0;
        p->pipe_sem_write.sem_queue[i] = 0;
    }

    return p;
}

void pipe_destroy(pipe_t *p)
{
    if (p == NULL) {
        return;
    }

    if (p->pipe_data != NULL) {
        SRAMfree((unsigned char *)p->pipe_data);
        p->pipe_data = NULL;
    }

    SRAMfree((unsigned char *)p);
}

// Fun��o antiga substituida pela pipe_create e pipe_destoy
//void create_pipe(pipe_t *p)
//{
//    p->pipe_pos_read    = 0;
//    p->pipe_pos_write   = 0;
//    // Inicializa os sem�foros de controle do pipe
//    sem_init(&p->pipe_sem_read, 0);
//    sem_init(&p->pipe_sem_write, PIPE_MAX_SIZE);
//}

void read_pipe(pipe_t *p, char *buffer)
{
    di();
    
    // Testa o sem�foro de leitura
    sem_wait(&p->pipe_sem_read);
    
    *buffer = p->pipe_data[p->pipe_pos_read];
//    p->pipe_pos_read = (p->pipe_pos_read + 1) % PIPE_MAX_SIZE;    // Antigo
    p->pipe_pos_read = (p->pipe_pos_read + 1) % p->pipe_capacity;
    
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
//    p->pipe_pos_write = (p->pipe_pos_write + 1) % PIPE_MAX_SIZE;  // Antigo
    p->pipe_pos_write = (p->pipe_pos_write + 1) % p->pipe_capacity;
    
    // Libera o sem�foro da leitura
    sem_post(&p->pipe_sem_read);
    
    ei();    
}

