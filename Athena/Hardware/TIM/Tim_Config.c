#include "Tim_Config.h"



/*************************************************
* ������:Enable_TIMX_OCXIni
* ��������:TIMͨ��X(1/2/3/4)���ʹ��
* ����: TIMX(TIM1/8/2/3/4/5)    
*		TIM_OCXInit(����ָ��:TIM_OC1Init/TIM_OC2Init/TIM_OC3Init/TIM_OC4Init)
* ���: ��
***************************************************/
void Enable_TIMX_OCXInit(TIM_TypeDef* TIMX, void(*TIM_OCXInit)(TIM_TypeDef*,TIM_OCInitTypeDef* ))
{
	TIM_OCInitTypeDef TIM_OCInitStructure;
	// ����ȽϽṹ���ʼ��
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;								// ����ΪPWMģʽ2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;					// ���ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;						// ���ͨ����ƽ��������
	TIM_OCXInit(TIMX, &TIM_OCInitStructure);   										// ��ʼ��TIMX��CHXΪPWN���
}


/*************************************************
* ������:Disable_TIMX_OCXInit
* ��������:TIMͨ��X(1/2/3/4)���ʹ��
* ����: TIMX(TIM1/8/2/3/4/5)   
*       TIM_OCXInit(����ָ��:TIM_OC1Init/TIM_OC2Init/TIM_OC3Init/TIM_OC4Init)
* ���: ��
***************************************************/
void Disable_TIMX_OCXInit(TIM_TypeDef* TIMX, void(*TIM_OCXInit)(TIM_TypeDef*,TIM_OCInitTypeDef* ))
{
	TIM_OCInitTypeDef TIM_OCInitStructure;
	// ����ȽϽṹ���ʼ��
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;								// ����ΪPWMģʽ2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;					// ���ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;						// ���ͨ����ƽ��������
	TIM_OCXInit(TIMX, &TIM_OCInitStructure);   										// ��ʼ��TIMX��CHXΪPWN���
}


/*************************************************
* ������:TIM2_Init
* ��������:��ʼ��TIM2
* ����: arr(��װ��ֵ)
		psc(Ԥ��Ƶֵ)
* ���: ��
***************************************************/
void TIM2_Init(uint16_t arr, uint16_t psc)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		//TIM_OCInitTypeDef TIM_OCInitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
		TIM_TimeBaseInitTypeDef TIM_Structure;
	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOA, ENABLE);     // ��������ʱ�Ӻ�GPIOAʱ��
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);        // ʹ��TIM2
	
		// GPIO��ʼ��
		GPIO_InitStructure.GPIO_Pin = TIM2_CH2_PIN|TIM2_CH3_PIN|TIM2_CH4_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;     // �������
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(TIM2_CH2_PORT, &GPIO_InitStructure);
	
		// ����ȽϽṹ���ʼ��
		Disable_TIMX_OCXInit(TIM2, TIM_OC2Init);
		Disable_TIMX_OCXInit(TIM2, TIM_OC3Init);
		Disable_TIMX_OCXInit(TIM2, TIM_OC4Init);
	
		
		
		// �����װ��ʹ��
		TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);  
		TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);  
		TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);            

		TIM_ClearFlag(TIM2, TIM_FLAG_Update);                         // ���ö�ʱ������FLAG
		TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);                    // ʹ�ܶ�ʱ�������ж�
	
		// TIM2Ƕ��������ʼ��
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);      						// ���ȼ���2
		NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;								// �ж�ͨ��
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;    			// ��ռ���ȼ�0x02 
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;						// �����ȼ�0x02		
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
		
		// TIM2ʱ���ṹ���ʼ��
		TIM_Structure.TIM_Period = arr;                        //�����Զ���װ��ֵ
		TIM_Structure.TIM_Prescaler = psc;                     //����Ԥ��Ƶֵ
		TIM_Structure.TIM_ClockDivision = 0;                   //����ʱ�ӷָTDTS = Tck_tim
		TIM_Structure.TIM_CounterMode = TIM_CounterMode_Up;    //TIM���ϼ���
		TIM_TimeBaseInit(TIM2, &TIM_Structure);                //��ʼ��TIM2
		
		TIM_Cmd(TIM2, DISABLE);
}



