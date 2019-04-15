#include "X_Setup.h"
#include "usart.h"

XspeedRampData srd_x; 
uint8_t X_Status = 0;             // �Ƿ����˶��� 0��ֹͣ�� 1���˶�
uint32_t X_pos = 0;               // ��ǰλ��


static float __FRE[X_STEP_S] = {0.0};        	// Ƶ�ʻ�����
static uint16_t __ARR[X_STEP_S] = {0};				// ��װ��ֵ������


/********************************************
* ������: exp
* �������ܣ�����e��n�η�
* ����: x
* ���: x
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
* ������: DIR_ENA_GPIO_Config
* �������ܣ�����������ʹ�ܶ˿ڳ�ʼ��
* ����: ��
* ���: ��
********************************************/
void DIR_ENA_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(X_DIR_CLK ,ENABLE);

    GPIO_InitStructure.GPIO_Pin = X_DIR_PIN|X_ENA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_Init(X_DIR_PORT, &GPIO_InitStructure);

	X_DIR_SET;        // ˳ʱ��
	X_ENA_RESET;      // �ر��ѻ�����
}





/*******************************************
* ������:	 X_ENA
*	��������: ����ѻ��Ƿ�ʹ��
* ����: MOTOx, NewState
* ���: ��
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
* ������:	 CalculateSModelLine
*	��������: ����S��������
* ����: fre_max(���Ƶ��), fre_min(��СƵ��), flexible(���ߵ�������)				
* ���: ��
*******************************************/
static void CalculateSModelLine(float fre_max, float fre_min)
{
    int32_t i=0;
    float x = 0.0;
    float y = 0;
    float D_value = fre_max - fre_min;
	
    for(; i<X_STEP_S; i++)
    {
        x = FLEXIBLE*(i-(X_STEP_S/2))/(X_STEP_S/2);                 //��һ��
        y = 1.0/(1+exp(-x));
		
        __FRE[i] = D_value*y + fre_min;
        __ARR[i] = (uint16_t)((T1_FREQ/__FRE[i]) -  1);
    }
}


void Motor_Speed_Config(int32_t step, float fre_max, float fre_min)
{
}

/************************************************
* ������:   X_Move
* ��������: X������˶�
* ����: step(������еĲ���)
* ���: ��
*************************************************/
void Motor_Move(int32_t step, float fre_max, float fre_min)
{
    CalculateSModelLine(fre_max, fre_min);      // ����S��������
	
    X_ENA(X_TIMx, ENABLE);

    // �����˶�����, step<0��ʱ��, >0 ˳ʱ��
    if(step < 0)
    {
				srd_x.dir = CCW;
				X_DIR_RESET;  
				step = -step; 
    }else{
				srd_x.dir = CW;
				X_DIR_SET;                 
    }

	srd_x.total_count = step;    // ��¼Ҫ�ߵ��ܲ���

    // ����ת�ǶȺ�Сʱ��ֱ��������Ƶ�ʿ�ʼ����
    if(step == 1)
    {
        srd_x.accel_count = 1;
        srd_x.step_arr = (int16_t)((T1_FREQ/fre_min) - 1);    // ������װ��ֵ
        srd_x.run_state = DECEL;
        X_Status = 1;
    }else if(step != 0){
        if(step <= (X_STEP_S*2))
        {
            srd_x.step_arr = __ARR[0];
            srd_x.accel_count = (int32_t)(step/2);
            srd_x.decel_start = srd_x.accel_count;
            srd_x.run_state = ACCEL;
            X_Status = 1;
			
        }else{
            srd_x.step_arr = __ARR[0];
            srd_x.accel_count = X_STEP_S;
            srd_x.decel_start = step - X_STEP_S;
            srd_x.run_state = ACCEL;
            X_Status = 1;
        }
    }
    X_TIM_SetAutoreload(X_TIMx, srd_x.step_arr);
    X_TIM_SetCompare(X_TIMx, (srd_x.step_arr/2));

    TIM_Cmd(X_TIMx, ENABLE);                    // ��ʱ��ʹ��
}


/********************************************************
 * step:  �ƶ��Ĳ�������Ϊ˳ʱ�룬��Ϊ��ʱ��
 * speed: �����ٶ�(rad/s)
 * 800����һȦ
 * �����˶�
 * ******************************************************/
