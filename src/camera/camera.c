#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/sys/printk.h>
#include "camera.h"

/* ------------------- Configurações (via Kconfig) ------------------- */
#define LPR_DELAY_MS        CONFIG_LPR_PROCESS_DELAY_MS
#define LPR_FAIL_RATE       CONFIG_LPR_FAIL_RATE   /* % de falha 0–100 */

/* ------------------ Declaração dos canais ZBUS --------------------- */
ZBUS_CHAN_DECLARE(camera_trigger);
ZBUS_CHAN_DECLARE(camera_result);

/* Estrutura publicada como resultado da câmera */
struct camera_result_msg {
    bool placa_valida;
    char placa[8]; /* ABC1D23 */
};

static uint32_t rng_state = 123456;

/* Gerador linear congruente simples */
static uint32_t prng(void)
{
    rng_state = rng_state * 1103515245 + 12345;
    return (rng_state >> 16) & 0x7FFF;
}

/* Função que gera uma placa aleatória */
static void gerar_placa(char *buf)
{
    buf[0] = 'A' + (prng() % 26);
    buf[1] = 'A' + (prng() % 26);
    buf[2] = 'A' + (prng() % 26);
    buf[3] = '0' + (prng() % 10);
    buf[4] = '0' + (prng() % 10);
    buf[5] = '0' + (prng() % 10);
    buf[6] = '\0';
}


void camera_thread(void)
{
    printk("Camera thread iniciada. Aguardando triggers...\n");

    while (1)
    {
        struct controller_trigger trig_msg;
        zbus_chan_sub_wait(&camera_trigger, &trig_msg, K_FOREVER);

        printk("Camera: Trigger recebido! Processando LPR...\n");

        k_msleep(LPR_DELAY_MS);

        struct camera_result_msg retorno;
        gerar_placa(retorno.placa);

        uint32_t chance = prng() % 100;
        retorno.placa_valida = (chance >= LPR_FAIL_RATE);

        printk("Camera: placa=%s | valida=%d\n",
               retorno.placa, retorno.placa_valida);

        zbus_chan_pub(&camera_result, &retorno, K_NO_WAIT);
    }
}
