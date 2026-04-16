#include <stdint.h>
#include "Songs.h"

// Tone_Tab[] unchanged...
const uint32_t Tone_Tab[] = {
  30534,27211,24242,22923,20408,18182,16194,
  15289,13621,12135,11454,10204, 9091, 8099,
   7645, 6810, 6067, 5727, 5102, 4545, 4050
};

// mysong[][] unchanged for first 3 songs...
NTyp mysong[][255] = {

  // 1) Mary Had a Little Lamb (your existing)
  { E4,4,D4,4,C4,4,D4,4,E4,4,E4,4,E4,8,
    D4,4,D4,4,D4,8,E4,4,G4,4,G4,8,
    E4,4,D4,4,C4,4,D4,4,E4,4,E4,4,E4,8,
    D4,4,D4,4,E4,4,D4,4,C4,8, 0,0 },

  // 2) Twinkle Twinkle (your existing)
  { C4,4,C4,4,G4,4,G4,4,A4,4,A4,4,G4,8,
    F4,4,F4,4,E4,4,E4,4,D4,4,D4,4,C4,8,
    G4,4,G4,4,F4,4,F4,4,E4,4,E4,4,D4,8,
    G4,4,G4,4,F4,4,F4,4,E4,4,E4,4,D4,8,
    C4,4,C4,4,G4,4,G4,4,A4,4,A4,4,G4,8,
    F4,4,F4,4,E4,4,E4,4,D4,4,D4,4,C4,8, 0,0 }, 
		// 0) Happy Birthday (your existing)
		{C4,2,C4,2,D4,4,C4,4,F4,4,E4,8,
			C4,2,C4,2,D4,4,C4,4,G4,4,F4,8,
			C4,2,C4,2,C5,4,A4,4,F4,4,E4,4,D4,8,
			B4,2,B4,2,A4,4,F4,4,G4,4,F4,12,0,0 },

  // 3) NEW: Retro 8-bit “Adventure” theme (C/D/E/F only)
  // Uses octave jumps + short pulses to feel chiptune-y.
  // (Ends with 0,0 like your other tables.)
 
		
		
		{
		// Fa La Si Fa La Si Fa La Si Mi' Re' Si Do' Si
		F4,4, A4,4, B4,4,  F4,4, A4,4, B4,4,  F4,4, A4,4, B4,4,  E5,4, D5,4, C5,4, C5,4, B4,8,

		// Sol Mi Re Mi Sol Mi
		G4,4, E4,4, D4,4, E4,4, G4,4, E4,8,

		// Fa La Si Fa La Si Fa La Si Mi' Re' Si Do' Mi'
		F4,4, A4,4, B4,4,  F4,4, A4,4, B4,4,  F4,4, A4,4, B4,4,  E5,4, D5,4, B4,4, C5,4, E5,8,

		// Si Si Sol Re Mi
		B4,4, B4,4, G4,4, D4,4, E4,8,

		// Re Mi Fa Sol La Si Do' Si Mi Re Mi Fa Sol La
		D4,4, E4,4, F4,4, G4,4, A4,4, B4,4, C5,4, B4,4, E4,4, D4,4, E4,4, F4,4, G4,4, A4,8,

		// Si Do' Si Mi'
		B4,4, C5,4, B4,4, E5,8,

		// Re Mi Fa Sol La Si Do' Si Mi
		D4,4, E4,4, F4,4, G4,4, A4,4, B4,4, C5,4, B4,4, E4,8,

		// Mi Re Fa Mi Sol Fa La Sol Si La Do' Si Re' Do'
		E4,4, D4,4, F4,4, E4,4, G4,4, F4,4, A4,4, G4,4, B4,4, A4,4, C5,4, B4,4, D5,4, C5,8,

		// Mi' Fa' Re' Mi'
		E5,4, F5,4, D5,4, E5,8,

		0,0
		}
		
 
};
