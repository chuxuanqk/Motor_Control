#include "Tim_Config.h"



/*************************************************
* 函数名:Enable_TIMX_OCXIni
* 函数功能:TIM通道X(1/2/3/4)输出使能
* 输入: TIMX(TIM1/8/2/3/4/5)    
*		TIM_OCXInit(函数指针:TIM_OC1Init/TIM_OC2Init/TIM_OC3Init/TIM_OC4Init)
* 输出: 无
***************************************************/
void Enable_TIMX_OCXInit(TIM_TypeDef* TIMX, void(*TIM_OCXInit)(TIM_TypeDef*,TIM_OCInitTypeDef* ))
{
	TIM_OCInitTypeDef TIM_OCInitStructure;
	// 输出比较结构体初始化
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;								// 配置为PWM模式2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;					// 输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;						// 输出通道电平极性配置
	TIM_OCXInit(TIMX, &TIM_OCInitStructure);   										// 初始化TIMX的CHX为PWN输出
}


/*************************************************
* 函数名:Disable_TIMX_OCXInit
* 函数功能:TIM通道X(1/2/3/4)输出使能
* 输入: TIMX(TIM1/8/2/3/4/5)   
*       TIM_OCXInit(函数指针:TIM_OC1Init/TIM_OC2Init/TIM_OC3Init/TIM_OC4Init)
* 输出: 无
***************************************************/
void Disable_TIMX_OCXInit(TIM_TypeDef* TIMX, void(*TIM_OCXInit)(TIM_TypeDef*,TIM_OCInitTypeDef* ))
{
	TIM_OCInitTypeDef TIM_OCInitStructure;
	// 输出比较结构体初始化
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;								// 配置为PWM模式2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;					// 输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;						// 输出通道电平极性配置
	TIM_OCXInit(TIMX, &TIM_OCInitStructure);   										// 初始化TIMX的CHX为PWN输出
}


/*************************************************
* 函数名:TIM2_Init
* 函数功能:初始化TIM2
* 输入: arr(重装载值)
		psc(预分频值)
* 输出: 无
***************************************************/
void TIM2_Init(uint16_t arr, uint16_t psc)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		//TIM_OCInitTypeDef TIM_OCInitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
		TIM_TimeBaseInitTypeDef TIM_Structure;
	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOA, ENABLE);     // 开启复用时钟和GPIOA时钟
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);        // 使能TIM2
	
		// GPIO初始化
		GPIO_InitStructure.GPIO_Pin = TIM2_CH2_PIN|TIM2_CH3_PIN|TIM2_CH4_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;     // 复用输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(TIM2_CH2_PORT, &GPIO_InitStructure);
	
		// 输出比较结构体初始化
		Disable_TIMX_OCXInit(TIM2, TIM_OC2Init);
		Disable_TIMX_OCXInit(TIM2, TIM_OC3Init);
		Disable_TIMX_OCXInit(TIM2, TIM_OC4Init);
	
		
		
		// 输出重装载使能
		TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);  
		TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);  
		TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);            

		TIM_ClearFlag(TIM2, TIM_FLAG_Update);                         // 设置定时器更新FLAG
		TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);                    // 使能定时器更新中断
	
		// TIM2嵌套向量初始化
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);      						// 优先级组2
		NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;								// 中断通道
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;    			// 抢占优先级0x02 
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;						// 子优先级0x02		
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
		
		// TIM2时基结构体初始化
		TIM_Structure.TIM_Period = arr;                        //设置自动重装载值
		TIM_Structure.TIM_Prescaler = psc;                     //设置预分频值
		TIM_Structure.TIM_ClockDivision = 0;                   //设置时钟分割：TDTS = Tck_tim
		TIM_Structure.TIM_CounterMode = TIM_CounterMode_Up;    //TIM向上计数
		TIM_TimeBaseInit(TIM2, &TIM_Structure);                //初始化TIM2
		
		TIM_Cmd(TIM2, DISABLE);
}



