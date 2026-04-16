#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "SysTick.h"
#include "S2_AudioDAC.h"
#include "Songs.h"
#include "PLL.h"

#define DAC_OUT (*((volatile uint32_t *)0x400050FC))
#define DAC_MASK 0x3F

static const uint8_t Sine64[64] = {
  32,35,38,41,44,47,50,52,
  55,57,59,61,62,63,63,63,
  63,63,62,61,59,57,55,52,
  50,47,44,41,38,35,32,29,
  26,23,20,17,14,12, 9, 7,
   4, 2, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 2, 4, 7, 9,12,
  14,17,20,23,26,29,32,32
};

static volatile uint8_t idx = 0;

static void S2_Task(void){
  DAC_OUT = (DAC_OUT & ~DAC_MASK) | (Sine64[idx] & DAC_MASK);
  idx = (uint8_t)((idx + 1) & 63);
}

// Intended note Hz based on original 16MHz square-wave relationship:
// f_note = 16MHz / (2*Tone_Tab[idx])
static uint32_t NoteHz_FromToneTab16(uint8_t toneIndex){
  uint32_t r16 = Tone_Tab[toneIndex];
  if(r16 == 0) return 0;
  return (uint32_t)(BASECLK_HZ / (2UL * r16));
}

void S2_Audio_Init(void){
  volatile uint32_t delay;
  SYSCTL_RCGCGPIO_R |= 0x02;
  delay = SYSCTL_RCGCGPIO_R;

  GPIO_PORTB_DIR_R |= 0x3F;
  GPIO_PORTB_AFSEL_R &= ~0x3F;
  GPIO_PORTB_DEN_R |= 0x3F;
  GPIO_PORTB_AMSEL_R &= ~0x3F;
  GPIO_PORTB_PCTL_R &= ~0x00FFFFFF;

  idx = 0;
}

void S2_Audio_Select(void){
  SysTick_SetTask(S2_Task);
}

void S2_Audio_PlayToneIndex(uint8_t toneIndex){
  uint32_t hz = NoteHz_FromToneTab16(toneIndex);
  if(hz == 0){
    SysTick_Stop();
    return;
  }

  // sample rate = noteHz * 64 samples/cycle
  uint32_t sampleRate = hz * 64UL;

  // SysTick reload based on BUS clock (25MHz)
  uint32_t reload = BUSCLK_HZ / sampleRate;
  if(reload < 2) reload = 2;

  idx = 0;
  SysTick_SetReload(reload);
  SysTick_Start();
}

void S2_Audio_Silence(void){
  SysTick_Stop();
  DAC_OUT = (DAC_OUT & ~DAC_MASK);
}
