#ifndef USER_APP_H
#define	USER_APP_H

#include "os_config.h"
#include "types.h"

#define EXAMPLE_1 NO
#define EXAMPLE_2 NO
#define EXAMPLE_DRONE YES

#if EXAMPLE_1 == YES

void config_app(void);

TASK tarefa_1(void);
TASK tarefa_2(void);
TASK tarefa_3(void);

#elif EXAMPLE_2 == YES
void config_app(void);

TASK tarefa_1(void);

#elif EXAMPLE_DRONE == YES

void config_app(void);

TASK tarefa_controle_central(void);
TASK tarefa_controle_motores(void);
TASK tarefa_monitor_bateria(void);
TASK tarefa_sensores_inerciais(void);

#endif




#endif	/* USER_APP_H */

