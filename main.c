#include <xc.h>
#include "syscall.h"
#include "kernel.h"
#include "user_app.h"


int main()
{
#if EXAMPLE_1 == YES
    os_config();
    
    // Cria tarefas de usu�rio
    os_create_task(100, tarefa_1, 5);
    os_create_task(101, tarefa_2, 5);
    os_create_task(102, tarefa_3, 5);
#elif EXAMPLE_2 == YES
    os_config();
    
    // Cria tarefas de usu�rio
    os_create_task(100, tarefa_1, 5);
    
#elif EXAMPLE_DRONE == YES
    os_config();

    // Cria tarefas da aplicação educativa do drone
    os_create_task(200, tarefa_controle_central, 8);
    os_create_task(201, tarefa_controle_motores, 7);
    os_create_task(202, tarefa_monitor_bateria, 5);
    os_create_task(203, tarefa_sensores_inerciais, 6);
    
#endif    
    
    os_start();
    
    while (1);
    
    return 0;
}


