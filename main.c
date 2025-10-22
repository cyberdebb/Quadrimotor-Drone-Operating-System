#include <xc.h>
#include "syscall.h"
#include "kernel.h"
#include "user_app.h"


int main()
{
#if EXAMPLE_1 == YES
    os_config();
    
    // Cria tarefas de usuário
    os_create_task(100, tarefa_1, 5);
    os_create_task(101, tarefa_2, 5);
    os_create_task(102, tarefa_3, 5);
#elif EXAMPLE_2 == YES
    os_config();
    
    // Cria tarefas de usuário
    os_create_task(100, tarefa_1, 5);
    
#endif    
    
    os_start();
    
    while (1);
    
    return 0;
}


