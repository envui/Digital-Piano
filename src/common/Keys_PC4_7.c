#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "Keys_PC4_7.h"

static uint8_t ReadRaw(void){
  // raw: PC4..7, negative logic -> pressed = 0
  return (uint8_t)((GPIO_PORTC_DATA_R >> 4) & 0x0F);
}

void Keys_Init_Disarmed(void){
  volatile uint32_t delay;
  SYSCTL_RCGCGPIO_R |= 0x04; // Port C
  delay = SYSCTL_RCGCGPIO_R;

  GPIO_PORTC_DIR_R &= ~0xF0;      // inputs
  GPIO_PORTC_DEN_R |= 0xF0;
  GPIO_PORTC_AFSEL_R &= ~0xF0;
  GPIO_PORTC_AMSEL_R &= ~0xF0;
  GPIO_PORTC_PCTL_R &= ~0xFFFF0000;

  // pull-ups for negative logic
  GPIO_PORTC_PUR_R |= 0xF0;

  // edge interrupt on both edges so release stops immediately
  GPIO_PORTC_IS_R  &= ~0xF0;      // edge
  GPIO_PORTC_IBE_R |=  0xF0;      // both edges
  GPIO_PORTC_ICR_R  =  0xF0;      // clear
  GPIO_PORTC_IM_R  &= ~0xF0;      // disarm

  NVIC_PRI0_R = (NVIC_PRI0_R & 0xFF00FFFF) | (3 << 21); // IRQ2 pri 3
  NVIC_EN0_R |= (1 << 2); // Port C
}

void Keys_ArmInterrupts(void){
  GPIO_PORTC_ICR_R = 0xF0;
  GPIO_PORTC_IM_R |= 0xF0;
}

void Keys_DisarmInterrupts(void){
  GPIO_PORTC_IM_R &= ~0xF0;
}

uint8_t Keys_ReadPressed4(void){
  // pressed=1 for convenience
  uint8_t raw = ReadRaw();       // pressed=0
  return (uint8_t)((~raw) & 0x0F);
}

uint8_t Keys_GetOneKey(void){
  uint8_t p = Keys_ReadPressed4();

  if(p == 0x01) return 0; // PC4
  if(p == 0x02) return 1; // PC5
  if(p == 0x04) return 2; // PC6
  if(p == 0x08) return 3; // PC7
  return 0xFF;
}