void Motor_MoveAbs(int32_t step, float fre_max, float fre_min)
{
    CalculateSModelLine(fre_max, fre_min);
  
    step = step - X_pos;         // ����λ��
	
    //X_ENA(X_TIMx, ENABLE);

    // �����˶�����
    if(step < 0)
    {
        srd_x.dir = CCW;
		X_DIR_RESET;
        
		step = -step; 
    }else{
        srd_x.dir = CW;
		X_DIR_SET;                   // stepΪ����ENAΪ+��˳ʱ��
    }

	srd_x.total_count = step;    // ��¼Ҫ�ߵ��ܲ���

    // ����ת�ǶȺ�Сʱ��ֱ��������Ƶ�ʿ�ʼ����
    if(step == 1)
    {
        srd_x.accel_count = 1;
        srd_x.step_arr = (int16_t)((T1_FREQ/fre_min) - 1);
        srd_x.run_state = DECEL;
        X_Status = 1;
    }else if(step != 0){
        if(step <= (X_STEP_S*2))
        {
            srd_x.step_arr = __ARR[0];
            srd_x.accel_count = (int32_t)(step/2);
            srd_x.decel_start = srd_x.accel_count;
            srd_x.run_state = ACCEL;
            X_Status = 1;
			
        }else{
            srd_x.step_arr = __ARR[0];
            srd_x.accel_count = X_STEP_S;
            srd_x.decel_start = step - X_STEP_S;
            srd_x.run_state = ACCEL;
            X_Status = 1;
        }
    }
    X_TIM_SetAutoreload(X_TIMx, srd_x.step_arr);
    X_TIM_SetCompare(X_TIMx, (srd_x.step_arr/2));

    TIM_Cmd(X_TIMx, ENABLE);                    // ��ʱ��ʹ��
}

#define V2


/***************************************
 * ��������X_Config
 * �������ܣ��̶��ٶ���ת
 * ���룺int32_t fre; int8_t DIR:(0:DIR_SET, 1:DIR_RESET)
 * �������
 * ************************************/
void Motor_Slow(int32_t fre, int8_t DIR)
{
    srd_x.step_arr = fre;

    if(DIR==0)
    {
        X_DIR_SET;          // ˳ʱ��
    }else if(DIR==1)
    {
        X_DIR_RESET;        // ��ʱ��
    }
	
    X_TIM_SetAutoreload(X_TIMx, srd_x.step_arr);
    X_TIM_SetCompare(X_TIMx, (srd_x.step_arr/2));
	
    TIM_Cmd(X_TIMx, ENABLE);
}

#ifdef V1
void TIM2_IRQHandler(void)
{
    if(TIM_GetITStatus(X_TIMx, TIM_IT_Update) != RESET)
    {
        X_TIM_SetAutoreload(X_TIMx, srd_x.step_arr);
        X_TIM_SetCompare(X_TIMx, (srd_x.step_arr/2));

        TIM_ClearITPendingBit(X_TIMx, TIM_IT_Update);
    }
}
#endif


#ifdef V2
/***************************************
 * 
 *           TIM2�жϴ�����
 * 
 * ************************************/
void TIM2_IRQHandler(void)
{
    static uint16_t step_count = 0;                 // ���Ʋ���������

    if(TIM_GetITStatus(X_TIMx, TIM_IT_Update) != RESET)
    {
		//printf("srd_x.run_state%d\r\n", srd_x.run_state);
		TIM_ClearITPendingBit(X_TIMx, TIM_IT_Update);
        switch(srd_x.run_state)
        {
            case STOP:	
                step_count = 0;

                TIM_Cmd(X_TIMx, DISABLE);
                X_ENA_RESET;
                X_Status = 0;

                break;

            case ACCEL:
                step_count++;
			
                if(srd_x.dir==CW)
                {
                    X_pos++;
                }else{
                    X_pos--;
                }

                if(step_count < srd_x.accel_count)
                {
                    srd_x.step_arr = __ARR[step_count];
                    srd_x.run_state = ACCEL;
					
                }else if(step_count >= srd_x.decel_start){                          // ֱ�ӽ�����ٽ׶�
                    srd_x.step_arr = __ARR[step_count-1];
                    srd_x.run_state = DECEL;
					
                }else if(step_count >= X_STEP_S){    
					// �������ٽ׶�
                    srd_x.step_arr = __ARR[X_STEP_S-1];
                    srd_x.run_state = RUN;
                }
				
                break;

            case RUN:
                step_count++;
			
                if(srd_x.dir==CW)
                {
                    X_pos++;
                }else{
                    X_pos--;
                }

                if(step_count >= srd_x.decel_start)
                {
                    srd_x.step_arr = __ARR[X_STEP_S-1];
                    srd_x.run_state = DECEL;
                }
                
                break;

            case DECEL:
                step_count++;
			
                if(srd_x.dir==CW)
                {
                    X_pos++;
                }else{
                    X_pos--;
                }
                
                if(step_count < srd_x.total_count)
                {
                    srd_x.step_arr = __ARR[srd_x.total_count-step_count];
                }else{				
					srd_x.run_state = STOP;
                }

                break;
            
            default:
                break;
        }
        X_TIM_SetAutoreload(X_TIMx, srd_x.step_arr);
        X_TIM_SetCompare(X_TIMx, (srd_x.step_arr/2));
    }
}
#endif


