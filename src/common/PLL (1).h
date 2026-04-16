// PLL.h
#ifndef PLL_H
#define PLL_H

#include <stdint.h>

// 400MHz/(SYSDIV2+1). For 25MHz -> SYSDIV2=15
#define SYSDIV2 15

// Bus clock after PLL (for SYSDIV2=15)
#define BUSCLK_HZ  25000000UL

// These constants are what your Tone_Tab[] and Min He Delay() were designed for
#define BASECLK_HZ 16000000UL

// Scale a value derived for 16MHz to the current bus clock (rounded)
static inline uint32_t ScaleFrom16MHz(uint32_t x16){
  return (uint32_t)((x16 * (uint64_t)BUSCLK_HZ + (BASECLK_HZ/2)) / BASECLK_HZ);
}

void PLL_Init(void);

#endif
