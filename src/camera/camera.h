#ifndef CAMERA_H
#define CAMERA_H

#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>
#include <stdbool.h>


struct controller_trigger {
    uint8_t dummy;
};

struct camera_result_msg {
    bool placa_valida;   /* true = leitura OK, false = falha */
    char placa[8];       /* formato ABC123 */
};

ZBUS_CHAN_DECLARE(camera_trigger);
ZBUS_CHAN_DECLARE(camera_result);

void camera_thread(void);

#endif /* CAMERA_H */
