#ifndef USER_APP_H
#define	USER_APP_H

#include "types.h"
#include "mutex.h"

Mutex mutex;

void config_app(void);

unsigned int readADC(unsigned char canal);
void __interrupt() TMR0_ISR(void);
uint8_t convert_to_duty_cycle(double motor_value);

TASK motors_control(void);
TASK sensors_reading(void);

#endif	/* USER_APP_H */