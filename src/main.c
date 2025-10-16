#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/printk.h>

LOG_MODULE_REGISTER(timer, LOG_LEVEL_DBG);
static struct k_timer timer;

static void handler(struct k_timer *timer_id)
{
	ARG_UNUSED(timer_id);

	printk("Hello World \n");
	LOG_DBG("Timer interval is %d ms", CONFIG_HELLO_TIMER_INTERV);
	LOG_INF("Hello World");
}

int main(void)
{
	k_timer_init(&timer, handler, NULL);
	k_timer_start(&timer, K_MSEC(CONFIG_HELLO_TIMER_INTERV), K_MSEC(CONFIG_HELLO_TIMER_INTERV));

	return 0;
}