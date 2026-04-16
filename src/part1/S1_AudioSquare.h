#ifndef S1_AUDIO_SQUARE_H
#define S1_AUDIO_SQUARE_H

#include <stdint.h>

void S1_Audio_Init(void);
void S1_Audio_Select(void);
void S1_Audio_PlayToneIndex(uint8_t toneIndex);
void S1_Audio_Silence(void);

#endif
