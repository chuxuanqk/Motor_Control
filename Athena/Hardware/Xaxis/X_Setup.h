#ifndef __X_SETUP_H
#define __X_SETUP_H

#include "tim_2.h"

// X��DIR
#define X_DIR_CLK       RCC_APB2Periph_GPIOC
#define X_DIR_PIN       GPIO_Pin_1
#define X_DIR_PORT      GPIOC
#define X_DIR_SET       GPIO_SetBits(X_DIR_PORT, X_DIR_PIN)
#define X_DIR_RESET     GPIO_ResetBits(X_DIR_PORT, X_DIR_PIN)

//X��ENA
#define X_ENA_CLK       RCC_APB2Periph_GPIOC
#define X_ENA_PIN       GPIO_Pin_3
#define X_ENA_PORT      GPIOC
#define X_ENA_SET       GPIO_SetBits(X_ENA_PORT, X_ENA_PIN)
#define X_ENA_RESET     GPIO_ResetBits(X_ENA_PORT, X_ENA_PIN)

// X�ᶨʱ������PUL
#define X_TIMx				 		TIM2
#define X_TIM_SetAutoreload  		TIM_SetAutoreload 
#define X_TIM_SetCompare    		TIM_SetCompare2



// Z��DIR
#define Z_DIR_CLK       RCC_APB2Periph_GPIOA
#define Z_DIR_PIN       GPIO_Pin_0
#define Z_DIR_PORT      GPIOA
#define Z_DIR_SET       GPIO_SetBits(Z_DIR_PORT, Z_DIR_PIN)
#define Z_DIR_RESET     GPIO_ResetBits(Z_DIR_PORT, Z_DIR_PIN)

// Z��ENA
#define Z_ENA_CLK       RCC_APB2Periph_GPIOC
#define Z_ENA_PIN       GPIO_Pin_2
#define Z_ENA_PORT      GPIOC
#define Z_ENA_SET       GPIO_SetBits(Z_ENA_PORT, Z_ENA_PIN)
#define Z_ENA_RESET     GPIO_ResetBits(Z_ENA_PORT, Z_ENA_PIN)

// Z�ᶨʱ������PUL
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

//#define CW      0				  				// ˳ʱ��
//#define CCW     1                 			// ��ʱ��

//#define STOP     0
//#define ACCEL    1
//#define DECEL    2
//#define RUN      3

#define X_TIM_PRESCALER       8                                 // ��ʱ��Ԥ��Ƶ
#define T1_FREQ     (SystemCoreClock/(X_TIM_PRESCALER + 1))     // ��ʱ��Ƶ��
#define SPR         200                                         // ���������תһȦ���������������
#define MICRO_STEP  4                                           // ���������ϸ����
#define X_STEP_S		200                                     		// ���ٵĲ���
#define FLEXIBLE    4.0																					// S���ߵ�������

//#define ALPHA       ((float)((2*3.1415*100/SPR)/100))                     // ÿ������Ļ��ȣ������
//#define A_T         ((float)(ALPHA*T1_FREQ))


typedef struct{
	uint8_t dir;                        	// ����
	int16_t total_count;									// �ܼ���
	int16_t step_arr;                 		// arrֵ����������ļ�������
	uint32_t decel_start;               	// ���ٿ�ʼ��������
	int32_t accel_count;                	// ���ٹ��̵�������
	uint8_t run_state;                 		// ����״̬
}XspeedRampData;


typedef enum{
	CW=0,     	// ˳ʱ��
	CCW,		// ��ʱ��
	STOP,		// ֹͣ
	ACCEL,		// ���� 
	DECEL,		// ����
	RUN,		// ��������
	X_MOTOR,	// X����
	Z_MOTOR,	// Z����
	TURNPLATE,	// ת��
}Motor_Status;

extern uint8_t X_Status; 					// X�Ƿ����˶�
extern uint32_t X_pos;						// ��ǰλ��
extern XspeedRampData srd_x;

#endif



