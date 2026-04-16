#ifndef AUDIO_H
#define AUDIO_H

#include <stdint.h>
#include "Songs.h"

// System 2 audio (6-bit DAC on PB0..PB5)

void Audio_Init(void);

void Audio_PlayToneIndex(uint8_t toneIndex);
void Audio_Stop(void);

/*
 * Auto-play continuously at octave until:
 * - *modePtr != 1 (SW1 toggles mode)
 * - *songSelPtr changes (SW2) -> restart immediately
 * - *forceExitPtr becomes 1 -> return immediately
 */
void Audio_AutoPlayLoop(uint8_t octave,
                        volatile uint8_t *songSelPtr,
                        volatile uint8_t *modePtr,
                        volatile uint8_t *forceExitPtr);

#endif
