#include "scheduler.h"
#include "os_config.h"
#include "syscall.h"
#include "kernel.h"

#include <stdio.h>

// Declara��o da fila de aptos
extern f_aptos_t readyQueue;


tcb_t *rr_scheduler()
{
    uint8_t pos_task_running = os_task_pos(readyQueue.taskRunning->task_func);
    uint8_t idle_selected = 0;
    
    do {
        pos_task_running = (pos_task_running+1) % readyQueue.readyQueueSize;
        if (readyQueue.readyQueue[pos_task_running].task_func == os_idle_task) {
            idle_selected++;
            if (idle_selected > 1) return &readyQueue.readyQueue[0];            
        }
        
    } while (readyQueue.readyQueue[pos_task_running].task_state != READY ||
             readyQueue.readyQueue[pos_task_running].task_func == os_idle_task);
    
    return &readyQueue.readyQueue[pos_task_running];
}

tcb_t *priority_scheduler()
{
    // Seleciona por prioridade; inicia assumindo a tarefa idle na posição 0
    tcb_t *selected = &readyQueue.readyQueue[0];
    uint8_t highest_priority = 0;

    for (uint8_t i = 0; i < readyQueue.readyQueueSize; i++) {
        tcb_t *candidate = &readyQueue.readyQueue[i];

        if (candidate->task_state != READY) {
            continue;   // Apenas tarefas prontas concorrem pela CPU
        }

        if (candidate->task_priority > highest_priority) {
            highest_priority = candidate->task_priority;
            selected = candidate;
            continue;
        }

        if (candidate->task_priority == highest_priority &&
            candidate == readyQueue.taskRunning) {
            selected = candidate; // Mantém a tarefa atual em caso de empate
        }
    }

    return selected;
}

void scheduler()
{
#if DEFAULT_SCHEDULER == RR_SCHEDULER
    readyQueue.taskRunning = rr_scheduler();
#elif DEFAULT_SCHEDULER == PRIORITY_SCHEDULER
    readyQueue.taskRunning = priority_scheduler();
#endif    
}

