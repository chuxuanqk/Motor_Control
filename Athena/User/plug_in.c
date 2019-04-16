#include "plug_in.h"


Command cmd;
float FRE_MAX = 12800.0;
float FRE_MIN = 400.0;

int32_t num = 800;
float m_fre_max = 5600;
float m_fre_min = 400;
float m_flexible = 4.0;


int8_t Servo_flag = 0;
uint16_t arr = 19999;
uint16_t psc = 71;

int Power(int base, int32_t exponent)
{
    int ret=1.0;
    int i = 0;
    for(; i<=exponent; ++i)
    {
        ret*=base;
    }

    return ret;
}


/**********************************
 * 
 *      ����usart���͵���Ϣsize
 * 
 * *******************************/
int32_t Usart_Size(u8 *buf)
{
    int count=0;

    while(*buf != 0x0d)
    {
        ++buf;
        ++count;
    }
    return count;
}


/****************************************
 * 
 *       ���ڽ���ֵתΪINT
 * 
 **************************************/
int32_t Recv_Int(void)
{
    int32_t Value=0;
    uint16_t i=0;
    int32_t temp;

    if(USART_RX_STA&0x8000)
    {
        for(; i<(USART_RX_STA&0x3FFF); ++i)
        {   
			printf("Buf:%d\r\n", USART_RX_BUF[i]);
            temp = (USART_RX_BUF[i] - 48)*Power(10, i);
            Value += temp;
        }
		USART_RX_STA=0;
    }
    
    return Value;
}

/****************************************
 * 
 *       �������ڴ��������
 * 
 **************************************/
int16_t Decode(void)
{
    //uint16_t i=0;

    if(USART_RX_STA&0x8000)
    {
        if((USART_RX_STA&0x3FFF) != 12){
            return -1;
        }else{
            cmd.X_STEP =  (USART_RX_BUF[0]-48)*100+(USART_RX_BUF[1]-48)*10+(USART_RX_BUF[2]-48);
            cmd.Y_STEP =  (USART_RX_BUF[3]-48)*100+(USART_RX_BUF[4]-48)*10+(USART_RX_BUF[5]-48);
            cmd.X_STEP =  (USART_RX_BUF[6]-48)*100+(USART_RX_BUF[7]-48)*10+(USART_RX_BUF[8]-48);
            cmd.Y_STEP =  (USART_RX_BUF[9]-48)*100+(USART_RX_BUF[10]-48)*10+(USART_RX_BUF[11]-48);
            cmd.Z_STEP =  (USART_RX_BUF[12]-48)*100+(USART_RX_BUF[13]-48)*10+(USART_RX_BUF[14]-48);
            cmd.state  =  USART_RX_BUF[15];
            cmd.flag   =  USART_RX_BUF[16];
        }
        USART_RX_STA=0;
    }
    return 0;
}



/*************************************
 * ������:Stamper_Init
 * ��������:ӡ�»����ó�ʼ��
 * ����:��
 * ���:��
 * ***********************************/
void Stamper_Init(void)
{
		delay_init();
		NVIC_Configuration();

		uart_init(9600);
		LED_Init();
		EXTIX_Config();
		DIR_ENA_GPIO_Config();

		TIM2_Init(9999, 8);
		TIM3_Init(9999, 8);
		TIM4_Init(9999, 8);	
}


/********************************************
* ������: Stamper_Ctr
* ��������: �����ģ��
* ����: ��
* ���: ��
*********************************************/
void SimpleTest(void)
{
		if(FLAG == Start)
		{
				FLAG = 0;
				printf("Start\r\n");
			
				// X���˶�
				Motor_Move(8000, 6400, 800, X_MOTOR);
				while(Status != 0);
				printf("finish:X\r\n");
//				
//				// Z���˶�
//				Enable_TIMX_OCXInit(TIM2, TIM_OC3Init);
//				Motor_Move(-3200, 6400, 800, Z_MOTOR);
//				while(Status != 0);
//				printf("finish:Z\r\n");
//				Disable_TIMX_OCXInit(TIM2, TIM_OC3Init); 
//			
//				// TP���˶�
//				Enable_TIMX_OCXInit(TIM2, TIM_OC4Init);
//				Motor_Move(800, 6400, 800, TP_MOTOR);
//				while(Status != 0);
//				printf("finish:TP\r\n");
//				Disable_TIMX_OCXInit(TIM2, TIM_OC4Init); 
				
				// Y1��Y2���˶�
//				Motor_Y1_Init(9999, CCW);
//				Motor_Y2_Init(4999, CCW);
//				while(FLAG == 0);
//				printf("finish:%d\r\n", FLAG);
				
		}		
}	


