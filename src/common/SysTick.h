#ifndef SYSTICK_H
#define SYSTICK_H

#include <stdint.h>

void SysTick_Init(uint8_t priority);
void SysTick_SetTask(void (*task)(void));
void SysTick_SetReload(uint32_t reload);

void SysTick_Start(void);
void SysTick_Stop(void);

#endif
