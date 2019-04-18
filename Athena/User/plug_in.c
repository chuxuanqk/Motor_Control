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
 *      计算usart发送的信息size
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
 *       串口接收值转为INT
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
 *       解析串口传输的命令
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
 * 函数名:Stamper_Init
 * 函数功能:印章机配置初始化
 * 输入:无
 * 输出:无
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
* 函数名: Stamper_Ctr
* 函数功能: 电控主模块
* 输入: 无
* 输出: 无
*********************************************/
void SimpleTest(void)
{	
		int16_t Servo_cc=5;
	
		while(1)
		{
			if(FLAG == Start)
			{
					FLAG = 0;
					printf("Start\r\n");
					Servo_Config();
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
* 函数名: 		Motor_Reset
* 函数功能: 	X/Z/抓手复位
* 输入: 			无
* 输出: 			无
*********************************************/
void Motor_Reset(void)
{
		int32_t x_rad = 800*4;
		int32_t z_rad = 800*3;
		int16_t x_maxfre = 800*5;
		int16_t x_minfre = 800;
		int16_t z_maxfre = 800*4;
		int16_t z_minfre = 800;
               
		// X轴复位
		EXTIX_DISABLE(EXTI2_IRQn);
		Motor_Move(x_rad, x_maxfre, x_minfre, X_MOTOR);
		while(Status != 0);
		Motor_Move(-(8*x_rad), x_maxfre, x_minfre, X_MOTOR);  // 开启9_5外部中断
		while(Status != 0);
		EXTIX_ENABLE(EXTI2_IRQn);
		// Z轴复位动作
		Motor_Move(-z_rad, z_maxfre, z_minfre, Z_MOTOR);
		while(Status != 0);
		Motor_Move(z_rad, z_maxfre, z_minfre, Z_MOTOR);
		while(Status != 0);
	
}	

	
/********************************************
* 函数名: 		Paper_Move_1
* 函数功能: 	走纸第一阶段
* 输入: 			无	
* 输出: 			无
*********************************************/
void Paper_Move_1(void)
{
		uint16_t Y1_arr = 9999;
		uint16_t Y2_arr = 4999;
	
		Motor_Y1_Init(Y1_arr, (Y1_arr/2), CCW);
		Motor_Y2_Init(Y2_arr, (Y2_arr/2), CCW);
	
		while(K1==1);
		if(K1 == 0)
		{
				delay_s(3);
				Y1_TIM_DisableOC;
		}
		while(K2==1);
		if(K2 == 0) Y2_TIM_DisableOC;
}
	

/********************************************
* 函数名: 		Paper_Move_2
* 函数功能: 	走纸第二阶段
* 输入: 			无	
* 输出: 			无
*********************************************/
void Paper_Move_2(void)
{
		uint16_t Y2_arr = 4999;
		FLAG = 0;
		Motor_Y2_Init(Y2_arr, (Y2_arr/2), CCW);
		while(K2 == 0);
		Y2_TIM_DisableOC;
}

/********************************************
* 函数名: 		Cover_Seal
* 函数功能: 	盖章模块
* 输入: 			无
* 输出: 			无
*********************************************/
void Cover_Seal(void)
{
		int32_t x_rad = 800*45;
		int32_t z_rad = 800*10;
		int16_t x_maxfre = 800*14;
		int16_t x_minfre = 800;
		int16_t z_maxfre = 800*4;
		int16_t z_minfre = 800;
		
		// X轴向右运动
		delay_ms(1000);
		Motor_Move(x_rad, x_maxfre, x_minfre, X_MOTOR);
		while(Status != 0);
	
		delay_ms(2000);
		// Z轴向下运动
		Motor_Move(-z_rad, z_maxfre, z_minfre, Z_MOTOR);
		while(Status != 0);
		delay_s(2);
		// Z轴向上运动
		Motor_Move(z_rad, z_maxfre, z_minfre, Z_MOTOR);
		while(Status != 0);
	
		// X轴向左运动
		Motor_Move(-x_rad, x_maxfre, x_minfre, X_MOTOR);
		while(Status != 0);
}


/********************************************
* 函数名: 		Stamper_Ctr
* 函数功能: 	电控主模块
* 输入: 		无
* 输出: 		无
*********************************************/
void Stamper_Ctr(void)
{
		delay_s(2);
		Motor_Reset();
		while(1)
		{
				if(FLAG == Start)
				{
						FLAG = 0;
						Paper_Move_1();
						EXTIX_DISABLE(EXTI9_5_IRQn);
						Cover_Seal();
						EXTIX_ENABLE(EXTI9_5_IRQn);
						Paper_Move_2();
						printf("finish\r\n");
				}
		}
}





