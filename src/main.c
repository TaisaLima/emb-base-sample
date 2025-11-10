#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/logging/log.h>

#define BLINK_TIME_MS   400
#define FADE_STEPS      50 
#define FADE_DELAY_US   (10000 / FADE_STEPS)

#define Led2_NODE DT_ALIAS(led2)
#define LED1_NODE DT_ALIAS(led1)
#define BUTTON_NODE DT_ALIAS(sw0)

static const struct gpio_dt_spec led2 = GPIO_DT_SPEC_GET(Led2_NODE, gpios);
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(BUTTON_NODE, gpios);
static const struct device* const console_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));

static struct gpio_callback button_cb;
static volatile bool pwm_mode = false;

void soft_pwm_set(int duty_cycle_step) {
    
    int high_us = duty_cycle_step * FADE_DELAY_US;
    int low_us = (FADE_STEPS - duty_cycle_step) * FADE_DELAY_US;
    
    // Simula um pulso único
    gpio_pin_set_dt(&led1, 1);
    k_usleep(high_us);
    gpio_pin_set_dt(&led1, 0);
    k_usleep(low_us);

}

void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    pwm_mode = !pwm_mode;
    
    if (pwm_mode) {
        gpio_pin_set_dt(&led2, 0); 
    } else {
        gpio_pin_set_dt(&led1, 0); // Desliga o LED de fade
    }

    printk("Botão pressionado → modo: %s\n", pwm_mode ? "PWM (fade)" : "Digital (blink)");
}

int main(void)
{

    unsigned char c;

    if (!device_is_ready(led2.port)) {
        printk("Error: Led2 port is not ready\n");
        return 0;
    }
    gpio_pin_configure_dt(&led2, GPIO_OUTPUT_INACTIVE);

    if (!device_is_ready(led1.port)) {
        printk("Error: Led1 port is not ready\n");
        return 0;
    }
    gpio_pin_configure_dt(&led1, GPIO_OUTPUT_INACTIVE);


    if (!device_is_ready(button.port)) {
        printk("Error: button port is not ready\n");
        return 0;
    }
    gpio_pin_configure_dt(&button, GPIO_INPUT);
    gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);

    gpio_init_callback(&button_cb, button_pressed, BIT(button.pin));
    gpio_add_callback(button.port, &button_cb);

    printk("Press the button or ENTER to switch mode\n");

    while (1) {

        if (!uart_poll_in(console_dev, &c) && (c == '\n' || c == '\r')) {
            button_pressed(button.port, &button_cb, BIT(button.pin));
        }
        
        if (!pwm_mode) {
            // Modo Digital (Blink)
            gpio_pin_set_dt(&led2, 1);
            k_msleep(BLINK_TIME_MS);
            gpio_pin_set_dt(&led2, 0);
            k_msleep(BLINK_TIME_MS);

        } else {
            // Modo Soft-PWM (Fade)
            
            // Fade-in
            for (int step = 0; step <= FADE_STEPS; step++) {
                soft_pwm_set(step);
            }
            // Fade-out
            for (int step = FADE_STEPS; step >= 0; step--) {
                soft_pwm_set(step);
            }
        }
    }

    return 0;
}