/********************************************
* ������: 		Motor_Reset
* ��������: 	X/Z/ץ�ָ�λ
* ����: 			��
* ���: 			��
*********************************************/
void Motor_Reset(void)
{
		int rad = 800*40;
		// Z�Ḵλ����
		Motor_Move(-2400, 3200, 400, Z_MOTOR);
		while(Status != 0);
		Motor_Move(2400, 3200, 400, Z_MOTOR);
		while(Status != 0);

		// X�Ḵλ
		Motor_Move(3200, 3200, 400, X_MOTOR);
		while(Status != 0);
		Motor_Move(-rad, 3200, 400, X_MOTOR);
		while(Status != 0);
}	

	
/********************************************
* ������: 		Paper_Move_1
* ��������: 	��ֽ��һ�׶�
* ����: 			��	
* ���: 			��
*********************************************/
void Paper_Move_1(void)
{
			Motor_Y1_Init(9999, CCW);
			Motor_Y2_Init(4999, CCW);
			while(1)
			{
				if(FLAG == PTE2)
				{
					FLAG = 0;
					break;
				}
			}
}
	
/********************************************
* ������: 		Paper_Move_2
* ��������: 	��ֽ�ڶ��׶�
* ����: 			��	
* ���: 			��
*********************************************/
void Paper_Move_2(void)
{
		FLAG = 0;
		//Motor_Y1_Init(9999, CCW);
		Motor_Y2_Init(4999, CCW);
	
		delay_s(8);
		Disable_TIMX_OCXInit(Y2_TIMx, TIM_OC2Init);
		//while(FLAG == 0);
}

/********************************************
* ������: 		Stamped
* ��������: 	����ģ��
* ����: 			��
* ���: 			��
*********************************************/
void Stamped(void)
{
		int rad = 800*45;
		// X�������˶�
		delay_ms(1000);
		Motor_Move(rad, 6200, 800, X_MOTOR);
		while(Status != 0);
	
		delay_ms(2000);
		// Z�������˶�
		Motor_Move(-4800, 3200, 800, Z_MOTOR);
		while(Status != 0);
		delay_s(3);
		// Z�������˶�
		Motor_Move(4800, 3200, 800, Z_MOTOR);
		while(Status != 0);
	
		// X�������˶�
		Motor_Move(-rad, 6200, 800, X_MOTOR);
		while(Status != 0);
}


/********************************************
* ������: 		Stamper_Ctr
* ��������: 	�����ģ��
* ����: 		��
* ���: 		��
*********************************************/
void Stamper_Ctr(void)
{
	while(1)
	{
		//SimpleTest();
		if(FLAG == Start)
		{
				FLAG = 0;
				Motor_Reset();
				Paper_Move_1();
				Stamped();
				Paper_Move_2();
				Motor_Reset();
		}
	}
}




/********************************************
* ������: Servo_Config
* ��������: �������
* ����: ��
* ���: ��
*********************************************/

/*
void Servo_Config(void)
{

	TIM2_Init(arr, psc);
	TIM_SetAutoreload(TIM2, arr);
	TIM_SetCompare2(TIM2, arr/40);
	TIM_Cmd(TIM2, ENABLE);
	
	while(1)
	{
		if(FLAG == Start)
		{
			Servo_flag = 1;
			FLAG = 0;
		}else if(FLAG == 10)
		{
			Disable_TIMX_OCXInit(TIM2, TIM_OC2Init);
			FLAG = 0;
		}else if(FLAG == 11)
		{
			Enable_TIMX_OCXInit(TIM2, TIM_OC2Init);
			FLAG = 0;
		}
	}
}


void TIM2_IRQHandler(void)
{
    if(TIM_GetITStatus(X_TIMx, TIM_IT_Update) != RESET)
    {
		TIM_ClearITPendingBit(X_TIMx, TIM_IT_Update);
		if(Servo_flag == 0)
		{
			TIM_SetAutoreload(TIM2, arr);
			TIM_SetCompare2(TIM2, (arr/40)*3);
			TIM_SetCompare3(TIM2, (arr/40)*3);
		}else if(Servo_flag == 1){
			TIM_SetAutoreload(TIM2, arr);
			TIM_SetCompare2(TIM2, (arr/40));
			TIM_SetCompare3(TIM2, (arr/40));
		}
		

    }
}
*/


