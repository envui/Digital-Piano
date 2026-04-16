#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "SysTick.h"
#include "S1_AudioSquare.h"
#include "Songs.h"
#include "PLL.h"

#define PA2_MASKED (*((volatile uint32_t *)0x400043FC))
#define PA2_BIT    0x04

static void S1_Task(void){
  PA2_MASKED ^= PA2_BIT;
}

void S1_Audio_Init(void){
  volatile uint32_t delay;
  SYSCTL_RCGCGPIO_R |= 0x01;
  delay = SYSCTL_RCGCGPIO_R;

  GPIO_PORTA_DIR_R |= 0x04;
  GPIO_PORTA_AFSEL_R &= ~0x04;
  GPIO_PORTA_DEN_R |= 0x04;
  GPIO_PORTA_AMSEL_R &= ~0x04;
  GPIO_PORTA_PCTL_R &= ~0x00000F00;
}

void S1_Audio_Select(void){
  SysTick_SetTask(S1_Task);
}

void S1_Audio_PlayToneIndex(uint8_t toneIndex){
  uint32_t reload16 = Tone_Tab[toneIndex];      // table assumes 16MHz
  uint32_t reload   = ScaleFrom16MHz(reload16); // scale to 25MHz bus
  SysTick_SetReload(reload);
  SysTick_Start();
}

void S1_Audio_Silence(void){
  SysTick_Stop();
  PA2_MASKED &= ~PA2_BIT;
}
