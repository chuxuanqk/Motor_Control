#include "plug_in.h"



int main(void)
{
//		uint16_t i = 0;
//		int32_t x_rad = 800*4;
//		int16_t x_maxfre = 800*5;
//		int16_t x_minfre = 800;
	
		Stamper_Init();
		Stamper_Ctr();

//		while(1)
//		{
//				if(USART_RX_STA&0x8000)
//				{
//						for(; i<(USART_RX_STA&0x3FFF); ++i)
//						{
//								printf("%c",USART_RX_BUF[i]);
//						}
//						LED = ~LED;
//						USART_RX_STA=0;
//						i = 0;
//				}
//				
//				if(FLAG == Start)
//				{
//						FLAG= 0;
//						//Paper_Move_In();
//						Motor_Move((8*x_rad), x_maxfre, x_minfre, X_MOTOR);     // X向右运动
//						while(Status != 0);
//						Motor_Move(-(8*x_rad), x_maxfre, x_minfre, X_MOTOR);     // X向右运动
//						while(Status != 0);
//				}
//				
//		}

}

