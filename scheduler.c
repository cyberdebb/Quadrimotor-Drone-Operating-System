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
    static uint8_t highest_priority;
    static uint8_t pos_highest_priority_task;
    static int i;

    highest_priority = 0;
    pos_highest_priority_task = 0;

    for (i = 0; i < readyQueue.readyQueueSize; i++) {
        if (readyQueue.readyQueue[i].task_priority > highest_priority && 
            readyQueue.readyQueue[i].task_state == READY) {
            highest_priority = readyQueue.readyQueue[i].task_priority;
            pos_highest_priority_task = (unsigned char)i;
        }
    }

    return &readyQueue.readyQueue[pos_highest_priority_task];
}

void scheduler()
{
#if DEFAULT_SCHEDULER == RR_SCHEDULER
    readyQueue.taskRunning = rr_scheduler();
#elif DEFAULT_SCHEDULER == PRIORITY_SCHEDULER
    readyQueue.taskRunning = priority_scheduler();
#endif    
}

