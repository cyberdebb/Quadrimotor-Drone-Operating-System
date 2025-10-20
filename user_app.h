#ifndef USER_APP_H
#define	USER_APP_H

#include "types.h"
#include "mutex.h"

Mutex mutex;

void config_app(void);

TASK motors_control(void);
TASK sensors_reading(void);

#endif	/* USER_APP_H */