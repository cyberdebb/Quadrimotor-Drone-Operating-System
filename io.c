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

