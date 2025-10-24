#include <xc.h>

#include "user_app.h"
#include "syscall.h"
#include "sync.h"
#include "pipe.h"
#include "mem.h"
#include "io.h"

#define PWM_PERIOD      255 // 8-bit PWM period
#define PWM_MIN_DUTY    0      
#define PWM_MAX_DUTY    255    
#define PWM_FREQ        50 // 50Hz PWM frequency
#define BATTERY_LOW_THRESHOLD_MV   3600U
#define BATTERY_ADC_CHANNEL        2U
#define BATTERY_MONITOR_DELAY_TICKS 100U

// Add global variables for software PWM for all motors
volatile uint8_t pwm_counter[4] = {0, 0, 0, 0}; // Counters for M1, M2, M3, M4
volatile uint8_t duty_cycle[4] = {0, 0, 0, 0}; // Duty cycles for M1, M2, M3, M4


#if EXAMPLE_1 == YES
// Exemplo 1

//sem_t semaforo_teste;
pipe_t canal;

void config_app(void)
{
    TRISDbits.RD0 = TRISDbits.RD1 = TRISDbits.RD2 = 0;
    
    TRISCbits.RC0 = TRISCbits.RC1                 = 0;             
    
    //sem_init(&semaforo_teste, 0);
    create_pipe(&canal);
    
    asm("GLOBAL _tarefa_1, _tarefa_2, _tarefa_3");
}

TASK tarefa_1(void)
{    
    char *dados = (char*)SRAMalloc(sizeof(char) * 3);
    dados[0] = 'a';
    dados[1] = 'b';
    dados[2] = 'c';
    
    int index = 0;
    while (1) {
        LATDbits.LD0 = ~PORTDbits.RD0;
        //sem_wait(&semaforo_teste);
        write_pipe(&canal, dados[index]);
        index = (index + 1) % 3;
        os_delay(50);
    }
    
    SRAMfree(dados);
}

TASK tarefa_2(void)
{
    char dado;
    
    while (1) {
        LATDbits.LD1 = ~PORTDbits.RD1;
        //os_delay(200);
        read_pipe(&canal, &dado);
        if (dado == 'a') {
            LATCbits.LATC0 = 1;
        }
        else if (dado == 'b') {
            LATCbits.LATC1 = 1;
        }
        else if (dado == 'c') {
            LATCbits.LATC0 = 0;
            LATCbits.LATC1 = 0;
        }
    }    
}

TASK tarefa_3(void)
{
    while (1) {
        LATDbits.LD2 = ~PORTDbits.RD2;
        //sem_post(&semaforo_teste);
    }    
}

#elif EXAMPLE_2 == YES

void config_app(void)
{
    set_channel(CHANNEL_0);
    set_port(AN00);
    config_adc(TAD12, FOSC4);
    
    TRISCbits.RC0 = TRISCbits.RC1 = 0;
    TRISDbits.RD0 = 0;
    
    asm("GLOBAL _tarefa_1");
}

TASK tarefa_1(void)
{
    int pot = 0;
    
    adc_go(1);
    
    while (1) {
        LATDbits.LD0 = ~PORTDbits.RD0;
        
        pot = adc_read();
        
        if (pot > 0 && pot < 500) {
            LATCbits.LATC0 = 1;
            LATCbits.LATC1 = 0;
        }
        else if (pot >= 500 && pot <= 1023) {
            LATCbits.LATC1 = 1;
            LATCbits.LATC0 = 0;
        }
    }
}

#elif TRABALHO_1 == YES

typedef struct {
    uint8_t M1, M2, M3, M4;
} motors;

static motors motors_data = {0};
static pipe_t control_pipe = {0};
static volatile uint8_t return_to_base_flag = 0;


