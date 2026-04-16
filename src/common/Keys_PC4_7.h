#ifndef KEYS_PC4_7_H
#define KEYS_PC4_7_H

#include <stdint.h>

// PC4..PC7 external keys, negative logic (pressed=0)
void Keys_Init_Disarmed(void);
void Keys_ArmInterrupts(void);
void Keys_DisarmInterrupts(void);

// returns bits for pressed keys: bit0=PC4, bit1=PC5, bit2=PC6, bit3=PC7 (pressed=1)
uint8_t Keys_ReadPressed4(void);

// returns one key index 0..3 if exactly one pressed, else 0xFF
uint8_t Keys_GetOneKey(void);

#endif
