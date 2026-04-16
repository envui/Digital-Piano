#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "LED.h"

static void DelayTiny(void){
  volatile uint32_t t = 80000;
  while(t){ t--; }
}

void LED_Init(void){
  volatile uint32_t delay;
  SYSCTL_RCGCGPIO_R |= 0x20;
  delay = SYSCTL_RCGCGPIO_R;

  GPIO_PORTF_DIR_R |= 0x0C;   // PF2 blue, PF3 green
  GPIO_PORTF_DEN_R |= 0x0C;
  GPIO_PORTF_AFSEL_R &= ~0x0C;
  GPIO_PORTF_AMSEL_R &= ~0x0C;
  GPIO_PORTF_PCTL_R &= ~0x0000FF00;
}

void LED_BlueOn(void){  GPIO_PORTF_DATA_R |=  0x04; }
void LED_BlueOff(void){ GPIO_PORTF_DATA_R &= ~0x04; }

void LED_GreenBlink(void){
  GPIO_PORTF_DATA_R |= 0x08;
  DelayTiny();
  GPIO_PORTF_DATA_R &= ~0x08;
}
