#include <xc.h>

#include "user_app.h"
#include "syscall.h"

void config_app(void)
{
    TRISDbits.RD0 = TRISDbits.RD1 = TRISDbits.RD2 = 0;
    
    asm("GLOBAL _tarefa_1, _tarefa_2, _tarefa_3");
}

TASK tarefa_1(void)
{
    while (1) {
        LATDbits.LD0 = ~PORTDbits.RD0;
    }
}

TASK tarefa_2(void)
{
    while (1) {
        LATDbits.LD1 = ~PORTDbits.RD1;
        os_delay(200);
    }    
}

TASK tarefa_3(void)
{
    while (1) {
        LATDbits.LD2 = ~PORTDbits.RD2;
    }    
}

