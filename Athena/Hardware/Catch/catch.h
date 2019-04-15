#ifndef __CATCH_H
#define __CATCH_H

#include "stm32f10x.h"
#include "tim8.h"

typedef enum 
{
	OPEN,           
	CLOSE,			
	OFF				
}sw;

extern uint8_t Catch_Status;

void Catch_GPIO_Init(void);
void Catch(sw SW);  


// 使能驱动器ENA
#define Catch_ENA_CLK    RCC_APB2Periph_GPIOC
#define Catch_ENA_PIN    GPIO_Pin_9
#define Catch_ENA_PORT   GPIOC

#define Catch_ENA_SET    GPIO_SetBits(Catch_ENA_PORT, Catch_ENA_PIN)
#define Catch_ENA_RESET  GPIO_ResetBits(Catch_ENA_PORT, Catch_ENA_PIN)


// 控制方向DIR (使能：顺时针，复位：逆时针)
#define Catch_DIR_CLK       RCC_APB2Periph_GPIOC
#define Catch_DIR_PIN       GPIO_Pin_8
#define Catch_DIR_PORT      GPIOC

#define Catch_DIR_SET       GPIO_SetBits(Catch_DIR_PORT, Catch_DIR_PIN)
#define Catch_DIR_RESET     GPIO_ResetBits(Catch_DIR_PORT, Catch_DIR_PIN)

// 定时器脉冲PUL
#define Catch_TIMx				  TIM8
#define Catch_TIM_SetAutoreload   TIM_SetAutoreload 
#define Catch_TIM_SetCompare2     TIM_SetCompare2



#define Catch_TIM_PRESCALER       8                                 // 定时器预分频
#define Catch_FREQ     (SystemCoreClock/(Catch_TIM_PRESCALER + 1))     // 定时器频率
#define SPR         200                                         // 步进电机旋转一圈脉冲数，电机常数
#define MICRO_STEP  4   

#define C_ARR 2400
#define C_PSC 8

#define Catch_TIM_Init  TIM8_CH2_Init(C_ARR, C_PSC)

void Catch_Init(void);
void Catch_Start(int32_t fre, int8_t DIR);

// 运动状态 
extern uint8_t Y1_Status;

#endif
