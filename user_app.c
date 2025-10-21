#include <xc.h>

#include "user_app.h"
#include "syscall.h"

#define PWM_PERIOD      255 // 8-bit PWM period
#define PWM_MIN_DUTY    0      
#define PWM_MAX_DUTY    255    
#define PWM_FREQ        50 // 50Hz PWM frequency

// Add global variables for software PWM for all motors
volatile uint8_t pwm_counter[4] = {0, 0, 0, 0}; // Counters for M1, M2, M3, M4
volatile uint8_t duty_cycle[4] = {0, 0, 0, 0}; // Duty cycles for M1, M2, M3, M4

void config_app(void) {
    TRISCbits.RD0 = 0; // M1
    TRISCbits.RD1 = 0; // M2
    TRISDbits.RD2 = 0; // M3
    TRISDbits.RD3 = 0; // M4
    TRISAbits.RA0 = 1; // gyroscope
    TRISAbits.RA1 = 1; // accelerometer
    TRISAbits.RA2 = 1; // batteries

    asm("GLOBAL _motors_control, _sensors_reading");

    // Configure Timer0 for software PWM
    T0CON = 0x88; // Enable Timer0, 8-bit, internal clock, 1:1 prescaler
    TMR0IE = 1; // Enable Timer0 interrupt
    PEIE = 1; // Enable peripheral interrupts
    GIE = 1; // Enable global interrupts

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

// Timer0 Interrupt Service Routine for software PWM
void __interrupt() TMR0_ISR(void) {
    if (TMR0IF) {
        // Update all motors PWM
        for(int i = 0; i < 4; i++) {
            pwm_counter[i]++;
            if (pwm_counter[i] >= PWM_PERIOD) {
                pwm_counter[i] = 0;
            }
        }
        
        // Set motor pins based on PWM counters
        PORTCbits.RC2 = (pwm_counter[0] < duty_cycle[0]) ? 1 : 0;  // M1
        PORTCbits.RC1 = (pwm_counter[1] < duty_cycle[1]) ? 1 : 0;  // M2
        PORTDbits.RD0 = (pwm_counter[2] < duty_cycle[2]) ? 1 : 0;  // M3
        PORTDbits.RD1 = (pwm_counter[3] < duty_cycle[3]) ? 1 : 0;  // M4

        TMR0IF = 0;  // Clear Timer0 interrupt flag
    }
}

// Convert motor value (0.0 to 1.0) to duty cycle (0 to 255)
uint8_t convert_to_duty_cycle(double motor_value) {
    if (motor_value < 0.0) motor_value = 0.0;
    if (motor_value > 1.0) motor_value = 1.0;
    return (uint8_t)(motor_value * PWM_MAX_DUTY);
}

// Controls the velocities of each motor
TASK motors_control(void) {
    while (1) {
        // Recieves the motor velocities (struct via pipe) from the central control task
        // ...

        if (mutex_lock(&mutex)) {
            // Update all duty cycles
            duty_cycle[0] = convert_to_duty_cycle(motors_data.M1);
            duty_cycle[1] = convert_to_duty_cycle(motors_data.M2);
            duty_cycle[2] = convert_to_duty_cycle(motors_data.M3);
            duty_cycle[3] = convert_to_duty_cycle(motors_data.M4);
            
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