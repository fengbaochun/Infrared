#ifndef __CORE_DELAY_H
#define __CORE_DELAY_H

#include "main.h"
#include "stm32f4xx.h"

void delay_ms_(int nTimer)  ;
void delay_us_(int nTimer) ;

extern void delay_init(uint8_t SYSCLK);
	
#endif /* __CORE_DELAY_H */
