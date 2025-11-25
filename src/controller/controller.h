#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>

struct controller_status {
    uint8_t num_eixos;       /* quantidade de eixos detectados */
    uint32_t velocidade_kmh; /* velocidade calculada */
    uint8_t estado;          /* 0=VERDE, 1=AMARELO, 2=VERMELHO */
};

ZBUS_CHAN_DECLARE(camera_trigger);
ZBUS_CHAN_DECLARE(display_status);

void controller_thread(void);

#endif 
