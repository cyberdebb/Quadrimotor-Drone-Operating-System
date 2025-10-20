#include <xc.h>
#include "syscall.h"
#include "kernel.h"
#include "user_app.h"


int main()
{
    os_config();
    
    // Cria tarefas de usuário
    os_create_task(100, motors_control, 5);
    os_create_task(101, sensors_reading, 5);
    
    os_start();
    
    while (1);
    
    return 0;
}
