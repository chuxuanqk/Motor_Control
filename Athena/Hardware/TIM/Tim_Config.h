#ifndef __TIM_2_H
#define __TIM_2_H

#include "stm32f10x.h"


// 定时器TIM2
#define TIM2_CH2_PORT 		GPIOA
#define TIM2_CH2_PIN  		GPIO_Pin_1
#define TIM2_CH3_PORT 		GPIOA
#define TIM2_CH3_PIN  		GPIO_Pin_2
#define TIM2_CH4_PORT 		GPIOA
#define TIM2_CH4_PIN  		GPIO_Pin_3

// 定时器TIM3
#define TIM3_CH1_PORT 		GPIOA
#define TIM3_CH1_PIN  		GPIO_Pin_6
#define TIM3_CH2_PORT 		GPIOA
#define TIM3_CH2_PIN  		GPIO_Pin_7

// 定时器TIM4
#define TIM4_CH2_PORT 		GPIOB
#define TIM4_CH2_PIN  		GPIO_Pin_7

// 定时器初始化函数
void TIM2_Init(uint16_t arr, uint16_t psc);
void TIM3_Init(uint16_t arr, uint16_t psc);
void TIM4_Init(uint16_t arr, uint16_t psc);


void Enable_TIMX_OCXInit(TIM_TypeDef* TIMX, void(*TIM_OCXInit)(TIM_TypeDef*,TIM_OCInitTypeDef* ));
// TIM通道X(1/2/3/4)输出屏蔽
void Disable_TIMX_OCXInit(TIM_TypeDef* TIMX, void(*TIM_OCXInit)(TIM_TypeDef*,TIM_OCInitTypeDef* ));


#endif
