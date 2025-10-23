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

// Prepara Timer2 e CCP1 para geração de PWM
void pwm_init(uint8_t period, pwm_prescaler_t prescaler)
{
    T2CONbits.TMR2ON = 0;
    T2CONbits.T2CKPS = prescaler;
    T2CONbits.TOUTPS = 0;
    PR2 = period;
    TMR2 = 0;

    TRISCbits.TRISC2 = 0;
    CCP1CONbits.CCP1M = 0b1100;
    pwm_set_duty(0);
}

// Ajusta ciclo ativo do PWM
void pwm_set_duty(uint16_t duty)
{
    if (duty > 1023) {
        duty = 1023;
    }

    CCPR1L = (uint8_t)(duty >> 2);
    CCP1CONbits.DC1B = (uint8_t)(duty & 0x03);
}

// Ativa o temporizador usado pelo PWM
void pwm_start(void)
{
    T2CONbits.TMR2ON = 1;
}

// Desliga o temporizador do PWM
void pwm_stop(void)
{
    T2CONbits.TMR2ON = 0;
}

// Configura borda de disparo da interrupção externa
void ext_int_config(ext_int_t source, ext_int_edge_t edge)
{
    switch (source) {
        case EXT_INT0:
            TRISBbits.TRISB0 = 1;
            INTCON2bits.INTEDG0 = edge;
            break;
        case EXT_INT1:
            TRISBbits.TRISB1 = 1;
            INTCON2bits.INTEDG1 = edge;
            break;
        case EXT_INT2:
            TRISBbits.TRISB2 = 1;
            INTCON2bits.INTEDG2 = edge;
            break;
        default:
            break;
    }
}

// Habilita a interrupção externa escolhida
void ext_int_enable(ext_int_t source)
{
    switch (source) {
        case EXT_INT0:
            INTCONbits.INT0IF = 0;
            INTCONbits.INT0IE = 1;
            break;
        case EXT_INT1:
            INTCON3bits.INT1IF = 0;
            INTCON3bits.INT1IE = 1;
            break;
        case EXT_INT2:
            INTCON3bits.INT2IF = 0;
            INTCON3bits.INT2IE = 1;
            break;
        default:
            break;
    }
}

// Desabilita a interrupção externa escolhida
void ext_int_disable(ext_int_t source)
{
    switch (source) {
        case EXT_INT0:
            INTCONbits.INT0IE = 0;
            break;
        case EXT_INT1:
            INTCON3bits.INT1IE = 0;
            break;
        case EXT_INT2:
            INTCON3bits.INT2IE = 0;
            break;
        default:
            break;
    }
}

// Limpa flag pendente da interrupção externa
void ext_int_clear_flag(ext_int_t source)
{
    switch (source) {
        case EXT_INT0:
            INTCONbits.INT0IF = 0;
            break;
        case EXT_INT1:
            INTCON3bits.INT1IF = 0;
            break;
        case EXT_INT2:
            INTCON3bits.INT2IF = 0;
            break;
        default:
            break;
    }
}

