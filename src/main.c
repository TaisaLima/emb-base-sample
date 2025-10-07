#include <zephyr.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>

#define INTERVAL_MS 10000  // Intervalo em milissegundos

LOG_MODULE_REGISTER(main);

void timer_handler(struct k_timer *dummy) {
    LOG_INF("Hello world com timer de %d ms", INTERVAL_MS);
    LOG_DBG("Hello world com timer de %d ms", INTERVAL_MS);
    LOG_ERR("Hello world com timer de %d ms", INTERVAL_MS);
}

// Define um timer
K_TIMER_DEFINE(my_timer, timer_handler, NULL);

void main(void)
{
    LOG_INF("Sistema inicializado. Intervalo do timer: %d ms", INTERVAL_MS);

    // Inicializa o timer para disparar periodicamente
    k_timer_start(&my_timer, K_MSEC(INTERVAL_MS), K_MSEC(INTERVAL_MS));
}
