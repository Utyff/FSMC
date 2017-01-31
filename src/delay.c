#include "delay.h"

static __IO uint32_t TimingDelay ;

void Delay(__IO uint32_t nTime)
{
  TimingDelay = nTime;

  while(TimingDelay != 0)
  {
	  for(int i=0; i<1000;  i++){i=i;}
	  TimingDelay --;
  }
}


void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  {
    TimingDelay--;
  }
}

