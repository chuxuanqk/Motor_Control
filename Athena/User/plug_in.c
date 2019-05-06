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
						//printf("Buf:%d\r\n", USART_RX_BUF[i]);
            temp = (USART_RX_BUF[i] - 48)*Power(10, i);
            Value += temp;
        }
				if(12 == i)
				{
						printf("Recv finish\r\n");
				}
				USART_RX_STA=0;
				i=0;
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
    //int16_t i=0;

    if(USART_RX_STA&0x8000)
    {
				//LED = ~LED;
        if((USART_RX_STA&0x3FFF) != 10){
            return -1;
        }else{
						//printf("usart:%d\r\n", (USART_RX_STA&0x3FFF));
						cmd.X_MM =  (USART_RX_BUF[0]-48)*100+(USART_RX_BUF[1]-48)*10+(USART_RX_BUF[2]-48);
						cmd.Y_1_MM =  (USART_RX_BUF[3]-48)*100+(USART_RX_BUF[4]-48)*10+(USART_RX_BUF[5]-48);
						cmd.Y_2_MM=  (USART_RX_BUF[6]-48)*100+(USART_RX_BUF[7]-48)*10+(USART_RX_BUF[8]-48);
						//cmd.Y_2_MM = cmd.Y_2_MM*1.5;
						cmd.SEAL_ID =  (USART_RX_BUF[9]-48);
						
						printf("X_mm:%d\r\n", cmd.X_MM);
						printf("Y_1_mm:%d\r\n", cmd.Y_1_MM);
						printf("Y_2_mm:%d\r\n", cmd.Y_2_MM);
						printf("Z_mm:%d\r\n", cmd.SEAL_ID);
          }

				printf("usart:%d\r\n", (USART_RX_STA&0x3FFF));

					
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
		int32_t x_rad = 800*8;
		int32_t z_rad = 800*3;
		int32_t tp_rad = 200;
		int16_t x_maxfre = 800*5;
		int16_t x_minfre = 800;
		int16_t z_maxfre = 800*4;
		int16_t z_minfre = 800;
		int16_t tp_maxfre = 800;
		int16_t tp_minfre = 400;
	
		FLAG = 0;
		// ת�̸�λ
		Motor_Move(tp_rad, tp_maxfre, tp_minfre, TP_MOTOR);
		while(Status != 0);

		Motor_Move(-(tp_rad*4), tp_maxfre, tp_minfre, TP_MOTOR);
		while(Status != 0);
			
		// X�Ḵλ
		EXTIX_DISABLE(EXTI9_5_IRQn);
		Motor_Move(-x_rad, x_maxfre, x_minfre, X_MOTOR);       	// X�����˶�
		while(Status != 0);
	
		// Z�Ḵλ����
		Motor_Move(-z_rad, z_maxfre, z_minfre, Z_MOTOR);				
		while(Status != 0);
		Motor_Move(z_rad, z_maxfre, z_minfre, Z_MOTOR);
		while(Status != 0);
	
		EXTIX_ENABLE(EXTI9_5_IRQn);
		Motor_Move((8*x_rad), x_maxfre, x_minfre, X_MOTOR);     // X�����˶�
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
		uint16_t Y1_arr = 39999;    	 // 9999/   (8*10^6)/2*10^4 = 200    
		uint16_t Y2_arr = 19999;      	 // 19999/   (8*10^6)/10^4 = 400	Y2�ܳ�:78.5mm, (78.5/2)=39.25mm/s		
		
		Motor_Y1_Init(Y1_arr, (Y1_arr/2), CCW);
		Motor_Y2_Init(Y2_arr, (Y2_arr/2), CCW);
	
		while(K1==1);

		printf("In FLAG:%d\r\n", FLAG);
		if(FLAG == PTE1)
		{
				FLAG = 0;
				delay_s(2);
				printf("Y1ֹͣ\r\n");
				Y1_TIM_DisableOC;
				EXTIX_ENABLE(EXTI9_5_IRQn);
		}
		while(K2==1);
		if(K2 == 0) 
		{
			delay_ms(300);
			Y2_TIM_DisableOC;
		}
}


