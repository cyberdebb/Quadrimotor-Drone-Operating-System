#include <xc.h>
#include "kernel.h"
#include "scheduler.h"
#include "mutex.h"

// Zera a estrutura interna do mutex e deixa-o disponível
void mutex_init(Mutex *mutex)
{
    mutex->locked         = 0;
    mutex->owner          = 0;
    mutex->waiting_count  = 0;

    for (uint8_t i = 0; i < MAX_TASKS_ON_READY_QUEUE; i++) {
        mutex->waiting_tasks[i] = 0;
    }
}

// Adquire o mutex; bloqueia a tarefa até que seja liberado
int mutex_lock(Mutex *mutex)
{
    di();

    while (1) {
        if (mutex->locked == 0) {
            mutex->locked = 1;
            mutex->owner  = readyQueue.taskRunning->task_id;
            ei();
            return 1;
        }

        if (mutex->owner == readyQueue.taskRunning->task_id) {
            ei();
            return 1;
        }

        if (mutex->waiting_count >= MAX_TASKS_ON_READY_QUEUE) {
            ei();
            return 0;
        }

        mutex->waiting_tasks[mutex->waiting_count] = readyQueue.taskRunning;
        mutex->waiting_count++;

        readyQueue.taskRunning->task_state = WAITING_SEM;
        SAVE_CONTEXT(WAITING_SEM);
        scheduler();
        RESTORE_CONTEXT();
    }
}

// Libera o mutex e desperta a próxima tarefa na fila de espera
int mutex_unlock(Mutex *mutex)
{
    di();

    if (mutex->locked == 0 || mutex->owner != readyQueue.taskRunning->task_id) {
        ei();
        return 0;
    }

    if (mutex->waiting_count == 0) {
        mutex->locked = 0;
        mutex->owner  = 0;
        ei();
        return 1;
    }

    tcb_t *task = mutex->waiting_tasks[0];

    for (uint8_t i = 0; i < (mutex->waiting_count - 1); i++) {
        mutex->waiting_tasks[i] = mutex->waiting_tasks[i + 1];
    }
    mutex->waiting_tasks[mutex->waiting_count - 1] = 0;
    mutex->waiting_count--;

    if (task != 0) {
        mutex->owner  = task->task_id;
        mutex->locked = 1;
        task->task_state = READY;
    }
    else {
        mutex->locked = 0;
        mutex->owner  = 0;
    }

    ei();
    return 1;
}