#ifndef __X_SETUP_H
#define __X_SETUP_H

#include "tim_2.h"

// X轴DIR
#define X_DIR_CLK       RCC_APB2Periph_GPIOC
#define X_DIR_PIN       GPIO_Pin_1
#define X_DIR_PORT      GPIOC
#define X_DIR_SET       GPIO_SetBits(X_DIR_PORT, X_DIR_PIN)
#define X_DIR_RESET     GPIO_ResetBits(X_DIR_PORT, X_DIR_PIN)

//X轴ENA
#define X_ENA_CLK       RCC_APB2Periph_GPIOC
#define X_ENA_PIN       GPIO_Pin_3
#define X_ENA_PORT      GPIOC
#define X_ENA_SET       GPIO_SetBits(X_ENA_PORT, X_ENA_PIN)
#define X_ENA_RESET     GPIO_ResetBits(X_ENA_PORT, X_ENA_PIN)

// X轴定时器脉冲PUL
#define X_TIMx				 		TIM2
#define X_TIM_SetAutoreload  		TIM_SetAutoreload 
#define X_TIM_SetCompare    		TIM_SetCompare2



// Z轴DIR
#define Z_DIR_CLK       RCC_APB2Periph_GPIOA
#define Z_DIR_PIN       GPIO_Pin_0
#define Z_DIR_PORT      GPIOA
#define Z_DIR_SET       GPIO_SetBits(Z_DIR_PORT, Z_DIR_PIN)
#define Z_DIR_RESET     GPIO_ResetBits(Z_DIR_PORT, Z_DIR_PIN)

// Z轴ENA
#define Z_ENA_CLK       RCC_APB2Periph_GPIOC
#define Z_ENA_PIN       GPIO_Pin_2
#define Z_ENA_PORT      GPIOC
#define Z_ENA_SET       GPIO_SetBits(Z_ENA_PORT, Z_ENA_PIN)
#define Z_ENA_RESET     GPIO_ResetBits(Z_ENA_PORT, Z_ENA_PIN)

// Z轴定时器脉冲PUL
#define Z_TIMx				 		TIM2
#define Z_TIM_SetAutoreload  		TIM_SetAutoreload 
#define Z_TIM_SetCompare    		TIM_SetCompare3				

static double exp(double x);

void DIR_ENA_GPIO_Config(void);
void X_ENA(TIM_TypeDef* MOTOx, FunctionalState NewState);

void Motor_Speed_Config(int32_t step, float fre_max, float fre_min);
void Motor_Move(int32_t step, float fre_max, float fre_min);
void Motor_MoveAbs(int32_t step, float fre_max, float fre_min);
static void CalculateSModelLine(float fre_max, float fre_min);
void Motor_Slow(int32_t fre, int8_t DIR);

//#define CW      0				  				// 顺时针
//#define CCW     1                 			// 逆时针

//#define STOP     0
//#define ACCEL    1
//#define DECEL    2
//#define RUN      3

#define X_TIM_PRESCALER       8                                 // 定时器预分频
#define T1_FREQ     (SystemCoreClock/(X_TIM_PRESCALER + 1))     // 定时器频率
#define SPR         200                                         // 步进电机旋转一圈脉冲数，电机常数
#define MICRO_STEP  4                                           // 电机驱动器细分数
#define X_STEP_S		200                                     		// 加速的步数
#define FLEXIBLE    4.0																					// S曲线调整因子

//#define ALPHA       ((float)((2*3.1415*100/SPR)/100))                     // 每个脉冲的弧度，步距角
//#define A_T         ((float)(ALPHA*T1_FREQ))


typedef struct{
	uint8_t dir;                        	// 方向
	int16_t total_count;									// 总计数
	int16_t step_arr;                 		// arr值，单个脉冲的计数个数
	uint32_t decel_start;               	// 减速开始的脉冲数
	int32_t accel_count;                	// 加速过程的脉冲数
	uint8_t run_state;                 		// 运行状态
}XspeedRampData;


typedef enum{
	CW=0,     	// 顺时针
	CCW,		// 逆时针
	STOP,		// 停止
	ACCEL,		// 加速 
	DECEL,		// 减速
	RUN,		// 匀速运行
	X_MOTOR,	// X轴电机
	Z_MOTOR,	// Z轴电机
	TURNPLATE,	// 转盘
}Motor_Status;

extern uint8_t X_Status; 					// X是否在运动
extern uint32_t X_pos;						// 当前位置
extern XspeedRampData srd_x;

#endif



