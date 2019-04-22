#include "plug_in.h"



int main(void)
{
		uint16_t i = 0;
		Stamper_Init();
		//Stamper_Ctr();

		while(1)
		{
				if(USART_RX_STA&0x8000)
				{
						for(; i<(USART_RX_STA&0x3FFF); ++i)
						{
								printf("%c",USART_RX_BUF[i]);
						}
						LED = ~LED;
						USART_RX_STA=0;
						i = 0;
				}
		}

}

