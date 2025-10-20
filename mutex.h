#ifndef MUTEX_H
#define	MUTEX_H

#include "types.h"

void mutex_init(Mutex* mutex);
int mutex_lock(Mutex* mutex);
int mutex_unlock(Mutex* mutex);

#endif	/* MUTEX_H */