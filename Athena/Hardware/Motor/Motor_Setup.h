#ifndef __X_SETUP_H
#define __X_SETUP_H

#include "TIM_Config.h"


/**************************************************************************
* GND接地
*
*	TIM2_CH2(X轴)
* X_ENA+ ---------------- PC3 
* X_DIR+ ---------------- PC1
* X_PWM+ ---------------- PA1
*
* TIM2_CH3(Z轴)
*	Z_ENA+ ---------------- PC2
* Z_DIR+ ---------------- PA0
* Z_PWM+ ---------------- PA2
*
* TIM2_CH4(转盘)
* TP_ENA+ --------------- PA5
* TP_DIR+ --------------- PA4
* TP_PWM+ --------------- PA3
*
******************************************************************************
* TIM3_CH1(Y1轴)
* Y1_ENA+ --------------- PB0
* Y1_DIR+ --------------- PC4
* Y1_PWM+ --------------- PA6
*
* TIM3_CH2(舵机抓手控制)
* Servo_PWM ------------- PA7
*
*****************************************************************************
* TIM4_CH2(Y2轴)
* Y2_ENA+ --------------- PB6
* Y2_DIR+ --------------- PB5
* Y2_PWM+ --------------- PA7
* 
**************************************************************************/

/***********************************X轴***************************************/
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
#define X_TIMx				 					TIM2
#define X_TIM_SetAutoreload  		TIM_SetAutoreload 
#define X_TIM_SetCompare    		TIM_SetCompare2


/***********************************Z轴***************************************/
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
#define Z_TIMx				 					TIM2
#define Z_TIM_SetAutoreload  		TIM_SetAutoreload 
#define Z_TIM_SetCompare    		TIM_SetCompare3	


/***********************************TP轴***************************************/
// TP轴ENA
#define TP_ENA_CLK       RCC_APB2Periph_GPIOA
#define TP_ENA_PIN       GPIO_Pin_5
#define TP_ENA_PORT      GPIOA
#define TP_ENA_SET       GPIO_SetBits(TP_ENA_PORT, TP_ENA_PIN)
#define TP_ENA_RESET     GPIO_ResetBits(TP_ENA_PORT, TP_ENA_PIN)

// TP轴DIR
#define TP_DIR_CLK       RCC_APB2Periph_GPIOA
#define TP_DIR_PIN       GPIO_Pin_4
#define TP_DIR_PORT      GPIOA
#define TP_DIR_SET       GPIO_SetBits(TP_DIR_PORT, TP_DIR_PIN)
#define TP_DIR_RESET     GPIO_ResetBits(TP_DIR_PORT, TP_DIR_PIN)

// TP轴定时器脉冲PUL
#define TP_TIMx				 					TIM2
#define TP_TIM_SetAutoreload  	TIM_SetAutoreload 
#define TP_TIM_SetCompare    		TIM_SetCompare4	


/***********************************Y1轴***************************************/
// Y1轴ENA
#define Y1_ENA_CLK       RCC_APB2Periph_GPIOB
#define Y1_ENA_PIN       GPIO_Pin_0
#define Y1_ENA_PORT      GPIOB
#define Y1_ENA_SET       GPIO_SetBits(Y1_ENA_PORT, Y1_ENA_PIN)
#define Y1_ENA_RESET     GPIO_ResetBits(Y1_ENA_PORT, Y1_ENA_PIN)

// Y1轴DIR
#define Y1_DIR_CLK       RCC_APB2Periph_GPIOC
#define Y1_DIR_PIN       GPIO_Pin_4
#define Y1_DIR_PORT      GPIOC
#define Y1_DIR_SET       GPIO_SetBits(Y1_DIR_PORT, Y1_DIR_PIN)
#define Y1_DIR_RESET     GPIO_ResetBits(Y1_DIR_PORT, Y1_DIR_PIN)

// Y1轴定时器脉冲PUL
#define Y1_TIMx				 					TIM3
#define Y1_TIM_SetAutoreload  	TIM_SetAutoreload 
#define Y1_TIM_SetCompare    		TIM_SetCompare1


