#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>
#include <stdbool.h>
#include <string.h>


struct sensor_msg {
    uint8_t num_eixos;     /* quantidade de eixos detectados */
    uint32_t tempo_ms;     /* timestamp ms do evento */
};

struct display_status {
    float velocidade_kmh;
    uint8_t tipo_veiculo;     /* 0 = leve, 1 = pesado */
    uint8_t cor;              /* 0 = verde, 1 = amarelo, 2 = vermelho */
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


#define SENSOR_MSGQ_MAX_MSG 10
#define SENSOR_MSG_SIZE sizeof(struct sensor_msg)

K_MSGQ_DEFINE(sensor_msgq, SENSOR_MSG_SIZE, SENSOR_MSGQ_MAX_MSG, 4);

struct k_msgq *get_sensor_msgq(void) {
    return &sensor_msgq;
}



ZBUS_SUBSCRIBER_DECLARE(sub_controller);
ZBUS_SUBSCRIBER_DECLARE(sub_camera);
ZBUS_SUBSCRIBER_DECLARE(sub_display);

/* Canal: controller - camera  */
ZBUS_CHAN_DEFINE(camera_trigger_chan,
    struct camera_trigger,
    NULL, NULL, ZBUS_OBSERVERS(sub_camera),
    ZBUS_MSG_INIT()
);

/* Canal: camera - controller  */
ZBUS_CHAN_DEFINE(camera_result_chan,
    struct camera_result,
    NULL, NULL, ZBUS_OBSERVERS(sub_controller),
    ZBUS_MSG_INIT()
);

/* Canal: controller - display  */
ZBUS_CHAN_DEFINE(display_status_chan,
    struct display_status,
    NULL, NULL, ZBUS_OBSERVERS(sub_display),
    ZBUS_MSG_INIT()
);


const struct zbus_channel *get_camera_trigger_chan(void) {
    return &camera_trigger_chan;
}

const struct zbus_channel *get_camera_result_chan(void) {
    return &camera_result_chan;
}

const struct zbus_channel *get_display_status_chan(void) {
    return &display_status_chan;
}
