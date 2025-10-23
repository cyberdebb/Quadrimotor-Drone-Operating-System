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
static pipe_t *g_sensor_pipe;    // fila básica para mensagens dos sensores
static uint8_t g_sensor_amostras[3]; // guarda última leitura (pitch, roll, accel)

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

    for (uint8_t eixo = 0; eixo < 3; eixo++) {
        g_sensor_amostras[eixo] = 0;
    }

    g_sensor_pipe = pipe_create(3); // 3 bytes: pitch, roll e aceleração

    asm("GLOBAL _tarefa_controle_central, _tarefa_controle_motores, _tarefa_monitor_bateria, _tarefa_sensores_inerciais");
}

// Gera comandos educacionais para os motores e os publica no mailbox
TASK tarefa_controle_central(void)
{
    uint8_t perfil = 0;

    while (1) {
        // Aguarda nova amostra dos sensores (pitch, roll, aceleração)
        if (g_sensor_pipe != 0) {
            char leitura;
            read_pipe(g_sensor_pipe, &leitura);      // pitch
            g_sensor_amostras[0] = (uint8_t)leitura;
            read_pipe(g_sensor_pipe, &leitura);      // roll
            g_sensor_amostras[1] = (uint8_t)leitura;
            read_pipe(g_sensor_pipe, &leitura);      // aceleração
            g_sensor_amostras[2] = (uint8_t)leitura;
        }

        if (mutex_lock(&g_motor_mutex)) {
            // Preenche pares |M?|VEL|
            for (uint8_t motor = 0; motor < 4; motor++) {
                uint8_t indice = motor * 2;
                uint8_t velocidade = 40 + (uint8_t)((perfil + motor) % 4) * 10;

                // Ajuste simples usando pitch (amostras[0]) para ilustrar feedback
                velocidade += (uint8_t)(g_sensor_amostras[0] / 16);

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
    uint8_t amostra = 0;

    while (1) {
        // Comentário: leitura simulada de pitch, roll e aceleração
        uint8_t pitch  = 20 + (amostra % 40);      // variação educativa
        uint8_t roll   = 15 + ((amostra / 2) % 30);
        uint8_t accel  = 10 + ((amostra / 3) % 20);

        if (g_sensor_pipe != 0) {
            write_pipe(g_sensor_pipe, (char)pitch);
            write_pipe(g_sensor_pipe, (char)roll);
            write_pipe(g_sensor_pipe, (char)accel);
        }

        amostra++;
        os_delay(25); // taxa de atualização dos sensores
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