/*************************************************
* 函数名:		TIM3_Init
* 函数功能:	初始化TIM3
* 输入: 		arr(重装载值)
						psc(预分频值)
* 输出: 		无
***************************************************/
void TIM3_Init(uint16_t arr, uint16_t psc)
{
		GPIO_InitTypeDef GPIO_InitStructure;
	
		//TIM_OCInitTypeDef TIM_OCInitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
		TIM_TimeBaseInitTypeDef TIM_Structure;
	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOA, ENABLE);     // 开启复用时钟和GPIOA时钟
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);        									// 使能TIM3
	
		// GPIO初始化
		GPIO_InitStructure.GPIO_Pin = TIM3_CH1_PIN|TIM3_CH2_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;     													// 复用输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(TIM3_CH1_PORT, &GPIO_InitStructure);
	
		// 输出比较结构体初始化
		Disable_TIMX_OCXInit(TIM3, TIM_OC1Init);
		Disable_TIMX_OCXInit(TIM3, TIM_OC2Init);
		
		
		// 输出重装载使能
		TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  
		TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);           

		TIM_ClearFlag(TIM3, TIM_FLAG_Update);                         // 设置定时器更新FLAG
		TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);                    // 使能定时器更新中断
	
		// TIM2嵌套向量初始化
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);      						// 优先级组2
		NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;									// 中断通道
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;    // 抢占优先级0x02 
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;						// 子优先级0x02		
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
		
		// TIM2时基结构体初始化
		TIM_Structure.TIM_Period = arr;                        //设置自动重装载值
		TIM_Structure.TIM_Prescaler = psc;                     //设置预分频值
		TIM_Structure.TIM_ClockDivision = 0;                   //设置时钟分割：TDTS = Tck_tim
		TIM_Structure.TIM_CounterMode = TIM_CounterMode_Up;    //TIM向上计数
		TIM_TimeBaseInit(TIM3, &TIM_Structure);                //初始化TIM2
		
		TIM_Cmd(TIM3, DISABLE);
}


/*************************************************
* 函数名:		TIM4_Init
* 函数功能:	初始化TIM4
* 输入: 		arr(重装载值)
						psc(预分频值)
* 输出: 		无
*************************************************/
void TIM4_Init(uint16_t arr, uint16_t psc)
{
		GPIO_InitTypeDef GPIO_InitStructure;
	
		//TIM_OCInitTypeDef TIM_OCInitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
		TIM_TimeBaseInitTypeDef TIM_Structure;
	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOB, ENABLE);     // 开启复用时钟和GPIOB时钟
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);        									// 使能TIM4
	
		// GPIO初始化
		GPIO_InitStructure.GPIO_Pin = TIM4_CH2_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;     													// 复用输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(TIM4_CH2_PORT, &GPIO_InitStructure);
	
		// 输出比较结构体初始化
		Disable_TIMX_OCXInit(TIM4, TIM_OC2Init);
		
		// 输出重装载使能 
		TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);           

		TIM_ClearFlag(TIM4, TIM_FLAG_Update);                         // 设置定时器更新FLAG
		TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);                    // 使能定时器更新中断
	
		// TIM2嵌套向量初始化
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);      						// 优先级组2
		NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;									// 中断通道
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;    // 抢占优先级0x02 
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;						// 子优先级0x02		
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
		
		// TIM2时基结构体初始化
		TIM_Structure.TIM_Period = arr;                        //设置自动重装载值
		TIM_Structure.TIM_Prescaler = psc;                     //设置预分频值
		TIM_Structure.TIM_ClockDivision = 0;                   //设置时钟分割：TDTS = Tck_tim
		TIM_Structure.TIM_CounterMode = TIM_CounterMode_Up;    //TIM向上计数
		TIM_TimeBaseInit(TIM4, &TIM_Structure);                //初始化TIM4
		
		TIM_Cmd(TIM4, DISABLE);
}
