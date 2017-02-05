#include "dwt.h"



inline uint32_t DWT_Get()
{
  return DWT->CYCCNT;
}


inline uint32_t DWT_GetDelta(uint32_t t0)
{
  return DWT->CYCCNT - t0;
}


void DWT_Init()
{
  if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk))
  {
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
  }
}

//volatile uint32_t ccc, cc2;

void DWT_Delay(uint32_t us) // microseconds
{
  uint32_t t0 = DWT_Get();
  uint32_t delta = us * (SystemCoreClock/1000000);
//  ccc = delta;

  while ( DWT_GetDelta(t0) < delta ) {
//	  ccc = DWT_GetDelta(t0);
//	  cc2 = DWT_Get();
  }
}


void DWT_Delay_With_Action(uint32_t us, int (*cond)(), void (*act)()) // microseconds
{
  uint32_t t0 = DWT_Get();
  uint32_t delta = us * (SystemCoreClock/1000000);
  static uint32_t trigger=0;

  while ( DWT_GetDelta(t0) < delta )
  {
	if( trigger==1 ) {
	  if( cond()==0 )
        trigger=0;
	}
	else {
      if( cond()==1 ) {
        trigger=1;
        act();
      }
	}
  }
}
