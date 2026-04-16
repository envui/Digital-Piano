#include <stdint.h>
#include "tm4c123gh6pm.h"

#include "PLL.h"
#include "SysTick.h"
#include "LED.h"
#include "Keys_PC4_7.h"
#include "SW1_PF4.h"
#include "S2_AudioDAC.h"
#include "Songs.h"

extern void EnableInterrupts(void);
extern void DisableInterrupts(void);
extern void WaitForInterrupt(void);

#define MODE_PIANO    0
#define MODE_AUTOPLAY 1

static volatile uint8_t g_mode   = MODE_PIANO;
static volatile uint8_t g_octave = 0;     // 0=C4,1=C5,2=C6
static volatile uint8_t g_song   = 0;     // 0..SONG_COUNT-1
static volatile uint8_t g_forceExit = 0;

static volatile uint8_t g_keyDown = 0;
static volatile uint8_t g_keyToneIndex = PAUSE;

static void Debounce_2ms(void){
  volatile uint32_t t = (BUSCLK_HZ/5000);
  while(t){ t--; }
}

static uint8_t KeyToToneIndex(uint8_t key, uint8_t octave){
  static const uint8_t base[4] = { 0,1,2,3 }; // C,D,E,F
  return (uint8_t)(base[key] + (uint8_t)(7*octave));
}

static void StopTone(void){
  S2_Audio_Silence();
  LED_BlueOff();
  g_keyDown = 0;
  g_keyToneIndex = PAUSE;
}

static void StartTone(uint8_t toneIndex){
  if(toneIndex == PAUSE){
    StopTone();
    return;
  }
  S2_Audio_PlayToneIndex(toneIndex);
  LED_BlueOn();
  g_keyDown = 1;
  g_keyToneIndex = toneIndex;
}

// --- minimal local autoplay loop (same logic as your Audio.c version) ---
static uint8_t Delay_0p1s_Abortable(uint8_t units,
                                   volatile uint8_t *modePtr,
                                   volatile uint8_t *forceExitPtr,
                                   volatile uint8_t *songSelPtr,
                                   uint8_t songStart)
{
  #define DELAY_0P1S_16  (727240UL*20UL/91UL)
  uint32_t delayNow = (uint32_t)((DELAY_0P1S_16*(uint64_t)BUSCLK_HZ + (BASECLK_HZ/2))/BASECLK_HZ);

  while(units--){
    uint32_t chunk = delayNow / 10UL;
    for(uint8_t k=0; k<10; k++){
      if(*modePtr != MODE_AUTOPLAY) return 1;
      if(*forceExitPtr) return 1;
      if(*songSelPtr != songStart) return 1;
      volatile uint32_t t = chunk;
      while(t){ t--; }
    }
  }
  return 0;
}

static void AutoPlayLoop(uint8_t octave,
                         volatile uint8_t *songSelPtr,
                         volatile uint8_t *modePtr,
                         volatile uint8_t *forceExitPtr)
{
  while(*modePtr == MODE_AUTOPLAY){

    if(*forceExitPtr){
      StopTone();
      return;
    }

    uint8_t songStart = *songSelPtr;

    uint8_t i = 0;
    while(mysong[songStart][i].delay){

      if(*modePtr != MODE_AUTOPLAY){ StopTone(); return; }
      if(*forceExitPtr){ StopTone(); return; }

      if(*songSelPtr != songStart){
        StopTone();
        break;
      }

      uint8_t idx = mysong[songStart][i].tone_index;

      if(idx == PAUSE){
        StopTone();
      } else {
        uint8_t base = (uint8_t)(idx % 7);
        uint8_t shifted = (uint8_t)(base + (uint8_t)(7*octave));
        if(shifted > 20) shifted = 20;
        StartTone(shifted);
      }

      if(Delay_0p1s_Abortable(mysong[songStart][i].delay,
                              modePtr, forceExitPtr, songSelPtr, songStart))
      {
        StopTone();
        if(*songSelPtr != songStart) break;
        return;
      }

      StopTone();
      i++;
    }

    if(*modePtr != MODE_AUTOPLAY) return;
    if(*forceExitPtr) return;

    if(*songSelPtr == songStart){
      if(Delay_0p1s_Abortable(15, modePtr, forceExitPtr, songSelPtr, songStart)){
        StopTone();
        if(*songSelPtr != songStart) continue;
        return;
      }
    }
  }
}

void GPIOPortC_Handler(void){
  uint32_t status = GPIO_PORTC_RIS_R;
  GPIO_PORTC_ICR_R = status;

  if(g_mode == MODE_PIANO){
    uint8_t k = Keys_GetOneKey();
    if(k == 0xFF) StopTone();
    else StartTone(KeyToToneIndex(k, g_octave));
  }
}

void GPIOPortF_Handler(void){
  uint32_t status = GPIO_PORTF_RIS_R;
  GPIO_PORTF_ICR_R = status;

  Debounce_2ms();

  // SW1: toggle mode (RESET SONG LIST ON ENTRY)
  if(Switches_SW1_Raw() == 0){
    StopTone();
    g_mode ^= 1;
    g_forceExit = 1;

    // NEW: whenever we ENTER autoplay, restart song list at the start
    if(g_mode == MODE_AUTOPLAY){
      g_song = 0;
    }
    return;
  }

  // SW2: octave or song
  if(Switches_SW2_Raw() == 0){
    if(g_mode == MODE_PIANO){
      g_octave = (uint8_t)((g_octave + 1) % 3);
      LED_GreenBlink();

      if(g_keyDown){
        uint8_t k = Keys_GetOneKey();
        if(k != 0xFF) StartTone(KeyToToneIndex(k, g_octave));
        else StopTone();
      }
    } else {
      g_song = (uint8_t)((g_song + 1) % SONG_COUNT);
      g_forceExit = 1;
    }
    return;
  }
}

int main(void){
  DisableInterrupts();

  PLL_Init();

  LED_Init();
  Keys_Init_Disarmed();
  Switches_Init_Disarmed();

  SysTick_Init(2);

  S2_Audio_Init();
  S2_Audio_Select();

  Keys_ArmInterrupts();
  Switches_ArmInterrupts();

  EnableInterrupts();

  while(1){
    if(g_mode == MODE_AUTOPLAY){
      g_forceExit = 0;
      AutoPlayLoop(g_octave, &g_song, &g_mode, &g_forceExit);
    } else {
      WaitForInterrupt();
    }
  }
}
