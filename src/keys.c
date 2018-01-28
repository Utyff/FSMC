#include <exti.h>
#include <adc.h>
#include <encoder.h>
#include <dac.h>
#include "keys.h"


#define DEBOUNCING_CNT 1


void KEYS_init() {
    GPIO_InitTypeDef GPIO_InitStructure;

    // Enable GPIOA clock
    RCC_AHB1PeriphClockCmd(BTN1_CLOCKPORT, ENABLE);

    // Configure PA0 pin as input floating. DISCO button 1
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_InitStructure.GPIO_Pin = BTN1_PIN;
    GPIO_Init(BTN1_PORT, &GPIO_InitStructure);
}

static u16 debounceCnt = 0;
static u16 btn1_last = 0;

void KEYS_scan() {
    if (debounceCnt > 0) {
        debounceCnt--;
        return;
    }

    if ((GPIOA->IDR & GPIO_Pin_0) != btn1_last) {
        debounceCnt = DEBOUNCING_CNT;
        btn1_last ^= 1;
        if (btn1_last != 0) {
            button0Count++;
//          menu1Next();
        }
    }

    if ((button0Count & 1) == 0) {
        ADC_step(Encoder_get());
    } else {
        DAC_step(Encoder_get());
    }
}
