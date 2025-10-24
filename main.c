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

#elif TRABALHO_1 == YES
    os_config();
    
    // Cria tarefas de usu�rio
    os_create_task(100, motors_control, 4);
    os_create_task(101, sensors_reading, 3);
    os_create_task(102, battery_monitor, 2);
    os_create_task(103, control_center, 5);

#endif    
    
    os_start();
    
    while (1);
    
    return 0;
}
