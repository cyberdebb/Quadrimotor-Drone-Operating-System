#include <pic18f4550.h>

#include "io.h"


void set_channel(channel_t channel)
{
    ADCON0bits.CHS = channel;
}

void set_port(port_conf_t port)
{
    ADCON1bits.PCFG = port;
}

void config_adc(tad_t tad, conversion_clock_t cclk)
{
    ADCON2bits.ACQT = tad;
    ADCON2bits.ADCS = cclk;
    ADCON2bits.ADFM = 1;
}

void adc_go(int go_done)
{
    ADCON0bits.ADON = go_done;
}

int adc_read()
{
    ADCON0bits.GO = 1;
    while (ADCON0bits.GODONE);
    return ADRES;
}

// Inicializa o PWM com o período e prescaler especificados
void pwm_init(uint8_t period, pwm_prescaler_t prescaler)
{
    T2CONbits.TMR2ON = 0;
    T2CONbits.T2CKPS = prescaler;
    PR2 = period;

    CCP1CONbits.CCP1M = 0b1100;
    pwm_set_duty(0);

    TRISCbits.TRISC2 = 0;
}

// Inicia o PWM
void pwm_start(void)
{
    CCP1CONbits.CCP1M = 0b1100;
    TMR2 = 0;
    T2CONbits.TMR2ON = 1;
}

// Para o PWM
void pwm_stop(void)
{
    T2CONbits.TMR2ON = 0;
    CCP1CONbits.CCP1M = 0;
    LATCbits.LATC2 = 0;
}

// Define o ciclo de trabalho do PWM (0-1023)
void pwm_set_duty(uint16_t duty)
{
    if (duty > 1023) {
        duty = 1023;
    }

    CCPR1L = (uint8_t)(duty >> 2);
    CCP1CONbits.DC1B = (uint8_t)(duty & 0x03);
}

// Inicializa a interrupção externa com a borda especificada
void ext_int_init(ext_int_edge_t edge)
{
    TRISBbits.TRISB0 = 1;
    INTCON2bits.INTEDG0 = edge;
    INTCONbits.INT0IE = 0;
    INTCONbits.INT0IF = 0;
}

// Habilita a interrupção externa
void ext_int_enable(void)
{
    INTCONbits.INT0IF = 0;
    INTCONbits.INT0IE = 1;
}

// Desabilita a interrupção externa
void ext_int_disable(void)
{
    INTCONbits.INT0IE = 0;
}

// Limpa a flag da interrupção externa
void ext_int_clear_flag(void)
{
    INTCONbits.INT0IF = 0;
}

