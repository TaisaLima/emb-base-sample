#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>
#include <zephyr/display/display.h>
#include <zephyr/zbus/zbus.h>

#include "ipc.h"
#include "sensors.h"
#include "controller.h"
#include "camera.h"
#include "display_dummy.h"   /* renomeado para evitar conflito com <display/display.h> */

 
LOG_MODULE_REGISTER(main_mod, LOG_LEVEL_INF);


static const struct device *dummy_display;

static void init_dummy_display(void)
{
    dummy_display = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
    if (!device_is_ready(dummy_display)) {
        LOG_ERR("Display Dummy NÃO está pronto! (verifique overlay DTS)");
        return;
    }

    LOG_INF("Display Dummy inicializado: %s", dummy_display->name);

    /* Limpa o display ao iniciar */
    display_blanking_off(dummy_display);
    struct display_buffer_descriptor desc = {
        .width = 1,
        .height = 1,
        .pitch = 1,
    };
    uint8_t blank = 0x00;
    display_write(dummy_display, 0, 0, &desc, &blank);
}

void main(void)
{
    LOG_INF("========================================");
    LOG_INF(" Radar Eletrônico - Sistema Iniciado");
    LOG_INF(" Plataforma: mps2_an385 (QEMU)");
    LOG_INF("========================================");

    LOG_INF("Inicializando IPC...");

    (void)get_sensor_msgq();
    (void)get_camera_trigger_chan();
    (void)get_camera_result_chan();
    (void)get_display_status_chan();

    LOG_INF("IPC OK.");

    init_dummy_display();

    LOG_INF("Inicializando módulos...");

    sensors_init();
    controller_init();
    camera_init();
    display_dummy_init();

    LOG_INF("Todos módulos inicializados.");

    LOG_INF("Sistema operacional. Threads ativas.");

    while (1) {
        k_sleep(K_FOREVER);
    }
}
