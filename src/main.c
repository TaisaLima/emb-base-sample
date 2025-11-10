#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/random/random.h>

LOG_MODULE_REGISTER(Sensors_output);

#define RANGE_TEMP_MIN 18
#define RANGE_TEMP_MAX 30
#define RANGE_HUM_MIN 40
#define RANGE_HUM_MAX 70

#define STACK_SIZE 1024
#define PRIORITY 5

typedef enum {
    SENSOR_HUMIDITY = 0,
    SENSOR_TEMPERATURE,
} sensor_kind_t;

typedef struct {
    sensor_kind_t kind;
    union {
        uint8_t humidity;
        int8_t temperature;
    } value;
} sensor;

K_MSGQ_DEFINE(queue_raw, sizeof(sensor), 10, 4);
K_MSGQ_DEFINE(queue_valid, sizeof(sensor), 10, 4);

static sensor generate_sample(sensor_kind_t kind)
{
    sensor sample;
    sample.kind = kind;

    if (kind == SENSOR_HUMIDITY) {
        sample.value.humidity = sys_rand32_get() % 101;
    } else {
        sample.value.temperature = (sys_rand32_get() % 41);  
    }

    return sample;
}

static void humidity_producer(void)
{
    sensor s = generate_sample(SENSOR_HUMIDITY);
    k_msgq_put(&queue_raw, &s, K_FOREVER);
}

static void temperature_producer(void)
{
    sensor s = generate_sample(SENSOR_TEMPERATURE);
    k_msgq_put(&queue_raw, &s, K_FOREVER);
}

static int filter_task(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    sensor sample;

    while (1) {
        if (k_msgq_get(&queue_raw, &sample, K_FOREVER) == 0) {
            bool valid = false;

            switch (sample.kind) {
            case SENSOR_HUMIDITY:
                if (sample.value.humidity >= RANGE_HUM_MIN &&
                    sample.value.humidity <= RANGE_HUM_MAX) {
                    valid = true;
                } else {
                    LOG_WRN("Humidity out of acceptable values: %d%%", sample.value.humidity);
                }
                break;

            case SENSOR_TEMPERATURE:
                if (sample.value.temperature >= RANGE_TEMP_MIN &&
                    sample.value.temperature <= RANGE_TEMP_MAX) {
                    valid = true;
                } else {
                    LOG_WRN("Temperature out of acceptable values: %d°C", sample.value.temperature);
                }
                break;
            }

            if (valid) {
                k_msgq_put(&queue_valid, &sample, K_FOREVER);
            }
        }
    }

    return 0;
}

static int consumer_task(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    sensor s;

    while (1) {
        if (k_msgq_get(&queue_valid, &s, K_FOREVER) == 0) {
            if (s.kind == SENSOR_HUMIDITY) {
                LOG_INF("Humidity: %d%%", s.value.humidity);
            } else {
                LOG_INF("Temperature: %d°C", s.value.temperature);
            }
        }
    }

    return 0;
}

K_THREAD_DEFINE(tid_filter, STACK_SIZE, filter_task, NULL, NULL, NULL, PRIORITY, 0, 0);
K_THREAD_DEFINE(tid_consumer, STACK_SIZE, consumer_task, NULL, NULL, NULL, PRIORITY, 0, 0);


int main(void)
{
    LOG_INF("Sistema de sensores iniciado.");

    const uint32_t interval_ms = 500;

    while (1) {
        humidity_producer();
        k_msleep(interval_ms);

        temperature_producer();
        k_msleep(interval_ms);
    }

    return 0;
}
