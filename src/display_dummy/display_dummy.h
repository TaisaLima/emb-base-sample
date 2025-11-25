#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdbool.h>
#include <zephyr/zbus/zbus.h>

enum cor_status {
    COR_VERDE = 0,
    COR_AMARELO,
    COR_VERMELHO
};

struct status_msg {
    int velocidade;      /* km/h */
    int eixos;           /* número de eixos detectados */
    enum cor_status cor; /* verde/amarelo/vermelho */
    bool infracao;       /* flag */
    char placa[8];       /* placa da possível infração */
};

ZBUS_CHAN_DECLARE(status_out);

void display_thread(void);

#endif 
