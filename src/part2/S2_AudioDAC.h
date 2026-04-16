#ifndef S2_AUDIO_DAC_H
#define S2_AUDIO_DAC_H

#include <stdint.h>

void S2_Audio_Init(void);
void S2_Audio_Select(void);
void S2_Audio_PlayToneIndex(uint8_t toneIndex);
void S2_Audio_Silence(void);

#endif
