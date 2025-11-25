#ifndef SENSORS_H
#define SENSORS_H

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

struct sensor_msg {
    uint32_t tempo_ms;   /* tempo entre sensor1 → sensor2 */
    uint8_t  num_eixos;  /* quantidade de eixos detectados */
};

extern struct k_msgq sensor_msgq;

void sensor_thread(void);

#endif 
