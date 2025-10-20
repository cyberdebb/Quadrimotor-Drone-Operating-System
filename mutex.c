#include <xc.h>
#include "mutex.h"

void mutex_lock(Mutex *m) {
    if (m->locked == 0) {
        m->locked = 1;
    }
}

void mutex_unlock(Mutex *m) {
    m->locked = 0;
}