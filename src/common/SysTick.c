#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "SysTick.h"

static void (*SysTickTask)(void) = 0;

void SysTick_SetTask(void (*task)(void)){
  SysTickTask = task;
}

void SysTick_Init(uint8_t priority){
  if(priority > 7) priority = 7;

  NVIC_ST_CTRL_R = 0;
  NVIC_ST_RELOAD_R = 0;
  NVIC_ST_CURRENT_R = 0;

  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R & 0x00FFFFFF) | ((uint32_t)priority << 29);
  NVIC_ST_CTRL_R = 0x06; // CLK_SRC=1, INTEN=1, ENABLE=0
}

void SysTick_SetReload(uint32_t reload){
  if(reload < 2) reload = 2;
  if(reload > 0x00FFFFFF) reload = 0x00FFFFFF;
  NVIC_ST_RELOAD_R = reload - 1;
}

void SysTick_Start(void){
  NVIC_ST_CURRENT_R = 0;
  NVIC_ST_CTRL_R |= 0x01;
}

void SysTick_Stop(void){
  NVIC_ST_CTRL_R &= ~0x01;
}

void SysTick_Handler(void){
  if(SysTickTask){
    SysTickTask();
  }
}