/***********************************Servo轴***************************************/
// Servo抓手PWM
#define Servo_PWM 									TIM3
#define Servo_TIM_SetAutoreload  		TIM_SetAutoreload 
#define Servo_TIM_SetCompare    		TIM_SetCompare2


/***********************************Y2轴***************************************/
// Y2轴ENA
#define Y2_ENA_CLK       RCC_APB2Periph_GPIOB
#define Y2_ENA_PIN       GPIO_Pin_6
#define Y2_ENA_PORT      GPIOB
#define Y2_ENA_SET       GPIO_SetBits(Y2_ENA_PORT, Y2_ENA_PIN)
#define Y2_ENA_RESET     GPIO_ResetBits(Y2_ENA_PORT, Y2_ENA_PIN)

// Y2轴DIR
#define Y2_DIR_CLK       RCC_APB2Periph_GPIOB
#define Y2_DIR_PIN       GPIO_Pin_5
#define Y2_DIR_PORT      GPIOB
#define Y2_DIR_SET       GPIO_SetBits(Y2_DIR_PORT, Y2_DIR_PIN)
#define Y2_DIR_RESET     GPIO_ResetBits(Y2_DIR_PORT, Y2_DIR_PIN)

// Y2定时器脉冲PUL
#define Y2_TIMx				 					TIM4
#define Y2_TIM_SetAutoreload  	TIM_SetAutoreload 
#define Y2_TIM_SetCompare    		TIM_SetCompare2



#define TIM_PRESCALER       8                                 // 定时器预分频
#define T1_FREQ     (SystemCoreClock/(TIM_PRESCALER + 1))     // 定时器频率
#define SPR         200                                         // 步进电机旋转一圈脉冲数，电机常数
#define MICRO_STEP  4                                           // 电机驱动器细分数
#define STEP_S			200                                     		// 加速的步数
#define FLEXIBLE    4.0																					// S曲线调整因子

//#define ALPHA       ((float)((2*3.1415*100/SPR)/100))                     // 每个脉冲的弧度，步距角
//#define A_T         ((float)(ALPHA*T1_FREQ))


typedef enum{
	CW=0,     	// 顺时针
	CCW,				// 逆时针
	STOP,				// 停止
	ACCEL,			// 加速 
	DECEL,			// 减速
	RUN,				// 匀速运行
	X_MOTOR,		// X轴电机
	Z_MOTOR,		// Z轴电机
	TP_MOTOR,	  // 转盘
	Y1_MOTOR,		// Y1电机
	Y2_MOTOR,		// Y2 电机
	Catch_Servo // 抓手舵机 
}Motor_Status;


typedef struct{
	uint8_t dir;                        	// 方向
	int16_t total_count;									// 总计数
	int16_t step_arr;                 		// arr值，单个脉冲的计数个数
	uint32_t decel_start;               	// 减速开始的脉冲数
	int32_t accel_count;                	// 加速过程的脉冲数
	uint8_t run_state;                 		// 运行状态
	Motor_Status MotorX;									// 记录电机编号
}SpeedRampData;


extern uint8_t Status; 									// X是否在运动
extern uint32_t X_pos;									// 当前位置
extern SpeedRampData srd;      					// X/Z/抓手电机状态记录


static double exp(double x);

void DIR_ENA_GPIO_Config(void);
static void CalculateSModelLine(float fre_max, float fre_min);
void X_ENA(TIM_TypeDef* MOTOx, FunctionalState NewState);


void Motor_Config(SpeedRampData temp);
void Motor_Speed_Adjust(TIM_TypeDef * TIM, SpeedRampData *m_srd);
void Motor_Move(int32_t step, float fre_max, float fre_min, Motor_Status flag);
void Motor_MoveAbs(int32_t step, float fre_max, float fre_min, Motor_Status flag);

void Motor_Y1_Init(uint16_t arr, Motor_Status Dir);
void Motor_Y2_Init(uint16_t arr, Motor_Status Dir);

#endif



