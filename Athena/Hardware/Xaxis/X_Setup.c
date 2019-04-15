#include "X_Setup.h"
#include "usart.h"

XspeedRampData srd_x; 
uint8_t X_Status = 0;             // 是否在运动？ 0：停止， 1：运动
uint32_t X_pos = 0;               // 当前位置


static float __FRE[X_STEP_S] = {0.0};        	// 频率缓冲区
static uint16_t __ARR[X_STEP_S] = {0};				// 重装载值缓冲区


/********************************************
* 函数名: exp
* 函数功能：常数e的n次方
* 输入: x
* 输出: x
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
* 函数名: DIR_ENA_GPIO_Config
* 函数功能：驱动器方向、使能端口初始化
* 输入: 无
* 输出: 无
********************************************/
void DIR_ENA_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(X_DIR_CLK ,ENABLE);

    GPIO_InitStructure.GPIO_Pin = X_DIR_PIN|X_ENA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_Init(X_DIR_PORT, &GPIO_InitStructure);

	X_DIR_SET;        // 顺时针
	X_ENA_RESET;      // 关闭脱机功能
}





/*******************************************
* 函数名:	 X_ENA
*	函数功能: 电机脱机是否使能
* 输入: MOTOx, NewState
* 输出: 无
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
* 函数名:	 CalculateSModelLine
*	函数功能: 计算S曲线数组
* 输入: fre_max(最大频率), fre_min(最小频率), flexible(曲线调整因子)				
* 输出: 无
*******************************************/
static void CalculateSModelLine(float fre_max, float fre_min)
{
    int32_t i=0;
    float x = 0.0;
    float y = 0;
    float D_value = fre_max - fre_min;
	
    for(; i<X_STEP_S; i++)
    {
        x = FLEXIBLE*(i-(X_STEP_S/2))/(X_STEP_S/2);                 //归一化
        y = 1.0/(1+exp(-x));
		
        __FRE[i] = D_value*y + fre_min;
        __ARR[i] = (uint16_t)((T1_FREQ/__FRE[i]) -  1);
    }
}


void Motor_Speed_Config(int32_t step, float fre_max, float fre_min)
{
}

/************************************************
* 函数名:   X_Move
* 函数功能: X轴绝对运动
* 输入: step(电机运行的步数)
* 输出: 无
*************************************************/
void Motor_Move(int32_t step, float fre_max, float fre_min)
{
    CalculateSModelLine(fre_max, fre_min);      // 计算S曲线数组
	
    X_ENA(X_TIMx, ENABLE);

    // 设置运动方向, step<0逆时针, >0 顺时针
    if(step < 0)
    {
				srd_x.dir = CCW;
				X_DIR_RESET;  
				step = -step; 
    }else{
				srd_x.dir = CW;
				X_DIR_SET;                 
    }

	srd_x.total_count = step;    // 记录要走的总步数

    // 当旋转角度很小时，直接以启动频率开始运行
    if(step == 1)
    {
        srd_x.accel_count = 1;
        srd_x.step_arr = (int16_t)((T1_FREQ/fre_min) - 1);    // 计算重装载值
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

    TIM_Cmd(X_TIMx, ENABLE);                    // 定时器使能
}


/********************************************************
 * step:  移动的步数，正为顺时针，负为逆时针
 * speed: 最大角速度(rad/s)
 * 800脉冲一圈
 * 绝对运动
 * ******************************************************/
void Motor_MoveAbs(int32_t step, float fre_max, float fre_min)
{
    CalculateSModelLine(fre_max, fre_min);
  
    step = step - X_pos;         // 绝对位移
	
    //X_ENA(X_TIMx, ENABLE);

    // 设置运动方向
    if(step < 0)
    {
        srd_x.dir = CCW;
		X_DIR_RESET;
        
		step = -step; 
    }else{
        srd_x.dir = CW;
		X_DIR_SET;                   // step为正，ENA为+，顺时针
    }

	srd_x.total_count = step;    // 记录要走的总步数

    // 当旋转角度很小时，直接以启动频率开始运行
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

    TIM_Cmd(X_TIMx, ENABLE);                    // 定时器使能
}

#define V2


/***************************************
 * 函数名：X_Config
 * 函数功能：固定速度旋转
 * 输入：int32_t fre; int8_t DIR:(0:DIR_SET, 1:DIR_RESET)
 * 输出：无
 * ************************************/
void Motor_Slow(int32_t fre, int8_t DIR)
{
    srd_x.step_arr = fre;

    if(DIR==0)
    {
        X_DIR_SET;          // 顺时针
    }else if(DIR==1)
    {
        X_DIR_RESET;        // 逆时针
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
 *           TIM2中断处理函数
 * 
 * ************************************/
void TIM2_IRQHandler(void)
{
    static uint16_t step_count = 0;                 // 总移步数计数器

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
					
                }else if(step_count >= srd_x.decel_start){                          // 直接进入减速阶段
                    srd_x.step_arr = __ARR[step_count-1];
                    srd_x.run_state = DECEL;
					
                }else if(step_count >= X_STEP_S){    
					// 进入匀速阶段
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


