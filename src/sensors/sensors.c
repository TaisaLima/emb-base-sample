#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

static const struct gpio_dt_spec sensor1 =
    GPIO_DT_SPEC_GET_BY_IDX(DT_PATH(zephyr_user), gpios, 0);

static const struct gpio_dt_spec sensor2 =
    GPIO_DT_SPEC_GET_BY_IDX(DT_PATH(zephyr_user), gpios, 1);

extern struct k_msgq sensor_msgq;

struct sensor_msg {
    uint32_t tempo_ms;
    uint8_t num_eixos;
};


enum sensor_state {
    ST_IDLE = 0,
    ST_PRIMEIRO_PULSO,
    ST_CONTANDO_EIXOS
};

static volatile enum sensor_state estado = ST_IDLE;

static volatile uint8_t eixo_count = 0;
static volatile int64_t t0 = 0;

static void sensor1_cb(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    switch (estado) {
    case ST_IDLE:
        eixo_count = 1;
        t0 = k_uptime_get();
        estado = ST_PRIMEIRO_PULSO;
        break;

    case ST_PRIMEIRO_PULSO:
        eixo_count++;
        estado = ST_CONTANDO_EIXOS;
        break;

    case ST_CONTANDO_EIXOS:
        eixo_count++;
        break;
    }
}

static void sensor2_cb(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    if (estado == ST_PRIMEIRO_PULSO || estado == ST_CONTANDO_EIXOS) {

        int64_t dt = k_uptime_get() - t0;

        struct sensor_msg msg = {
            .tempo_ms = (uint32_t)dt,
            .num_eixos = eixo_count
        };

        k_msgq_put(&sensor_msgq, &msg, K_NO_WAIT);

        /* Reset FSM */
        estado = ST_IDLE;
        eixo_count = 0;
    }
}


void sensor_thread(void)
{
    gpio_pin_configure_dt(&sensor1, GPIO_INPUT);
    gpio_pin_configure_dt(&sensor2, GPIO_INPUT);

    static struct gpio_callback cb1, cb2;

    gpio_pin_interrupt_configure_dt(&sensor1, GPIO_INT_EDGE_RISING);
    gpio_pin_interrupt_configure_dt(&sensor2, GPIO_INT_EDGE_RISING);

    gpio_init_callback(&cb1, sensor1_cb, BIT(sensor1.pin));
    gpio_init_callback(&cb2, sensor2_cb, BIT(sensor2.pin));

    gpio_add_callback(sensor1.port, &cb1);
    gpio_add_callback(sensor2.port, &cb2);

    printk("Sensores prontos. FSM iniciada.\n");

    while (1) {
        k_sleep(K_FOREVER);
    }
}
