#include <stdint.h>
#include "tm4c123gh6pm.h"

#include "PLL.h"
#include "SysTick.h"
#include "LED.h"
#include "Keys_PC4_7.h"
#include "SW1_PF4.h"
#include "Audio.h"
#include "Songs.h"

extern void EnableInterrupts(void);
extern void DisableInterrupts(void);
extern void WaitForInterrupt(void);

volatile uint32_t Debug_BusClock = BUSCLK_HZ;

#define MODE_PIANO    0
#define MODE_AUTOPLAY 1

static volatile uint8_t g_mode   = MODE_PIANO;
static volatile uint8_t g_octave = 0;     // 0=C4,1=C5,2=C6
static volatile uint8_t g_song   = 0;     // 0..SONG_COUNT-1

// used to force Audio_AutoPlayLoop() to return immediately on SW2/mode change
static volatile uint8_t g_forceExit = 0;

// piano note state
static volatile uint8_t g_keyDown = 0;
static volatile uint8_t g_keyToneIndex = PAUSE;

static void Debounce_2ms(void){
  volatile uint32_t t = (BUSCLK_HZ/5000); // ~2ms-ish, rough
  while(t){ t--; }
}

static uint8_t KeyToToneIndex(uint8_t key, uint8_t octave){
  static const uint8_t base[4] = { 0,1,2,3 }; // C,D,E,F
  return (uint8_t)(base[key] + (uint8_t)(7*octave));
}

static void StopTone(void){
  Audio_Stop();
  LED_BlueOff();
  g_keyDown = 0;
  g_keyToneIndex = PAUSE;
}

static void StartTone(uint8_t toneIndex){
  if(toneIndex == PAUSE){
    StopTone();
    return;
  }
  Audio_PlayToneIndex(toneIndex);
  LED_BlueOn();
  g_keyDown = 1;
  g_keyToneIndex = toneIndex;
}

void GPIOPortC_Handler(void){
  uint32_t status = GPIO_PORTC_RIS_R;
  GPIO_PORTC_ICR_R = status;

  if(g_mode == MODE_PIANO){
    uint8_t k = Keys_GetOneKey();
    if(k == 0xFF){
      StopTone();
    } else {
      StartTone(KeyToToneIndex(k, g_octave));
    }
  }
}

void GPIOPortF_Handler(void){
  uint32_t status = GPIO_PORTF_RIS_R;
  GPIO_PORTF_ICR_R = status;

  Debounce_2ms();

  // ---------------- SW1: Toggle Piano/Auto-play ----------------
  if(Switches_SW1_Raw() == 0){
    StopTone();

    // toggle mode
    g_mode ^= 1;

    // force autoplay loop to exit immediately if we were in it
    g_forceExit = 1;

    // NEW: whenever we ENTER autoplay, restart song list at the start
    if(g_mode == MODE_AUTOPLAY){
      g_song = 0;   // start from first song every time
    }

    return;
  }

  // ---------------- SW2: Octave or Song ----------------
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
      g_forceExit = 1; // restart song immediately
    }
    return;
  }
}

int main(void){
  DisableInterrupts();

  PLL_Init();            // bus clock ~25MHz

  LED_Init();
  Keys_Init_Disarmed();
  Switches_Init_Disarmed();

  SysTick_Init(2);

  Audio_Init();          // (S1-only or S2-only Audio.c you chose)

  Keys_ArmInterrupts();
  Switches_ArmInterrupts();

  EnableInterrupts();

  while(1){
    if(g_mode == MODE_AUTOPLAY){
      g_forceExit = 0;
      Audio_AutoPlayLoop(g_octave, &g_song, &g_mode, &g_forceExit);
    } else {
      WaitForInterrupt();
    }
  }
}
