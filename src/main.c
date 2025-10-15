#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/printk.h>

LOG_MODULE_REGISTER(hello_timer, LOG_LEVEL_DBG);

static struct k_timer hello_timer;

static void hello_timer_handler(struct k_timer *timer_id)
{
	ARG_UNUSED(timer_id);

	static int cycle_count;
	cycle_count++;

	LOG_INF("Hello World #%d", cycle_count);
	printk("Hello World #%d (printk)\n", cycle_count);
	LOG_DBG("Timer interval is %d ms", CONFIG_HELLO_TIMER_INTERV);
}

int main(void)
{
	k_timer_init(&hello_timer, hello_timer_handler, NULL);
	k_timer_start(&hello_timer, K_MSEC(CONFIG_HELLO_TIMER_INTERV), K_MSEC(CONFIG_HELLO_TIMER_INTERV));

	return 0;
}