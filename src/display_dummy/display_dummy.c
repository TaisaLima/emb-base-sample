#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/sys/printk.h>
#include "display.h"

/* Declara o canal onde o controller envia status */
ZBUS_CHAN_DECLARE(status_out);

/* Códigos ANSI */
#define ANSI_RESET    "\x1b[0m"
#define ANSI_RED      "\x1b[31m"
#define ANSI_GREEN    "\x1b[32m"
#define ANSI_YELLOW   "\x1b[33m"

/* Converte enum para cor ANSI */
static const char *cor_estado(enum cor_status c)
{
    switch (c) {
        case COR_VERDE:    return ANSI_GREEN;
        case COR_AMARELO:  return ANSI_YELLOW;
        case COR_VERMELHO: return ANSI_RED;
        default:           return ANSI_RESET;
    }
}

void display_thread(void)
{
    printk("Display: iniciado e aguardando mensagens...\n");

    while (1) {
        struct status_msg msg;

        zbus_chan_sub_wait(&status_out, &msg, K_FOREVER);

        const char *cor = cor_estado(msg.cor);

        printk("\n----------------------------------------------\n");
        printk("   DISPLAY DO RADAR\n");
        printk("----------------------------------------------\n");

        printk(" Velocidade: %d km/h\n", msg.velocidade);
        printk(" Eixos: %d\n", msg.eixos);

        printk(" Estado: %s%s%s\n",
               cor,
               (msg.cor == COR_VERDE)   ? "VERDE" :
               (msg.cor == COR_AMARELO) ? "AMARELO" :
               (msg.cor == COR_VERMELHO)? "VERMELHO" : "DESCONHECIDO",
               ANSI_RESET);

        if (msg.infracao) {
            printk(" Infração: SIM\n");
            printk(" Placa lida: %s\n", msg.placa);
        } else {
            printk(" Infração: NÃO\n");
        }

        printk("----------------------------------------------\n\n");
    }
}
