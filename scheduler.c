#include "scheduler.h"
#include "os_config.h"
#include "syscall.h"
#include "kernel.h"

#include <stdio.h>

// Declaração da fila de aptos
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
    tcb_t *next = NULL;
    
    return next;
}

void scheduler()
{
#if DEFAULT_SCHEDULER == RR_SCHEDULER
    readyQueue.taskRunning = rr_scheduler();
#elif DEFAULT_SCHEDULER == PRIORITY_SCHEDULER
    readyQueue.taskRunning = priority_scheduler();
#endif    
}

