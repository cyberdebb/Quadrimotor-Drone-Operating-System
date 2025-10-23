#ifndef MUTEX_H
#define	MUTEX_H

#include "types.h"

// Inicializa a estrutura mutex deixando-a desbloqueada
void mutex_init(Mutex *mutex);

// Tenta adquirir o mutex, bloqueando a tarefa caso já esteja em uso
int mutex_lock(Mutex *mutex);

// Libera o mutex e acorda a próxima tarefa da fila, se existir
int mutex_unlock(Mutex *mutex);

#endif	/* MUTEX_H */