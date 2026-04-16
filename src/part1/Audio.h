#ifndef AUDIO_H
#define AUDIO_H

#include <stdint.h>
#include "Songs.h"

#define AUDIO_SYS_S1 1
#define AUDIO_SYS_S2 2

void Audio_Init(void);
void Audio_SelectSystem(uint8_t sys);

void Audio_PlayToneIndex(uint8_t toneIndex);
void Audio_Stop(void);

/*
 * Auto-play continuously at octave until:
 * - modePtr != 1 (SW1)
 * - songSelPtr changes (SW2) -> restart immediately
 * - forceExitPtr becomes 1 (SW2/system change) -> return immediately
 */
void Audio_AutoPlayLoop(uint8_t octave,
                        volatile uint8_t *songSelPtr,
                        volatile uint8_t *modePtr,
                        volatile uint8_t *forceExitPtr);

#endif
