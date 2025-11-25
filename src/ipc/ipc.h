#ifndef IPC_H
#define IPC_H

#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>
#include <stdbool.h>


struct sensor_msg {
    uint8_t num_eixos;
    uint32_t tempo_ms;
};

struct display_status {
    float velocidade_kmh;
    uint8_t tipo_veiculo;   /* 0 leve, 1 pesado */
    uint8_t cor;            /* 0 verde, 1 amarelo, 2 vermelho */
    uint16_t limite_kmh;
};

struct camera_trigger {
    float velocidade_kmh;
    uint8_t tipo_veiculo;
};

struct camera_result {
    bool placa_valida;
    char placa[16];
};


struct k_msgq *get_sensor_msgq(void);

const struct zbus_channel *get_camera_trigger_chan(void);
const struct zbus_channel *get_camera_result_chan(void);
const struct zbus_channel *get_display_status_chan(void);


ZBUS_SUBSCRIBER_DECLARE(sub_controller);
ZBUS_SUBSCRIBER_DECLARE(sub_camera);
ZBUS_SUBSCRIBER_DECLARE(sub_display);

#endif 
