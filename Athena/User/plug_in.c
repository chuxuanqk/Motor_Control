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
* ������:		Usart_Size
* ��������:	����usart����Ϣsize
* ����:			*buf:uart�Ļ�����
*	���:			count:����ֵ
**********************************/
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
* ������:		Recv_Int
*	��������: 	���ڽ���ֵתΪINT
* ����:			��
* ���:			Value:
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
* ������:		Decode
* ��������:	�������ڴ��������
* ����:			��
* ���:			��
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
* ������:		Stamper_Init
* ��������:	ӡ�»����ó�ʼ��
* ����:			��
* ���:			��
* ***********************************/
void Stamper_Init(void)
{
		delay_init();
		NVIC_Configuration();

		uart_init(9600);
		LED_Init();
		EXTIX_Config();
		DIR_ENA_GPIO_Config();

		TIM2_Init(TIM_PRELOAD, TIM_PRESCALER);
		TIM3_Init(TIM_PRELOAD, TIM_PRESCALER);
		//TIM3_Init(SERVO_PRELOAD, SERVO_PRESCALER);
		TIM4_Init(TIM_PRELOAD, TIM_PRESCALER);	
}


/********************************************
* ������: 		SimpleTest
* ��������: 	��ز���ģ��
* ����: 		��
* ���: 		��
*********************************************/
void SimpleTest(void)
{	
		int16_t Servo_cc=5;
		Servo_Config();
		while(1)
		{
			if(FLAG == Start)
			{
					FLAG = 0;
					printf("Start\r\n");
					Servo_Open();
			}		
			
			if(USART_RX_STA&0x8000)
			{
					Servo_cc =  (USART_RX_BUF[0]-48)*10+(USART_RX_BUF[1]-48);
					Set_TIM3_CC(Servo_cc);
					printf("CC:%d\r\n", Servo_cc);
					USART_RX_STA=0;
			}
		}
}	


/********************************************
* ������: 			Motor_Reset
* ��������: 		X/Z/ץ�ָ�λ
* ����: 			��
* ���: 			��
*********************************************/
void Motor_Reset(void)
{
		int32_t x_rad = 800*4;
		int32_t z_rad = 800*3;
		int16_t x_maxfre = 800*5;
		int16_t x_minfre = 800;
		int16_t z_maxfre = 800*4;
		int16_t z_minfre = 800;
        

		// X�Ḵλ
		EXTIX_DISABLE(EXTI2_IRQn);
		Motor_Move(-x_rad, x_maxfre, x_minfre, X_MOTOR);       	// X�����˶�
		while(Status != 0);
	
		Motor_Move((8*x_rad), x_maxfre, x_minfre, X_MOTOR);     // X�����˶�
		while(Status != 0);
		EXTIX_ENABLE(EXTI2_IRQn);
	
		// Z�Ḵλ����
		Motor_Move(-z_rad, z_maxfre, z_minfre, Z_MOTOR);				
		while(Status != 0);
		Motor_Move(z_rad, z_maxfre, z_minfre, Z_MOTOR);
		while(Status != 0);

		// ���ץ�ָ�λ
		Servo_Config();
		delay_s(1);
		
		TIM_Cmd(Servo_TIMx, DISABLE);
		Servo_TIM_DisableOC;
}	

	
/********************************************
* ������: 			Paper_Move_In
* ��������: 		��ֽ��һ�׶�
* ����: 			��	
* ���: 			��
*********************************************/
void Paper_Move_In(void)
{
		uint16_t Y1_arr = 4999;     // 9999
		uint16_t Y2_arr = 4999;
	
		Motor_Y1_Init(Y1_arr, (Y1_arr/2), CCW);
		Motor_Y2_Init(Y2_arr, (Y2_arr/2), CCW);
	
		while(K1==1);
		if(FLAG == PTE1)
		{
				FLAG = 0;
				delay_s(3);
				printf("Y1ֹͣ\r\n");
				Y1_TIM_DisableOC;
				EXTIX_ENABLE(EXTI9_5_IRQn);
		}
		while(K2==1);
		if(K2 == 0) Y2_TIM_DisableOC;
}
	

/********************************************
* ������: 			Paper_Move_Out
* ��������: 		��ֽ�ڶ��׶�
* ����: 			��	
* ���: 			��
*********************************************/
void Paper_Move_Out(void)
{
		uint16_t Y2_arr = 4999;
		FLAG = 0;
		Motor_Y2_Init(Y2_arr, (Y2_arr/2), CCW);
		while(K2 == 0);
		Y2_TIM_DisableOC;
}


