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
        }
    }

    s16 step = Encoder_get();
    if (step == 0) return;

    // choose type of encoder action
    u8 action = button0Count % (u8) 3;
    if (action == 0) {
        ADC_step(step);
    } else if (action == 1) {
        DAC_step(step);
    } else {
        DAC_NextGeneratorSignal();
    }
}
