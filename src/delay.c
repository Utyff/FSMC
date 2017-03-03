#include "delay.h"


/*static __IO uint32_t TimingDelay;
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  {
    TimingDelay--;
  }
}//*/


void delay_us(uint32_t us) {
  DWT_Delay(us);
}

void delay_ms(uint32_t ms) {
  DWT_Delay_ms(ms);
}


void Delay(__IO uint32_t nTime)
{
  DWT_Delay_ms(nTime);
}
