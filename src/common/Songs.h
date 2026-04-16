#ifndef SONGS_H
#define SONGS_H

#include <stdint.h>

struct Note{
  uint8_t tone_index;
  uint8_t delay;
};
typedef const struct Note NTyp;

#define PAUSE 255

// NOTE INDEXES (unchanged)
#define C4 0
#define D4 1
#define E4 2
#define F4 3
#define G4 4
#define A4 5
#define B4 6
#define C5 0+7
#define D5 1+7
#define E5 2+7
#define F5 3+7
#define G5 4+7
#define A5 5+7
#define B5 6+7
#define C6 0+2*7
#define D6 1+2*7
#define E6 2+2*7
#define F6 3+2*7
#define G6 4+2*7
#define A6 5+2*7
#define B6 6+2*7

extern const uint32_t Tone_Tab[];
extern NTyp mysong[][255];

// song IDs
#define SONG_HBDAY    2
#define SONG_MARY     0
#define SONG_TWINKLE  1
#define SONG_RETRO    3   // NEW

#define SONG_COUNT    4   // UPDATED

#endif
