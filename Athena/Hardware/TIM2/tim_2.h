#ifndef __TIM_2_H
#define __TIM_2_H

#include "stm32f10x.h"


#define TIM2_CH2_PORT 		GPIOA
#define TIM2_CH2_PIN  		GPIO_Pin_1
#define TIM2_CH3_PORT 		GPIOA
#define TIM2_CH3_PIN  		GPIO_Pin_2
#define TIM2_CH4_PORT 		GPIOA
#define TIM2_CH4_PIN  		GPIO_Pin_3

void TIM2_Init(uint16_t arr, uint16_t psc);


void Enable_TIMX_OCXInit(TIM_TypeDef* TIMX, void(*TIM_OCXInit)(TIM_TypeDef*,TIM_OCInitTypeDef* ));
// TIMÍ¨µÀX(1/2/3/4)Êä³öÆÁ±Î
void Disable_TIMX_OCXInit(TIM_TypeDef* TIMX, void(*TIM_OCXInit)(TIM_TypeDef*,TIM_OCInitTypeDef* ));


#endif
