#include "catch.h"


uint8_t Catch_Status = 0;                 // 抓手状态
int32_t m_Fre=0;

void Catch_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOC,&GPIO_InitStructure);	
	
    GPIO_SetBits(GPIOC,GPIO_Pin_7);
    GPIO_SetBits(GPIOC,GPIO_Pin_8);
}

void Catch(sw SW)
{
	switch(SW)
	{
		case OPEN:
			Catch_Status = 1;
			GPIO_SetBits(GPIOC, GPIO_Pin_7);
			GPIO_ResetBits(GPIOC,GPIO_Pin_8);
			break;

		case CLOSE:
			Catch_Status = 1;
			GPIO_SetBits(GPIOC,GPIO_Pin_8);
			GPIO_ResetBits(GPIOC,GPIO_Pin_7);
			break;

		case OFF:
			GPIO_SetBits(GPIOC,GPIO_Pin_7);
			GPIO_SetBits(GPIOC,GPIO_Pin_8);
			Catch_Status = 0;
			break;
	}	
}


/**************************************
 * 函数名：Catch_Init
 * 函数功能：初始化Catch电机控制器
 * 输入：无
 * 输出：无
 * **************************************/
void Catch_Init(void)
{
	  GPIO_InitTypeDef GPIO_InitStructure;
    
		RCC_APB2PeriphClockCmd(Catch_DIR_CLK ,ENABLE);

		// 初始化定时器TIM8_CH
		TIM8_CH2_Init(9999, 8);

    // 初始化DIR、ENA
	
    GPIO_InitStructure.GPIO_Pin = Catch_DIR_PIN|Catch_ENA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_Init(Catch_DIR_PORT, &GPIO_InitStructure);

    Catch_DIR_SET;         // 初始化为顺时针
    Catch_ENA_RESET;       // 关闭脱机功能

    Catch_TIM_Init;        // 初始化PUL定时器

    TIM_OC2PreloadConfig(Catch_TIMx, TIM_OCPreload_Enable);
    TIM_ClearFlag(Catch_TIMx, TIM_FLAG_Update);
    TIM_ITConfig(Catch_TIMx, TIM_IT_Update, ENABLE);

    TIM_Cmd(Catch_TIMx, ENABLE);
}



/****************************************
 * 函数名：Catch_Start
 * 函数功能：启动电机
 * 输入：int32_t fre; int8_t DIR:(0:DIR_SET, 1:DIR_RESET)
 * 输出：无
 * *************************************/
void Catch_Start(int32_t fre, int8_t DIR)
{
    m_Fre = fre;

    if(DIR==0)
    {
        Catch_DIR_SET;          // 顺时针
    }else if(DIR==1)
    {
        Catch_DIR_RESET;        // 逆时针
    }

    Catch_TIM_SetAutoreload(Catch_TIMx, m_Fre);
    Catch_TIM_SetCompare2(Catch_TIMx, (m_Fre/2));

    TIM_Cmd(Catch_TIMx, ENABLE);
    TIM_Cmd(Catch_TIMx, ENABLE);
}


void TIM8_IRQHandler(void)
{
    if(TIM_GetITStatus(Catch_TIMx, TIM_IT_Update) != RESET)
    {
        Catch_TIM_SetAutoreload(Catch_TIMx, m_Fre);
        Catch_TIM_SetCompare2(Catch_TIMx, (m_Fre/2));

        TIM_ClearITPendingBit(Catch_TIMx, TIM_IT_Update);
    }
}
