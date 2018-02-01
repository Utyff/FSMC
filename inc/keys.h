#ifndef KEYS_H
#define KEYS_H

#include "stm32f4xx_conf.h"
#include "pins.h"

#define BUTTON1 0x01
#define BUTTON2 0x02
#define BUTTON3 0x04
#define BUTTON4 0x08

extern u16 btns_state;

void KEYS_init();
void KEYS_scan();

#endif //KEYS_H
