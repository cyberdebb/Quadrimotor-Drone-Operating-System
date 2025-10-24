#ifndef USER_APP_H
#define	USER_APP_H

#include "os_config.h"
#include "types.h"
#include "mutex.h"

#define EXAMPLE_1 NO
#define EXAMPLE_2 NO
#define TRABALHO_1 YES

Mutex mutex;

#if EXAMPLE_1 == YES

void config_app(void);

TASK tarefa_1(void);
TASK tarefa_2(void);
TASK tarefa_3(void);

#elif EXAMPLE_2 == YES
void config_app(void);

TASK tarefa_1(void);

#elif TRABALHO_1 == YES
void config_app(void);

unsigned int readADC(unsigned char canal);
void __interrupt() TMR0_ISR(void);
uint8_t convert_to_duty_cycle(double motor_value);

TASK motors_control(void);
TASK sensors_reading(void);

#endif




#endif	/* USER_APP_H */