/********************************************
* ������: 			Paper_Move_In2
* ��������: 		��ֽ��һ�׶�Y1��Y2��ֽ�������, Y1:Y2 = 1:1.68
* ����: 			Y1_speed  Y1���ٶ�  			(mm/s)
*             Y2_speed  Y2���ٶ�  			(mm/s)
*							y_coord   Y����±��		(mm)
* ���: 			��
*********************************************/
void Paper_Move_In2(uint16_t Y1_speed, uint16_t Y2_speed,uint16_t Y1_coord, uint16_t Y2_coord)
{
		uint16_t Y1_arr = ARR_Y1;    	 									//  9999/   (8*10^6)/2*10^4 = 200    
		uint16_t Y2_arr = ARR_Y2;      	 								// 19999/  (8*10^6)/10^4 = 400	Y2�ܳ�:78.5mm, (78.5/2)=39.25mm/s		
		uint16_t Y1_delay_time = ((Y1_coord)/Y1_speed);								// ��ʱ Y1_delay_time ms
		uint16_t Y2_delay_time = ((Y2_coord*500)/Y2_speed);   							// ��ʱ Y2_delay_time ms

		EXTIX_DISABLE(EXTI9_5_IRQn);
		Motor_Y1_Init(Y1_arr, (Y1_arr/2), CCW);
		Motor_Y2_Init(Y2_arr, (Y2_arr/2), CCW);

		while(K1==1);

		printf("delay_time:%d\r\n", Y1_delay_time);
		delay_s(Y1_delay_time);
		delay_ms(100);
		Y1_TIM_DisableOC;
		printf("Y1ֹͣ\r\n");
		
		printf("delay_time:%d\r\n", Y2_delay_time);
		delay_ms(Y2_delay_time);
		delay_ms(Y2_delay_time);
	
		Y2_TIM_DisableOC;
		FLAG = 0;
		EXTIX_ENABLE(EXTI9_5_IRQn);
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
		delay_s(2);
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
* ����:			Z_mm:Z������(�½��ĸ߶�)
						Seal_id:ӡ��id
* ���:			��
************************************************/
void Exchange_Seal(int32_t Z_mm, int32_t Seal_id)
{
		int32_t z_rad = MM2Step(Z_mm);
	
		// ����ٶȲ���
		int16_t z_maxfre = 800*4;
		int16_t z_minfre = 800;
		int16_t tp_maxfre = 800;
		int16_t tp_minfre = 400;

		// ����ӡ��
		if(Seal_id != 0)
		{
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
		}else{
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
}


/***********************************************
* ������: 		Reset_Seal
* ��������:	��λӡ��
* ����:			Z_mm:Z������(�½��ĸ߶�)
						Seal_id:ӡ��id
* ���:			��
************************************************/
void Reset_Seal(int32_t Z_mm, int32_t Seal_id)
{
		int32_t z_rad = MM2Step(Z_mm);
	
		// ����ٶȲ���
		int16_t z_maxfre = 800*4;
		int16_t z_minfre = 800;
		int16_t tp_maxfre = 800;
		int16_t tp_minfre = 400;
	
		if(Seal_id != 0)
		{
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
		}else{
				// Z�������˶�
				Motor_Move(-z_rad, z_maxfre, z_minfre, Z_MOTOR);
				while(Status != 0);

				Servo_Open();
				
				// Z�������˶�
				Motor_Move(z_rad, z_maxfre, z_minfre, Z_MOTOR);
				while(Status != 0);	
			
				Servo_Close();
		}
}	



/********************************************
* ������: 		Stamper_Ctr
* ��������: 	�����ģ��
* ����: 		��
* ���: 		��
*********************************************/
void Stamper_Ctr(void)
{	
		int16_t ret = 0;									
		int32_t z_mm = 20;                  // ����ȡӡ�µ��г�
		int32_t z_down_mm = 40;							// ����z���г�
		int32_t seal_id = SEAL_ID[2];

		// 1.�����λ��ʼ��
		//Motor_Reset(); 
		while(1)
		{
				if(USART_RX_STA&0x8000)
				{
						ret = Decode();
						if(ret !=-1)
						{
								FLAG = 0;
					
								// 1.1 ��ֽ
								printf("usrat start\r\n");
								//Paper_Move_In();
								Paper_Move_In2(Y1_SPEED, Y2_SPEED, cmd.Y_1_MM, cmd.Y_2_MM);
							
								EXTIX_DISABLE(EXTI9_5_IRQn);
							
								// 1.2 ѡȡӡ��
								Exchange_Seal(z_mm, SEAL_ID[cmd.SEAL_ID]);	
								// 1.3 ���ø�������
								Cover_Seal(cmd.X_MM, z_down_mm);   
								// 1.4 ӡ�¸�λ
								Reset_Seal(z_mm, SEAL_ID[cmd.SEAL_ID]);
								EXTIX_ENABLE(EXTI9_5_IRQn);
							
								// 1.5 ��ֽ
								Paper_Move_Out();
							
								FLAG = 0;
								printf("finish\r\n");
						}
				}
			
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
						Cover_Seal(190, z_down_mm);   
						// 1.4 ӡ�¸�λ
						Reset_Seal(z_mm, seal_id);
						EXTIX_ENABLE(EXTI9_5_IRQn);
					
						// 1.5 ��ֽ
						Paper_Move_Out();
						FLAG = 0;
						printf("finish\r\n");
				}
				
				// 2. ������λ(����ӡ��) 
				if(FLAG == S2_KEY)
				{
							Motor_Reset();
							//Paper_Move_In2(Y1_SPEED, Y2_SPEED, 60, 270);
					
//						FLAG = 0;
//					
//						EXTIX_DISABLE(EXTI9_5_IRQn);
//						// 1.2 ѡȡӡ��
//						Exchange_Seal(z_mm, seal_id);	
//						// 1.4 ӡ�¸�λ
//						Reset_Seal(z_mm, seal_id);
//						EXTIX_ENABLE(EXTI9_5_IRQn);
//						Paper_Move_In();
//						FLAG = 0;
				}
		}
}