/********************************************
* ������: 			Cover_Seal
* ��������: 		����ģ��
* ����: 			X_mm:X������, Z_mm:Z������
* ���: 			��
*********************************************/
void Cover_Seal(int32_t X_mm, int32_t Z_mm)
{
//		int32_t x_rad = 800*45;      // 180mm
//		int32_t z_rad = 800*10;      // 40mm
		int32_t x_rad = MM2Step(X_mm);
		int32_t z_rad = MM2Step(Z_mm);
	
		// ����ٶȲ���
		int16_t x_maxfre = 800*10;
		int16_t x_minfre = 800;
		int16_t z_maxfre = 800*4;
		int16_t z_minfre = 800;
		
		// X�������˶�
		delay_ms(1000);
		Motor_Move(-x_rad, x_maxfre, x_minfre, X_MOTOR);
		while(Status != 0);
		delay_ms(2000);
		// Z�������˶�
		Motor_Move(-z_rad, z_maxfre, z_minfre, Z_MOTOR);
		while(Status != 0);
		delay_s(2);
		// Z�������˶�
		Motor_Move(z_rad, z_maxfre, z_minfre, Z_MOTOR);
		while(Status != 0);
		// X�������˶�
		Motor_Move(x_rad, x_maxfre, x_minfre, X_MOTOR);
		while(Status != 0);
}


/***********************************************
* ������: 		Exchange_Seal
* ��������:	����ӡ��
* ����:			Z_mm:Z������
* ���:			��
************************************************/
void Exchange_Seal(int32_t Z_mm, int32_t Seal_id)
{
		int32_t z_rad = MM2Step(Z_mm);
	
		// ����ٶȲ���
		int16_t z_maxfre = 800*4;
		int16_t z_minfre = 800;
		int16_t tp_maxfre = 800*4;
		int16_t tp_minfre = 800;

	
		// ����ӡ��
		Motor_Move(Seal_id, tp_maxfre, tp_minfre, TP_MOTOR);
		while(Status != 0);
	

		Servo_Open();
	
		printf("�����˶�\r\n");
		// Z�������˶�
		Motor_Move(-z_rad, z_maxfre, z_minfre, Z_MOTOR);
		while(Status != 0);

		Servo_Close();
	
		
		printf("�����˶�\r\n");
		// Z�������˶�
		Motor_Move(z_rad, z_maxfre, z_minfre, Z_MOTOR);
		while(Status != 0);	
}


void Reset_Seal(int32_t Z_mm, int32_t Seal_id)
{
		int32_t z_rad = MM2Step(Z_mm);
	
		// ����ٶȲ���
		int16_t z_maxfre = 800*4;
		int16_t z_minfre = 800;
		int16_t tp_maxfre = 800*4;
		int16_t tp_minfre = 800;
	
		// Z�������˶�
		Motor_Move(-z_rad, z_maxfre, z_minfre, Z_MOTOR);
		while(Status != 0);

		Servo_Open();
		
		// Z�������˶�
		Motor_Move(z_rad, z_maxfre, z_minfre, Z_MOTOR);
		while(Status != 0);	
	
		Servo_Close();
	
		// ��λӡ��λ��
		Motor_Move(-Seal_id, tp_maxfre, tp_minfre, TP_MOTOR);
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
		int32_t z_mm = 15;                  // ����ȡӡ�µ��г�
		int32_t seal_id = SEAL_ID[2];
		
		//delay_s(2);

		// 1.�����λ��ʼ��
		Motor_Reset(); 
	
		//Servo_Config();

		while(1)
		{
				// 1.��������
				if(FLAG == Start)
				{
						FLAG = 0;
					
						// 1.1 ��ֽ
						Paper_Move_In();
					
						EXTIX_DISABLE(EXTI9_5_IRQn);
					
						// 1.2 ѡȡӡ��
						Exchange_Seal(z_mm, seal_id);	
						// 1.3 ���ø�������
						Cover_Seal(190, 40);   
						// 1.4 ӡ�¸�λ
						Reset_Seal(z_mm, seal_id);
					
						EXTIX_ENABLE(EXTI9_5_IRQn);
					
						// 1.5 ��ֽ
						Paper_Move_Out();
						FLAG = 0;
						printf("finish\r\n");
				}
				
				// 2. ����ӡ�� 
				if(FLAG == S2_KEY)
				{
						FLAG = 0;
					
						EXTIX_DISABLE(EXTI9_5_IRQn);
						// 1.2 ѡȡӡ��
						Exchange_Seal(z_mm, seal_id);	
						// 1.4 ӡ�¸�λ
						Reset_Seal(z_mm, seal_id);
						EXTIX_ENABLE(EXTI9_5_IRQn);
						Paper_Move_In();
						FLAG = 0;
				}
		}
}





