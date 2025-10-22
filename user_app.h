#ifndef USER_APP_H
#define	USER_APP_H

#include "os_config.h"
#include "types.h"

#define EXAMPLE_1 NO
#define EXAMPLE_2 YES

#if EXAMPLE_1 == YES

void config_app(void);

TASK tarefa_1(void);
TASK tarefa_2(void);
TASK tarefa_3(void);

#elif EXAMPLE_2 == YES
void config_app(void);

TASK tarefa_1(void);

#endif




#endif	/* USER_APP_H */

