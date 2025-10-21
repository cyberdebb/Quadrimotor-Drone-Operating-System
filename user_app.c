#include <xc.h>

#include "user_app.h"
#include "syscall.h"

void config_app(void) {
    TRISCbits.RC2 = 0; // M1
    TRISCbits.RC1 = 0; // M2
    TRISDbits.RD0 = 0; // M3
    TRISDbits.RD1 = 0; // M4
    TRISAbits.RA0 = 1; // gyroscope
    TRISAbits.RA1 = 1; // accelerometer
    TRISAbits.RA2 = 1; // batteries

    asm("GLOBAL _motors_control, _sensors_reading");

    mutex_init(&mutex);
}

typedef struct {
    double M1, M2, M3, M4;  
} motors;

motors motors_data = {0};

// Reads ADC values
unsigned int readADC(unsigned char canal) {
    ADCON0bits.CHS = canal; // Select channel
    ADCON0bits.ADON = 1;  // Turn on ADC module
    ADCON0bits.GO = 1;    // Init the conversion

    while (ADCON0bits.GO); // Wait for the conversion

    ADCON0bits.ADON = 0; // Turn off ADC module

    // Return the result
    return ((ADRESH << 8) + ADRESL); 
}

// Controls the velocities of each motor
TASK motors_control(void) {
    while (1) {
        // Recieves the motor velocities (struct via pipe) from the central control task
        // ...

        if (mutex_lock(&mutex)) {
            // Changes the motors velocities
            PORTCbits.RC2 = motors_data.M1
            PORTCbits.RC1 = motors_data.M2
            PORTDbits.RD0 = motors_data.M3
            PORTDbits.RD1 = motors_data.M4
    
            mutex_unlock(&mutex);
        }

        os_delay(10);
    }
}

// Reads the accelerometer sensor and the gyroscope sensor
TASK sensors_reading(void) {
    while (1) {
        // Reads pin ADC values
        unsigned int valueSensorAcc = readADC(0);
        unsigned int valueSensorGyr = readADC(1);

        // Converts to tensions (0.0V to 5.0V)
        float accelerometer = (float)valueSensorAcc * (5.0 / 1023.0);
        float gyroscope = (float)valueSensorGyr * (5.0 / 1023.0);
        
        // Sends to the central control task
        // ...
        os_delay(10);
    }
}