/*************************************************
* ������:		TIM3_Init
* ��������:	��ʼ��TIM3
* ����: 		arr(��װ��ֵ)
						psc(Ԥ��Ƶֵ)
* ���: 		��
***************************************************/
void TIM3_Init(uint16_t arr, uint16_t psc)
{
		GPIO_InitTypeDef GPIO_InitStructure;
	
		//TIM_OCInitTypeDef TIM_OCInitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
		TIM_TimeBaseInitTypeDef TIM_Structure;
	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOA, ENABLE);     // ��������ʱ�Ӻ�GPIOAʱ��
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);        									// ʹ��TIM3
	
		// GPIO��ʼ��
		GPIO_InitStructure.GPIO_Pin = TIM3_CH1_PIN|TIM3_CH2_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;     													// �������
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(TIM3_CH1_PORT, &GPIO_InitStructure);
	
		// ����ȽϽṹ���ʼ��
		Disable_TIMX_OCXInit(TIM3, TIM_OC1Init);
		Disable_TIMX_OCXInit(TIM3, TIM_OC2Init);
		
		
		// �����װ��ʹ��
		TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  
		TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);           

		TIM_ClearFlag(TIM3, TIM_FLAG_Update);                         // ���ö�ʱ������FLAG
		TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);                    // ʹ�ܶ�ʱ�������ж�
	
		// TIM2Ƕ��������ʼ��
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);      						// ���ȼ���2
		NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;									// �ж�ͨ��
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;    // ��ռ���ȼ�0x02 
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;						// �����ȼ�0x02		
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
		
		// TIM2ʱ���ṹ���ʼ��
		TIM_Structure.TIM_Period = arr;                        //�����Զ���װ��ֵ
		TIM_Structure.TIM_Prescaler = psc;                     //����Ԥ��Ƶֵ
		TIM_Structure.TIM_ClockDivision = 0;                   //����ʱ�ӷָTDTS = Tck_tim
		TIM_Structure.TIM_CounterMode = TIM_CounterMode_Up;    //TIM���ϼ���
		TIM_TimeBaseInit(TIM3, &TIM_Structure);                //��ʼ��TIM2
		
		TIM_Cmd(TIM3, DISABLE);
}


/*************************************************
* ������:		TIM4_Init
* ��������:	��ʼ��TIM4
* ����: 		arr(��װ��ֵ)
						psc(Ԥ��Ƶֵ)
* ���: 		��
*************************************************/
void TIM4_Init(uint16_t arr, uint16_t psc)
{
		GPIO_InitTypeDef GPIO_InitStructure;
	
		//TIM_OCInitTypeDef TIM_OCInitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
		TIM_TimeBaseInitTypeDef TIM_Structure;
	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOB, ENABLE);     // ��������ʱ�Ӻ�GPIOBʱ��
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);        									// ʹ��TIM4
	
		// GPIO��ʼ��
		GPIO_InitStructure.GPIO_Pin = TIM4_CH2_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;     													// �������
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(TIM4_CH2_PORT, &GPIO_InitStructure);
	
		// ����ȽϽṹ���ʼ��
		Disable_TIMX_OCXInit(TIM4, TIM_OC2Init);
		
		// �����װ��ʹ�� 
		TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);           

		TIM_ClearFlag(TIM4, TIM_FLAG_Update);                         // ���ö�ʱ������FLAG
		TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);                    // ʹ�ܶ�ʱ�������ж�
	
		// TIM2Ƕ��������ʼ��
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);      						// ���ȼ���2
		NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;									// �ж�ͨ��
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;    // ��ռ���ȼ�0x02 
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;						// �����ȼ�0x02		
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
		
		// TIM2ʱ���ṹ���ʼ��
		TIM_Structure.TIM_Period = arr;                        //�����Զ���װ��ֵ
		TIM_Structure.TIM_Prescaler = psc;                     //����Ԥ��Ƶֵ
		TIM_Structure.TIM_ClockDivision = 0;                   //����ʱ�ӷָTDTS = Tck_tim
		TIM_Structure.TIM_CounterMode = TIM_CounterMode_Up;    //TIM���ϼ���
		TIM_TimeBaseInit(TIM4, &TIM_Structure);                //��ʼ��TIM4
		
		TIM_Cmd(TIM4, DISABLE);
}
