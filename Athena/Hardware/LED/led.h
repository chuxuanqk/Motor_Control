#ifndef __LED_H
#define __LED_H

#include "sys.h"

// #define LED_ON GPIO_ResetBits(GPIOB, GPIO_Pin_5)
// #define LED_OFF  GPIO_SetBits(GPIOB, GPIO_Pin_5)
#define LED PBout(5)

void LED_Init(void);

#endif
