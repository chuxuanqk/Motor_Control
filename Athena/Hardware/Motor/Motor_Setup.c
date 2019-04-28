#include "Motor_Setup.h"
#include "usart.h"
#include "led.h"
#include "delay.h"

SpeedRampData srd; 
uint8_t Status = 0;             	// �Ƿ����˶��� 0��ֹͣ�� 1���˶�
uint32_t X_pos = 0;               // ��ǰλ��


static float __FRE[STEP_S] = {0.0};        	// Ƶ�ʻ�����
static uint16_t __ARR[STEP_S] = {0};				// ��װ��ֵ������

/********************Y1��Y2������Ƶ��*********************************/
static float TIM3_ARR	= 0;
static float TIM3_CC	= 0;
static float Y2_ARR	= 0;
static float Y2_CC  = 0;



/********************************************
* ������: 		exp
* �������ܣ�	����e��n�η�
* ����: 		x
* ���: 		x
********************************************/
static double exp(double x)
{
    uint16_t i = 0;

    x = 1.0 + x/256;
    for(; i<8; i++)
    {
        x = x*x;
    }
    return x;
}



/********************************************
* ������: 		DIR_ENA_GPIO_Config
* �������ܣ�	����������ʹ�ܶ˿ڳ�ʼ��
* ����: 		��
* ���: 		��
********************************************/
void DIR_ENA_GPIO_Config(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
	
		RCC_APB2PeriphClockCmd(X_DIR_CLK ,ENABLE);
		RCC_APB2PeriphClockCmd(X_ENA_CLK ,ENABLE);
		RCC_APB2PeriphClockCmd(Z_DIR_CLK ,ENABLE);
		RCC_APB2PeriphClockCmd(Z_ENA_CLK ,ENABLE);
	
		// X��DIR_ENA��GPIO��ʼ��
		GPIO_InitStructure.GPIO_Pin = X_DIR_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(X_DIR_PORT, &GPIO_InitStructure);
	
		GPIO_InitStructure.GPIO_Pin = X_ENA_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(X_ENA_PORT, &GPIO_InitStructure);
	
	
		// Z��DIR_ENA��GPIO��ʼ��
		GPIO_InitStructure.GPIO_Pin = Z_DIR_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(Z_DIR_PORT, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin = Z_ENA_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(Z_ENA_PORT, &GPIO_InitStructure);
		
		// TP��DIR_ENA��GPIO��ʼ��
		GPIO_InitStructure.GPIO_Pin = TP_DIR_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(TP_DIR_PORT, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin = TP_ENA_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(TP_ENA_PORT, &GPIO_InitStructure);	

		// Y1��DIR_ENA��GPIO��ʼ��
		GPIO_InitStructure.GPIO_Pin = Y1_DIR_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(Y1_DIR_PORT, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin = Y1_ENA_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(Y1_ENA_PORT, &GPIO_InitStructure);	


		// Y2��DIR_ENA��GPIO��ʼ��
		GPIO_InitStructure.GPIO_Pin = Y2_DIR_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(Y2_DIR_PORT, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin = Y2_ENA_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(Y2_ENA_PORT, &GPIO_InitStructure);	


		X_DIR_SET;        // ˳ʱ��
		X_ENA_RESET;      // �ر��ѻ�����
		Z_DIR_SET;       
		Z_ENA_RESET;    
		TP_DIR_SET;       
		TP_ENA_RESET;   
		Y1_DIR_SET;       
		Y1_ENA_RESET;   
		Y2_DIR_SET;       
		Y2_ENA_RESET;    		
}


/*******************************************
* ������:	 	X_ENA
*	��������: 	����ѻ��Ƿ�ʹ��
* ����: 		MOTOx, NewState
* ���: 		��
*******************************************/
void X_ENA(TIM_TypeDef* MOTOx, FunctionalState NewState)
{
	u16 Ena = 0;
	assert_param(IS_TIM_LIST6_PERIPH(MOTOx));
	if(NewState == ENABLE)
	{
		Ena = 1;
	}
	switch(Ena)
	{
		case 1:
			if(MOTOx == TIM2)
				X_ENA_RESET;
			break;
		case 0:
			if(MOTOx == TIM2)
				X_ENA_SET;
			break;
	}
}


/*******************************************
* ������:	 	CalculateSModelLine
*	��������: ����S��������
* ����: 		fre_max(���Ƶ��)
						fre_min(��СƵ��) 
						flexible(���ߵ�������)				
* ���: 		��
*******************************************/
static void CalculateSModelLine(float fre_max, float fre_min)
{
    int32_t i=0;
    float x = 0.0;
    float y = 0;
    float D_value = fre_max - fre_min;
	
    for(; i<STEP_S; i++)
    {
        x = FLEXIBLE*(i-(STEP_S/2))/(STEP_S/2);           //��һ��
        y = 1.0/(1+exp(-x));
		
        __FRE[i] = D_value*y + fre_min;
        __ARR[i] = (uint16_t)((T1_FREQ/__FRE[i]) -  1);
    }
}


/************************************************
* ������:   	Motor_Config
* ��������: 	���״̬����
* ����: 		temp(����XspeedRampData)
* ���: 		��
*************************************************/
void Motor_Config(SpeedRampData temp)
{
		srd = temp;
	
		
		if(X_MOTOR == srd.MotorX)
		{
			if(CW == srd.dir) X_DIR_SET;
			else if(CCW == srd.dir) X_DIR_RESET; 
			
			TIM_PrescalerConfig(X_TIMx, TIM_PRESCALER, TIM_PSCReloadMode_Immediate); 
			X_TIM_SetAutoreload(X_TIMx, srd.step_arr);
			X_TIM_SetCompare(X_TIMx, (srd.step_arr/2));
		}else if(Z_MOTOR == srd.MotorX){
			if(CW == srd.dir) Z_DIR_SET;
			else if(CCW == srd.dir) Z_DIR_RESET;

			TIM_PrescalerConfig(Z_TIMx, TIM_PRESCALER, TIM_PSCReloadMode_Immediate); 
			Z_TIM_SetAutoreload(Z_TIMx, srd.step_arr);
			Z_TIM_SetCompare(Z_TIMx, (srd.step_arr/2));
		}else if(TP_MOTOR == srd.MotorX){
			if(CW == srd.dir) TP_DIR_SET;
			else if(CCW == srd.dir) TP_DIR_RESET;

			TIM_PrescalerConfig(TP_TIMx, TIM_PRESCALER, TIM_PSCReloadMode_Immediate); 			
			TP_TIM_SetAutoreload(TP_TIMx, srd.step_arr);
			TP_TIM_SetCompare(TP_TIMx, (srd.step_arr/2));
		}
}


/************************************************
* ������:   	Motor_Move
* ��������: 	X������˶�
* ����: 		step(������еĲ���)
* ���: 		��
*************************************************/
void Motor_Move(int32_t step, float fre_max, float fre_min, Motor_Status flag)
{
		SpeedRampData temp;
		
		temp.MotorX = flag;
    CalculateSModelLine(fre_max, fre_min);      // ����S��������
		
    // X_ENA(X_TIMx, ENABLE);
    // �����˶�����, step<0��ʱ��, >0 ˳ʱ��
    if(step < 0)
    {
			temp.dir = CCW; 
			step = -step; 
    }else{
			temp.dir = CW;               
    }

		temp.total_count = step;    // ��¼Ҫ�ߵ��ܲ���

    // ����ת�ǶȺ�Сʱ��ֱ��������Ƶ�ʿ�ʼ����
    if(step == 1)
    {
        temp.accel_count = 1;
        temp.step_arr = (int16_t)((T1_FREQ/fre_min) - 1);    // ������װ��ֵ
        temp.run_state = DECEL;
        Status = 1;
    }else if(step != 0){
        if(step <= (STEP_S*2))
        {
            temp.step_arr = __ARR[0];
            temp.accel_count = (int32_t)(step/2);
            temp.decel_start = temp.accel_count;
            temp.run_state = ACCEL;
            Status = 1;	
        }else{
            temp.step_arr = __ARR[0];
            temp.accel_count = STEP_S;
            temp.decel_start = step - STEP_S;
            temp.run_state = ACCEL;
            Status = 1;
        }
    }
		
		Motor_Config(temp);

		// TIM���ͨ��ʹ��
		if(X_MOTOR == srd.MotorX) X_TIM_EnableOC;
		else if(Z_MOTOR == srd.MotorX) Z_TIM_EnableOC;
		else if(TP_MOTOR == srd.MotorX) TP_TIM_EnableOC;
		
    TIM_Cmd(X_TIMx, ENABLE);                    // ��ʱ��ʹ��
}


/************************************************
* ������:   	Motor_MoveAbs
* ��������: 	X������˶�
* ����: 		step(������еĲ���)
						fre_max(����������Ƶ��)
						fre_min(���������СƵ��)
						flag(������)
* ���: 		��
*************************************************/
void Motor_MoveAbs(int32_t step, float fre_max, float fre_min, Motor_Status flag)
{
		SpeedRampData temp;  

		CalculateSModelLine(fre_max, fre_min);
  
    step = step - X_pos;         // ����λ��
		temp.MotorX = flag;
	
    //X_ENA(X_TIMx, ENABLE);

    // X_ENA(X_TIMx, ENABLE);
    // �����˶�����, step<0��ʱ��, >0 ˳ʱ��
    if(step < 0)
    {
			temp.dir = CCW; 
			step = -step; 
    }else{
			temp.dir = CW;               
    }

		temp.total_count = step;    // ��¼Ҫ�ߵ��ܲ���

    // ����ת�ǶȺ�Сʱ��ֱ��������Ƶ�ʿ�ʼ����
    if(step == 1)
    {
        temp.accel_count = 1;
        temp.step_arr = (int16_t)((T1_FREQ/fre_min) - 1);    // ������װ��ֵ
        temp.run_state = DECEL;
        Status = 1;
    }else if(step != 0){
        if(step <= (STEP_S*2))
        {
            temp.step_arr = __ARR[0];
            temp.accel_count = (int32_t)(step/2);
            temp.decel_start = temp.accel_count;
            temp.run_state = ACCEL;
            Status = 1;	
        }else{
            temp.step_arr = __ARR[0];
            temp.accel_count = STEP_S;
            temp.decel_start = step - STEP_S;
            temp.run_state = ACCEL;
            Status = 1;
        }
    }
		Motor_Config(temp);						  // ������ó�ʼ��	

		// TIM���ͨ��ʹ��
		if(X_MOTOR == srd.MotorX) X_TIM_EnableOC;
		else if(Z_MOTOR == srd.MotorX) Z_TIM_EnableOC;
		else if(TP_MOTOR == srd.MotorX) TP_TIM_EnableOC;
		
    TIM_Cmd(X_TIMx, ENABLE);        // ��ʱ��ʹ��
}



/************************************************
* ������:   	Motor_Speed_Adjust
* ��������: 	Motor�ٶȵ���
* ����: 		TIM
	
* ���:			��
*************************************************/
void Motor_Speed_Adjust(TIM_TypeDef *TIM, SpeedRampData *m_srd)
{
		static uint16_t step_count = 0;                 // ���Ʋ���������
	
		switch(m_srd->run_state)
		{
				case STOP:	
						step_count = 0;
						Status = 0;        // ���ü�¼״̬
						TIM_Cmd(TIM, DISABLE);
						//X_ENA_RESET;
						
						if(X_MOTOR == srd.MotorX) X_TIM_DisableOC;
						else if(Z_MOTOR == srd.MotorX) Z_TIM_DisableOC;
						else if(TP_MOTOR == srd.MotorX) TP_TIM_DisableOC;

						break;

				case ACCEL:
						step_count++;

						if(m_srd->dir==CW)
						{
								X_pos++;
						}else{
								X_pos--;
						}

						if(step_count < m_srd->accel_count)
						{
								m_srd->step_arr = __ARR[step_count];
								m_srd->run_state = ACCEL;
			
						}else if(step_count >= m_srd->decel_start){                          // ֱ�ӽ�����ٽ׶�
								m_srd->step_arr = __ARR[step_count-1];
								m_srd->run_state = DECEL;
			
						}else if(step_count >= STEP_S){    
								// �������ٽ׶�
								m_srd->step_arr = __ARR[STEP_S-1];
								m_srd->run_state = RUN;
						}

						break;

				case RUN:
						step_count++;

						if(m_srd->dir==CW)
						{
								X_pos++;
						}else{
								X_pos--;
						}

						if(step_count >= m_srd->decel_start)
						{
								m_srd->step_arr = __ARR[STEP_S-1];
								m_srd->run_state = DECEL;
						}
						
						break;

				case DECEL:
						step_count++;

						if(m_srd->dir==CW)
						{
								X_pos++;
						}else{
								X_pos--;
						}
						
						if(step_count < m_srd->total_count)
						{
								m_srd->step_arr = __ARR[m_srd->total_count-step_count];
						}else{				
								m_srd->run_state = STOP;
						}

						break;
				
				default:
						break;
		}
}


/************************************************
* ������:	  	Motor_Y1_Init
* ��������:  	��ʼ��Y1���
* ����: 			fre(�������Ƶ��)
							Dir(����:CW˳ʱ��, CCW��ʱ��)
* ���: 			��
************************************************/
void Motor_Y1_Init(uint16_t arr, uint16_t ccr, Motor_Status Dir)
{
		TIM3_ARR = arr;
		TIM3_CC  = ccr; 

		srd.MotorX = Y1_MOTOR;                           // ���õ���ͺ�
		TIM_PrescalerConfig(Y1_TIMx, TIM_PRESCALER, TIM_PSCReloadMode_Immediate);  // �������ö�ʱ��Ԥ��Ƶϵ��

	
		if(CW == Dir) Y1_DIR_SET;
		else if(CCW == Dir) Y1_DIR_RESET; 
	
		Y1_TIM_SetAutoreload(Y1_TIMx, TIM3_ARR);
		Y1_TIM_SetCompare(Y1_TIMx, TIM3_CC);
	
		Y1_TIM_EnableOC;								 								// ʹ��Y1��ʱ��ͨ��
	  TIM_Cmd(Y1_TIMx, ENABLE);        								// ��ʱ��ʹ��
}	


/************************************************
* ������:	  	Motor_Y1_Init
* ��������:  	��ʼ��Y1���
* ����: 			fre(�������Ƶ��)
							Dir(����:CW˳ʱ��, CCW��ʱ��)
* ���: 			��
************************************************/
void Motor_Y2_Init(uint16_t arr, uint16_t ccr, Motor_Status Dir)
{
		Y2_ARR = arr;
		Y2_CC	 = ccr;

		if(CW == Dir) Y2_DIR_SET;
		else if(CCW == Dir) Y2_DIR_RESET; 
	
		Y2_TIM_SetAutoreload(Y2_TIMx, Y2_ARR);
		Y2_TIM_SetCompare(Y2_TIMx, Y2_CC);
	
		Y2_TIM_EnableOC;								 // ʹ��Y2ʱ��ͨ��
	  TIM_Cmd(Y2_TIMx, ENABLE);        // ��ʱ��ʹ��
}	


/********************************************
* ������: 		Servo_Config
* ��������: 	�������ץ�ֳ�ʼ��
* ����: 		��
* ���: 		��
*********************************************/
void Servo_Config(void)
{
		Servo_Close();
		srd.MotorX = Catch_Servo;                           														// ���õ���ͺ�
		TIM_PrescalerConfig(Servo_TIMx, SERVO_PRESCALER, TIM_PSCReloadMode_Immediate);  // �������ö�ʱ��Ԥ��Ƶϵ��
		
		Servo_TIM_SetAutoreload(Servo_TIMx , TIM3_ARR);
		Servo_TIM_SetCompare(Servo_TIMx, TIM3_CC);																	 		// ��ʼ���Ƕ�Ϊ0
		
		Servo_TIM_EnableOC;																															// ʹ�ܶ��ͨ��
		TIM_Cmd(Servo_TIMx, ENABLE);
	
		delay_ms(1000);
		TIM_Cmd(Servo_TIMx, DISABLE);
		Servo_TIM_DisableOC;
}


/************************************************
* ������:	  	Set_TIM3_CC
* ��������:  	����TIM3�Ƚ�ֵ
* ����: 			Compare: 10~
* ���: 			��
************************************************/
void Set_TIM3_CC(float Compare)
{
		TIM3_CC = (SERVO_PRELOAD/400)*Compare;
}


/*******************************************************
* ������:			Servo_Close
* ��������:		ץ�ֱպ�,(((20000/200)*11)/2000)*160 = 198�� 
* ����:				��
* ���:				��
********************************************************/
void Servo_Close(void)
{
		delay_ms(500);
		TIM3_ARR = SERVO_PRELOAD;
		TIM3_CC  = (SERVO_PRELOAD/400)*25;   
	
		srd.MotorX = Catch_Servo;                           														// ���õ���ͺ�
		TIM_PrescalerConfig(Servo_TIMx, SERVO_PRESCALER, TIM_PSCReloadMode_Immediate);  // �������ö�ʱ��Ԥ��Ƶϵ��
		
		Servo_TIM_SetAutoreload(Servo_TIMx , TIM3_ARR);
		Servo_TIM_SetCompare(Servo_TIMx, TIM3_CC);																	 		// ��ʼ���Ƕ�Ϊ0
		
		Servo_TIM_EnableOC;																															// ʹ�ܶ��ͨ��
		TIM_Cmd(Servo_TIMx, ENABLE);
		
		delay_ms(500);
		TIM_Cmd(Servo_TIMx, DISABLE);
		Servo_TIM_DisableOC;
}


/*******************************************************
* ������:			Servo_Open
* ��������:		ץ���ſ�, 0��
* ����:				��
* ���:				��
********************************************************/
void Servo_Open(void)
{	
		delay_ms(500);
		TIM3_ARR = SERVO_PRELOAD;
		TIM3_CC  = (SERVO_PRELOAD/400)*10; 

		srd.MotorX = Catch_Servo;                           														// ���õ���ͺ�
		TIM_PrescalerConfig(Servo_TIMx, SERVO_PRESCALER, TIM_PSCReloadMode_Immediate);  // �������ö�ʱ��Ԥ��Ƶϵ��
		
		Servo_TIM_SetAutoreload(Servo_TIMx , TIM3_ARR);
		Servo_TIM_SetCompare(Servo_TIMx, TIM3_CC);																	 		// ��ʼ���Ƕ�Ϊ0
		
		Servo_TIM_EnableOC;																															// ʹ�ܶ��ͨ��
		TIM_Cmd(Servo_TIMx, ENABLE);

		delay_ms(800);
		TIM_Cmd(Servo_TIMx, DISABLE);
		Servo_TIM_DisableOC;	
}


/************************************************
* ������:	  	TIM2_IRQHandler
* ��������:  	TIM2�жϴ���, ����X/Z/ץ�ֵ���ٶ�
* ����: 			��
* ���: 			��
************************************************/
void TIM2_IRQHandler(void)
{
    if(TIM_GetITStatus(X_TIMx, TIM_IT_Update) != RESET)
    {
				TIM_ClearITPendingBit(X_TIMx, TIM_IT_Update);
   
				Motor_Speed_Adjust(X_TIMx, &srd);    																					// ����srd
			
        X_TIM_SetAutoreload(X_TIMx, srd.step_arr);	
				if(X_MOTOR == srd.MotorX) X_TIM_SetCompare(X_TIMx, (srd.step_arr/2));      		// ����X��CCR
				else if(Z_MOTOR == srd.MotorX) Z_TIM_SetCompare(X_TIMx, (srd.step_arr/2)); 		// ����Z��CCR
				else if(TP_MOTOR == srd.MotorX) TP_TIM_SetCompare(X_TIMx, (srd.step_arr/2)); 	// ����TP��CCR
    }
}


/************************************************
* ������:	  	TIM3_IRQHandler
* ��������:  	TIM3�жϴ���, ������װ��ֵ,Y1
* ����: 			��
* ���: 			��
************************************************/
void TIM3_IRQHandler(void)
{
	 if(TIM_GetITStatus(Y1_TIMx, TIM_IT_Update) != RESET)
	 {
				TIM_ClearITPendingBit(Y1_TIMx, TIM_IT_Update);
				
				if(srd.MotorX == Y1_MOTOR)
				{
						Y1_TIM_SetAutoreload(Y1_TIMx, TIM3_ARR);
						Y1_TIM_SetCompare(Y1_TIMx, TIM3_CC);
				}else if(srd.MotorX == Catch_Servo)
				{
						//LED = ~LED;
						Servo_TIM_SetAutoreload(Servo_TIMx, TIM3_ARR);
						Servo_TIM_SetCompare(Servo_TIMx, TIM3_CC);
				}
	 }
}


/************************************************
* ������:	  	TIM4_IRQHandler
* ��������:  	TIM4�жϴ���, ������װ��ֵ
* ����: 			��
* ���: 			��
************************************************/
void TIM4_IRQHandler(void)
{
	 if(TIM_GetITStatus(Y2_TIMx, TIM_IT_Update) != RESET)
	 {
				TIM_ClearITPendingBit(Y2_TIMx, TIM_IT_Update);

				Y2_TIM_SetAutoreload(Y2_TIMx, Y2_ARR);
				Y2_TIM_SetCompare(Y2_TIMx, Y2_CC);
	 }
}







