#ifndef TYPES_H
#define	TYPES_H

#include <stdint.h>
#include "os_config.h"

#define RR_SCHEDULER            1
#define PRIORITY_SCHEDULER      2

// Define o tipo tarefa
typedef void TASK;

// Define o ponteiro para a função
typedef void (*f_ptr)(void);

// Define os estados possíveis
typedef enum {READY = 0, RUNNING, WAITING} state_t;

// Define a TCB 
typedef struct tcb {
    uint8_t     task_id;
    f_ptr       task_func;
    state_t     task_state;
    uint8_t     task_priority;
    uint8_t     task_time_to_waiting;
    // Contexto de hardware
    uint8_t     BSR_reg;
    uint8_t     WORK_reg;
    uint8_t     STATUS_reg;
    uint24_t     STACK[32];
    uint8_t     task_sp;    
} tcb_t;

// Fila de aptos
typedef struct f_aptos {
    tcb_t readyQueue[MAX_TASKS_ON_READY_QUEUE];
    uint8_t readyQueueSize;
    tcb_t *taskRunning;
} f_aptos_t;

typedef struct semaphore {
    
} sem_t;

#endif	/* TYPES_H */

