#ifndef __X_SETUP_H
#define __X_SETUP_H

#include "TIM_Config.h"


/**************************************************************************
* GND�ӵ�
*
*	TIM2_CH2(X��)
* X_ENA+ ---------------- PC3 
* X_DIR+ ---------------- PC1
* X_PWM+ ---------------- PA1
*
* TIM2_CH3(Z��)
*	Z_ENA+ ---------------- PC2
* Z_DIR+ ---------------- PA0
* Z_PWM+ ---------------- PA2
*
* TIM2_CH4(ת��)
* TP_ENA+ --------------- PA5
* TP_DIR+ --------------- PA4
* TP_PWM+ --------------- PA3
*
******************************************************************************
* TIM3_CH1(Y1��)
* Y1_ENA+ --------------- PB0
* Y1_DIR+ --------------- PC4
* Y1_PWM+ --------------- PA6
*
* TIM3_CH2(���ץ�ֿ���)
* Servo_PWM ------------- PA7
*
*****************************************************************************
* TIM4_CH2(Y2��)
* Y2_ENA+ --------------- PB6
* Y2_DIR+ --------------- PB5
* Y2_PWM+ --------------- PA7
* 
**************************************************************************/

/***********************************X��***************************************/
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
#define X_TIMx				 					TIM2
#define X_TIM_SetAutoreload  		TIM_SetAutoreload 
#define X_TIM_SetCompare    		TIM_SetCompare2


/***********************************Z��***************************************/
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
#define Z_TIMx				 					TIM2
#define Z_TIM_SetAutoreload  		TIM_SetAutoreload 
#define Z_TIM_SetCompare    		TIM_SetCompare3	


/***********************************TP��***************************************/
// TP��ENA
#define TP_ENA_CLK       RCC_APB2Periph_GPIOA
#define TP_ENA_PIN       GPIO_Pin_5
#define TP_ENA_PORT      GPIOA
#define TP_ENA_SET       GPIO_SetBits(TP_ENA_PORT, TP_ENA_PIN)
#define TP_ENA_RESET     GPIO_ResetBits(TP_ENA_PORT, TP_ENA_PIN)

// TP��DIR
#define TP_DIR_CLK       RCC_APB2Periph_GPIOA
#define TP_DIR_PIN       GPIO_Pin_4
#define TP_DIR_PORT      GPIOA
#define TP_DIR_SET       GPIO_SetBits(TP_DIR_PORT, TP_DIR_PIN)
#define TP_DIR_RESET     GPIO_ResetBits(TP_DIR_PORT, TP_DIR_PIN)

// TP�ᶨʱ������PUL
#define TP_TIMx				 					TIM2
#define TP_TIM_SetAutoreload  	TIM_SetAutoreload 
#define TP_TIM_SetCompare    		TIM_SetCompare4	


/***********************************Y1��***************************************/
// Y1��ENA
#define Y1_ENA_CLK       RCC_APB2Periph_GPIOB
#define Y1_ENA_PIN       GPIO_Pin_0
#define Y1_ENA_PORT      GPIOB
#define Y1_ENA_SET       GPIO_SetBits(Y1_ENA_PORT, Y1_ENA_PIN)
#define Y1_ENA_RESET     GPIO_ResetBits(Y1_ENA_PORT, Y1_ENA_PIN)

// Y1��DIR
#define Y1_DIR_CLK       RCC_APB2Periph_GPIOC
#define Y1_DIR_PIN       GPIO_Pin_4
#define Y1_DIR_PORT      GPIOC
#define Y1_DIR_SET       GPIO_SetBits(Y1_DIR_PORT, Y1_DIR_PIN)
#define Y1_DIR_RESET     GPIO_ResetBits(Y1_DIR_PORT, Y1_DIR_PIN)

// Y1�ᶨʱ������PUL
#define Y1_TIMx				 					TIM3
#define Y1_TIM_SetAutoreload  	TIM_SetAutoreload 
#define Y1_TIM_SetCompare    		TIM_SetCompare1


/***********************************Servo��***************************************/
// Servoץ��PWM
#define Servo_PWM 									TIM3
#define Servo_TIM_SetAutoreload  		TIM_SetAutoreload 
#define Servo_TIM_SetCompare    		TIM_SetCompare2


/***********************************Y2��***************************************/
// Y2��ENA
#define Y2_ENA_CLK       RCC_APB2Periph_GPIOB
#define Y2_ENA_PIN       GPIO_Pin_6
#define Y2_ENA_PORT      GPIOB
#define Y2_ENA_SET       GPIO_SetBits(Y2_ENA_PORT, Y2_ENA_PIN)
#define Y2_ENA_RESET     GPIO_ResetBits(Y2_ENA_PORT, Y2_ENA_PIN)

// Y2��DIR
#define Y2_DIR_CLK       RCC_APB2Periph_GPIOB
#define Y2_DIR_PIN       GPIO_Pin_5
#define Y2_DIR_PORT      GPIOB
#define Y2_DIR_SET       GPIO_SetBits(Y2_DIR_PORT, Y2_DIR_PIN)
#define Y2_DIR_RESET     GPIO_ResetBits(Y2_DIR_PORT, Y2_DIR_PIN)

// Y2��ʱ������PUL
#define Y2_TIMx				 					TIM4
#define Y2_TIM_SetAutoreload  	TIM_SetAutoreload 
#define Y2_TIM_SetCompare    		TIM_SetCompare2



#define TIM_PRESCALER       8                                 // ��ʱ��Ԥ��Ƶ
#define T1_FREQ     (SystemCoreClock/(TIM_PRESCALER + 1))     // ��ʱ��Ƶ��
#define SPR         200                                         // ���������תһȦ���������������
#define MICRO_STEP  4                                           // ���������ϸ����
#define STEP_S			200                                     		// ���ٵĲ���
#define FLEXIBLE    4.0																					// S���ߵ�������

//#define ALPHA       ((float)((2*3.1415*100/SPR)/100))                     // ÿ������Ļ��ȣ������
//#define A_T         ((float)(ALPHA*T1_FREQ))


typedef enum{
	CW=0,     	// ˳ʱ��
	CCW,				// ��ʱ��
	STOP,				// ֹͣ
	ACCEL,			// ���� 
	DECEL,			// ����
	RUN,				// ��������
	X_MOTOR,		// X����
	Z_MOTOR,		// Z����
	TP_MOTOR,	  // ת��
	Y1_MOTOR,		// Y1���
	Y2_MOTOR,		// Y2 ���
	Catch_Servo // ץ�ֶ�� 
}Motor_Status;


typedef struct{
	uint8_t dir;                        	// ����
	int16_t total_count;									// �ܼ���
	int16_t step_arr;                 		// arrֵ����������ļ�������
	uint32_t decel_start;               	// ���ٿ�ʼ��������
	int32_t accel_count;                	// ���ٹ��̵�������
	uint8_t run_state;                 		// ����״̬
	Motor_Status MotorX;									// ��¼������
}SpeedRampData;


extern uint8_t Status; 									// X�Ƿ����˶�
extern uint32_t X_pos;									// ��ǰλ��
extern SpeedRampData srd;      					// X/Z/ץ�ֵ��״̬��¼


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



