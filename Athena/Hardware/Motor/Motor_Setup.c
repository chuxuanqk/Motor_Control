#include "Motor_Setup.h"
#include "usart.h"
#include "led.h"
#include "delay.h"

SpeedRampData srd; 
uint8_t Status = 0;             	// 是否在运动？ 0：停止， 1：运动
uint32_t X_pos = 0;               // 当前位置


static float __FRE[STEP_S] = {0.0};        	// 频率缓冲区
static uint16_t __ARR[STEP_S] = {0};				// 重装载值缓冲区

/********************Y1和Y2轴运行频率*********************************/
static float TIM3_ARR	= 0;
static float TIM3_CC	= 0;
static float Y2_ARR	= 0;
static float Y2_CC  = 0;



/********************************************
* 函数名: 		exp
* 函数功能：	常数e的n次方
* 输入: 		x
* 输出: 		x
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
* 函数名: 		DIR_ENA_GPIO_Config
* 函数功能：	驱动器方向、使能端口初始化
* 输入: 		无
* 输出: 		无
********************************************/
void DIR_ENA_GPIO_Config(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
	
		RCC_APB2PeriphClockCmd(X_DIR_CLK ,ENABLE);
		RCC_APB2PeriphClockCmd(X_ENA_CLK ,ENABLE);
		RCC_APB2PeriphClockCmd(Z_DIR_CLK ,ENABLE);
		RCC_APB2PeriphClockCmd(Z_ENA_CLK ,ENABLE);
	
		// X轴DIR_ENA的GPIO初始化
		GPIO_InitStructure.GPIO_Pin = X_DIR_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(X_DIR_PORT, &GPIO_InitStructure);
	
		GPIO_InitStructure.GPIO_Pin = X_ENA_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(X_ENA_PORT, &GPIO_InitStructure);
	
	
		// Z轴DIR_ENA的GPIO初始化
		GPIO_InitStructure.GPIO_Pin = Z_DIR_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(Z_DIR_PORT, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin = Z_ENA_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(Z_ENA_PORT, &GPIO_InitStructure);
		
		// TP轴DIR_ENA的GPIO初始化
		GPIO_InitStructure.GPIO_Pin = TP_DIR_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(TP_DIR_PORT, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin = TP_ENA_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(TP_ENA_PORT, &GPIO_InitStructure);	

		// Y1轴DIR_ENA的GPIO初始化
		GPIO_InitStructure.GPIO_Pin = Y1_DIR_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(Y1_DIR_PORT, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin = Y1_ENA_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(Y1_ENA_PORT, &GPIO_InitStructure);	


		// Y2轴DIR_ENA的GPIO初始化
		GPIO_InitStructure.GPIO_Pin = Y2_DIR_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(Y2_DIR_PORT, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin = Y2_ENA_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(Y2_ENA_PORT, &GPIO_InitStructure);	


		X_DIR_SET;        // 顺时针
		X_ENA_RESET;      // 关闭脱机功能
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
* 函数名:	 	X_ENA
*	函数功能: 	电机脱机是否使能
* 输入: 		MOTOx, NewState
* 输出: 		无
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
* 函数名:	 	CalculateSModelLine
*	函数功能: 计算S曲线数组
* 输入: 		fre_max(最大频率)
						fre_min(最小频率) 
						flexible(曲线调整因子)				
* 输出: 		无
*******************************************/
static void CalculateSModelLine(float fre_max, float fre_min)
{
    int32_t i=0;
    float x = 0.0;
    float y = 0;
    float D_value = fre_max - fre_min;
	
    for(; i<STEP_S; i++)
    {
        x = FLEXIBLE*(i-(STEP_S/2))/(STEP_S/2);           //归一化
        y = 1.0/(1+exp(-x));
		
        __FRE[i] = D_value*y + fre_min;
        __ARR[i] = (uint16_t)((T1_FREQ/__FRE[i]) -  1);
    }
}


/************************************************
* 函数名:   	Motor_Config
* 函数功能: 	电机状态配置
* 输入: 		temp(设置XspeedRampData)
* 输出: 		无
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
* 函数名:   	Motor_Move
* 函数功能: 	X轴绝对运动
* 输入: 		step(电机运行的步数)
* 输出: 		无
*************************************************/
void Motor_Move(int32_t step, float fre_max, float fre_min, Motor_Status flag)
{
		SpeedRampData temp;
		
		temp.MotorX = flag;
    CalculateSModelLine(fre_max, fre_min);      // 计算S曲线数组
		
    // X_ENA(X_TIMx, ENABLE);
    // 设置运动方向, step<0逆时针, >0 顺时针
    if(step < 0)
    {
			temp.dir = CCW; 
			step = -step; 
    }else{
			temp.dir = CW;               
    }

		temp.total_count = step;    // 记录要走的总步数

    // 当旋转角度很小时，直接以启动频率开始运行
    if(step == 1)
    {
        temp.accel_count = 1;
        temp.step_arr = (int16_t)((T1_FREQ/fre_min) - 1);    // 计算重装载值
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

		// TIM输出通道使能
		if(X_MOTOR == srd.MotorX) X_TIM_EnableOC;
		else if(Z_MOTOR == srd.MotorX) Z_TIM_EnableOC;
		else if(TP_MOTOR == srd.MotorX) TP_TIM_EnableOC;
		
    TIM_Cmd(X_TIMx, ENABLE);                    // 定时器使能
}


/************************************************
* 函数名:   	Motor_MoveAbs
* 函数功能: 	X轴相对运动
* 输入: 		step(电机运行的步数)
						fre_max(电机运行最大频率)
						fre_min(电机运行最小频率)
						flag(电机编号)
* 输出: 		无
*************************************************/
void Motor_MoveAbs(int32_t step, float fre_max, float fre_min, Motor_Status flag)
{
		SpeedRampData temp;  

		CalculateSModelLine(fre_max, fre_min);
  
    step = step - X_pos;         // 绝对位移
		temp.MotorX = flag;
	
    //X_ENA(X_TIMx, ENABLE);

    // X_ENA(X_TIMx, ENABLE);
    // 设置运动方向, step<0逆时针, >0 顺时针
    if(step < 0)
    {
			temp.dir = CCW; 
			step = -step; 
    }else{
			temp.dir = CW;               
    }

		temp.total_count = step;    // 记录要走的总步数

    // 当旋转角度很小时，直接以启动频率开始运行
    if(step == 1)
    {
        temp.accel_count = 1;
        temp.step_arr = (int16_t)((T1_FREQ/fre_min) - 1);    // 计算重装载值
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
		Motor_Config(temp);						  // 电机配置初始化	

		// TIM输出通道使能
		if(X_MOTOR == srd.MotorX) X_TIM_EnableOC;
		else if(Z_MOTOR == srd.MotorX) Z_TIM_EnableOC;
		else if(TP_MOTOR == srd.MotorX) TP_TIM_EnableOC;
		
    TIM_Cmd(X_TIMx, ENABLE);        // 定时器使能
}



/************************************************
* 函数名:   	Motor_Speed_Adjust
* 函数功能: 	Motor速度调节
* 输入: 		TIM
	
* 输出:			无
*************************************************/
void Motor_Speed_Adjust(TIM_TypeDef *TIM, SpeedRampData *m_srd)
{
		static uint16_t step_count = 0;                 // 总移步数计数器
	
		switch(m_srd->run_state)
		{
				case STOP:	
						step_count = 0;
						Status = 0;        // 设置记录状态
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
			
						}else if(step_count >= m_srd->decel_start){                          // 直接进入减速阶段
								m_srd->step_arr = __ARR[step_count-1];
								m_srd->run_state = DECEL;
			
						}else if(step_count >= STEP_S){    
								// 进入匀速阶段
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
* 函数名:	  	Motor_Y1_Init
* 函数功能:  	初始化Y1电机
* 输入: 			fre(电机运行频率)
							Dir(方向:CW顺时针, CCW逆时针)
* 输出: 			无
************************************************/
void Motor_Y1_Init(uint16_t arr, uint16_t ccr, Motor_Status Dir)
{
		TIM3_ARR = arr;
		TIM3_CC  = ccr; 

		srd.MotorX = Y1_MOTOR;                           // 设置电机型号
		TIM_PrescalerConfig(Y1_TIMx, TIM_PRESCALER, TIM_PSCReloadMode_Immediate);  // 重新设置定时器预分频系数

	
		if(CW == Dir) Y1_DIR_SET;
		else if(CCW == Dir) Y1_DIR_RESET; 
	
		Y1_TIM_SetAutoreload(Y1_TIMx, TIM3_ARR);
		Y1_TIM_SetCompare(Y1_TIMx, TIM3_CC);
	
		Y1_TIM_EnableOC;								 								// 使能Y1定时器通道
	  TIM_Cmd(Y1_TIMx, ENABLE);        								// 定时器使能
}	


/************************************************
* 函数名:	  	Motor_Y1_Init
* 函数功能:  	初始化Y1电机
* 输入: 			fre(电机运行频率)
							Dir(方向:CW顺时针, CCW逆时针)
* 输出: 			无
************************************************/
void Motor_Y2_Init(uint16_t arr, uint16_t ccr, Motor_Status Dir)
{
		Y2_ARR = arr;
		Y2_CC	 = ccr;

		if(CW == Dir) Y2_DIR_SET;
		else if(CCW == Dir) Y2_DIR_RESET; 
	
		Y2_TIM_SetAutoreload(Y2_TIMx, Y2_ARR);
		Y2_TIM_SetCompare(Y2_TIMx, Y2_CC);
	
		Y2_TIM_EnableOC;								 // 使能Y2时器通道
	  TIM_Cmd(Y2_TIMx, ENABLE);        // 定时器使能
}	


/********************************************
* 函数名: 		Servo_Config
* 函数功能: 	舵机控制抓手初始化
* 输入: 		无
* 输出: 		无
*********************************************/
void Servo_Config(void)
{
		Servo_Close();
		srd.MotorX = Catch_Servo;                           														// 设置电机型号
		TIM_PrescalerConfig(Servo_TIMx, SERVO_PRESCALER, TIM_PSCReloadMode_Immediate);  // 重新设置定时器预分频系数
		
		Servo_TIM_SetAutoreload(Servo_TIMx , TIM3_ARR);
		Servo_TIM_SetCompare(Servo_TIMx, TIM3_CC);																	 		// 初始化角度为0
		
		Servo_TIM_EnableOC;																															// 使能舵机通道
		TIM_Cmd(Servo_TIMx, ENABLE);
	
		delay_ms(1000);
		TIM_Cmd(Servo_TIMx, DISABLE);
		Servo_TIM_DisableOC;
}


/************************************************
* 函数名:	  	Set_TIM3_CC
* 函数功能:  	设置TIM3比较值
* 输入: 			Compare: 10~
* 输出: 			无
************************************************/
void Set_TIM3_CC(float Compare)
{
		TIM3_CC = (SERVO_PRELOAD/400)*Compare;
}


/*******************************************************
* 函数名:			Servo_Close
* 函数功能:		抓手闭合,(((20000/200)*11)/2000)*160 = 198° 
* 输入:				无
* 输出:				无
********************************************************/
void Servo_Close(void)
{
		delay_ms(500);
		TIM3_ARR = SERVO_PRELOAD;
		TIM3_CC  = (SERVO_PRELOAD/400)*25;   
	
		srd.MotorX = Catch_Servo;                           														// 设置电机型号
		TIM_PrescalerConfig(Servo_TIMx, SERVO_PRESCALER, TIM_PSCReloadMode_Immediate);  // 重新设置定时器预分频系数
		
		Servo_TIM_SetAutoreload(Servo_TIMx , TIM3_ARR);
		Servo_TIM_SetCompare(Servo_TIMx, TIM3_CC);																	 		// 初始化角度为0
		
		Servo_TIM_EnableOC;																															// 使能舵机通道
		TIM_Cmd(Servo_TIMx, ENABLE);
		
		delay_ms(500);
		TIM_Cmd(Servo_TIMx, DISABLE);
		Servo_TIM_DisableOC;
}


/*******************************************************
* 函数名:			Servo_Open
* 函数功能:		抓手张开, 0°
* 输入:				无
* 输出:				无
********************************************************/
void Servo_Open(void)
{	
		delay_ms(500);
		TIM3_ARR = SERVO_PRELOAD;
		TIM3_CC  = (SERVO_PRELOAD/400)*10; 

		srd.MotorX = Catch_Servo;                           														// 设置电机型号
		TIM_PrescalerConfig(Servo_TIMx, SERVO_PRESCALER, TIM_PSCReloadMode_Immediate);  // 重新设置定时器预分频系数
		
		Servo_TIM_SetAutoreload(Servo_TIMx , TIM3_ARR);
		Servo_TIM_SetCompare(Servo_TIMx, TIM3_CC);																	 		// 初始化角度为0
		
		Servo_TIM_EnableOC;																															// 使能舵机通道
		TIM_Cmd(Servo_TIMx, ENABLE);

		delay_ms(800);
		TIM_Cmd(Servo_TIMx, DISABLE);
		Servo_TIM_DisableOC;	
}


/************************************************
* 函数名:	  	TIM2_IRQHandler
* 函数功能:  	TIM2中断处理, 调节X/Z/抓手电机速度
* 输入: 			无
* 输出: 			无
************************************************/
void TIM2_IRQHandler(void)
{
    if(TIM_GetITStatus(X_TIMx, TIM_IT_Update) != RESET)
    {
				TIM_ClearITPendingBit(X_TIMx, TIM_IT_Update);
   
				Motor_Speed_Adjust(X_TIMx, &srd);    																					// 设置srd
			
        X_TIM_SetAutoreload(X_TIMx, srd.step_arr);	
				if(X_MOTOR == srd.MotorX) X_TIM_SetCompare(X_TIMx, (srd.step_arr/2));      		// 设置X轴CCR
				else if(Z_MOTOR == srd.MotorX) Z_TIM_SetCompare(X_TIMx, (srd.step_arr/2)); 		// 设置Z轴CCR
				else if(TP_MOTOR == srd.MotorX) TP_TIM_SetCompare(X_TIMx, (srd.step_arr/2)); 	// 设置TP轴CCR
    }
}


/************************************************
* 函数名:	  	TIM3_IRQHandler
* 函数功能:  	TIM3中断处理, 设置重装载值,Y1
* 输入: 			无
* 输出: 			无
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
* 函数名:	  	TIM4_IRQHandler
* 函数功能:  	TIM4中断处理, 设置重装载值
* 输入: 			无
* 输出: 			无
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







