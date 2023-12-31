#ifndef __TIMER_LIB_H
#define __TIMER_LIB_H

#include <stddef.h>
#include <stdint.h>
#include "asm_utils.h"


#define TIMER0_ADDR 0x10011000
#define TIMER1_ADDR 0x10012000
#define TIMER2_ADDR 0x10018000
#define TIMER3_ADDR 0x10019000

typedef volatile struct {
  uint32_t Timer1Load;       
  uint32_t Timer1Value;      
  uint32_t Timer1Ctrl;       
  uint32_t Timer1IntClr;     
  uint32_t Timer1RIS;        
  uint32_t Timer1MIS;        
  uint32_t Timer1BGLoad;     
  reserved_bits( 0, 0x001C, 0x001F );
  uint32_t Timer2Load;       
  uint32_t Timer2Value;      
  uint32_t Timer2Ctrl;       
  uint32_t Timer2IntClr;     
  uint32_t Timer2RIS;        
  uint32_t Timer2MIS;        
  uint32_t Timer2BGLoad;     
  reserved_bits( 1, 0x003C, 0x0EFF );
  uint32_t TimerITCR;        
  uint32_t TimerITOP;        
  reserved_bits( 2, 0x0F08, 0x0FDF );
  uint32_t PeriphID[ 4 ];    
  uint32_t  PCellID[ 4 ];    
} _timer_t;

#endif // __TIMER_LIB_H
