// PLL.c
#include "PLL.h"
#include "tm4c123gh6pm.h"

void PLL_Init(void){
  SYSCTL_RCC2_R |= SYSCTL_RCC2_USERCC2;      // advanced RCC2
  SYSCTL_RCC2_R |= SYSCTL_RCC2_BYPASS2;      // bypass PLL during init

  SYSCTL_RCC_R &= ~SYSCTL_RCC_XTAL_M;
  SYSCTL_RCC_R += SYSCTL_RCC_XTAL_16MHZ;     // 16 MHz crystal

  SYSCTL_RCC2_R &= ~SYSCTL_RCC2_OSCSRC2_M;
  SYSCTL_RCC2_R += SYSCTL_RCC2_OSCSRC2_MO;   // main oscillator

  SYSCTL_RCC2_R &= ~SYSCTL_RCC2_PWRDN2;      // activate PLL
  SYSCTL_RCC2_R |= SYSCTL_RCC2_DIV400;       // 400MHz PLL

  SYSCTL_RCC2_R = (SYSCTL_RCC2_R & ~0x1FC00000) + (SYSDIV2 << 22);

  while((SYSCTL_RIS_R & SYSCTL_RIS_PLLLRIS) == 0){}; // wait lock
  SYSCTL_RCC2_R &= ~SYSCTL_RCC2_BYPASS2;     // use PLL
}

