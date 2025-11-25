#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>
#include <zephyr/zbus/zbus.h>
#include "sensors.h"

/* ---------------- CONFIGURAÇÕES VIA KCONFIG ---------------- */
#define DISTANCIA_ENTRE_SENSORES_M   CONFIG_DISTANCIA_SENSORES_M
#define LIMITE_AMARELO_KMH           CONFIG_LIMITE_AMARELO_KMH
#define LIMITE_VERMELHO_KMH          CONFIG_LIMITE_VERMELHO_KMH

/* ------------------- ESTRUTURAS DO PROJETO ------------------- */

struct controller_status {
    uint8_t num_eixos;
    uint32_t velocidade_kmh;
    uint8_t estado;   // 0=VERDE, 1=AMARELO, 2=VERMELHO
};

/* Canis ZBUS para a câmera e para o display */
ZBUS_CHAN_DECLARE(camera_trigger);
ZBUS_CHAN_DECLARE(display_status);

/* Fila de mensagens dos sensores */
extern struct k_msgq sensor_msgq;

/* --------------------- FUNÇÃO DE VELOCIDADE --------------------- */

static uint32_t calcular_velocidade_kmh(uint32_t tempo_ms)
{
    if (tempo_ms == 0) return 0;

    float tempo_s = tempo_ms / 1000.0f;
    float velocidade_ms = DISTANCIA_ENTRE_SENSORES_M / tempo_s;   // m/s
    float velocidade_kmh = velocidade_ms * 3.6f;

    return (uint32_t)velocidade_kmh;
}


static uint8_t classificar_estado(uint32_t velocidade)
{
    if (velocidade <= LIMITE_AMARELO_KMH) {
        return 0; 
    } 
    else if (velocidade <= LIMITE_VERMELHO_KMH) {
        return 1; 
    } 
    else {
        return 2; 
    }
}


void controller_thread(void)
{
    printk("[Controller] Iniciado.\n");

    struct sensor_msg msg;

    while (1) {

        k_msgq_get(&sensor_msgq, &msg, K_FOREVER);

        uint32_t velocidade = calcular_velocidade_kmh(msg.tempo_ms);
        uint8_t estado = classificar_estado(velocidade);

        struct controller_status status = {
            .num_eixos = msg.num_eixos,
            .velocidade_kmh = velocidade,
            .estado = estado
        };

        zbus_chan_pub(&display_status, &status, K_NO_WAIT);

        if (estado == 2) {  
            printk("[Controller] Infração detectada! Vel=%d km/h → disparando câmera.\n", velocidade);
            uint8_t dummy = 1; 
            zbus_chan_pub(&camera_trigger, &dummy, K_NO_WAIT);
        }
    }
}
