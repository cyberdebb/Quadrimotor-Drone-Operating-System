#ifndef MUTEX_H
#define	MUTEX_H

#include "types.h"

void mutex_lock(Mutex *m);
void mutex_unlock(Mutex *m);

#endif	/* MUTEX_H */