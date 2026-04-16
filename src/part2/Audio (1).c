#include <stdint.h>
#include "Audio.h"
#include "SysTick.h"
#include "S2_AudioDAC.h"
#include "LED.h"
#include "PLL.h"
#include "Songs.h"

// Min He Delay() calibrated for 16MHz: ~0.1s
#define DELAY_0P1S_16  (727240UL*20UL/91UL)

// scale to current BUS clock (25MHz)
#define DELAY_0P1S_NOW ((uint32_t)((DELAY_0P1S_16*(uint64_t)BUSCLK_HZ + (BASECLK_HZ/2))/BASECLK_HZ))

static uint8_t Delay_0p1s_Abortable(uint8_t units,
                                   volatile uint8_t *modePtr,
                                   volatile uint8_t *forceExitPtr,
                                   volatile uint8_t *songSelPtr,
                                   uint8_t songStart)
{
  while(units--){
    uint32_t chunk = DELAY_0P1S_NOW / 10UL;

    for(uint8_t k=0; k<10; k++){
      if(*modePtr != 1) return 1;
      if(*forceExitPtr) return 1;
      if(*songSelPtr != songStart) return 1;

      volatile uint32_t t = chunk;
      while(t){ t--; }
    }
  }
  return 0;
}

void Audio_Init(void){
  S2_Audio_Init();
  S2_Audio_Select(); // SysTick task = DAC sample output
}

void Audio_Stop(void){
  SysTick_Stop();
  S2_Audio_Silence();
}

void Audio_PlayToneIndex(uint8_t toneIndex){
  if(toneIndex == PAUSE){
    Audio_Stop();
    return;
  }
  S2_Audio_PlayToneIndex(toneIndex);
}

void Audio_AutoPlayLoop(uint8_t octave,
                        volatile uint8_t *songSelPtr,
                        volatile uint8_t *modePtr,
                        volatile uint8_t *forceExitPtr)
{
  while(*modePtr == 1){

    if(*forceExitPtr){
      Audio_Stop();
      return;
    }

    uint8_t songStart = *songSelPtr;

    uint8_t i = 0;
    while(mysong[songStart][i].delay){

      if(*modePtr != 1) { Audio_Stop(); return; }
      if(*forceExitPtr) { Audio_Stop(); return; }

      if(*songSelPtr != songStart){
        Audio_Stop();
        break;
      }

      uint8_t idx = mysong[songStart][i].tone_index;

      if(idx == PAUSE){
        Audio_Stop();
        LED_BlueOff();
      } else {
        uint8_t base = (uint8_t)(idx % 7);
        uint8_t shifted = (uint8_t)(base + (uint8_t)(7*octave));
        if(shifted > 20) shifted = 20;

        Audio_PlayToneIndex(shifted);
        LED_BlueOn();
      }

      if(Delay_0p1s_Abortable(mysong[songStart][i].delay,
                              modePtr, forceExitPtr, songSelPtr, songStart))
      {
        Audio_Stop();
        LED_BlueOff();
        if(*songSelPtr != songStart) break;
        return;
      }

      Audio_Stop();
      LED_BlueOff();
      i++;
    }

    if(*modePtr != 1) return;
    if(*forceExitPtr) return;

    if(*songSelPtr == songStart){
      if(Delay_0p1s_Abortable(15, modePtr, forceExitPtr, songSelPtr, songStart)){
        Audio_Stop();
        LED_BlueOff();
        if(*songSelPtr != songStart) continue;
        return;
      }
    }
  }
}
