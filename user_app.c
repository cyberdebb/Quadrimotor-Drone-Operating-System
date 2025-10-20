#include <xc.h>

#include "user_app.h"
#include "syscall.h"

void config_app(void) {
    TRISDbits.RD0 = 0; // M1
    TRISDbits.RD1 = 0; // M2
    TRISDbits.RD2 = 0; // M3
    TRISDbits.RD3 = 0; // M4
    TRISDbits.RD4 = 1; // gyroscope
    TRISDbits.RD5 = 1; // accelerometer

    asm("GLOBAL _motors_control, _sensors_reading");
}

typedef struct {
    double M1, M2, M3, M4;  
} motors;

TASK motors_control(void) {
    mutex_lock(&x);
    while (1) {
        
    }
    mutex_unlock(&x);
}

TASK sensors_reading(void) {
    mutex_lock(&x);
    while (1) {
        
    }
    mutex_unlock(&x);
}