#include <xc.h>

#include "user_app.h"
#include "syscall.h"
#include "sync.h"
#include "pipe.h"
#include "mem.h"
#include "io.h"
#include "mutex.h"

#if EXAMPLE_1 == YES
// Exemplo 1

//sem_t semaforo_teste;
pipe_t *canal;

void config_app(void)
{
    TRISDbits.RD0 = TRISDbits.RD1 = TRISDbits.RD2 = 0;
    
    TRISCbits.RC0 = TRISCbits.RC1                 = 0;             
    
    //sem_init(&semaforo_teste, 0);
    canal = pipe_create(PIPE_MAX_SIZE);
    
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
        write_pipe(canal, dados[index]);
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
        read_pipe(canal, &dado);
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

#elif EXAMPLE_DRONE == YES

// Mensagem simples “|M1|VEL_M1|...|M4|VEL_M4|” compartilhada via mutex
typedef struct {
    uint8_t payload[8];        // pares motor/velocidade
    uint8_t nova_mensagem;     // flag indicando dado inédito
} motor_mailbox_t;

static motor_mailbox_t g_motor_mailbox;
static Mutex g_motor_mutex;
static uint8_t g_motor_duty[4]; // última velocidade aplicada a cada motor

static void aplicar_comando_motor(const uint8_t *payload);

void config_app(void)
{
    // Configura pinos imaginários dos motores como saída
    TRISCbits.TRISC0 = 0;
    TRISCbits.TRISC1 = 0;
    TRISCbits.TRISC2 = 0;
    TRISCbits.TRISC3 = 0;

    mutex_init(&g_motor_mutex);
    g_motor_mailbox.nova_mensagem = 0;

    for (uint8_t i = 0; i < sizeof(g_motor_mailbox.payload); i++) {
        g_motor_mailbox.payload[i] = 0;
    }

    for (uint8_t motor = 0; motor < 4; motor++) {
        g_motor_duty[motor] = 0;
    }

    asm("GLOBAL _tarefa_controle_central, _tarefa_controle_motores, _tarefa_monitor_bateria, _tarefa_sensores_inerciais");
}

// Gera comandos educacionais para os motores e os publica no mailbox
TASK tarefa_controle_central(void)
{
    uint8_t perfil = 0;

    while (1) {
        if (mutex_lock(&g_motor_mutex)) {
            // Preenche pares |M?|VEL|
            for (uint8_t motor = 0; motor < 4; motor++) {
                uint8_t indice = motor * 2;
                uint8_t velocidade = 40 + (uint8_t)((perfil + motor) % 4) * 10;

                g_motor_mailbox.payload[indice]     = motor + 1;
                g_motor_mailbox.payload[indice + 1] = velocidade;
            }

            g_motor_mailbox.nova_mensagem = 1;
            mutex_unlock(&g_motor_mutex);
        }

        perfil = (perfil + 1) % 4;
        os_delay(20); // ritmo de atualização do controle central
    }
}

// Consome a mensagem compartilhada e aplica os valores nos motores
TASK tarefa_controle_motores(void)
{
    uint8_t comando_local[8];

    while (1) {
        uint8_t possui_comando = 0;

        if (mutex_lock(&g_motor_mutex)) {
            if (g_motor_mailbox.nova_mensagem) {
                for (uint8_t i = 0; i < sizeof(comando_local); i++) {
                    comando_local[i] = g_motor_mailbox.payload[i];
                }
                g_motor_mailbox.nova_mensagem = 0;
                possui_comando = 1;
            }
            mutex_unlock(&g_motor_mutex);
        }

        if (possui_comando) {
            aplicar_comando_motor(comando_local);
        }

        os_delay(10); // janela para atualizar suavemente os motores
    }
}

// Placeholder para monitor de bateria (será refinado nas demais tarefas)
TASK tarefa_monitor_bateria(void)
{
    while (1) {
        // Comentário: aqui faria a leitura do ADC da bateria e registraria alarmes
        os_delay(100);
    }
}

// Placeholder para leitura dos sensores inerciais (giroscópio/acelerômetro)
TASK tarefa_sensores_inerciais(void)
{
    while (1) {
        // Comentário: aqui coletaríamos dados IMU e disponibilizaríamos para controle
        os_delay(50);
    }
}

// Interpreta a mensagem |M|VEL| e atualiza o dever dos quatro motores
static void aplicar_comando_motor(const uint8_t *payload)
{
    for (uint8_t i = 0; i < 8; i += 2) {
        uint8_t motor_id = payload[i];
        uint8_t velocidade = payload[i + 1];

        if (motor_id >= 1 && motor_id <= 4) {
            g_motor_duty[motor_id - 1] = velocidade;

            // Comentário: em hardware real, ajustaríamos o PWM correspondente aqui
        }
    }
}

#endif
