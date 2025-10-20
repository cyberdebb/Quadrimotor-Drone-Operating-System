#include <xc.h>
#include "kernel.h"
#include "scheduler.h"
#include "mutex.h"

void mutex_init(Mutex* mutex) {
    mutex->locked = 0;
    mutex->owner = 0;
    mutex->waiting_count = 0;
}

int mutex_lock(Mutex* mutex) {
    if (mutex->locked) {
        // Add running task to the waiting list
        mutex->waiting_tasks[mutex->waiting_count++] = readyQueue.taskRunning;
        readyQueue.taskRunning->task_state = WAITING;
        // Call scheduler to change task
        scheduler();
        return 0;
    }
    
    // Lock the running task
    mutex->locked = 1;
    mutex->owner = readyQueue.taskRunning->task_id;
    return 1;
}

int mutex_unlock(Mutex* mutex) {    
    if (mutex->owner != readyQueue.taskRunning->task_id) {
        // Only the Mutex owner can unclock it
        return 0;
    }
    
    mutex->locked = 0;
    mutex->owner = 0;
    
    // Free the first task in the waiting list
    if (mutex->waiting_count > 0) {
        mutex->waiting_count--;
        tcb_t* task = mutex->waiting_tasks[0];
        task->task_state = READY;
        
        // Reorganize the waiting list
        for(int i = 0; i < mutex->waiting_count; i++) {
            mutex->waiting_tasks[i] = mutex->waiting_tasks[i + 1];
        }
    }
    
    return 1;
}