void config_app(void) {
    TRISDbits.RD0 = 0; // M1
    TRISDbits.RD1 = 0; // M2
    TRISDbits.RD2 = 0; // M3
    TRISDbits.RD3 = 0; // M4
    TRISAbits.RA0 = 1; // gyroscope
    TRISAbits.RA1 = 1; // accelerometer
    TRISAbits.RA2 = 1; // batteries

    asm("GLOBAL _motors_control, _sensors_reading, _battery_monitor, _control_center");

    // Configure Timer0 for software PWM
    T0CON = 0x88; // Enable Timer0, 8-bit, internal clock, 1:1 prescaler
    TMR0IE = 1; // Enable Timer0 interrupt
    PEIE = 1; // Enable peripheral interrupts
    GIE = 1; // Enable global interrupts

    mutex_init(&mutex);
    create_pipe(&control_pipe);
    return_to_base_flag = 0;
}

// Reads ADC values
unsigned int readADC(unsigned char canal) {
    ADCON0bits.CHS = canal; // Select channel
    ADCON0bits.ADON = 1;  // Turn on ADC module
    ADCON0bits.GO = 1;    // Init the conversion

    while (ADCON0bits.GO); // Wait for the conversion

    ADCON0bits.ADON = 0; // Turn off ADC module

    // Return the result
    return ((uint16_t)ADRESH << 8) + ADRESL; 
}

// Timer0 Interrupt Service Routine for software PWM
void __interrupt() TMR0_ISR(void) {
    static uint8_t i;
    
    if (TMR0IF) {
        // Update all motors PWM
        for(i = 0; i < 4; i++) {
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

// Convert motor value (0-255) to duty cycle (0-255)
uint8_t convert_to_duty_cycle(uint8_t motor_value) {
    return motor_value;
}

// Controls the velocities of each motor
TASK motors_control(void) {
    while (1) {
        // Recieves the motor velocities (struct via pipe) from the central control task
        // ...

        if (mutex_lock(&mutex)) {
            if (return_to_base_flag) {
                duty_cycle[0] = PWM_MIN_DUTY;
                duty_cycle[1] = PWM_MIN_DUTY;
                duty_cycle[2] = PWM_MIN_DUTY;
                duty_cycle[3] = PWM_MIN_DUTY;
            } else {
                duty_cycle[0] = convert_to_duty_cycle(motors_data.M1);
                duty_cycle[1] = convert_to_duty_cycle(motors_data.M2);
                duty_cycle[2] = convert_to_duty_cycle(motors_data.M3);
                duty_cycle[3] = convert_to_duty_cycle(motors_data.M4);
            }

            mutex_unlock(&mutex);
        }

        os_delay(10);
    }
}

// Reads the accelerometer sensor and the gyroscope sensor
TASK sensors_reading(void) {
    static unsigned int valueSensorAcc;
    static unsigned int valueSensorGyr;
    static uint16_t accelerometer_mV;
    static uint16_t gyroscope_mV;

    while (1) {
        valueSensorAcc = readADC(0);
        valueSensorGyr = readADC(1);

        accelerometer_mV = ((uint16_t)valueSensorAcc * 5000U) / 1023U;
        gyroscope_mV    = ((uint16_t)valueSensorGyr * 5000U) / 1023U;

        // TODO: enviar dados para o centro de controle
        os_delay(10);
    }
}

TASK battery_monitor(void) {
    static uint8_t last_status = 0xFF;
    static unsigned int raw_value;
    static uint16_t millivolts;
    static uint8_t status;

    while (1) {
        raw_value = readADC(BATTERY_ADC_CHANNEL);
        millivolts = ((uint16_t)raw_value * 5000U) / 1023U;
        status = (millivolts < BATTERY_LOW_THRESHOLD_MV) ? 1U : 0U;

        if (status != last_status) {
            if (status) {
                write_pipe(&control_pipe, 'L');
            } else {
                write_pipe(&control_pipe, 'N');
            }
            last_status = status;
        }

        os_delay(BATTERY_MONITOR_DELAY_TICKS);
    }
}

TASK control_center(void) {
    static char message;

    while (1) {
        read_pipe(&control_pipe, &message);

        if (message == 'L') {
            return_to_base_flag = 1;
        } else if (message == 'N') {
            return_to_base_flag = 0;
        }
    }
}

#endif